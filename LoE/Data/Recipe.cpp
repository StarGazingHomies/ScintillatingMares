#include "Recipe.h"

Recipe::Recipe() {
	id = 0;
	itemID = 0;
	talent = Talent::INVALID;
	yield = 0;
	requiredLevel = 0;
	craftingStations = 0;
	requiresRecipe = false;
	name = "";
	reagents = std::vector<Reagent>();
	recipeTag = RecipeTag::INVALID;
	craftingTime = 0.0;
	isRNG = false;
	experienceGiven = 0;
}

Recipe::Recipe(pugi::xml_node node) {
	id = readInt(node, "ID");
	itemID = readInt(node, "ItemId");
	talent = readTalent(node, "Talent");
	yield = readInt(node, "Yield");
	requiredLevel = readInt(node, "RequiredLevel");
	craftingStations = readStations(node, "CraftingStations");
	requiresRecipe = readBool(node, "RequiresRecipe");
	name = readString(node, "Name");

	recipeTag = readRecipeTag(node, "RecipeTag");
	craftingTime = readDouble(node, "CraftingTime");
	isRNG = readBool(node, "IsRNG");
	experienceGiven = readInt(node, "ExperienceGiven");

	reagents = std::vector<Reagent>();
	for (pugi::xml_node reagent : node.child("Reagents").children("Reagent")) {
		reagents.push_back(Reagent(reagent));
	}
}

std::string Recipe::toString() const {
	std::string str = std::format("Recipe for {0} (ItemID: {1}, RecipeID: {2}) by using ", this->name, this->itemID, this->id);
	for (Reagent reagent : this->reagents) {
		str += std::format("{0} x {1}, ", reagent.quantity, reagent.id);
	}
	return str;
}

