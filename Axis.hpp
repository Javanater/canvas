//
// Created by Madison on 12/6/2016.
//

#ifndef PROJECTS_AXIS_HPP
#define PROJECTS_AXIS_HPP

#include <Utilities/utilities.hpp>

namespace flabs
{
class Axis
{
protected:
	double center;
	double scale;
	double minimum;
	double maximum;
	int    size;
	double panStartUnit;
	double panStartPixel;
	double panStartCenter;
	bool   invert;

public:
	Axis(bool invert = false);

	virtual ~Axis() = default;

	virtual void startPan(int pixel);

	int getSize() const;

	void setSize(int size);

	double getMinimum() const;

	double getMaximum() const;

	double pixelToUnit(int pixel) const;

	int unitToPixel(double unit) const;

	template<class T, class... Args>
	inline void minmax(const T t, const Args... args)
	{
		std::tie(minimum, maximum) = minmax(minimum, maximum, t, args...);
//		minimum = min<double, T, Args...>(minimum, t, args...);
//		maximum = max<double, T, Args...>(maximum, t, args...);
	};

	virtual void panTo(int pixel) = 0;

	virtual void zoom(int pixel, int clicks) = 0;

protected:
	//TODO: Better name
	virtual double pixelToUnitImpl(int pixel) const = 0;

	virtual int unitToPixelImpl(double unit) const = 0;
};
}

#endif //PROJECTS_AXIS_HPP
