//
// Created by Madison on 12/6/2016.
//

#ifndef PROJECTS_AXIS_HPP
#define PROJECTS_AXIS_HPP

#include <Utilities/Notifier.hpp>

namespace flabs
{
class Axis : public Notifier<Axis&>
{
private:
	double center;
	double scale;

public:
	double pixelToUnit(int pixel);

	int uintToPixel(double unit);

	void pan(int pixels);

	void zoom(int clicks);
};
}

#endif //PROJECTS_AXIS_HPP
