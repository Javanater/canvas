//
// Created by Madison on 12/7/2016.
//

#include "LinearAxis.hpp"

using namespace std;

namespace flabs
{
double LinearAxis::pixelToUnitImpl(int pixel) const
{
	//TODO: Optimize
	return (pixel - size / 2) * scale + center;
}

int LinearAxis::unitToPixelImpl(double unit) const
{
	//TODO: Optimize
	return (int) floor((unit - center) / scale + .5) + size / 2;
}

void LinearAxis::panTo(int pixel)
{
	center += panStartUnit - pixelToUnit(pixel);
	panStartUnit = pixelToUnit(pixel);
}

void LinearAxis::zoom(int pixel, int clicks)
{
	double oldUnit = pixelToUnit(pixel);
	scale = (double) pow(2, log2(scale) - clicks / 120. / 10);
	double newUnit = pixelToUnit(pixel);
	center += oldUnit - newUnit;
}

void LinearAxis::home()
{
	center = 0;
}

LinearAxis::LinearAxis(bool invert) : Axis(invert)
{
}

void LinearAxis::zoomFit()
{
	if (minimum >= maximum)
		return;

	center = (maximum + minimum) / 2;
	scale  = (maximum - minimum) / size * 1.2;
}
}