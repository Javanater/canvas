//
// Created by Madison on 12/6/2016.
//

#include <canvas/Axis.hpp>

using namespace std;

namespace flabs
{
Axis::Axis(bool invert, double scale, double center) :
	invert(invert), scale(scale), center(center)
{
	resetMinmax();
}

void Axis::startPan(int pixel)
{
	panStartPixel  = pixel;
	panStartUnit   = pixelToUnit(pixel);
	panStartCenter = center;
}

int Axis::getSize() const
{
	return size;
}

void Axis::setSize(int size)
{
	Axis::size = size;
}

double Axis::getMinimum() const
{
	return minimum;
}

double Axis::getMaximum() const
{
	return maximum;
}

double Axis::pixelToUnit(int pixel) const
{
	if (invert)
		pixel = size - pixel;

	return pixelToUnitImpl(pixel);
}

int Axis::unitToPixel(double unit) const
{
	int pixel = unitToPixelImpl(unit);

	if (invert)
		pixel = size - pixel;

	return pixel;
}

void Axis::resetMinmax()
{
	minimum = numeric_limits<double>::infinity();
	maximum = -numeric_limits<double>::infinity();
}

double Axis::length(int start, int length) const
{
	return pixelToUnit(start + length) - pixelToUnit(start);
}

int Axis::pixelLength(double start, double length) const
{
	return unitToPixel(start) - unitToPixel(start + length);
}

double Axis::getScale() const
{
	return scale;
}

void Axis::setScale(double scale)
{
	Axis::scale = scale;
}

void Axis::getBounds(double& min, double& max)
{
	if (invert)
	{
		min = pixelToUnit(size);
		max = pixelToUnit(0);
	}
	else
	{
		min = pixelToUnit(0);
		max = pixelToUnit(size);
	}
}
}
