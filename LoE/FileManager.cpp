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

	for (pugi::xml_node item : itemsArrayNode.children("Item"))
	{
		printf("Item: \n");

		for (pugi::xml_node child : item.children())
		{
			printf("  %s = %s\n", child.name(), child.text().as_string());
		}

		std::cout << std::endl;
	}

	return true;
}