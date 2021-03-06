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
#include "Drawable.hpp"
#include "BoundedDrawable.hpp"
#include "DrawableTree.hpp"
#include "Axis.hpp"
#include <utilities/utilities.hpp>
#include <utilities/Notifier.hpp>
#include <utilities/Watchdog.hpp>
#include <wx/dcbuffer.h>
#include <iterator>
#include <iostream>

namespace flabs
{

using UCHAR = unsigned char;

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
	Axis* xAxis;
	Axis* yAxis;
	//TODO: Move to tree
	double                      minBoundedX, minBoundedY,
								maxBoundedX, maxBoundedY;
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
	bool                                     showGridLabels;
	double                                   labelX, labelY;
	std::string                              label;
	std::list<BoundedDrawable*> boundedDrawables;

public:
	Canvas(wxWindow* parent, int id = wxID_ANY);

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

	Axis* getXAxis() const;

	Axis* getYAxis() const;

	void drawGrid();

	void rectangle(double x, double y, double width, double height);

	void rectanglePixel(double x, double y, int ndx, int pdx, int ndy, int pdy);

	void circle(double x, double y, double radius);

	void ellipse(double x, double y, double xRadius, double yRadius);

	void ellipsePixel(double x, double y, int xRadius, int yRadius);

	void lineSegment(double x1, double y1, double x2, double y2);

	void
	lineSegmentPixel(double x, double y, int ndx, int pdx, int ndy, int pdy);

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

	const wxColour& getBackgroundColor() const;

	const wxColour& getMajorDivisionColor() const;

	const wxColour& getMinorDivisionColor() const;

	const wxColour& getMajorDivisionLabelColor() const;

	void setBackgroundColor(const wxColour& backgroundColor);

	void setMajorDivisionColor(const wxColour& majorDivisionColor);

	void setMinorDivisionColor(const wxColour& minorDivisionColor);

	void setMajorDivisionLabelColor(const wxColour& majorDivisionLabelColor);

	bool isShowGridLabels() const;

	void setShowGridLabels(bool showGridLabels);

	Drawable* getClosest(double x, double y);

	Drawable* getClosestPixel(int x, int y);

	/**
	 * Places the label at the specified point. Overrides previous label.
	 * @param x
	 * @param y
	 * @param label
	 */
	void setLabel(double x, double y, std::string label);

private:
	inline void
	updateMinMax(const double& minX, const double& minY, const double& maxX,
		const double& maxY);

	void notifyBoundsChanged();
};
}

#endif
