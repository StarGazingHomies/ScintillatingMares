#pragma once

#include <shlobj_core.h>
#include <filesystem>
#include <iostream>

#include "../PugiXML/pugixml.hpp"
#include "../Logger.h"

#include "Data/Item.h"

class FileManager
{
public:
	FileManager();

	bool Initialize();
	bool loadItems();

	// Items
	Item findItem(std::string name);
	TexLocation getTextureLocation(int id);

	// Recipes


private:
	std::filesystem::path dataPath;

	pugi::xml_document itemsXMLDoc;
	std::unordered_map<int, Item> items;
};

