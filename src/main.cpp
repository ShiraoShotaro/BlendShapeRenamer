#include "RenameSettings.hpp"
#include "BlendShapeRenamer.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <Windows.h>

#define BD_LANG_JP

namespace {

int process(int argc, char * argv[]) {

	if (argc != 2) {
		std::cerr << "Invalid Argument." << std::endl;
		std::cerr << "Usage : BlendshapeDuplicator.exe [Settings json filepath]" << std::endl;
		return -1;
	}

	wlib::RenameSettings rename_settings = wlib::RenameSettings::loadFromJson(argv[1]);
	if (!rename_settings.isValid()) {
		std::cerr << "An error has occured." << std::endl;
		return -2;
	}

	if (!rename_settings.showOverwriteWarning()) {
		std::cerr << "Operation is canceled by user." << std::endl;
		return -3;
	}

	int ret = wlib::BlendshapeRenamer(rename_settings).process();

	if (ret == 0) {
		std::cout << "Finished successfully." << std::endl;
	}
	else {
		std::cerr << "An error has occured." << std::endl;
	}

	return ret;
}

}

int main(int argc, char * argv[]) {

	// CP932 is DEAD.
	SetConsoleOutputCP(CP_UTF8);

	std::cout << "Blendshape Duplicator Version 0.1.0 - Shirao Shotaro" << std::endl;
	std::cout << "Github on https://github.com/.....hogehoge" << std::endl;

#if defined(BD_LANG_EN)
	std::cout << "Thank you for using this software." << std::endl;
	std::cout << "If you find some problems, please report them to github issue or contact me on twitter @shiraoshotaro." << std::endl;
	std::cout << "Also if you like this software, please visit my booth and give me a drink :-)" << std::endl;
	std::cout << "http://booth.///" << std::endl;
#elif defined(BD_LANG_JP)
	std::cout << u8"Blendshape Duplicatorのご利用ありがとうございます" << std::endl;
	std::cout << u8"もともと自分用に開発したものです。不具合などございましたらgithubのissueかTwitterアカウント@shiraoshotaroまでコンタクトをお願い致します" << std::endl;
	std::cout << u8"また、もし気に入って頂けましたら、Boothによるご支援も宜しくお願いいたします" << std::endl;
	std::cout << "http://booth.///" << std::endl;
#endif

	int ret = process(argc, argv);

	std::cout << "Type any character(s) to exit." << std::endl;
	
	std::string dmp;
	std::cin >> dmp;

	return ret;
}