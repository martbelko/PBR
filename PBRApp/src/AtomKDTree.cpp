#include "AtomKDTree.h"

#include <limits>

AtomKDTree::AtomKDTree(std::vector<Atom>&& atoms, uint32_t depth)
{
	constexpr uint32_t AXIS_COUNT = 3;
	uint32_t axis = depth % AXIS_COUNT;

	constexpr float minFloat = std::numeric_limits<float>::min();
	constexpr float maxFloat = std::numeric_limits<float>::max();
	m_BoxMin[axis] = maxFloat;
	m_BoxMax[axis] = minFloat;

	for (const Atom& atom : atoms)
	{
		const glm::vec3& pos = atom.position;
		m_BoxMin[axis] = std::min(m_BoxMin[axis], pos[axis] - atom.atomTemplate->radius);
		m_BoxMax[axis] = std::max(m_BoxMax[axis], pos[axis] + atom.atomTemplate->radius);
	}

	float half = (m_BoxMin[axis] + m_BoxMax[axis]) / 2.0f;
	std::vector<Atom> left, right;
	for (const Atom& atom : atoms)
	{
		if (atom.position[axis] - atom.atomTemplate->radius <= half)
		{
			left.emplace_back(atom);
		}
		else if (atom.position[axis] + atom.atomTemplate->radius >= half)
		{
			right.emplace_back(atom);
		}
		else
		{
			left.emplace_back(atom);
			right.emplace_back(atom);
		}
	}

	constexpr size_t minCount = 10;
	if (left.size() + right.size() <= minCount)
	{
		m_Atoms = std::move(left);
		m_Atoms.insert(m_Atoms.end(), std::make_move_iterator(right.begin()), std::make_move_iterator(right.end()));
	}
	else if (left.size() <= minCount)
	{
		m_Atoms = std::move(left);
		m_RightChild = new AtomKDTree(std::move(right), depth + 1);
	}
	else if (right.size() <= minCount)
	{
		m_Atoms = std::move(right);
		m_LeftChild = new AtomKDTree(std::move(left), depth + 1);
	}
	else
	{
		m_LeftChild = new AtomKDTree(std::move(left), depth + 1);
		m_RightChild = new AtomKDTree(std::move(right), depth + 1);
	}
}

AtomKDTree::~AtomKDTree()
{
	if (m_LeftChild)
	{
		delete m_LeftChild;
	}
	if (m_RightChild)
	{
		delete m_RightChild;
	}
}
