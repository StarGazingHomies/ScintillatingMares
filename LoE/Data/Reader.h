#pragma once

#include "../../PugiXML/pugixml.hpp"
#include <string>
#include "Talent.h"
#include "RecipeTag.h"

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

// Item tags
#define LOE_ITEMTYPE_INDESTRUCTIBLE 0x01
#define LOE_ITEMTYPE_QUEST_ITEM     0x02
#define LOE_ITEMTYPE_QUEST_REWARD   0x04

// Item slots
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
	unsigned char r, g, b;
};

// Crafting stations
#define LOE_CRAFTING_STATION_ALCHEMYSET         0x0001
#define LOE_CRAFTING_STATION_ASSEMBLYRUNES      0x0002
#define LOE_CRAFTING_STATION_BLENDER            0x0004
#define LOE_CRAFTING_STATION_CUTTINGBOARD       0x0008
#define LOE_CRAFTING_STATION_FORGE              0x0010
#define LOE_CRAFTING_STATION_HARMONYTABLE       0x0020
#define LOE_CRAFTING_STATION_INVENTORY          0x0040
#define LOE_CRAFTING_STATION_MIXINGBOWL         0x0080
#define LOE_CRAFTING_STATION_OVEN               0x0100
#define LOE_CRAFTING_STATION_SEWING             0x0200
#define LOE_CRAFTING_STATION_WORKBENCH          0x0400

inline int readInt(pugi::xml_node node, const char* name) {
	pugi::xml_node intNode = node.child(name);
	if (intNode) {
		return intNode.text().as_int();
	}
	else {
		return 0;
	}
}

inline double readDouble(pugi::xml_node node, const char* name) {
	pugi::xml_node doubleNode = node.child(name);
	if (doubleNode) {
		return doubleNode.text().as_double();
	}
	else {
		return 0.0;
	}
}

inline std::string readString(pugi::xml_node node, const char* name) {
	pugi::xml_node stringNode = node.child(name);
	if (stringNode) {
		return stringNode.text().as_string();
	}
	else {
		return "";
	}
}

inline int readBool(pugi::xml_node node, const char* name) {
	pugi::xml_node stringNode = node.child(name);
	if (stringNode) {
		return stringNode.text().as_bool() ? true : false;
	}
	else {
		return -1;
	}
}

inline Talent readTalent(pugi::xml_node node, const char* name) {
	pugi::xml_node talentNode = node.child(name);
	if (talentNode) {
		std::string talentStr = talentNode.text().as_string();
		return talentFromStr(talentStr);
	}
	return Talent::INVALID;
}

inline RecipeTag readRecipeTag(pugi::xml_node node, const char* name) {
	pugi::xml_node recipeTagNode = node.child(name);
	if (recipeTagNode) {
		std::string recipeTagStr = recipeTagNode.text().as_string();
		return recipeTagFromStr(recipeTagStr);
	}
	return RecipeTag::INVALID;
}

inline int readSlots(pugi::xml_node node, const char* name) {
	pugi::xml_node slotsNode = node.child(name);
	int slots = 0;
	if (slotsNode) {
		std::string slotsStr = slotsNode.text().as_string();
		// Filter out the None's
		if (slotsStr.find("None") != std::string::npos) {
			return 0;
		}

		// Beeg if statement list
		if (slotsStr.find("Hat") != std::string::npos) {
			slots |= LOE_SLOT_HAT;
		}
		if (slotsStr.find("Mask") != std::string::npos) {
			slots |= LOE_SLOT_MASK;
		}
		if (slotsStr.find("Ears") != std::string::npos) {
			slots |= LOE_SLOT_EARS;
		}
		if (slotsStr.find("Mouth") != std::string::npos) {
			slots |= LOE_SLOT_MOUTH;
		}
		if (slotsStr.find("Necklace") != std::string::npos) {
			slots |= LOE_SLOT_NECKLACE;
		}
		if (slotsStr.find("Saddle") != std::string::npos) {
			slots |= LOE_SLOT_SADDLE;
		}
		if (slotsStr.find("Shirt") != std::string::npos) {
			slots |= LOE_SLOT_SHIRT;
		}
		if (slotsStr.find("Tail") != std::string::npos) {
			slots |= LOE_SLOT_TAIL;
		}
		if (slotsStr.find("Backshoes") != std::string::npos) {
			slots |= LOE_SLOT_BACKSHOES;
		}
		if (slotsStr.find("Backsocks") != std::string::npos) {
			slots |= LOE_SLOT_BACKSOCKS;
		}
		if (slotsStr.find("Backknees") != std::string::npos) {
			slots |= LOE_SLOT_BACKKNEES;
		}
		if (slotsStr.find("Frontshoes") != std::string::npos) {
			slots |= LOE_SLOT_FRONTSHOES;
		}
		if (slotsStr.find("Frontsocks") != std::string::npos) {
			slots |= LOE_SLOT_FRONTSOCKS;
		}
		if (slotsStr.find("Frontknees") != std::string::npos) {
			slots |= LOE_SLOT_FRONTKNEES;
		}
		return slots;
	}
	return 0;
}

inline int readStations(pugi::xml_node node, const char* name) {
	pugi::xml_node stationNode = node.child(name);
	int stations = 0;
	if (stationNode) {
		std::string stationStr = stationNode.text().as_string();

		if (stationStr.find("AlchemySet") != std::string::npos) {
			stations |= LOE_CRAFTING_STATION_ALCHEMYSET;
		}
		if (stationStr.find("AssemblyRunes") != std::string::npos) {
			stations |= LOE_CRAFTING_STATION_ASSEMBLYRUNES;
		}
		if (stationStr.find("Blender") != std::string::npos) {
			stations |= LOE_CRAFTING_STATION_BLENDER;
		}
		if (stationStr.find("CuttingBoard") != std::string::npos) {
			stations |= LOE_CRAFTING_STATION_CUTTINGBOARD;
		}
		if (stationStr.find("Forge") != std::string::npos) {
			stations |= LOE_CRAFTING_STATION_FORGE;
		}
		if (stationStr.find("HarmonyTable") != std::string::npos) {
			stations |= LOE_CRAFTING_STATION_HARMONYTABLE;
		}
		if (stationStr.find("Inventory") != std::string::npos) {
			stations |= LOE_CRAFTING_STATION_INVENTORY;
		}
		if (stationStr.find("MixingBowl") != std::string::npos) {
			stations |= LOE_CRAFTING_STATION_MIXINGBOWL;
		}
		if (stationStr.find("Oven") != std::string::npos) {
			stations |= LOE_CRAFTING_STATION_OVEN;
		}
		if (stationStr.find("Sewing") != std::string::npos) {
			stations |= LOE_CRAFTING_STATION_SEWING;
		}
		if (stationStr.find("Workbench") != std::string::npos) {
			stations |= LOE_CRAFTING_STATION_WORKBENCH;
		}
	}
	return stations;
}

inline std::string stationsToStr(int stations) {
	std::string str = "";
	if (stations & LOE_CRAFTING_STATION_ALCHEMYSET) {
		str += "AlchemySet, ";
	}
	if (stations & LOE_CRAFTING_STATION_ASSEMBLYRUNES) {
		str += "AssemblyRunes, ";
	}
	if (stations & LOE_CRAFTING_STATION_BLENDER) {
		str += "Blender, ";
	}
	if (stations & LOE_CRAFTING_STATION_CUTTINGBOARD) {
		str += "CuttingBoard, ";
	}
	if (stations & LOE_CRAFTING_STATION_FORGE) {
		str += "Forge, ";
	}
	if (stations & LOE_CRAFTING_STATION_HARMONYTABLE) {
		str += "HarmonyTable, ";
	}
	if (stations & LOE_CRAFTING_STATION_INVENTORY) {
		str += "Inventory, ";
	}
	if (stations & LOE_CRAFTING_STATION_MIXINGBOWL) {
		str += "MixingBowl, ";
	}
	if (stations & LOE_CRAFTING_STATION_OVEN) {
		str += "Oven, ";
	}
	if (stations & LOE_CRAFTING_STATION_SEWING) {
		str += "Sewing, ";
	}
	if (stations & LOE_CRAFTING_STATION_WORKBENCH) {
		str += "Workbench, ";
	}
	return str.substr(0, str.size()-2);
}

inline ColorR8G8B8 readColor(pugi::xml_node node, const char* name) {
	pugi::xml_node colorNode = node.child(name);
	if (colorNode) {
		std::string colorHex = colorNode.text().as_string();
		std::string red = colorHex.substr(0, 2);
		std::string green = colorHex.substr(2, 2);
		std::string blue = colorHex.substr(4, 2);
		return ColorR8G8B8{ (unsigned char)std::stoi(red, nullptr, 16), (unsigned char)std::stoi(green, nullptr, 16), (unsigned char)std::stoi(blue, nullptr, 16) };
	}
	return ColorR8G8B8{ 0, 0, 0 };
}