#include "FileManager.h"

const char* node_types[] = {
	"null", "document", "element", "pcdata", "cdata", "comment", "pi", "declaration"
};

FileManager::FileManager() {
}

bool FileManager::Initialize() {
	wchar_t* localLow;
	HRESULT hr = SHGetKnownFolderPath(
		FOLDERID_LocalAppDataLow,
		0,
		NULL,
		&localLow
	);

	dataPath = std::filesystem::path(localLow) / "LoE" / "Legends of Equestria";

	if (!std::filesystem::exists(dataPath)) {
		Logger::Log("Data folder not found, have you installed Legends of Equestria?");
		return false;
	}

	if (!loadItems()) return false;

	return true;
}

bool FileManager::loadItems() {

	std::filesystem::path itemsXMLPath = dataPath / "data" / "Items.xml";
	pugi::xml_parse_result result = itemsXMLDoc.load_file(itemsXMLPath.c_str());

	if (!result) {
		std::string errorStr = "Error loading Items.xml from:\n";
		errorStr += itemsXMLPath.string();
		Logger::Log(result, errorStr);
		return false;
	}

	pugi::xml_node itemsArrayNode = itemsXMLDoc.child("ArrayOfItem");

	for (pugi::xml_node itemNode : itemsArrayNode.children("Item")) {
		//printf("Item loaded: ");
		Item itemObj = Item(itemNode);
		items.insert({itemObj.ID, itemObj});
		std::printf("ID: %d | Icon: %d | Talent: %s | Name: %s | Desc: %s\n",
			itemObj.ID, itemObj.icon, talentToStr(itemObj.talent).c_str(), itemObj.name.c_str(), itemObj.description.c_str());
	}

	return true;
}

Item FileManager::findItem(std::string name) {
	for (std::pair<int, Item> item : items) {
		if (item.second.name == name) {
			return item.second;
		}
	}
	return Item();
}

TexLocation FileManager::getTextureLocation(int id) {
	auto item = items.find(id);
	if (item != items.end()) {
		return item->second.getTextureLocation();
	}
	return TexLocation(0.0f, 0.0f, 0.0f, 0.0f);
}
