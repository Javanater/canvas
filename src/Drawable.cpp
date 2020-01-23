/*
 * Drawable.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: Madison
 */

#include <canvas/Drawable.hpp>

namespace flabs
{
	Drawable::Drawable(int level) :
			level(level)
	{
	}

	Drawable::~Drawable()
	{
	}

	bool Drawable::operator <(const Drawable& other) const
	{
		return level < other.level;
	}
}
