#pragma once

#include <glm/glm.hpp>

#include <cstdint>

#include <string>
#include <vector>
#include <unordered_map>

struct Residue
{
	glm::vec3 color;
};

struct AtomTemplate
{
	glm::vec3 color;
	float radius;
};

struct Atom
{
	glm::vec3 position;
	const AtomTemplate* atomTemplate;
	Residue* residue;
	uint32_t index;
};

class AtomLoader
{
public:
	AtomLoader(const std::string& pdbPath, const std::string& xmlPath);

	const std::unordered_map<std::string, Residue>& GetResidues() const { return mResidues; }
	const std::unordered_map<char, AtomTemplate>& GetAtomTemplates() const { return mAtomTemplates; }
	const std::vector<Atom>& GetAtoms() const { return mAtoms; }
private:
	std::unordered_map<std::string, Residue> mResidues;
	std::unordered_map<char, AtomTemplate> mAtomTemplates;
	std::vector<Atom> mAtoms;
};
