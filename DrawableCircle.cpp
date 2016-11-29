//
// Created by Madison on 11/10/2016.
//

#include "DrawableCircle.hpp"

flabs::DrawableCircle::DrawableCircle(double x, double y, double radius,
	wxColour color, int level) :
	x(x), y(y), radius(radius), color(color), BoundedDrawable(level)
{
	minX = x - radius;
	maxX = x + radius;
	minY = y - radius;
	maxY = y + radius;
}

flabs::DrawableCircle::~DrawableCircle()
{
}

void flabs::DrawableCircle::draw(flabs::Canvas& canvas)
{
	canvas.setColor(color).draw().circle(x, y, radius);
}