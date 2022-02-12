#include "AtomKDTree.h"

#include <array>
#include <limits>

AtomKDTree::AtomKDTree(const std::vector<Atom>& atoms)
{
	constexpr float minFloat = std::numeric_limits<float>::min();
	constexpr float maxFloat = std::numeric_limits<float>::max();
	for (int i = 0; i < 3; ++i)
	{
		m_BoxMin[i] = maxFloat;
		m_BoxMax[i] = minFloat;
	}

	for (const Atom& atom : atoms)
	{
		float radius = atom.atomTemplate->radius;
		for (int i = 0; i < 3; ++i)
		{
			const float currMin = atom.position[i] - radius;
			const float currMax = atom.position[i] + radius;

			if (currMin < m_BoxMin[i])
			{
				m_BoxMin[i] = currMin;
			}
			if (currMax > m_BoxMax[i])
			{
				m_BoxMax[i] = currMax;
			}
		}
	}

	AAtomKDTree(std::vector<Atom>(atoms), 0);
}

AtomKDTree::AtomKDTree(std::vector<Atom>&& atoms, const glm::vec3& boxMin, const glm::vec3& boxMax, uint32_t depth)
	: m_BoxMin(boxMin), m_BoxMax(boxMax)
{
	AAtomKDTree(std::move(atoms), depth);
}

void AtomKDTree::AAtomKDTree(std::vector<Atom>&& atoms, uint32_t depth)
{
	constexpr uint32_t AXIS_COUNT = 3;
	uint32_t axis = depth % AXIS_COUNT;

	float totalAxisSum = 0.0f;
	for (const auto& atom : atoms)
	{
		totalAxisSum += atom.position[axis];
	}

	float half = totalAxisSum / atoms.size();
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
	}

	constexpr size_t minCount = 12;
	if (left.size() + right.size() <= minCount)
	{
		m_Atoms = std::move(left);
		m_Atoms.insert(m_Atoms.end(), std::make_move_iterator(right.begin()), std::make_move_iterator(right.end()));
	}
	else
	{
		glm::vec3 minHalfBounds = m_BoxMin;
		glm::vec3 maxHalfBounds = m_BoxMax;
		minHalfBounds[axis] = half;
		maxHalfBounds[axis] = half;

		m_LeftChild = new AtomKDTree(std::move(left), m_BoxMin, maxHalfBounds, depth + 1);
		m_RightChild = new AtomKDTree(std::move(right), minHalfBounds, m_BoxMax, depth + 1);
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
