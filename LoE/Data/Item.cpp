#include "Item.h"

Item::Item(pugi::xml_node item) {
	pugi::xml_node nameNode = item.child("Name");
	this->name = nameNode.text().as_string();

	pugi::xml_node descNode = item.child("Description");
	this->description = descNode.text().as_string();

	pugi::xml_node idNode = item.child("ID");
	this->ID = idNode.text().as_int();

	pugi::xml_node iconNode = item.child("Icon");
	this->icon = iconNode.text().as_int();
}
