#include "AtomLoader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include <limits>

struct FileMapping
{
	std::string filepath;

	std::pair<uint64_t, uint64_t> atoms;
	std::pair<uint64_t, uint64_t> atomsMapping;
	std::pair<uint64_t, uint64_t> residues;
	std::pair<uint64_t, uint64_t> residuesMapping;
	std::pair<uint64_t, uint64_t> radius;
};

static void GotoLine(std::ifstream& file, uint64_t num)
{
	file.seekg(std::ios::beg);
	for (uint64_t i = 0; i < num; ++i)
	{
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
}

static std::string FindPropertyInXMLLine(const std::string& line, const std::string& prop)
{
	uint64_t startPos = line.find(prop + '=');
	if (startPos == std::string::npos)
	{
		std::cerr << "Could not find " << prop << " in the line " << line;
		return "";
	}

	startPos += prop.length() + 2; // + 2 to account for '="'

	uint64_t endPos = line.find('"', startPos);
	if (endPos == std::string::npos)
	{
		std::cerr << "Invalid line " << line << '\n';
		return "";
	}

	return line.substr(startPos, endPos - startPos);
}

static glm::vec3 LoadColorFromXMLLine(const std::string& line)
{
	glm::vec3 color;
	const std::string rgb = "rgb";
	for (uint8_t i = 0; i < 3; ++i)
	{
		std::string channel = rgb.substr(i, 1);
		std::string valueStr = FindPropertyInXMLLine(line, channel);
		std::stringstream ss;
		ss << valueStr;
		ss >> color[i];
	}

	return color;
}

static std::unordered_map<std::string, Residue> LoadResidues(const FileMapping& fileMapping)
{
	std::ifstream file(fileMapping.filepath);
	if (!file)
	{
		return {};
	}

	std::unordered_map<uint64_t, glm::vec3> colors;
	GotoLine(file, fileMapping.residues.first);
	for (uint64_t i = fileMapping.residues.first; i < fileMapping.residues.second; ++i)
	{
		std::string line;
		std::getline(file, line);
		if (line._Starts_with("    <!--"))
		{
			continue;
		}

		std::string idStr = FindPropertyInXMLLine(line, "id");
		std::stringstream ss;
		ss << idStr;
		uint64_t id;
		ss >> id;

		glm::vec3 color = LoadColorFromXMLLine(line);
		colors[id] = color;
	}

	std::unordered_map<std::string, Residue> result;
	GotoLine(file, fileMapping.residuesMapping.first);
	for (uint64_t i = fileMapping.residuesMapping.first; i < fileMapping.residuesMapping.second; ++i)
	{
		std::string line;
		std::getline(file, line);
		if (line._Starts_with("    <!--"))
		{
			continue;
		}

		std::string idStr = FindPropertyInXMLLine(line, "id");
		std::stringstream ss;
		ss << idStr;
		uint64_t id;
		ss >> id;

		std::string identifier = FindPropertyInXMLLine(line, "identifier");
		Residue residue;
		residue.color = colors[id];
		result[identifier] = residue;
	}

	file.close();
	return result;
}

static std::unordered_map<char, AtomTemplate> LoadAtomTemplates(const FileMapping& fileMapping)
{
	std::ifstream file(fileMapping.filepath);
	if (!file)
	{
		return {};
	}

	GotoLine(file, fileMapping.atoms.first);
	std::unordered_map<uint64_t, glm::vec3> colors;
	for (uint64_t i = fileMapping.atoms.first; i < fileMapping.atoms.second; ++i)
	{
		std::string line;
		std::getline(file, line);

		std::string idStr = FindPropertyInXMLLine(line, "id");
		std::stringstream ss;
		ss << idStr;
		uint64_t id;
		ss >> id;

		glm::vec3 color = LoadColorFromXMLLine(line);
		colors[id] = color;
	}

	GotoLine(file, fileMapping.radius.first);
	std::unordered_map<uint64_t, float> radius;
	for (uint64_t i = fileMapping.radius.first; i < fileMapping.radius.second; ++i)
	{
		std::string line;
		std::getline(file, line);

		uint64_t id;
		{
			std::string idStr = FindPropertyInXMLLine(line, "id");
			std::stringstream ss;
			ss << idStr;
			ss >> id;
		}

		float r;
		{
			std::string radiusStr = FindPropertyInXMLLine(line, "radius");
			std::stringstream ss;
			ss << radiusStr;
			ss >> r;
		}

		radius[id] = r;
	}

	GotoLine(file, fileMapping.atomsMapping.first);
	std::unordered_map<char, AtomTemplate> result;
	for (uint64_t i = fileMapping.atomsMapping.first; i < fileMapping.atomsMapping.second; ++i)
	{
		std::string line;
		std::getline(file, line);

		std::string identifier = FindPropertyInXMLLine(line, "identifier");
		if (identifier.length() == 1)
		{
			char element = identifier[0];
			uint64_t number;
			{
				std::string numberStr = FindPropertyInXMLLine(line, "number");
				std::stringstream ss;
				ss << numberStr;
				ss >> number;
			}

			AtomTemplate atomTemplate;
			atomTemplate.color = colors[number];
			atomTemplate.radius = radius[number];
			result[element] = atomTemplate;
		}
	}

	file.close();
	return result;
}

static std::vector<Atom> LoadAtoms(const std::string& pdbPath, std::unordered_map<char, AtomTemplate>& atomTemplates, std::unordered_map<std::string, Residue>& residues)
{
	std::vector<Atom> atoms;
	std::ifstream file(pdbPath);
	if (!file)
	{
		std::cerr << "Could not open " << pdbPath << '\n';
		return {};
	}

	std::string line;
	while (std::getline(file, line))
	{
		if (line._Starts_with("ATOM"))
		{
			std::istringstream iss(line);
			std::string temp[2];
			std::string residueString;
			std::string atomTag;
			uint64_t atomId, residueId;
			glm::vec3 position;

			// ATOM 1 N ILE A 15 11.749 81.774 51.160 1.00 13.80 N
			iss >> temp[0] >> atomId >> atomTag >> residueString >> temp[1] >> residueId >> position[0] >> position[1] >> position[2];

			Atom atom;
			char element = atomTag[0];
			atom.atomTemplate = &atomTemplates[element];
			atom.position = position;
			atom.residue = &residues[temp[1]];
			atoms.push_back(std::move(atom));
		}
	}

	file.close();
	return atoms;
}

static FileMapping PreProcessXML(const std::string& xmlPath)
{
	FileMapping mapping;
	mapping.filepath = xmlPath;

	std::ifstream file(xmlPath);
	if (!file)
	{
		std::cerr << "Could not open " << xmlPath << '\n';
		return mapping;
	}

	std::string line;
	uint64_t lineNum = 0;

	uint64_t* end = nullptr;
	while (std::getline(file, line))
	{
		if (line._Starts_with("<scheme type="))
		{
			if (line._Starts_with("<scheme type=\"Atoms\""))
			{
				mapping.atoms.first = lineNum + 1;
				end = &mapping.atoms.second;
			}
			else if (line._Starts_with("<scheme type=\"Residues\""))
			{
				mapping.residues.first = lineNum + 1;
				end = &mapping.residues.second;
			}
			else if (line._Starts_with("<scheme type=\"ResiduesMapping\""))
			{
				mapping.residuesMapping.first = lineNum + 1;
				end = &mapping.residuesMapping.second;
			}
			else if (line._Starts_with("<scheme type=\"AtomsMapping\""))
			{
				mapping.atomsMapping.first = lineNum + 1;
				end = &mapping.atomsMapping.second;
			}
			else if (line._Starts_with("<scheme type=\"Radius\""))
			{
				mapping.radius.first = lineNum + 1;
				end = &mapping.radius.second;
			}
			else
			{
				std::cerr << "Invalid scheme type " << line << '\n';
				goto cleanup;
			}
		}
		else if (line == "</scheme>" && end)
		{
			*end = lineNum;
		}

		++lineNum;
	}

cleanup:
	file.close();
	return mapping;
}

static void ProcessAtoms(std::vector<Atom>& atoms, const FileMapping& mapping)
{
	std::ifstream file(mapping.filepath);
	if (!file)
	{
		std::cerr << "Could not open " << mapping.filepath << '\n';
		return;
	}

	std::unordered_map<std::string, uint64_t> elementToXMLIdMapping;
	{
		uint64_t curLineNum = mapping.atomsMapping.first;
		GotoLine(file, curLineNum);
		std::string line;
		for (; curLineNum < mapping.atomsMapping.second; ++curLineNum)
		{
			std::getline(file, line);

			// <atom identifier = "H" name = "Hydrogen" number = "1" electronegativity = "2.00" valenceElectrons = "1" />
			std::string identifier = FindPropertyInXMLLine(line, "identifier");

			std::stringstream ss;
			ss << FindPropertyInXMLLine(line, "number");
			uint64_t number;
			ss >> number;

			elementToXMLIdMapping[identifier] = number;
		}
	}

	std::unordered_map<uint64_t, glm::vec3> xmlIdToColorMapping;
	{
		uint64_t curLineNum = mapping.atoms.first;
		GotoLine(file, curLineNum);
		for (; curLineNum < mapping.atoms.second; ++curLineNum)
		{
			std::string line;
			std::getline(file, line);

			// <color id="1" r="0.900" g="0.900" b="0.900"/>
			std::stringstream ss;
			ss << FindPropertyInXMLLine(line, "id") << ' ';
			ss << FindPropertyInXMLLine(line, "r") << ' ';
			ss << FindPropertyInXMLLine(line, "g") << ' ';
			ss << FindPropertyInXMLLine(line, "b");

			uint64_t id;
			glm::vec3 color;
			ss >> id >> color.r >> color.g >> color.b;
			xmlIdToColorMapping[id] = color;
		}
	}

	file.close();
}

AtomLoader::AtomLoader(const std::string& pdbPath, const std::string& xmlPath)
{
	const FileMapping fileMapping = PreProcessXML(xmlPath);

	mResidues = LoadResidues(fileMapping);
	mAtomTemplates = LoadAtomTemplates(fileMapping);
	mAtoms = LoadAtoms(pdbPath, mAtomTemplates, mResidues);
}
