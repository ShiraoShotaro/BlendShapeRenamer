#include "RenameSettings.hpp"
#include "json11.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

wlib::RenameSettings::RenameSettings(const std::string & src_fbx_fname, const std::string & dst_fbx_fname, const std::unordered_map<std::string, std::vector<std::string>>& rename_map, const bool overwrite_ok)
	: src_fbx_filename_(src_fbx_fname), dst_fbx_filename_(dst_fbx_fname), rename_map_(rename_map), overwrite_ok_(overwrite_ok) {}

wlib::RenameSettings::RenameSettings(const RenameSettings & cp) 
	: src_fbx_filename_(cp.src_fbx_filename_), dst_fbx_filename_(cp.dst_fbx_filename_), rename_map_(cp.rename_map_), overwrite_ok_(cp.overwrite_ok_) {}

const std::string & wlib::RenameSettings::getSourceFBXFilename() const { return this->src_fbx_filename_; }

const std::string & wlib::RenameSettings::getDestinationFBXFilename() const { return this->dst_fbx_filename_; }

const std::unordered_map<std::string, std::vector<std::string>> & wlib::RenameSettings::getRenameMap() const { return this->rename_map_; }

bool wlib::RenameSettings::isValid() const { return !this->getSourceFBXFilename().empty() && !this->getDestinationFBXFilename().empty(); }

bool wlib::RenameSettings::showOverwriteWarning() const {
	if (!this->overwrite_ok_){
		std::ifstream ifst(this->getDestinationFBXFilename(), std::ios::binary);
		if (ifst) {
			std::string temp;
			do {
				temp.clear();
				std::cout << "Destination file [ " << this->getDestinationFBXFilename() << " ] has existed. Overwrite (y / n) ? ";
				std::cin >> temp;
			} while (temp != "y" && temp != "n");
			if (temp == "n") return false;
		}
	}
	return true;
}

wlib::RenameSettings wlib::RenameSettings::loadFromJson(const std::string & json_filename)
{
	std::ifstream json_ifst(json_filename);
	if (!json_ifst) {
		std::cerr << "Failed to open json settings file." << std::endl;
		return RenameSettings("", "", std::unordered_map<std::string, std::vector<std::string>>(), false);
	}

	// ÉtÉ@ÉCÉãì«çûçÏã∆
	std::istreambuf_iterator<char> it(json_ifst), last;
	std::string json_str(it, last);

	std::string error_msg;
	const json11::Json json = json11::Json::parse(json_str, error_msg);

	if (!error_msg.empty()) {
		std::cerr << "Failed to parse json." << std::endl;
		std::cerr << "Reason : " << error_msg << std::endl;
		return RenameSettings("", "", std::unordered_map<std::string, std::vector<std::string>>(), false);
	}

	const std::string src = json["src"].asString();
	const std::string dst = json["dst"].asString();
	const bool is_overwrite = json["overwrite"].asBool(false);

	std::unordered_map<std::string, std::vector<std::string>> rename_map;

	if (json["map"].is_object()) {
		const auto & map_items = json["map"].objectItems();
		for (const auto & map : map_items) {
			if (map.second.is_array()) {
				for (const auto & item : map.second.arrayItems()) {
					rename_map[map.first].push_back(item.asString());
				}
			}
		}
	}

	return RenameSettings(src, dst, rename_map, is_overwrite);
}
