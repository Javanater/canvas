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
	double xRadius = canvas.xPixelsToUnits(4);
	double yRadius = canvas.yPixelsToUnits(4);
	switch (type)
	{
		case STAR:
			canvas.lineSegment(x - xRadius, y - yRadius, x + xRadius,
				y + yRadius);
			canvas.lineSegment(x - xRadius, y + yRadius, x + xRadius,
				y - yRadius);

		case PLUS:
			canvas.lineSegment(x - xRadius, y, x + xRadius, y);
			canvas.lineSegment(x, y + yRadius, x, y - yRadius);
			break;

		case CIRCLE:
			canvas.ellipse(x, y, xRadius, yRadius);
			break;

		case X:
			canvas.lineSegment(x - xRadius, y - yRadius, x + xRadius,
				y + yRadius);
			canvas.lineSegment(x - xRadius, y + yRadius, x + xRadius,
				y - yRadius);
			break;

		case SQUARE:
			canvas
				.rectangle(x - xRadius, y - yRadius, xRadius * 2, yRadius * 2);
			break;

		case BLOCK:
			canvas.fill()
				.rectangle(x - xRadius, y - yRadius, xRadius * 2, yRadius * 2);
			break;

		case DOT:
			canvas.fill().ellipse(x, y, xRadius, yRadius);
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
