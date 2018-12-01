#include <fbxsdk.h>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <Windows.h>

namespace {

std::unordered_map<std::string, std::string> rename_map;

void initializeRenameMap(void) {
	rename_map["vrc.v_aa"] = u8"‚ ";
	rename_map["vrc.v_ih"] = u8"‚¦";
	rename_map["vrc.v_ou"] = u8"‚¤";
	rename_map["vrc.v_e"] = u8"‚¢";
	rename_map["vrc.v_oh"] = u8"‚¨";
	rename_map["vrc.eye_close"] = u8"‚Ü‚Î‚½‚«";
}

}

/**
 * Print an attribute.
 */
void PrintAttribute(FbxNodeAttribute* pAttribute) {
	if (!pAttribute) return;

	//FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
	//FbxString attrName = pAttribute->GetName();
	//PrintTabs();
	// Note: to retrieve the character array of a FbxString, use its Buffer() method.
	//printf("ATTR : %s, %s [c = %d]\n", typeName.Buffer(), attrName.Buffer(), pAttribute->GetNodeCount());
	
	if (pAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
		FbxGeometry * geometry = (FbxGeometry*)pAttribute;
		int blend_shape_count = geometry->GetDeformerCount();
		for (int def_idx = 0; def_idx < blend_shape_count; ++def_idx) {
			FbxBlendShape * blend_shape = (FbxBlendShape*)geometry->GetDeformer(def_idx, FbxDeformer::eBlendShape);
			if (blend_shape == nullptr) continue;
			int blend_shape_channel_count = blend_shape->GetBlendShapeChannelCount();
			for (int ch_idx = 0; ch_idx < blend_shape_channel_count; ch_idx++) {
				FbxBlendShapeChannel * blend_shape_ch = (FbxBlendShapeChannel*)blend_shape->GetBlendShapeChannel(ch_idx);
				std::cout << "-- BlendShape is found = " << blend_shape_ch->GetName();
				
				if (rename_map.count(blend_shape_ch->GetName())) {
					//blend_shape_ch->SetName(rename_map.at(blend_shape_ch->GetName()).c_str());
					FbxBlendShapeChannel * dup = (FbxBlendShapeChannel*)blend_shape_ch->Clone();
					dup->SetName(rename_map.at(blend_shape_ch->GetName()).c_str());
					blend_shape->AddBlendShapeChannel(dup);
					std::cout << " is duprecated as " << dup->GetName();
				}
				std::cout << std::endl;
				//std::stringstream sstr;
				//sstr << blend_shape->GetName() << u8".‚Ù‚°" << ch_idx << std::ends;
				//blend_shape_ch->SetName(sstr.str().c_str());
			}
		}
	}
}

void PrintNode(FbxNode* pNode) {
	std::cout << "Seek : " << pNode->GetName() << std::endl;

	// Print the node's attributes.
	for (int i = 0; i < pNode->GetNodeAttributeCount(); i++) PrintAttribute(pNode->GetNodeAttributeByIndex(i));

	// Recursively print the children.
	for (int j = 0; j < pNode->GetChildCount(); j++) PrintNode(pNode->GetChild(j));
}

int main(int argc, char** argv) {
	SetConsoleOutputCP(CP_UTF8);
	if (argc != 2) {
		std::cerr << "wlib Blendshape Renamer ver 0.1" << std::endl;
		std::cerr << "usage: BlendShapeRenamer.exe [fbx file]" << std::endl;
		return -1;
	}

	initializeRenameMap();

	const std::string filename(argv[1]);
	const std::string export_filename("modified.fbx");
	
	FbxManager * fbx_manager = FbxManager::Create();

	FbxIOSettings * ios = FbxIOSettings::Create(fbx_manager, IOSROOT);
	fbx_manager->SetIOSettings(ios);

	FbxImporter * fbx_importer = FbxImporter::Create(fbx_manager, "");
	FbxExporter * fbx_exporter = FbxExporter::Create(fbx_manager, "");

	std::cout << "input filename  : " << filename << " ........ ";
	if (!fbx_importer->Initialize(filename.c_str(), -1, fbx_manager->GetIOSettings())) {
		std::cerr << "[FAILED]" << std::endl << "Call to FbxImporter::Initialize() failed." << std::endl;
		std::cerr << "Error returned: " << fbx_importer->GetStatus().GetErrorString() << std::endl;
		return -1;
	}
	std::cout << "[SUCCESS]" << std::endl;

	std::cout << "output filename : " << export_filename << " ........ ";
	if (!fbx_exporter->Initialize(export_filename.c_str(), -1, fbx_manager->GetIOSettings())) {
		std::cerr << "[FAILED]" << std::endl << "Call to FbxExporter::Initialize() failed." << std::endl;
		std::cerr << "Error returned: " << fbx_exporter->GetStatus().GetErrorString() << std::endl;
		return -2;
	}
	std::cout << "[SUCCESS]" << std::endl;

	FbxScene * fbx_scene = FbxScene::Create(fbx_manager, "myScene");
	std::cout << "Scene created" << std::endl;

	std::cout << "Importing fbx ........ ";
	fbx_importer->Import(fbx_scene);
	std::cout << "[FINISHED]" << std::endl;
	fbx_importer->Destroy();

	FbxNode * root_node = fbx_scene->GetRootNode();
	if(root_node) {
		for (int i = 0; i < root_node->GetChildCount(); i++) PrintNode(root_node->GetChild(i));
	}

	//Export
	std::cout << "Exporting fbx ........ ";
	fbx_exporter->Export(fbx_scene);
	std::cout << "[FINISHED]" << std::endl;
	fbx_exporter->Destroy();

	fbx_manager->Destroy();

	std::cout << "Finished all processes successfully." << std::endl;

#ifdef _DEBUG
	getchar();
#endif

	return 0;
}