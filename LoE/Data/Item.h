#pragma once

#include <string>
#include <format>
#include "../../PugiXML/pugixml.hpp"
#include "Talent.h"
#include "Reader.h"

struct TexLocation {
	float x1, y1, x2, y2;
	TexLocation(float x1, float y1, float x2, float y2) {
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = x2;
		this->y2 = y2;
	}
};

class Item {
public:
	// Constants
	static const int ItemAtlasWidth = 32;
	static const int ItemAtlasHeight = 32;

	// Constructor
	Item();
	Item(pugi::xml_node item);
	TexLocation getTextureLocation() const;
	std::string toString() const;
	bool match(std::string search) const;

	// Making these public because why the FUCK will I write 100000 get methods?
	// also ugh items have so much data!

public:
	int ID;
	int icon;
	int itemLevel;
	Talent talent;
	int requiredLevel;
	std::string description;
	std::string name;

	std::string resourcePath;
	std::string group; // Unused

	int stackable;        // Nullable bool
	int maxStackSize;
	int purchasePrice;
	int sellPrice;
	int salable;          // Nullable bool
	int tradeable;        // Nullable bool

	//bool ns1_nil; // Dunno what this is for
	int wearableSlots; // XOR all the relevant slots
	int socketCount;
	//Coloring coloring;
	//ItemType itemType; // Only general
	//int Tags; // A bunch of tags bitwise OR'd together
	ColorR8G8B8 color1, color2, color3;
	//std::string statString;
	//std::string ns1_type; // String for now, handle parsing later
	//int useAnimationSkill;
	//int cooldown;
	//std::string name2;
	//int subID;
	//std::string stat;
	//std::string target;
	//int timeStamp;
	//int skillID;
};