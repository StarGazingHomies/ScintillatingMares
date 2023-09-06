#pragma once

#include <shlobj_core.h>
#include <filesystem>
#include <iostream>

#include "../PugiXML/pugixml.hpp"
#include "../Logger.h"

#include "Data/Item.h"
#include "Data/Recipe.h"


class FileManager {
public:
	FileManager();

	// Initialization
	bool Initialize();
	bool loadItems();
	bool loadRecipes();

	// Items
	Item findItem(std::string name);
	std::vector<Item> searchItems(std::string name);
	TexLocation getTextureLocation(int id);

	// Recipes


private:
	std::filesystem::path dataPath;

	pugi::xml_document itemsXMLDoc;
	pugi::xml_document recipesXMLDoc;
	std::unordered_map<int, Item> items;
};

