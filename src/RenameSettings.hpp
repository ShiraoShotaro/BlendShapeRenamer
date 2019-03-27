#ifndef WLIB_RENAMESETTINGS_HPP
#define WLIB_RENAMESETTINGS_HPP

#include <unordered_map>
#include <string>
#include <vector>

namespace wlib {

class RenameSettings {
	const std::string src_fbx_filename_;
	const std::string dst_fbx_filename_;
	const bool overwrite_ok_;
	const std::unordered_map<std::string, std::vector<std::string>> rename_map_;

	RenameSettings(const std::string & src_fbx_fname, const std::string & dst_fbx_fname, const std::unordered_map<std::string, std::vector<std::string>> & rename_map, const bool overwrite_ok);

public:

	/// @brief Copy Constructor.
	RenameSettings(const RenameSettings & cp);

	/// @brief Get source fbx fullname.
	const std::string & getSourceFBXFilename() const;

	/// @brief Get destination fbx fullname.
	/// If the file has already existed, it will be overwritten.
	const std::string & getDestinationFBXFilename() const;

	/// @brief Get rename map.
	const std::unordered_map<std::string, std::vector<std::string>> & getRenameMap() const;

	/// @brief Get if this instance is valid.
	bool isValid() const;

	bool showOverwriteWarning() const;

	/// @brief Create this instance
	static RenameSettings loadFromJson(const std::string & json_filename);

};

}

#endif