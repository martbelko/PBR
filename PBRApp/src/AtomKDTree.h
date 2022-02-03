#pragma once

#include "AtomLoader.h"

#include <vector>

class AtomKDTree
{
public:
	AtomKDTree(const std::vector<Atom>& atoms)
		: AtomKDTree(std::vector<Atom>(atoms), 0) { }

	~AtomKDTree();

	const AtomKDTree* GetLeftChild() const { return m_LeftChild; }
	const AtomKDTree* GetRightChild() const { return m_RightChild; }
	const glm::vec3& GetBoxMin() const { return m_BoxMin; }
	const glm::vec3& GetBoxMax() const { return m_BoxMax; }
	const std::vector<Atom>& GetAtoms() const { return m_Atoms; }
private:
	AtomKDTree(std::vector<Atom>&& atoms, uint32_t depth);
private:
	glm::vec3 m_BoxMin;
	glm::vec3 m_BoxMax;
	std::vector<Atom> m_Atoms;
	AtomKDTree* m_LeftChild = nullptr;
	AtomKDTree* m_RightChild = nullptr;
};
