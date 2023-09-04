#pragma once

#include <shlobj_core.h>
#include <filesystem>
#include <iostream>

#include "../PugiXML/pugixml.hpp"
#include "../Logger.h"

class FileManager
{
public:
	FileManager();

	bool loadXML();

private:
	std::filesystem::path dataPath;

	pugi::xml_document itemsXMLDoc;
};

