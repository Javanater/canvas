/*
 * Canvas.h
 *
 *  Created on: Mar 10, 2016
 *      Author: Madison
 */

#ifndef CANVAS_H_
#define CANVAS_H_

#include <wx/wx.h>
#include <set>
#include "Drawable.h"
#include "DrawableTree.hpp"
#include <Utilities/utilities.h>
#include <Utilities/Notifier.h>
#include <Utilities/Watchdog.h>
#include <wx/dcbuffer.h>
#include <iterator>
#include <iostream>

namespace flabs
{
class Canvas : public wxPanel
{
public:
	enum MouseEventType
	{
		UP, DOWN, MOVE, DRAG
	};

	struct MouseEvent
	{
		double         x;
		double         y;
		int            pixelX;
		int            pixelY;
		MouseEventType type;
	};

private:
	wxWindow* parent;
	typedef std::multiset<Drawable*, lessPointer<Drawable*>> DrawableSet;
	DrawableSet                                              drawables;
	wxDC* dc;

	/**
	 * Units per pixel.
	 */
	double                                   xScale;
	double                                   yScale;
	double                                   x;
	double                                   y;
	double                                   minX, minY, maxX, maxY;
	double                                   minBoundedX, minBoundedY,
											 maxBoundedX, maxBoundedY;
	bool                                     dragging;
	int                                      lastMouseX;
	int                                      lastMouseY;
	wxColour                                 drawColor;
	wxBrushStyle                             brushStyle;
	wxColour                                 backgroundColor;
	wxColour                                 majorDivisionColor;
	wxColour                                 minorDivisionColor;
	wxColour                                 majorDivisionLabelColor;
	bool                                     showGrid;
	Notifier<MouseEvent>                     mouseLeftDownNotifier;
	Notifier<double, double, double, double> boundsNotifier;
	std::string                              message;
	std::unique_ptr<Watchdog<>>              messageWatchdog;
	DrawableTree                             drawableTree;
	bool                                     drawnOnce;
	bool                                     zoomFitPending;

public:
	Canvas(wxWindow* parent, int id);

	virtual ~Canvas();

	void OnSize(wxSizeEvent& event);

	void paintEvent(wxPaintEvent& event);

	void OnMouseMoved(wxMouseEvent& event);

	void OnMouseButton(wxMouseEvent& event);

	void OnMouseLeftButton(wxMouseEvent& event);

	void OnMouseWheel(wxMouseEvent& event);

	void OnRightDown(wxMouseEvent& event);

	void OnEnter(wxMouseEvent& event);

	void OnPopupSelected(wxCommandEvent& event);

	void OnEraseBackground(wxEraseEvent& event);

	void add(Drawable* drawable);

	Canvas& draw();

	Canvas& fill();

	Canvas& setColor(UCHAR r, UCHAR g, UCHAR b, UCHAR alpha = 255);

	Canvas& setColor(wxColour color);

	void drawGrid();

	void rectangle(double x, double y, double width, double height);

	void circle(double x, double y, double radius);

	void ellipse(double x, double y, double xRadius, double yRadius);

	void lineSegment(double x1, double y1, double x2, double y2);

	/**
	 * Draws an infinite line.
	 *
	 * @param x - X coord of point on line
	 * @param y  - Y coord of point on line
	 * @param nx - normalized x component of direction vecotor
	 * @param ny - normalized y component of direction vecotor
	 */
	void line(double x, double y, double nx, double ny);

	void string(double x, double y, const char* str);

	void string(double x, double y, const std::string str);

	template<class Iterator>
	void polygon(Iterator start, Iterator end)
	{
		auto count = std::distance(start, end);
		if (count <= 0)
			return;
		wxPoint* array = (wxPoint*) malloc(count * sizeof(wxPoint));
		int i = 0;
		while (start != end)
		{
			array[i].x = unitXToPixelX((*start)[0]);
			array[i].y = unitYToPixelY((*start)[1]);
			++start;
			++i;
		}
		dc->DrawPolygon(count, array);
		free(array);
	}

	inline double xPixelsToUnits(int pixels)
	{
		return pixels * xScale;
	}

	inline int xUnitsToPixels(double units)
	{
		return (int) (units / xScale + .5);
	}

	inline double yPixelsToUnits(int pixels)
	{
		return pixels * yScale;
	}

	inline int yUnitsToPixels(double units)
	{
		return (int) (units / yScale + .5);
	}

	int unitXToPixelX(double x);

	int unitYToPixelY(double y);

	inline double pixelXToUnitX(int x);

	inline double pixelYToUnitY(int y);

	void getBounds(double& minX, double& maxX, double& minY, double& maxY);

	void setShowGrid(bool enable);

	void addMouseLeftDownNotifier(std::function<void(MouseEvent)> f);

	void
	addBoundsListener(std::function<void(double, double, double, double)> f);

	void briefMessage(std::string message);

	void saveImage();

	void paint(wxDC* dc);

	void setScale(double scale);

	void zoomFit();

	double getXScale() const;

	double getYScale() const;

	const wxColour& getBackgroundColor() const;

	const wxColour& getMajorDivisionColor() const;

	const wxColour& getMinorDivisionColor() const;

	const wxColour& getMajorDivisionLabelColor() const;

private:
	inline void
	updateMinMax(const double& minX, const double& minY, const double& maxX,
		const double& maxY);

	void notifyBoundsChanged();
};
}

#endif
