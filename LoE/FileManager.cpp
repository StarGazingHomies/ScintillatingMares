#include "FileManager.h"

const char* node_types[] = {
	"null", "document", "element", "pcdata", "cdata", "comment", "pi", "declaration"
};

FileManager::FileManager() {
}

bool FileManager::Initialize() {
	wchar_t* localLow;
	HRESULT hr = SHGetKnownFolderPath(
		FOLDERID_LocalAppDataLow,
		0,
		NULL,
		&localLow
	);

	dataPath = std::filesystem::path(localLow) / "LoE" / "Legends of Equestria";

	if (!std::filesystem::exists(dataPath)) {
		Logger::Log("Data folder not found, have you installed Legends of Equestria?");
		return false;
	}

	if (!loadItems()) return false;

	return true;
}

bool FileManager::loadItems() {

	std::filesystem::path itemsXMLPath = dataPath / "data" / "Items.xml";
	pugi::xml_parse_result result = itemsXMLDoc.load_file(itemsXMLPath.c_str());

	if (!result) {
		std::string errorStr = "Error loading Items.xml from:\n";
		errorStr += itemsXMLPath.string();
		Logger::Log(result, errorStr);
		return false;
	}

	pugi::xml_node itemsArrayNode = itemsXMLDoc.child("ArrayOfItem");

	for (pugi::xml_node itemNode : itemsArrayNode.children("Item")) {
		Item itemObj = Item(itemNode);
		items.insert({itemObj.ID, itemObj});
		std::printf("Item loaded: %s\n", itemObj.toString().c_str());
	}

	return true;
}

bool FileManager::loadRecipes() {

	std::filesystem::path recipesXMLPath = dataPath / "data" / "Recipes.xml";
	pugi::xml_parse_result result = recipesXMLDoc.load_file(recipesXMLPath.c_str());

	if (!result) {
		std::string errorStr = "Error loading Recipes.xml from:\n";
		errorStr += recipesXMLPath.string();
		Logger::Log(result, errorStr);
		return false;
	}

	pugi::xml_node recipesArrayNode = recipesXMLDoc.child("ArrayOfRecipe");

	return true;
}

Item FileManager::findItem(std::string name) {
	for (std::pair<int, Item> item : items) {
		if (item.second.name == name) {
			return item.second;
		}
	}
	return Item();
}

std::vector<Item> FileManager::searchItems(std::string name) {
	std::vector<Item> results;
	for (std::pair<int, Item> item : items) {
		if (item.second.match(name)) {
			results.push_back(item.second);
		}
	}
	return results;
}

TexLocation FileManager::getTextureLocation(int id) {
	auto item = items.find(id);
	if (item != items.end()) {
		return item->second.getTextureLocation();
	}
	return TexLocation(0.0f, 0.0f, 0.0f, 0.0f);
}
