#include "RenameSettings.hpp"
#include "BlendShapeRenamer.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <Windows.h>


int main(int argc, char * argv[]) {

	// CP932 is DEAD.
	SetConsoleOutputCP(CP_UTF8);

	std::cout << "Blendshape Duplicator Version 0.1.0 - Shirao Shotaro" << std::endl;
	std::cout << "Github on https://github.com/.....hogehoge" << std::endl;

#if defined(BD_LANG_EN)
	std::cout << "Thank you for using this software and your support." << std::endl;
	std::cout << "If you find some problems, please report to github issue or contact me twitter @shiraoshotaro." << std::endl;
	std::cout << "Also if you like this software, please visit my booth and give me a drink :-)" << std::endl;
	std::cout << "http://booth.///" << std::endl;
#elseif defined(BD_LANG_JP)
	std::cout << "Blendshape Duplicatorのご利用ありがとうございます" << std::endl;
	std::cout << "もともと自分用に開発したものです。不具合などございましたらgithubのissueかTwitterアカウント@shiraoshotaroまでコンタクトをお願い致します"
	std::cout << "また、もし気に入って頂けましたら、Boothによるご支援も宜しくお願いいたします" << std::endl;
	std::cout << "http://booth.///" << std::endl;
#endif

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

	std::cout << "Push ENTER to exit." << std::endl;
	getchar();

	return ret;
}