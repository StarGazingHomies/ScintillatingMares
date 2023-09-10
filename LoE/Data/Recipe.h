#pragma once

#include <string>
#include <format>
#include "../../PugiXML/pugixml.hpp"
#include "Talent.h"
#include "RecipeTag.h"
#include "Reader.h"

struct Reagent {
	Reagent() : id(0), quantity(0), quality(0), tool(false) {}
	Reagent(int id, int quantity, int quality, bool tool) : id(id), quantity(quantity), quality(quality), tool(tool) {}
	Reagent(pugi::xml_node node) {
		// The reagents are stored in attributes... hmph
		id = node.attribute("ID").as_int();
		quantity = node.attribute("QTY").as_int();
		quality = node.attribute("QLTY").as_int();
		tool = node.attribute("TOOL").as_bool();
	}
	int id;
	int quantity;
	int quality;
	bool tool;
};

class Recipe {
public:
	Recipe();
	Recipe(pugi::xml_node node);
	std::string toString() const;

public:
	int id;
	int itemID;
	Talent talent;
	int yield;
	int requiredLevel;
	int craftingStations;
	bool requiresRecipe;
	std::string name;
	std::vector<Reagent> reagents;
	// Byproducts don't exist.
	RecipeTag recipeTag;
	double craftingTime;
	bool isRNG;
	int experienceGiven;
};

