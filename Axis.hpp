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
	/**
	 * Units per pixel.
	 */
	double scale;
	double minimum;
	double maximum;
	int    size;
	double panStartUnit;
	double panStartPixel;
	double panStartCenter;
	bool   invert;

public:
	Axis(bool invert = false, double scale = .01, double center = 0);

	virtual ~Axis() = default;

	virtual void startPan(int pixel);

	int getSize() const;

	void setSize(int size);

	double getMinimum() const;

	double getMaximum() const;

	double pixelToUnit(int pixel) const;

	int unitToPixel(double unit) const;

	double length(int start, int length) const;

	int pixelLength(double start, double length) const;

	double getScale() const;

	void setScale(double scale);

	void minmax(const std::initializer_list<double> values)
	{
		for (const double& value : values)
		{
			minimum = std::min(minimum, value);
			maximum = std::max(maximum, value);
		}
	}

	void resetMinmax();

	virtual void panTo(int pixel) = 0;

	/**
	 * Adjusts scale acordingly.
	 * @param pixel: the pixel location of the zoom request
	 * @param clicks: the amount of the zoom
	 * @return false if the zoom exceeds precision limits, true otherwise
	 */
	virtual bool zoom(int pixel, int clicks) = 0;

	virtual void zoomFit() = 0;

	virtual void home() = 0;

protected:
	//TODO: Better name
	virtual double pixelToUnitImpl(int pixel) const = 0;

	virtual int unitToPixelImpl(double unit) const = 0;
};
}

#endif //PROJECTS_AXIS_HPP
