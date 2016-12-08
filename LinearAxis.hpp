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

	virtual void zoom(int pixel, int clicks);

	virtual void home();

	virtual void zoomFit();
};
}

#endif //PROJECTS_LINEARAXIS_HPP
