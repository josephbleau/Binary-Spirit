///////////////////////////////////////////////////////////////////////////////
// Spatial Hash
// Joseph Bleau (8/30/2012)
// A simple Spatial hash implemention for use in video games where objects need
// to be divided into buckets for processing by their relative Spatial distances.
////////////////////

#pragma once

#include <algorithm>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// SPECIFICATION
////////////////////

template <class T>
class SpatialHash
{
private:
	float m_GridWidth;
	float m_GridHeight;
	float m_CellWidth;
	float m_CellHeight;

	std::vector<std::vector<std::vector<T* const>>> m_Grid;

public:
	void clear();

	void insert(T* const item, float x, float y, float w, float h);
	void remove(T* const item);

	std::vector<T*> getItems(float x, float y, float w, float h);

	size_t getNumItems();

	SpatialHash(float grid_width, float grid_height, int horiz_divisions, int vert_divisions);
};

///////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
////////////////////
template <class T>
void SpatialHash<T>::clear()
{
	for(size_t x = 0; x < m_Grid.size(); x++)
	{
		for(size_t y = 0; y < m_Grid[x].size(); y++)
		{
			m_Grid[x][y].clear();
		}
	}
}

template <class T>
void SpatialHash<T>::insert(T* const item, float x, float y, float w, float h)
{
	for(float lx = x; lx < x + w; lx += m_CellWidth)
	{
		for(float ly = y; ly < y + h; ly += m_CellHeight)
		{
			unsigned int ix = (unsigned int) (lx / m_CellWidth);
			unsigned int iy = (unsigned int) (ly / m_CellHeight);

			m_Grid[ix][iy].push_back(item);
		}
	}
}

template <class T>
void SpatialHash<T>::remove(T* const item)
{
	for(size_t x = 0; x < m_Grid.size(); x++)
	{
		for(size_t y = 0; y < m_Grid[x].size(); y++)
		{
			auto it = std::find(m_Grid[x][y].begin(), m_Grid[x][y].end(), item);
			if(it != m_Grid[x][y].end())
			{
				m_Grid[x][y].erase(it);
			}
		}
	}
}

template <class T>
std::vector<T*> SpatialHash<T>::getItems(float x, float y, float w, float h)
{
	// Merge buckets
	std::vector<T*> ret;

	for(float lx = x; lx < x + w; lx += m_CellHeight)
	{
		for(float ly = y; ly < y + h; ly += m_CellHeight)
		{
			unsigned int ix = (unsigned int) (lx / m_CellWidth);
			unsigned int iy = (unsigned int) (ly / m_CellHeight);

			ret.insert(ret.end(), m_Grid[ix][iy].begin(), m_Grid[ix][iy].end());
		}
	}

	// Remove duplicates
	std::sort(ret.begin(), ret.end());
	ret.erase(std::unique(ret.begin(), ret.end()), ret.end());

	return ret;
}

template <class T>
size_t SpatialHash<T>::getNumItems()
{
	size_t num = 0;
	for(size_t x = 0; x < m_Grid.size(); x++)
	{
		for(size_t y = 0; y < m_Grid[x].size(); y++)
		{
			num += m_Grid[x][y].size();
		}
	}

	return num;
}

template <class T>
SpatialHash<T>::SpatialHash(float grid_width, float grid_height, int horiz_divisions, int vert_divisions)
{
	m_GridWidth = grid_width;
	m_GridHeight = grid_height;
	m_CellWidth = m_GridWidth / horiz_divisions;
	m_CellHeight = m_GridHeight / vert_divisions;

	for(int x = 0; x < horiz_divisions; x++)
	{
		m_Grid.push_back(std::vector<std::vector<T* const>>());

		for(int y = 0; y < vert_divisions; y++)
		{
			m_Grid[x].push_back(std::vector<T* const>());
		}
	}
}