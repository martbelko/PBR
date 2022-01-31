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
};

class AtomLoader
{
public:
	AtomLoader(const std::string& pdbPath, const std::string& xmlPath);

	std::unordered_map<std::string, Residue> GetResidues() { return mResidues; }
	std::unordered_map<char, AtomTemplate> GetAtomTemplates() { return mAtomTemplates; }
	std::vector<Atom> GetAtoms() { return mAtoms; }
private:
	std::unordered_map<std::string, Residue> mResidues;
	std::unordered_map<char, AtomTemplate> mAtomTemplates;
	std::vector<Atom> mAtoms;
};
