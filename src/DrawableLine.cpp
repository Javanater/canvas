//
// Created by Madison on 10/26/2016.
//

#include <canvas/DrawableLine.hpp>

flabs::DrawableLine::DrawableLine(double x, double y, double nx, double ny,
	wxColour color, int level) :
	x(x), y(y), nx(nx), ny(ny), color(color), Drawable(level)
{
}

flabs::DrawableLine::~DrawableLine()
{
}

void flabs::DrawableLine::draw(flabs::Canvas& canvas)
{
	canvas.setColor(color).line(x, y, nx, ny);
}
