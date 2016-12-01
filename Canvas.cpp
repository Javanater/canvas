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
#define ID_SYNC_X_AND_Y_SCALE 4

namespace flabs
{
Canvas::Canvas(wxWindow* parent, int id) :
	wxPanel(parent, id, wxDefaultPosition, wxSize(-1, 30), wxSUNKEN_BORDER),
	parent(parent), dc(NULL), xScale(.01), yScale(.01), x(0), y(0),
	dragging(false), lastMouseX(0), lastMouseY(0), drawColor(0, 0, 0),
	brushStyle(wxBRUSHSTYLE_SOLID), backgroundColor(220, 220, 220),
	majorDivisionColor(190, 190, 190), minorDivisionColor(205, 205, 205),
	majorDivisionLabelColor(120, 120, 120), showGrid(true)
{
	wxInitAllImageHandlers();
	//TODO: Switch to Bind
	Connect(wxEVT_PAINT, wxPaintEventHandler(Canvas::paintEvent));
	Connect(wxEVT_SIZE, wxSizeEventHandler(Canvas::OnSize));
	Connect(wxEVT_MOTION, wxMouseEventHandler(Canvas::OnMouseMoved));
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(Canvas::OnMouseLeftButton));
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

	double minX, minY, maxX, maxY;
	getBounds(minX, maxX, minY, maxY);
	std::list<Drawable*>
		treeDrawables = drawableTree.getDrawables(minX, minY, maxX, maxY);
	for (Drawable* drawable : treeDrawables)
		drawable->draw(*this);

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
	double xGridScale     = (double) pow(2, floor(log2(xScale)));
	double xMajorDivision = xGridScale * 100;
	double xMinorDivision = xGridScale * 10;
	double yGridScale     = (double) pow(2, floor(log2(yScale)));
	double yMajorDivision = yGridScale * 100;
	double yMinorDivision = yGridScale * 10;
	double minX, maxX, minY, maxY;
	getBounds(minX, maxX, minY, maxY);
	double start, end;
	int    w              = 0;

	setColor(minorDivisionColor);
	start = ceil(minX / xMinorDivision) * xMinorDivision;
	end   = floor(maxX / xMinorDivision) * xMinorDivision;
	for (double i = start; i <= end && w < 1000; i += xMinorDivision, ++w)
		lineSegment(i, minY, i, maxY);

	start = ceil(minY / yMinorDivision) * yMinorDivision;
	end   = floor(maxY / yMinorDivision) * yMinorDivision;
	for (double i = start; i <= end && w < 1000; i += yMinorDivision, ++w)
		lineSegment(minX, i, maxX, i);

	setColor(majorDivisionColor);
	start = ceil(minX / xMajorDivision) * xMajorDivision;
	end   = floor(maxX / xMajorDivision) * xMajorDivision;
	for (double i = start; i <= end && w < 1000; i += xMajorDivision, ++w)
		lineSegment(i, minY, i, maxY);

	start = ceil(minY / yMajorDivision) * yMajorDivision;
	end   = floor(maxY / yMajorDivision) * yMajorDivision;
	for (double i = start; i <= end && w < 1000; i += yMajorDivision, ++w)
		lineSegment(minX, i, maxX, i);

	int lineHeight = dc->GetFontMetrics().height;
	dc->SetTextForeground(majorDivisionLabelColor);
	start = ceil(minX / xMajorDivision) * xMajorDivision;
	end   = floor(maxX / xMajorDivision) * xMajorDivision;
	for (double i = start; i <= end && w < 1000; i += xMajorDivision, ++w)
	{
		ostringstream stringStream;
		stringStream.precision(1);
		stringStream << scientific << i;
		string(i, minY + yPixelsToUnits(lineHeight),
			stringStream.str().c_str());
		string(i, maxY, stringStream.str().c_str());
	}

	start = ceil(minY / yMajorDivision) * yMajorDivision;
	end   = floor(maxY / yMajorDivision) * yMajorDivision;
	for (double i = start; i <= end && w < 1000; i += yMajorDivision, ++w)
	{
		ostringstream stringStream;
		stringStream.precision(1);
		stringStream << scientific << i;
		string(minX, i, stringStream.str().c_str());
		string(maxX -
				xPixelsToUnits(dc->GetTextExtent(stringStream.str().c_str()).x + 5),
			i, stringStream.str().c_str());
	}

	if (w >= 1000)
	{
		x      = y      = 0;
		xScale = yScale = .01;
		briefMessage("Zoom exceeded 64 bit precision limits");
	}
}

void Canvas::OnMouseMoved(wxMouseEvent& event)
{
	SetCursor(wxCursor(wxCURSOR_CROSS));
	if (dragging)
	{
		x += xPixelsToUnits(lastMouseX - event.m_x);
		y -= yPixelsToUnits(lastMouseY - event.m_y);
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
	menu.Append(ID_SYNC_X_AND_Y_SCALE, wxT("Sync X and Y Scale"));
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
				xScale = (maxX - minX) / size.x * 1.2;
				yScale = (maxY - minY) / size.y * 1.2;
				Refresh();
			}
			break;

		case ID_SAVE_IMAGE:
			saveImage();
			break;

		case ID_SYNC_X_AND_Y_SCALE:
			if (xScale > yScale)
				yScale = xScale;
			else
				xScale = yScale;
			Refresh();
			break;

		default:
			break;
	}
}

void Canvas::OnMouseWheel(wxMouseEvent& event)
{
	double oldX = pixelXToUnitX(event.m_x);
	double oldY = pixelYToUnitY(event.m_y);

	if (!event.ControlDown() && !event.ShiftDown())
		SetCursor(wxCursor(wxCURSOR_SIZING));
	else if (!event.ControlDown())
		SetCursor(wxCursor(wxCURSOR_SIZENS));
	else if (!event.ShiftDown())
		SetCursor(wxCursor(wxCURSOR_SIZEWE));

	if (!event.ControlDown())
		yScale = (double) pow(2,
			log2(yScale) - event.GetWheelRotation() / 120. / 10);
	if (!event.ShiftDown())
		xScale = (double) pow(2,
			log2(xScale) - event.GetWheelRotation() / 120. / 10);

	double newX = pixelXToUnitX(event.m_x);
	double newY = pixelYToUnitY(event.m_y);
	x += oldX - newX;
	y += oldY - newY;
	Refresh();
}

void Canvas::OnEnter(wxMouseEvent& event)
{
	SetFocus();
	SetCursor(wxCursor(wxCURSOR_CROSS));
}

void Canvas::add(Drawable* drawable)
{
	if (drawable)
	{
		BoundedDrawable
			* boundedDrawable = dynamic_cast<BoundedDrawable*>(drawable);
		if (boundedDrawable)
			drawableTree.insert(boundedDrawable);
		else
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
	return setColor(color.Red(), color.Green(), color.Blue(), color.Alpha());
}

void Canvas::rectangle(double x, double y, double width, double height)
{
	updateMinMax(x, y, x + width, y + height);
	dc->DrawRectangle(unitXToPixelX(x), unitYToPixelY(y), xUnitsToPixels(width),
		yUnitsToPixels(-height));
}

void Canvas::circle(double x, double y, double radius)
{
	updateMinMax(x - radius, y - radius, x + radius, y + radius);
	dc->DrawEllipticArc(unitXToPixelX(x - radius), unitYToPixelY(y + radius),
		xUnitsToPixels(radius * 2), yUnitsToPixels(radius * 2), 0, 360);
}

void Canvas::ellipse(double x, double y, double xRadius, double yRadius)
{
	updateMinMax(x - xRadius, y - yRadius, x + xRadius, y + yRadius);
	dc->DrawEllipticArc(unitXToPixelX(x - xRadius), unitYToPixelY(y + yRadius),
		xUnitsToPixels(xRadius * 2), yUnitsToPixels(yRadius * 2), 0, 360);
}

void Canvas::lineSegment(double x1, double y1, double x2, double y2)
{
	updateMinMax(min(x1, x2), min(y1, y2), max(x1, x2), max(y1, y2));
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
	updateMinMax(x, y, x + xPixelsToUnits(textSize.x),
		y + yPixelsToUnits(textSize.y));
	dc->DrawText(wxString::FromUTF8(str), unitXToPixelX(x), unitYToPixelY(y));
}

void Canvas::string(double x, double y, const std::string str)
{
	wxSize textSize = dc->GetTextExtent(str);
	updateMinMax(x, y, x + xPixelsToUnits(textSize.x),
		y + yPixelsToUnits(textSize.y));
	dc->DrawText(wxString::FromUTF8(str.c_str()), unitXToPixelX(x),
		unitYToPixelY(y));
}

int Canvas::unitXToPixelX(double x)
{
	return (int) ((x - this->x) / xScale + .5) + GetSize().x / 2;
}

int Canvas::unitYToPixelY(double y)
{
	return GetSize().y / 2 - (int) ((y - this->y) / yScale + .5);
}

inline double Canvas::pixelXToUnitX(int x)
{
	return (x - GetSize().x / 2) * xScale + this->x;
}

inline double Canvas::pixelYToUnitY(int y)
{
	return this->y - (y - GetSize().y / 2) * yScale;
}

void Canvas::OnEraseBackground(wxEraseEvent& event)
{
}

void Canvas::getBounds(double& minX, double& maxX, double& minY, double& maxY)
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
	wxFileDialog dlg(this, wxT("Choose file..."), wxEmptyString, wxEmptyString,
		wxString(wxT("PNG files (*.png)|*.png")),
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

void Canvas::setScale(double scale)
{
	xScale = yScale = scale;
}

void
Canvas::updateMinMax(const double& minX, const double& minY, const double& maxX,
	const double& maxY)
{
	this->minX = min(this->minX, minX);
	this->minY = min(this->minY, minY);
	this->maxX = max(this->maxX, maxX);
	this->maxY = max(this->maxY, maxY);
}
}
