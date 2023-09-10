#pragma once

#include <unordered_map>
#include <string>

enum class RecipeTag {
	GENERAL,
	FOOD,
	DRINK,
	ARTIFICE,
	POTION,
	SWEET,

	INVALID
};

inline std::unordered_map<std::string, RecipeTag> const recipeTagFromStrTable = {
	{"General", RecipeTag::GENERAL},
	{"Food", RecipeTag::FOOD},
	{"Drink", RecipeTag::DRINK},
	{"Artifice", RecipeTag::ARTIFICE},
	{"Potion", RecipeTag::POTION},
	{"Sweet", RecipeTag::SWEET}
};

inline std::unordered_map<RecipeTag, std::string> const recipeTagToStrTable = {
	{RecipeTag::GENERAL, "General"},
	{RecipeTag::FOOD, "Food"},
	{RecipeTag::DRINK, "Drink"},
	{RecipeTag::ARTIFICE, "Artifice"},
	{RecipeTag::POTION, "Potion"},
	{RecipeTag::SWEET, "Sweet"}
};

inline RecipeTag recipeTagFromStr(std::string str) {
	if (recipeTagFromStrTable.find(str) != recipeTagFromStrTable.end()) {
		return recipeTagFromStrTable.at(str);
	}
	return RecipeTag::INVALID;
}

inline std::string recipeTagToStr(RecipeTag tag) {
	if (recipeTagToStrTable.find(tag) != recipeTagToStrTable.end()) {
		return recipeTagToStrTable.at(tag);
	}
	return "Invalid";
}