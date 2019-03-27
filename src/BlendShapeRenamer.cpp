#include "BlendShapeRenamer.hpp"
#include <iostream>

wlib::BlendshapeRenamer::BlendshapeRenamer(const RenameSettings & rename_settings)
	: rename_settings_(rename_settings) {}

int wlib::BlendshapeRenamer::process(){

	FbxManager * fbx_manager = FbxManager::Create();
	FbxIOSettings * ios = FbxIOSettings::Create(fbx_manager, IOSROOT);
	fbx_manager->SetIOSettings(ios);

	FbxImporter * fbx_importer = FbxImporter::Create(fbx_manager, "");
	FbxExporter * fbx_exporter = FbxExporter::Create(fbx_manager, "");

	// Load Source FBX
	std::cout << "Source Filename : " << this->rename_settings_.getSourceFBXFilename() << " ...... ";
	if (!fbx_importer->Initialize(this->rename_settings_.getSourceFBXFilename().c_str(), -1, fbx_manager->GetIOSettings())) {
		std::cout << "[FAILED]" << std::endl;
		std::cerr << "Call to FbxImporter::Initialize() failed." << std::endl;
		std::cerr << "Returned error : " << fbx_importer->GetStatus().GetErrorString() << std::endl;
		return -4;
	}
	std::cout << "[SUCCESS]" << std::endl;

	// Open Destination FBX
	std::cout << "Destination filename : " << this->rename_settings_.getDestinationFBXFilename() << " ........ ";
	if (!fbx_exporter->Initialize(this->rename_settings_.getDestinationFBXFilename().c_str(), -1, fbx_manager->GetIOSettings())) {
		std::cerr << "[FAILED]" << std::endl << "Call to FbxExporter::Initialize() failed." << std::endl;
		std::cerr << "Error returned: " << fbx_exporter->GetStatus().GetErrorString() << std::endl;
		return -5;
	}
	std::cout << "[SUCCESS]" << std::endl;

	// Scene creating
	FbxScene * fbx_scene = FbxScene::Create(fbx_manager, "myScene");

	// Import FBX
	std::cout << "Importing fbx ...... ";
	fbx_importer->Import(fbx_scene);
	std::cout << "[FINISHED]" << std::endl;
	fbx_importer->Destroy();

	// Search Blendshape and duplicate
	FbxNode * root_node = fbx_scene->GetRootNode();
	if (root_node) {
		for (int i = 0; i < root_node->GetChildCount(); i++) this->_seekNode(root_node->GetChild(i));
	}

	// Export FBX
	std::cout << "Exporting fbx ...... ";
	fbx_exporter->Export(fbx_scene);
	std::cout << "[FINISHED]" << std::endl;
	fbx_exporter->Destroy();

	// Manager destroy
	fbx_manager->Destroy();

	return 0;
}

void wlib::BlendshapeRenamer::_seekNode(FbxNode * pNode)
{
	// Search the node's attributes.
	for (int i = 0; i < pNode->GetNodeAttributeCount(); i++) this->_seekNodeAttribute(pNode, pNode->GetNodeAttributeByIndex(i));

	// Recursively search the children.
	for (int j = 0; j < pNode->GetChildCount(); j++) this->_seekNode(pNode->GetChild(j));
}

void wlib::BlendshapeRenamer::_seekNodeAttribute(FbxNode * pNode, FbxNodeAttribute * pAttribute){
	if (!pAttribute) return;

	if (pAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
		FbxGeometry * geometry = (FbxGeometry*)pAttribute;
		int blend_shape_count = geometry->GetDeformerCount();
		for (int def_idx = 0; def_idx < blend_shape_count; ++def_idx) {
			FbxBlendShape * blend_shape = (FbxBlendShape*)geometry->GetDeformer(def_idx, FbxDeformer::eBlendShape);
			if (blend_shape == nullptr) continue;
			int blend_shape_channel_count = blend_shape->GetBlendShapeChannelCount();
			for (int ch_idx = 0; ch_idx < blend_shape_channel_count; ch_idx++) {
				FbxBlendShapeChannel * blend_shape_ch = (FbxBlendShapeChannel*)blend_shape->GetBlendShapeChannel(ch_idx);
				const std::string current_blendshape_name = blend_shape_ch->GetName();

				std::cout << "[Mesh] " << pNode->GetName() << " --[Blendshape] " << current_blendshape_name << std::endl;

				if (this->rename_settings_.getRenameMap().count(current_blendshape_name)) {
					for (auto & renamer : this->rename_settings_.getRenameMap().at(current_blendshape_name)) {
						FbxBlendShapeChannel * dup = (FbxBlendShapeChannel*)blend_shape_ch->Clone();
						dup->SetName(renamer.c_str());
						blend_shape->AddBlendShapeChannel(dup);
						std::cout << " *** " << current_blendshape_name << " is duplicated as " << dup->GetName() << std::endl;
					}
				}
			}
		}
	}
}

