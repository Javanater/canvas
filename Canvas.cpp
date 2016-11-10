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

using namespace std;

//TODO: Make this static instead of global
#define ID_SHOWGRID 0
#define ID_GOTO_HOME 1
#define ID_ZOOM_FIT 2
#define ID_SAVE_IMAGE 3

namespace flabs
{
	Canvas::Canvas(wxWindow* parent, int id) :
		wxPanel(parent, id, wxDefaultPosition, wxSize(-1, 30), wxSUNKEN_BORDER),
		parent(parent), dc(NULL), scale(.01), x(0), y(0), dragging(false),
		lastMouseX(0), lastMouseY(0), drawColor(0, 0, 0),
		brushStyle(wxBRUSHSTYLE_SOLID), backgroundColor(220, 220, 220),
		majorDivisionColor(190, 190, 190), minorDivisionColor(205, 205, 205),
		majorDivisionLabelColor(120, 120, 120), showGrid(true)
	{
		wxInitAllImageHandlers();
		//TODO: Switch to Bind
		Connect(wxEVT_PAINT, wxPaintEventHandler(Canvas::paintEvent));
		Connect(wxEVT_SIZE, wxSizeEventHandler(Canvas::OnSize));
		Connect(wxEVT_MOTION, wxMouseEventHandler(Canvas::OnMouseMoved));
		Connect(wxEVT_LEFT_DOWN,
			wxMouseEventHandler(Canvas::OnMouseLeftButton));
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

	void Canvas::paintEvent(wxPaintEvent& event)
	{
		// TODO: Optimize bitmap creation
		wxBufferedPaintDC dc(this);
		paint(&dc);
	}

	void Canvas::paint(wxDC* dc)
	{
		//TODO: Should dc be atomic? Guarded? Yes
		this->dc = dc;
		fill().setColor(backgroundColor);
		wxSize size = GetSize();
		dc->DrawRectangle(0, 0, size.x, size.y);
		if (showGrid)
			drawGrid();

		minX = minY = numeric_limits<double>::max();
		maxX = maxY = numeric_limits<double>::lowest();
		for (DrawableSet::iterator drawable = drawables.begin();
			drawable != drawables.end(); ++drawable)
			(*drawable)->draw(*this);

		if (message.length())
		{
			wxColour color(255, 0, 0);
			dc->SetTextForeground(color);
			dc->DrawText(message,
				size.x / 2 - dc->GetTextExtent(message.c_str()).x / 2,
				(wxCoord) (size.y * .8));
		}

		this->dc = nullptr;
	}

	void Canvas::drawGrid()
	{
		// TODO: increase efficiency
		double gridScale     = (double) pow(2, floor(log2(scale)));
		double majorDivision = gridScale * 100;
		double minorDivision = gridScale * 10;
		double minX, maxX, minY, maxY;
		getBounds(minX, maxX, minY, maxY);
		double start, end;
		int    w             = 0;

		setColor(minorDivisionColor);
		start = ceil(minX / minorDivision) * minorDivision;
		end   = floor(maxX / minorDivision) * minorDivision;
		for (double i = start; i <= end && w < 1000; i += minorDivision, ++w)
			lineSegment(i, minY, i, maxY);

		start = ceil(minY / minorDivision) * minorDivision;
		end   = floor(maxY / minorDivision) * minorDivision;
		for (double i = start; i <= end && w < 1000; i += minorDivision, ++w)
			lineSegment(minX, i, maxX, i);

		setColor(majorDivisionColor);
		start = ceil(minX / majorDivision) * majorDivision;
		end   = floor(maxX / majorDivision) * majorDivision;
		for (double i = start; i <= end && w < 1000; i += majorDivision, ++w)
			lineSegment(i, minY, i, maxY);

		start = ceil(minY / majorDivision) * majorDivision;
		end   = floor(maxY / majorDivision) * majorDivision;
		for (double i = start; i <= end && w < 1000; i += majorDivision, ++w)
			lineSegment(minX, i, maxX, i);

		int lineHeight = dc->GetFontMetrics().height;
		dc->SetTextForeground(majorDivisionLabelColor);
		start = ceil(minX / majorDivision) * majorDivision;
		end   = floor(maxX / majorDivision) * majorDivision;
		for (double i = start; i <= end && w < 1000; i += majorDivision, ++w)
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
		for (double i = start; i <= end && w < 1000; i += majorDivision, ++w)
		{
			ostringstream stringStream;
			stringStream.precision(1);
			stringStream << scientific << i;
			string(minX, i, stringStream.str().c_str());
			string(maxX - pixelsToUnits(
				dc->GetTextExtent(stringStream.str().c_str()).x + 5), i,
				stringStream.str().c_str());
		}

		if (w >= 1000)
		{
			x     = 0;
			y     = 0;
			scale = 100;
			briefMessage("Zoom exceeded 64 bit precision limits");
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

	void Canvas::OnMouseLeftButton(wxMouseEvent& event)
	{
		if (event.m_leftDown)
		{
			MouseEvent mouseEvent;
			mouseEvent.pixelX = event.m_x;
			mouseEvent.pixelY = event.m_y;
			mouseEvent.x      = pixelXToUnitX(event.m_x);
			mouseEvent.y      = pixelYToUnitY(event.m_y);
			mouseEvent.type   = DOWN;
			mouseLeftDownNotifier.notify(mouseEvent);
		}
	}

	void Canvas::OnRightDown(wxMouseEvent& event)
	{
		wxMenu menu;
		menu.AppendCheckItem(ID_SHOWGRID, wxT("Show Grid"))->Check(showGrid);
		menu.Append(ID_GOTO_HOME, wxT("Go to (0, 0)"));
		menu.Append(ID_ZOOM_FIT, wxT("Zoom Fit"));
		menu.Append(ID_SAVE_IMAGE, wxT("Save Image"));
		menu.Connect(wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(OnPopupSelected), NULL, this);
		//TODO: Add Go to coordinate
		//TODO: Add lock screen
		//TODO: Add show mouse option
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

			case ID_ZOOM_FIT:
				if (minX < maxX && minY < maxY)
				{
					x = (maxX + minX) / 2;
					y = (maxY + minY) / 2;
					wxSize size = GetSize();
					scale =
						max((maxX - minX) / size.x, (maxY - minY) / size.y) *
							1.2;
					Refresh();
				}
				break;

			case ID_SAVE_IMAGE:
				saveImage();
				break;

			default:
				break;
		}
	}

	void Canvas::OnMouseWheel(wxMouseEvent& event)
	{
		double oldX = pixelXToUnitX(event.m_x);
		double oldY = pixelYToUnitY(event.m_y);
		scale =
			(double) pow(2, log2(scale) - event.GetWheelRotation() / 120. / 10);
		double newX = pixelXToUnitX(event.m_x);
		double newY = pixelYToUnitY(event.m_y);
		x += oldX - newX;
		y += oldY - newY;
		Refresh();
	}

	void Canvas::OnEnter(wxMouseEvent& event)
	{
		SetFocus();
	}

	void Canvas::add(Drawable* drawable)
	{
		if (drawable != NULL)
		{
			drawables.insert(drawable);
			Refresh();
		}
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
		minX = min(x, minX);
		minY = min(y, minY);
		maxX = max(x + width, maxX);
		maxY = max(y + height, maxY);
		dc->DrawRectangle(unitXToPixelX(x), unitYToPixelY(y),
			unitsToPixels(width), unitsToPixels(height));
	}

	void Canvas::circle(double x, double y, double radius)
	{
		minX = min(x - radius, minX);
		minY = min(y - radius, minY);
		maxX = max(x + radius, maxX);
		maxY = max(y + radius, maxY);
		dc->DrawEllipticArc(unitXToPixelX(x - radius),
			unitYToPixelY(y + radius), unitsToPixels(radius * 2),
			unitsToPixels(radius * 2), 0, 360);
	}

	void Canvas::lineSegment(double x1, double y1, double x2, double y2)
	{
		minX = min(x2, min(x1, minX));
		minY = min(y2, min(y1, minY));
		maxX = max(x2, max(x1, maxX));
		maxY = max(y2, max(y1, maxY));
		dc->DrawLine(unitXToPixelX(x1), unitYToPixelY(y1), unitXToPixelX(x2),
			unitYToPixelY(y2));
	}

	void Canvas::line(double x, double y, double nx, double ny)
	{
		//TODO: Implement minX, minY, maxX, maxY calculation
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
		dc->DrawLine(unitXToPixelX(x1), unitYToPixelY(y1), unitXToPixelX(x2),
			unitYToPixelY(y2));
	}

	void Canvas::string(double x, double y, const char* str)
	{
		wxSize textSize = dc->GetTextExtent(str);
		minX = min(x, minX);
		minY = min(y, minY);
		maxX = max(x + pixelsToUnits(textSize.x), maxX);
		maxY = max(y + pixelsToUnits(textSize.y), maxY);
		dc->DrawText(wxString::FromUTF8(str), unitXToPixelX(x),
			unitYToPixelY(y));
	}

	int Canvas::unitXToPixelX(double x)
	{
		return (int) ((x - this->x) / scale + .5) + GetSize().x / 2;
	}

	int Canvas::unitYToPixelY(double y)
	{
		return GetSize().y / 2 - (int) ((y - this->y) / scale + .5);
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

	void Canvas::addMouseLeftDownNotifier(std::function<void(MouseEvent)> f)
	{
		mouseLeftDownNotifier.addListener(f);
	}

	void Canvas::briefMessage(std::string message)
	{
		this->message = message;
		Refresh();
		messageWatchdog = make_unique<Watchdog<>>([this]()
		{
			this->message = "";
			Refresh();
		}, 3000);
	}

	void Canvas::saveImage()
	{
		wxFileDialog dlg(this, wxT("Choose file..."), wxEmptyString,
			wxEmptyString, wxString(wxT("PNG files (*.png)|*.png")),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

		if (dlg.ShowModal() != wxID_OK)
			return;

		wxBitmap bitmap;
		bitmap.Create(GetSize().x, GetSize().y);
		wxMemoryDC mdc;
		mdc.SelectObject(bitmap);
		paint(&mdc);
		bitmap.ConvertToImage().SaveFile(dlg.GetPath(), wxBITMAP_TYPE_PNG);
	}
}
