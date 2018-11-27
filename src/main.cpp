#include <fbxsdk.h>
#include <iostream>
#include <string>
#include <sstream>

/* Tab character ("\t") counter */
int numTabs = 0;

void PrintNode(FbxNode* pNode);

/**
 * Print the required number of tabs.
 */
void PrintTabs() {
	for (int i = 0; i < numTabs; i++)
		printf(" ");
}

/**
 * Return a string-based representation based on the attribute type.
 */
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

/**
 * Print an attribute.
 */
#include <tchar.h>
void PrintAttribute(FbxNodeAttribute* pAttribute) {
	if (!pAttribute) return;

	FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
	FbxString attrName = pAttribute->GetName();
	PrintTabs();
	// Note: to retrieve the character array of a FbxString, use its Buffer() method.
	printf("ATTR : %s, %s [c = %d]\n", typeName.Buffer(), attrName.Buffer(), pAttribute->GetNodeCount());
	if (pAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
		FbxGeometry * geometry = (FbxGeometry*)pAttribute;
		int blend_shape_count = geometry->GetDeformerCount();
		for (int def_idx = 0; def_idx < blend_shape_count; ++def_idx) {
			FbxBlendShape * blend_shape = (FbxBlendShape*)geometry->GetDeformer(def_idx, FbxDeformer::eBlendShape);
			if (blend_shape == nullptr) continue;
			int blend_shape_channel_count = blend_shape->GetBlendShapeChannelCount();
			numTabs++;
			for (int ch_idx = 0; ch_idx < blend_shape_channel_count; ch_idx++) {
				FbxBlendShapeChannel * blend_shape_ch = (FbxBlendShapeChannel*)blend_shape->GetBlendShapeChannel(ch_idx);
				PrintTabs();
				std::cout << "BS[" << ch_idx << "]" << blend_shape_ch->GetName() << " rename -> ";
				std::stringstream sstr;
				sstr << blend_shape->GetName() << u8".‚Ù‚°" << ch_idx << std::ends;
				blend_shape_ch->SetName(sstr.str().c_str());
				std::cout << blend_shape_ch->GetName() << std::endl;
			}
			numTabs--;
		}
	}
}

/**
 * Print a node, its attributes, and all its children recursively.
 */
void PrintNode(FbxNode* pNode) {
	PrintTabs();
	const char* nodeName = pNode->GetName();
	FbxDouble3 translation = pNode->LclTranslation.Get();
	FbxDouble3 rotation = pNode->LclRotation.Get();
	FbxDouble3 scaling = pNode->LclScaling.Get();

	// Print the contents of the node.
	printf("NODE : %s => t(%.1f, %.1f, %.1f) r(%.1f, %.1f, %.1f) s(%.1f, %.1f, %.1f)\n",
		nodeName,
		translation[0], translation[1], translation[2],
		rotation[0], rotation[1], rotation[2],
		scaling[0], scaling[1], scaling[2]
	);
	numTabs++;

	// Print the node's attributes.
	for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
		PrintAttribute(pNode->GetNodeAttributeByIndex(i));

	// Recursively print the children.
	for (int j = 0; j < pNode->GetChildCount(); j++)
		PrintNode(pNode->GetChild(j));

	numTabs--;
	PrintTabs();
	printf("\n");
}




int main(int argc, char** argv) {

	if (argc != 2) {
		std::cerr << "wlib Blendshape Renamer ver 0.1" << std::endl;
		std::cerr << "usage: BlendShapeRenamer.exe [fbx file]" << std::endl;
		return -1;
	}

	const std::string filename(argv[1]);
	std::cout << "filename : " << filename << std::endl;
	const std::string export_filename("modified.fbx");

	// Initialize the SDK manager. This object handles all our memory management.
	FbxManager * fbx_manager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings * ios = FbxIOSettings::Create(fbx_manager, IOSROOT);
	fbx_manager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	FbxImporter * fbx_importer = FbxImporter::Create(fbx_manager, "");
	FbxExporter * fbx_exporter = FbxExporter::Create(fbx_manager, "");

	// Use the first argument as the filename for the importer.
	if (!fbx_importer->Initialize(filename.c_str(), -1, fbx_manager->GetIOSettings())) {
		std::cerr << "Call to FbxImporter::Initialize() failed." << std::endl;
		std::cerr << "Error returned: " << fbx_importer->GetStatus().GetErrorString() << std::endl;
		return -1;
	}

	if (!fbx_exporter->Initialize(export_filename.c_str(), -1, fbx_manager->GetIOSettings())) {
		std::cerr << "Call to FbxExporter::Initialize() failed." << std::endl;
		std::cerr << "Error returned: " << fbx_exporter->GetStatus().GetErrorString() << std::endl;
		return -2;
	}

	// Create a new scene so that it can be populated by the imported file.
	FbxScene * fbx_scene = FbxScene::Create(fbx_manager, "myScene");

	// Import the contents of the file into the scene.
	fbx_importer->Import(fbx_scene);

	// The file is imported; so get rid of the importer.
	fbx_importer->Destroy();

	// Print the nodes of the scene and their attributes recursively.
	// Note that we are not printing the root node because it should
	// not contain any attributes.
	FbxNode * root_node = fbx_scene->GetRootNode();
	if(root_node) {
		for (int i = 0; i < root_node->GetChildCount(); i++) PrintNode(root_node->GetChild(i));
	}

	//Export
	fbx_exporter->Export(fbx_scene);
	fbx_exporter->Destroy();

	std::cout << "Exported." << std::endl;


	// Destroy the SDK manager and all the other objects it was handling.
	fbx_manager->Destroy();

	std::cout << "finished." << std::endl;
	getchar();

	return 0;
}