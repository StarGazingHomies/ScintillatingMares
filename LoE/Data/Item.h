#pragma once

#include <string>
#include "../../PugiXML/pugixml.hpp"
#include "Talent.h"

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

#define LOE_ITEMTYPE_INDESTRUCTIBLE 0x01
#define LOE_ITEMTYPE_QUEST_ITEM     0x02
#define LOE_ITEMTYPE_QUEST_REWARD   0x04

#define LOE_SLOT_HAT            0x0001
#define LOE_SLOT_MASK           0x0002
#define LOE_SLOT_EARS           0x0004
#define LOE_SLOT_MOUTH          0x0008
#define LOE_SLOT_NECKLACE       0x0010
#define LOE_SLOT_SADDLE         0x0020
#define LOE_SLOT_SHIRT          0x0040
#define LOE_SLOT_TAIL           0x0080
#define LOE_SLOT_BACKSHOES      0x0100
#define LOE_SLOT_BACKSOCKS      0x0200
#define LOE_SLOT_BACKKNEES      0x0400
#define LOE_SLOT_FRONTSHOES     0x0800
#define LOE_SLOT_FRONTSOCKS     0x1000
#define LOE_SLOT_FRONTKNEES     0x2000

struct ColorR8G8B8 {
	char r, g, b;
};

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

private:
	static int readInt(pugi::xml_node node, const char* name);
	static std::string readString(pugi::xml_node node, const char* name);
	static Talent readTalent(pugi::xml_node node, const char* name);

	// Making these public because why the FUCK will I write 100000 get methods?
	// also ugh items have so much data!

public:
	int ID;
	int icon;
	//int itemLevel;
	Talent talent;
	//int requiredLevel;
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
	//int wearableSlots; // XOR all the relevant slots
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