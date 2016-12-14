//
// Created by Madison on 12/7/2016.
//

#ifndef PROJECTS_LINEARAXIS_HPP
#define PROJECTS_LINEARAXIS_HPP

#include "Axis.hpp"

namespace flabs
{
class LinearAxis : public Axis
{
public:
	LinearAxis(bool invert = false);

	virtual ~LinearAxis() = default;

	virtual double pixelToUnitImpl(int pixel) const;

	virtual int unitToPixelImpl(double unit) const;

	virtual void panTo(int pixel);

	virtual bool zoom(int pixel, int clicks);

	virtual void home();

	virtual void zoomFit();

	virtual int firstMinorTickMark();

	virtual int nextMinorTickMark();

	virtual int lastMinorTickMark();

	virtual int firstMajorTickMark();

	virtual int nextMajorTickMark();

	virtual int lastMajorTickMark();

	double gridScale;
	double majorDivision;
	double minorDivision;
	double start;
	double end;
	double minBound;
	double maxBound;
};
}

#endif //PROJECTS_LINEARAXIS_HPP
