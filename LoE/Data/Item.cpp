#include "Item.h"
#include <algorithm>

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
	this->resourcePath = readString(item, "ResourcePath");
	this->group = readString(item, "Group");
	// Read ints
	this->ID = readInt(item, "ID");
	this->icon = readInt(item, "Icon");
	this->itemLevel = readInt(item, "ItemLevel");
	this->requiredLevel = readInt(item, "RequiredLevel");
	this->maxStackSize = readInt(item, "MaxStackSize");
	this->purchasePrice = readInt(item, "PurchasePrice");
	this->sellPrice = readInt(item, "SellPrice");
	// Read bools
	this->tradeable = readBool(item, "Tradeable");
	this->salable = readBool(item, "Salable");
	this->stackable = readBool(item, "Stackable");
	// Coloring stuff
	this->color1 = readColor(item, "DefaultColor");
	this->color2 = readColor(item, "DefaultColor2");
	this->color3 = readColor(item, "DefaultColor3");
	// Read others
	this->talent = readTalent(item, "Talent");
	this->wearableSlots = readSlots(item, "WearableSlots");
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

std::string Item::toString() const {
	return std::format("{0} (ID: {1}) - {2}", this->name, this->ID, this->description);
}

bool Item::match(std::string search) const {
	auto it = std::search(
		name.begin(), name.end(),
		search.begin(), search.end(),
		[](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
	);
	return (it != name.end());
}
