//
// Created by Madison on 11/11/2016.
//

#include "DrawableReferenceFrame2d.hpp"

using namespace std;

flabs::DrawableReferenceFrame2d::DrawableReferenceFrame2d(
	flabs::ReferenceFrame<2>& referenceFrame, wxColour color, int level) :
	referenceFrame(referenceFrame), color(color), Drawable(level)
{
}

flabs::DrawableReferenceFrame2d::~DrawableReferenceFrame2d()
{
}

void flabs::DrawableReferenceFrame2d::draw(flabs::Canvas& canvas)
{
	double x, y, t;
	referenceFrame.getXYYaw(x, y, t);
	canvas.setColor(color);
	canvas.lineSegment(x, y, x + cos(t + M_PI / 2), y + sin(t + M_PI / 2));
	canvas.lineSegment(x, y, x + cos(t) / 2, y + sin(t) / 2);
}
