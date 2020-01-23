//
// Created by Madison on 11/10/2016.
//

#ifndef PROJECTS_DRAWABLECIRCLE_HPP
#define PROJECTS_DRAWABLECIRCLE_HPP

#include "Canvas.hpp"

namespace flabs
{
class DrawableCircle : public BoundedDrawable
{
public:
	double   x, y, radius;
	wxColour color;

	DrawableCircle(double x, double y, double radius,
		wxColour color = wxColor(0, 0, 255), int level = 0);

	virtual ~DrawableCircle();

	virtual void draw(Canvas& canvas);
};
}

#endif //PROJECTS_DRAWABLECIRCLE_HPP
