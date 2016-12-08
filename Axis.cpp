//
// Created by Madison on 12/6/2016.
//

#include "Axis.hpp"

namespace flabs
{
Axis::Axis(bool invert) : invert(invert)
{
}

void Axis::startPan(int pixel)
{
	panStartPixel = pixel;
	panStartUnit  = pixelToUnit(pixel);
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
}