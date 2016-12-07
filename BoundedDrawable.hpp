//
// Created by Madison on 11/28/2016.
//

#ifndef PROJECTS_BOUNDEDDRAWABLE_HPP
#define PROJECTS_BOUNDEDDRAWABLE_HPP

#include "Drawable.hpp"

namespace flabs
{
class BoundedDrawable : public Drawable
{
public:
	double minX;
	double minY;
	double maxX;
	double maxY;

	BoundedDrawable(int level = 0);
};
}

#endif //PROJECTS_BOUNDEDDRAWABLE_HPP
