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
	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "Failed to get LocalLow folder path");
		return false;
	}

	dataPath = std::filesystem::path(localLow) / "LoE" / "Legends of Equestria";

	if (!std::filesystem::exists(dataPath)) {
		ErrorLogger::Log("Data folder not found, have you installed Legends of Equestria?");
		return false;
	}

	if (!loadItems()) return false;
	if (!loadRecipes()) return false;

	return true;
}

bool FileManager::loadItems() {

	std::filesystem::path itemsXMLPath = dataPath / "data" / "Items.xml";
	pugi::xml_parse_result result = itemsXMLDoc.load_file(itemsXMLPath.c_str());

	if (!result) {
		std::string errorStr = "Error loading Items.xml from:\n";
		errorStr += itemsXMLPath.string();
		ErrorLogger::Log(result, errorStr);
		return false;
	}

	pugi::xml_node itemsArrayNode = itemsXMLDoc.child("ArrayOfItem");

	for (pugi::xml_node itemNode : itemsArrayNode.children("Item")) {
		Item itemObj = Item(itemNode);
		items.insert({itemObj.id, itemObj});
		//std::printf("Item loaded: %s\n", itemObj.toString().c_str());
	}

	return true;
}

bool FileManager::loadRecipes() {

	std::filesystem::path recipesXMLPath = dataPath / "data" / "Recipes.xml";
	pugi::xml_parse_result result = recipesXMLDoc.load_file(recipesXMLPath.c_str());

	if (!result) {
		std::string errorStr = "Error loading Recipes.xml from:\n";
		errorStr += recipesXMLPath.string();
		ErrorLogger::Log(result, errorStr);
		return false;
	}

	pugi::xml_node recipesArrayNode = recipesXMLDoc.child("ArrayOfRecipe");

	for (pugi::xml_node recipeNode : recipesArrayNode.children("Recipe")) {
		Recipe recipeObj = Recipe(recipeNode);
		recipes.insert({recipeObj.id, recipeObj});
		std::printf("Recipe loaded: %s\n", recipeObj.toString().c_str());
	}

	return true;
}

Item FileManager::getItem(int id) {
	return items.find(id)->second;
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
	if (name == lastSearchQuery) {
		return lastSearchResult;
	}

	std::vector<Item> results;
	for (std::pair<int, Item> item : items) {
		if (item.second.match(name)) {
			results.push_back(item.second);
		}
	}
	lastSearchResult = results;
	return results;
}

TexLocation FileManager::getTextureLocation(int id) {
	auto item = items.find(id);
	if (item != items.end()) {
		return item->second.getTextureLocation();
	}
	return TexLocation(0.0f, 0.0f, 0.0f, 0.0f);
}

Recipe FileManager::getRecipe(int id) {
	return recipes.find(id)->second;
}

Recipe FileManager::findRecipeFor(Item item) {
	for (std::pair<int, Recipe> recipe : recipes) {
		if (recipe.second.itemID == item.id) {
			return recipe.second;
		}
	}
	return Recipe();
}

Recipe FileManager::findRecipeUsing(Item item) {
	for (std::pair<int, Recipe> recipe : recipes) {
		for (Reagent reagent : recipe.second.reagents) {
			if (reagent.id == item.id) {
				return recipe.second;
			}
		}
	}
	return Recipe();
}

std::string FileManager::prettyRecipeString(Recipe recipe) {
	std::string str = std::format("Recipe for {0}:\n", getItem(recipe.itemID).name);
	for (Reagent r : recipe.reagents) {
		str += std::format("- {0}x {1}\n", r.quantity, getItem(r.id).name);
	}
	str += std::format("Requires {0} level {1}\n", talentToStr(recipe.talent), recipe.requiredLevel);
	str += std::format("Made at {0}\n", stationsToStr(recipe.craftingStations));
	str += std::format("Takes {0} seconds to craft for {1} XP", recipe.craftingTime, recipe.experienceGiven);
	return str;
}
