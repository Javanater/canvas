/*
 * Canvas.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: Madison
 */

#include "Canvas.h"
#include <list>
#include <math.h>
#include <iostream>
#include <sstream>
#include <Utilities/Watchdog.h>

using namespace std;

//TODO: Make this static instead of global
#define ID_SHOWGRID 0
#define ID_GOTO_HOME 1

namespace flabs
{
	//TODO: Get rid of this bandaid
	const double Canvas::MINIMUM_SCALE = 1e-5;
	const double Canvas::MAXIMUM_SCALE = 1e8;

	Canvas::Canvas(wxWindow* parent, int id) : wxPanel(parent, id,
		wxDefaultPosition, wxSize(-1, 30), wxSUNKEN_BORDER), parent(parent),
		dc(NULL), scale(.01), x(0), y(0), dragging(false), lastMouseX(0),
		lastMouseY(0), drawColor(0, 0, 0), brushStyle(wxBRUSHSTYLE_SOLID),
		backgroundColor(220, 220, 220), majorDivisionColor(190, 190, 190),
		minorDivisionColor(205, 205, 205),
		majorDivisionLabelColor(120, 120, 120), showGrid(true)
	{
		//TODO: Switch to Bind
		Connect(wxEVT_PAINT, wxPaintEventHandler(Canvas::OnPaint));
		Connect(wxEVT_SIZE, wxSizeEventHandler(Canvas::OnSize));
		Connect(wxEVT_MOTION, wxMouseEventHandler(Canvas::OnMouseMoved));
		Connect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(Canvas::OnMouseButton));
		Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(Canvas::OnRightDown));
		Connect(wxEVT_MIDDLE_UP, wxMouseEventHandler(Canvas::OnMouseButton));
		Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(Canvas::OnMouseWheel));
		Connect(wxEVT_ERASE_BACKGROUND,
			wxEraseEventHandler(Canvas::OnEraseBackground));
		Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(Canvas::OnEnter));
	}

	Canvas::~Canvas()
	{
	}

	void Canvas::OnSize(wxSizeEvent& event)
	{
		Refresh();
	}

	void Canvas::OnPaint(wxPaintEvent& event)
	{
		// TODO: Optimize bitmap creation
		wxBufferedPaintDC dc(this);
		//TODO: Should dc be atomic? Guarded?
		//TODO: Is there a way to keep dc from escaping local scope?
		this->dc = &dc;
		fill().setColor(backgroundColor);
		wxSize size = GetSize();
		dc.DrawRectangle(0, 0, size.x, size.y);
		if (showGrid)
			drawGrid();

		for (DrawableSet::iterator drawable = drawables.begin();
			drawable != drawables.end(); ++drawable)
			(*drawable)->draw(*this);

		this->dc = nullptr;
	}

	void Canvas::drawGrid()
	{
		// TODO: increase efficiency
		// TODO: Fix infinite loop (probably check if start < end)
//				Watchdog watchdog(100, [](Watchdog& watchdog){cout << __FUNCTION__ << " is unresponsive" << endl;});
		double gridScale     = (double) pow(2, floor(log2(scale)));
		double majorDivision = gridScale * 100;
		double minorDivision = gridScale * 10;
		double minX, maxX, minY, maxY;
		getBounds(minX, maxX, minY, maxY);
		double start, end;

		setColor(minorDivisionColor);
		start = ceil(minX / minorDivision) * minorDivision;
		end   = floor(maxX / minorDivision) * minorDivision;
		for (double i = start; i <= end; i += minorDivision)
			lineSegment(i, minY, i, maxY);

		start = ceil(minY / minorDivision) * minorDivision;
		end   = floor(maxY / minorDivision) * minorDivision;
		for (double i = start; i <= end; i += minorDivision)
			lineSegment(minX, i, maxX, i);

		setColor(majorDivisionColor);
		start = ceil(minX / majorDivision) * majorDivision;
		end   = floor(maxX / majorDivision) * majorDivision;
		for (double i = start; i <= end; i += majorDivision)
			lineSegment(i, minY, i, maxY);

		start = ceil(minY / majorDivision) * majorDivision;
		end   = floor(maxY / majorDivision) * majorDivision;
		for (double i = start; i <= end; i += majorDivision)
			lineSegment(minX, i, maxX, i);

		int lineHeight = dc->GetFontMetrics().height;
		dc->SetTextForeground(majorDivisionLabelColor);
		start = ceil(minX / majorDivision) * majorDivision;
		end   = floor(maxX / majorDivision) * majorDivision;
		for (double i = start; i <= end; i += majorDivision)
		{
			ostringstream stringStream;
			stringStream.precision(1);
			stringStream << scientific << i;
			string(i, minY + pixelsToUnits(lineHeight),
				stringStream.str().c_str());
			string(i, maxY, stringStream.str().c_str());
		}

		start = ceil(minY / majorDivision) * majorDivision;
		end   = floor(maxY / majorDivision) * majorDivision;
		for (double i = start; i <= end; i += majorDivision)
		{
			ostringstream stringStream;
			stringStream.precision(1);
			stringStream << scientific << i;
			string(minX, i, stringStream.str().c_str());
			string(maxX - pixelsToUnits(
				dc->GetTextExtent(stringStream.str().c_str()).x + 5), i,
				stringStream.str().c_str());
		}
	}

	void Canvas::OnMouseMoved(wxMouseEvent& event)
	{
		if (dragging)
		{
			x += pixelsToUnits(lastMouseX - event.m_x);
			y -= pixelsToUnits(lastMouseY - event.m_y);
			lastMouseX = event.m_x;
			lastMouseY = event.m_y;
			Refresh();
		}
	}

	void Canvas::OnMouseButton(wxMouseEvent& event)
	{
		if (event.m_middleDown)
		{
			lastMouseX = event.m_x;
			lastMouseY = event.m_y;
			dragging   = true;
		}
		else
			dragging = false;
	}

	void Canvas::OnRightDown(wxMouseEvent& event)
	{
		wxMenu menu;
		menu.AppendCheckItem(ID_SHOWGRID, wxT("Show Grid"))->Check(showGrid);
		menu.Append(ID_GOTO_HOME, wxT("Go to (0, 0)"));
		menu.Connect(wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(OnPopupSelected), NULL, this);
		//TODO: Add Go to coordinate
		//TODO: Add lock screen
		//TODO: Add show mouse option
		//TODO: Add Zoom Fit option
		PopupMenu(&menu, event.GetPosition());
	}

	void Canvas::OnPopupSelected(wxCommandEvent& event)
	{
		switch (event.GetId())
		{
			case ID_SHOWGRID:
				showGrid = !showGrid;
				Refresh();
				break;

			case ID_GOTO_HOME:
				x = 0;
				y = 0;
				Refresh();
				break;

			default:
				break;
		}
	}

	void Canvas::OnMouseWheel(wxMouseEvent& event)
	{
		double
			newScale = pow(2, log2(scale) - event.m_wheelRotation / 120. / 10);
		if (newScale >= MINIMUM_SCALE && newScale <= MAXIMUM_SCALE)
		{
			double oldX = pixelXToUnitX(event.m_x);
			double oldY = pixelYToUnitY(event.m_y);
			scale = newScale;
			double newX = pixelXToUnitX(event.m_x);
			double newY = pixelYToUnitY(event.m_y);
			x += oldX - newX;
			y += oldY - newY;
			Refresh();
		}
	}

	void Canvas::OnEnter(wxMouseEvent& event)
	{
		SetFocus();
	}

	void Canvas::add(Drawable* drawable)
	{
		if (drawable != NULL)
			drawables.insert(drawable);
	}

	Canvas& Canvas::draw()
	{
		brushStyle = wxBRUSHSTYLE_TRANSPARENT;
		dc->SetBrush(wxBrush(drawColor, brushStyle));
		return *this;
	}

	Canvas& Canvas::fill()
	{
		brushStyle = wxBRUSHSTYLE_SOLID;
		dc->SetBrush(wxBrush(drawColor, brushStyle));
		return *this;
	}

	Canvas& Canvas::setColor(UCHAR r, UCHAR g, UCHAR b, UCHAR alpha)
	{
		drawColor.Set(r, g, b, alpha);
		dc->SetBrush(wxBrush(drawColor, brushStyle));
		dc->SetPen(wxPen(drawColor));
		return *this;
	}

	Canvas& Canvas::setColor(wxColour color)
	{
		return setColor(color.Red(), color.Green(), color.Blue(),
			color.Alpha());
	}

	void Canvas::rectangle(double x, double y, double width, double height)
	{
		dc->DrawRectangle(unitXToPixelX(x), unitYToPixelY(y),
			unitsToPixels(width), unitsToPixels(height));
	}

	void Canvas::circle(double x, double y, double radius)
	{
		dc->DrawEllipticArc(unitXToPixelX(x - radius),
			unitYToPixelY(y + radius), unitsToPixels(radius * 2),
			unitsToPixels(radius * 2), 0, 360);
	}

	void Canvas::lineSegment(double x1, double y1, double x2, double y2)
	{
		dc->DrawLine(unitXToPixelX(x1), unitYToPixelY(y1), unitXToPixelX(x2),
			unitYToPixelY(y2));
	}

	void Canvas::line(double x, double y, double nx, double ny)
	{
		double minX, maxX, minY, maxY;
		getBounds(minX, maxX, minY, maxY);
		double t1, t2;
		double x1, y1, x2, y2;
		if (nx != 0)
		{
			t1 = (minX - x) / nx;
			t2 = (maxX - x) / nx;
		}
		else
		{
			t1 = (minY - y) / ny;
			t2 = (maxY - y) / ny;
		}
		x1 = t1 * nx + x;
		x2 = t2 * nx + x;
		y1 = t1 * ny + y;
		y2 = t2 * ny + y;
		lineSegment(x1, y1, x2, y2);
	}

	void Canvas::string(double x, double y, const char* str)
	{
		dc->DrawText(wxString::FromUTF8(str), unitXToPixelX(x),
			unitYToPixelY(y));
	}

	inline double Canvas::pixelsToUnits(int pixels)
	{
		return pixels * scale;
	}

	inline int Canvas::unitsToPixels(double units)
	{
		return (int) (units / scale + .5);
	}

	int Canvas::unitXToPixelX(double x)
	{
		return (int) ((x - this->x) / scale + .5) + GetSize().x / 2;
	}

	int Canvas::unitYToPixelY(double y)
	{
		return  GetSize().y / 2 - (int) ((y - this->y) / scale + .5);
	}

	inline double Canvas::pixelXToUnitX(int x)
	{
		return (x - GetSize().x / 2) * scale + this->x;
	}

	inline double Canvas::pixelYToUnitY(int y)
	{
		return this->y - (y - GetSize().y / 2) * scale;
	}

	void Canvas::OnEraseBackground(wxEraseEvent& event)
	{
	}

	void
	Canvas::getBounds(double& minX, double& maxX, double& minY, double& maxY)
	{
		wxSize size = GetSize();
		minX = pixelXToUnitX(0);
		maxX = pixelXToUnitX(size.x);
		minY = pixelYToUnitY(size.y);
		maxY = pixelYToUnitY(0);
	}

	void Canvas::setShowGrid(bool enable)
	{
		showGrid = enable;
	}
}
