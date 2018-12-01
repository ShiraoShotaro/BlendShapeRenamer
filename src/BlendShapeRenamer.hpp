#pragma once

#include <unordered_map>

namespace wlib {

extern std::unordered_map<std::string, std::string> createRenamerMap(const int argc, char * argv[]);

}