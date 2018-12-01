#include "BlendShapeRenamer.hpp"
#include <regex>

std::unordered_map<std::string, std::string> wlib::createRenamerMap(const int argc, char * argv[])
{
	std::unordered_map<std::string, std::string> ret;
	// argument is required more than 3.
	if (argc < 2) return ret;

	for (int i = 2; i < argc; i++) {
		const std::string arg(argv[i]);
		std::regex regex(R"((.*)=>(.*))");
		std::smatch match;
		std::regex_match(arg.cbegin(), arg.cend(), match, regex);
		if (match.size() >= 3) {
			ret[match[1]] = match[2];
		}
	}

	return ret;
}
