#pragma once

#include "Common.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>

struct Settings
{
	std::string ip;
	std::string port;
};

class Config
{
public:
	static ClientSettings getSettings(const char* filename);
	static std::string getAbsolutePath(std::string path);
};
