//
// Created by Madison on 11/10/2016.
//

#include "DrawableMarker.hpp"

flabs::DrawableMarker::DrawableMarker(double x, double y,
	flabs::DrawableMarker::Type type, wxColour color, int level) :
	x(x), y(y), type(type), color(color), Drawable(level)
{
}

flabs::DrawableMarker::DrawableMarker(double x, double y, char type,
	wxColour color, int level) :
	x(x), y(y), type(typeFromChar(type)), color(color), Drawable(level)
{
}

flabs::DrawableMarker::~DrawableMarker()
{
}

void flabs::DrawableMarker::draw(flabs::Canvas& canvas)
{
	canvas.setColor(color).draw();
	double radius = canvas.pixelsToUnits(4);
	switch (type)
	{
		case STAR:
			canvas.lineSegment(x - radius, y - radius, x + radius, y + radius);
			canvas.lineSegment(x - radius, y + radius, x + radius, y - radius);

		case PLUS:
			canvas.lineSegment(x - radius, y, x + radius, y);
			canvas.lineSegment(x, y + radius, x, y - radius);
			break;

		case CIRCLE:
			canvas.circle(x, y, radius);
			break;

		case X:
			canvas.lineSegment(x - radius, y - radius, x + radius, y + radius);
			canvas.lineSegment(x - radius, y + radius, x + radius, y - radius);
			break;

		case SQUARE:
			canvas.rectangle(x - radius, y - radius, radius * 2, radius * 2);
			break;

		case BLOCK:
			canvas.fill()
				.rectangle(x - radius, y - radius, radius * 2, radius * 2);
			break;

		case DOT:
			canvas.fill().circle(x, y, radius);
			break;
	}
}

flabs::DrawableMarker::Type flabs::DrawableMarker::typeFromChar(char c)
{
	if (c == '*')
		return STAR;
	if (c == '+')
		return PLUS;
	if (c == 'o' || c == 'O')
		return CIRCLE;
	if (c == 'x' || c == 'X')
		return X;
	return DOT;
}
