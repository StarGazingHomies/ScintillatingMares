#include "Item.h"

Item::Item() {
	this->name = "INVALID ITEM";
    this->description = "INVALID ITEM";
    this->ID = 0;
    this->icon = 0;
	this->talent = Talent();
}

Item::Item(pugi::xml_node item) {
	// Read strings
	this->name = readString(item, "Name");
	this->description = readString(item, "Description");
	// Read ints
	this->ID = readInt(item, "ID");
	this->icon = readInt(item, "Icon");
	// Read talents
	this->talent = readTalent(item, "Talent");
}

TexLocation Item::getTextureLocation() const {
	printf("icon: %d\n", this->icon);
	int xCoord = this->icon % ItemAtlasWidth;
	int yCoord = this->icon / ItemAtlasWidth;
	float x1 = (float)xCoord / ItemAtlasWidth;
	float x2 = (float)(xCoord + 1) / ItemAtlasWidth;
	float y1 = (float)yCoord / ItemAtlasHeight;
	float y2 = (float)(yCoord + 1) / ItemAtlasHeight;
	return TexLocation(x1, y1, x2, y2);
}

int Item::readInt(pugi::xml_node node, const char* name) {
	pugi::xml_node intNode = node.child(name);
	if (intNode) {
		return intNode.text().as_int();
	}
	else {
		return 0;
	}
}

std::string Item::readString(pugi::xml_node node, const char* name) {
	pugi::xml_node stringNode = node.child(name);
	if (stringNode) {
		return stringNode.text().as_string();
	}
	else {
		return "";
	}
}

Talent Item::readTalent(pugi::xml_node node, const char* name) {
	pugi::xml_node talentNode = node.child(name);
	if (talentNode) {
		std::string talentStr = talentNode.text().as_string();
		return talentFromStr(talentStr);
	}
	return Talent::INVALID;
}