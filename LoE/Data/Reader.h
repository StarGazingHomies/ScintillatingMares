#pragma once

#include "../../PugiXML/pugixml.hpp"
#include <string>

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
	unsigned char r, g, b;
};

inline int readInt(pugi::xml_node node, const char* name) {
	pugi::xml_node intNode = node.child(name);
	if (intNode) {
		return intNode.text().as_int();
	}
	else {
		return 0;
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