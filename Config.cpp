#include "Config.h"

Settings Config::getSettings(const char* filename)
{
	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(filename, pt);

	ClientSettings settings;
	settings.ip = pt.get<std::string>("settings.ip", std::string());
	settings.port = pt.get("settings.port", std::string());

	return settings;
}

std::string Config::getAbsolutePath(std::string path)
{
	boost::filesystem::path audioPath(audio);
	if ( ! audioPath.is_absolute())
		return boost::filesystem::absolute(audioPath).generic_string();
	else
		return audio;
}