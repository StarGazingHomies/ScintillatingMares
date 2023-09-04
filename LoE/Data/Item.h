#pragma once

#include <string>
#include "../../PugiXML/pugixml.hpp"

enum class Talent {
	COMBAT,
	PARTYING,
	MEDICAL,

	MINING,
	ARTISAN,
	COOKING,

	FLYING,
	WEATHER,

	MAGIC,
	SCIENCE,

	ANIMAL_CARE,
	FARMING,
};

enum class Coloring {
	NONE,
	SINGLE,
	DUAL,
	TRIPLE,
	METADATA
};

enum class ItemType {
	GENERAL,
};

#define INDESTRUCTIBLE 0x01
#define QUEST_ITEM     0x02
#define QUEST_REWARD   0x04

struct WearableSlots {
	bool hat, mask, ears, mouth, necklace, saddle, shirt, tail;
	bool backShoes, backSocks, backKnees, frontShoes, frontSocks, frontKnees;
};

struct ColorR8G8B8 {
	std::byte r, g, b;
};

class Item {
public:
	Item(pugi::xml_node item);

	// Making these public because why the FUCK will I write 100000 get methods?
	// also ugh items have so much data!
	int ID;
	int icon;
	int itemLevel;
	Talent talent;
	int requiredLevel;
	std::string description;
	std::string name;
	//std::string resourcePath;
	//std::string group; // Unused
	//bool stackable;
	//int maxStackSize;
	//int purchasePrice;
	//int sellPrice;
	//bool salable;
	//bool tradeable;
	//bool ns1_nil; // Dunno what this is for
	//WearableSlots wearableSlots;
	//int socketCount;
	//Coloring coloring;
	//ItemType itemType; // Only general
	//int Tags; // A bunch of tags bitwise OR'd together
	//ColorR8G8B8 color1, color2, color3;
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