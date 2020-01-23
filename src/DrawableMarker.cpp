//
// Created by Madison on 11/10/2016.
//

#include <canvas/DrawableMarker.hpp>
#include <limits>

using namespace std;

flabs::DrawableMarker::DrawableMarker(double x, double y,
	flabs::DrawableMarker::Type type, wxColour color, int level) :
	x(x), y(y), type(type), color(color), BoundedDrawable(level)
{
	updateBounds();
}

flabs::DrawableMarker::DrawableMarker(double x, double y, char type,
	wxColour color, int level) :
	x(x), y(y), type(typeFromChar(type)), color(color), BoundedDrawable(level)
{
	updateBounds();
}

flabs::DrawableMarker::~DrawableMarker()
{
}

void flabs::DrawableMarker::draw(flabs::Canvas& canvas)
{
	canvas.setColor(color).draw();
	switch (type)
	{
		case STAR:
			canvas.lineSegmentPixel(x, y, -4, 4, -4, 4);
			canvas.lineSegmentPixel(x, y, 4, -4, -4, 4);

		case PLUS:
			canvas.lineSegmentPixel(x, y, 0, 0, -4, 4);
			canvas.lineSegmentPixel(x, y, -4, 4, 0, 0);
			break;

		case CIRCLE:
			canvas.ellipsePixel(x, y, 4, 4);
			break;

		case X:
			canvas.lineSegmentPixel(x, y, -4, 4, -4, 4);
			canvas.lineSegmentPixel(x, y, 4, -4, -4, 4);
			break;

		case SQUARE:
			canvas.rectanglePixel(x, y, -4, -4, 4, 4);
			break;

		case BLOCK:
			canvas.fill().rectanglePixel(x, y, -4, -4, 4, 4);
			break;

		case DOT:
			canvas.fill().ellipsePixel(x, y, 4, 4);
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

void flabs::DrawableMarker::updateBounds()
{
	minX = x - numeric_limits<double>::epsilon();
	maxX = x + numeric_limits<double>::epsilon();
	minY = y - numeric_limits<double>::epsilon();
	maxY = y + numeric_limits<double>::epsilon();
}
