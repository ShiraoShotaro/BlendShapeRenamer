#pragma once

#include "RenameSettings.hpp"
#include <fbxsdk.h>

namespace wlib {

class BlendshapeRenamer {
public:
	BlendshapeRenamer(const RenameSettings & rename_settings);

	int process();

private:
	const RenameSettings rename_settings_;

	void _seekNode(FbxNode * pNode);
	void _seekNodeAttribute(FbxNode * pNode, FbxNodeAttribute * pAttribute);

};

}