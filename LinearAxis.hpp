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
	virtual double pixelToUnitImpl(int pixel);

	virtual int unitToPixelImpl(double unit);

	virtual void panTo(int pixel);

	virtual void zoom(int pixel, int clicks);
};
}

#endif //PROJECTS_LINEARAXIS_HPP
