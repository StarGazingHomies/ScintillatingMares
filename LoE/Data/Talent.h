#pragma once

#include <unordered_map>
#include <string>

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

	// Not implemented in game
	MUSIC,
	WRITING,
	FASHION,
	ECONOMIC,

	// Placeholder for talent not found
	INVALID
};

inline std::unordered_map<std::string, Talent> const talentFromStrTable = {
	{"Combat", Talent::COMBAT},
	{"Partying", Talent::PARTYING},
	{"Medical", Talent::MEDICAL},
	{"Mining", Talent::MINING},
	{"Artisan", Talent::ARTISAN},
	{"Cooking", Talent::COOKING},
	{"Flying", Talent::FLYING},
	{"Weather", Talent::WEATHER},
	{"Magic", Talent::MAGIC},
	{"Science", Talent::SCIENCE},
	{"Animal Care", Talent::ANIMAL_CARE},
	{"Farming", Talent::FARMING},
	{"Music", Talent::MUSIC},
	{"Writing", Talent::WRITING},
	{"Fashion", Talent::FASHION},
	{"Economic", Talent::ECONOMIC}
};

inline std::unordered_map<Talent, std::string> const talentToStrTable = {
	{Talent::COMBAT, "Combat"},
	{Talent::PARTYING, "Partying"},
	{Talent::MEDICAL, "Medical"},
	{Talent::MINING, "Mining"},
	{Talent::ARTISAN, "Artisan"},
	{Talent::COOKING, "Cooking"},
	{Talent::FLYING, "Flying"},
	{Talent::WEATHER, "Weather"},
	{Talent::MAGIC, "Magic"},
	{Talent::SCIENCE, "Science"},
	{Talent::ANIMAL_CARE, "Animal Care"},
	{Talent::FARMING, "Farming"},
	{Talent::MUSIC, "Music"},
	{Talent::WRITING, "Writing"},
	{Talent::FASHION, "Fashion"},
	{Talent::ECONOMIC, "Economic"}
};

inline Talent talentFromStr(std::string talentStr) {
	auto it = talentFromStrTable.find(talentStr);
	if (it != talentFromStrTable.end()) {
		return it->second;
	}
	return Talent::INVALID;
}

inline std::string talentToStr(Talent talentType) {
	auto it = talentToStrTable.find(talentType);
	if (it != talentToStrTable.end()) {
		return it->second;
	}
	return "Invalid Talent";
}
