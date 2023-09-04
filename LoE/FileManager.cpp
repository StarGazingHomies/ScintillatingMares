#include "FileManager.h"

const char* node_types[] =
{
	"null", "document", "element", "pcdata", "cdata", "comment", "pi", "declaration"
};

FileManager::FileManager() {

	wchar_t* localLow;
	HRESULT hr = SHGetKnownFolderPath(
		FOLDERID_LocalAppDataLow,
		0,
		NULL,
		&localLow
	);

	dataPath = std::filesystem::path(localLow) / "LoE" / "Legends of Equestria";

	bool result = loadXML();
}

bool FileManager::loadXML() {

	std::filesystem::path itemsXMLPath = dataPath / "data" / "Items.xml";
	pugi::xml_parse_result result = itemsXMLDoc.load_file(itemsXMLPath.c_str());

	if (!result) {
		std::string errorStr = "Error loading Items.xml from:\n";
		errorStr += itemsXMLPath.string();
		Logger::Log(result, errorStr);
		return false;
	}

	printf("Load result: %s \n", result.description());

	pugi::xml_node itemsArrayNode = itemsXMLDoc.child("ArrayOfItem");
	//printf("itemsArrayNode: %s\n");

	for (pugi::xml_node itemNode : itemsArrayNode.children("Item"))
	{
		//printf("Item loaded: ");
		Item itemObj = Item(itemNode);
		items.push_back(itemObj);
		//std::printf("ID: %d | Icon: %d | Name: %s | Desc: %s \n",
		//	itemObj.ID, itemObj.icon, itemObj.name.c_str(), itemObj.description.c_str());
	}

	return true;
}