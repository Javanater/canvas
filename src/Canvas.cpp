/*
 * Canvas.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: Madison
 */

#include <canvas/Canvas.hpp>
#include <list>
#include <math.h>
#include <iostream>
#include <sstream>
#include <math/Math.hpp>
#include <canvas/LinearAxis.hpp>

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
	wxPanel(parent, id), parent(parent), dc(nullptr), drawColor(0, 0, 0),
	brushStyle(wxBRUSHSTYLE_SOLID), backgroundColor(220, 220, 220),
	majorDivisionColor(190, 190, 190), minorDivisionColor(205, 205, 205),
	majorDivisionLabelColor(120, 120, 120), showGrid(true),
	zoomFitPending(false), drawnOnce(false), showGridLabels(true),
	xAxis(new LinearAxis()), yAxis(new LinearAxis(true))
{
	minBoundedX = minBoundedY = numeric_limits<double>::infinity();
	maxBoundedX = maxBoundedY = -numeric_limits<double>::infinity();
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	wxInitAllImageHandlers();
	Bind(wxEVT_PAINT, &Canvas::paintEvent, this);
	Bind(wxEVT_SIZE, &Canvas::OnSize, this);
	Bind(wxEVT_MOTION, &Canvas::OnMouseMoved, this);
	Bind(wxEVT_LEFT_DOWN, &Canvas::OnMouseLeftButton, this);
	Bind(wxEVT_MIDDLE_DOWN, &Canvas::OnMouseButton, this);
	Bind(wxEVT_RIGHT_DOWN, &Canvas::OnRightDown, this);
	Bind(wxEVT_MIDDLE_UP, &Canvas::OnMouseButton, this);
	Bind(wxEVT_MOUSEWHEEL, &Canvas::OnMouseWheel, this);
	Bind(wxEVT_ERASE_BACKGROUND, &Canvas::OnEraseBackground, this);
	Bind(wxEVT_ENTER_WINDOW, &Canvas::OnEnter, this);
}

Canvas::~Canvas()
{
}

void Canvas::OnSize(wxSizeEvent& event)
{
	xAxis->setSize(event.m_size.x);
	yAxis->setSize(event.m_size.y);
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
	wxSize size = GetSize();

	//TODO: Simplify
	while (true)
	{
		fill().setColor(backgroundColor);
		dc->DrawRectangle(0, 0, size.x, size.y);
		if (showGrid)
			drawGrid();

		xAxis->resetMinmax();
		yAxis->resetMinmax();
		xAxis->minmax({minBoundedX, maxBoundedX});
		yAxis->minmax({minBoundedY, maxBoundedY});
		for (DrawableSet::iterator drawable = drawables.begin();
			drawable != drawables.end(); ++drawable)
			(*drawable)->draw(*this);

		drawnOnce = true;
		if (!zoomFitPending)
			break;

		zoomFit();
	}

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

	if (label.length())
	{
		dc->SetTextForeground(majorDivisionLabelColor);
		dc->DrawText(label, unitXToPixelX(labelX), unitYToPixelY(labelY));
	}

	this->dc = nullptr;
}

void Canvas::drawGrid()
{
	setColor(minorDivisionColor);
	for (int i = xAxis->firstMinorTickMark(); i <= xAxis->lastMinorTickMark();
		i = xAxis->nextMinorTickMark())
		dc->DrawLine(i, 0, i, GetSize().y);

	for (int i = yAxis->firstMinorTickMark(); i >= yAxis->lastMinorTickMark();
		i = yAxis->nextMinorTickMark())
		dc->DrawLine(0, i, GetSize().x, i);

	setColor(majorDivisionColor);
	for (int i = xAxis->firstMajorTickMark(); i <= xAxis->lastMajorTickMark();
		i = xAxis->nextMajorTickMark())
		dc->DrawLine(i, 0, i, GetSize().y);

	for (int i = yAxis->firstMajorTickMark(); i >= yAxis->lastMajorTickMark();
		i = yAxis->nextMajorTickMark())
		dc->DrawLine(0, i, GetSize().x, i);

	if (showGridLabels)
	{
//		int lineHeight = dc->GetFontMetrics().height;
//		dc->SetTextForeground(majorDivisionLabelColor);
//		start = ceil(minX / xMajorDivision) * xMajorDivision;
//		end   = floor(maxX / xMajorDivision) * xMajorDivision;
//		for (double i = start; i <= end; i += xMajorDivision)
//		{
//			ostringstream stringStream;
//			stringStream.precision(1);
//			stringStream << scientific << i;
//			string(i, minY + yPixelsToUnits(lineHeight),
//				stringStream.str().c_str());
//			string(i, maxY, stringStream.str().c_str());
//		}

//		start = ceil(minY / yMajorDivision) * yMajorDivision;
//		end   = floor(maxY / yMajorDivision) * yMajorDivision;
//		for (double i = start; i <= end; i += yMajorDivision)
//		{
//			ostringstream stringStream;
//			stringStream.precision(1);
//			stringStream << scientific << i;
//			string(minX, i, stringStream.str().c_str());
//			string(maxX - xPixelsToUnits(
//				dc->GetTextExtent(stringStream.str().c_str()).x + 5), i,
//				stringStream.str().c_str());
//		}
	}
}

void Canvas::OnMouseMoved(wxMouseEvent& event)
{
	if (event.MiddleIsDown() && event.Dragging())
	{
		SetCursor(wxCursor(wxCURSOR_CROSS));
		xAxis->panTo(event.m_x);
		yAxis->panTo(event.m_y);
		notifyBoundsChanged();
		Refresh();
	}
}

void Canvas::OnMouseButton(wxMouseEvent& event)
{
	if (event.m_middleDown)
	{
		xAxis->startPan(event.m_x);
		yAxis->startPan(event.m_y);
	}
}

void Canvas::OnMouseLeftButton(wxMouseEvent& event)
{
	if (event.m_leftDown)
	{
		MouseEvent mouseEvent;
		mouseEvent.pixelY = event.m_y;
		mouseEvent.pixelX = event.m_x;
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
	menu.Append(ID_GOTO_HOME, wxT("Go Home"));
	menu.Append(ID_ZOOM_FIT, wxT("Zoom Fit"));
	menu.Append(ID_SYNC_X_AND_Y_SCALE, wxT("Sync X and Y Scale"));
	menu.Append(ID_SAVE_IMAGE, wxT("Save Image"));
	menu.Bind(wxEVT_COMMAND_MENU_SELECTED, &Canvas::OnPopupSelected, this);
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
			xAxis->home();
			yAxis->home();
			Refresh();
			break;

		case ID_ZOOM_FIT:
			zoomFit();
			break;

		case ID_SAVE_IMAGE:
			saveImage();
			break;

		case ID_SYNC_X_AND_Y_SCALE:
			if (xAxis->getScale() > yAxis->getScale())
				yAxis->setScale(xAxis->getScale());
			else
				xAxis->setScale(yAxis->getScale());
			Refresh();
			break;

		default:
			break;
	}
}

void Canvas::OnMouseWheel(wxMouseEvent& event)
{
	if (!event.ControlDown() && !event.ShiftDown())
		SetCursor(wxCursor(wxCURSOR_SIZING));
	else if (!event.ControlDown())
		SetCursor(wxCursor(wxCURSOR_SIZENS));
	else if (!event.ShiftDown())
		SetCursor(wxCursor(wxCURSOR_SIZEWE));

	if (!event.ControlDown())
		if (!yAxis->zoom(event.m_y, event.GetWheelRotation()))
			briefMessage("Zoom exceeds 64-bit precision");

	if (!event.ShiftDown())
		if (!xAxis->zoom(event.m_x, event.GetWheelRotation()))
			briefMessage("Zoom exceeds 64-bit precision");

	notifyBoundsChanged();
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
		{
			drawableTree.insert(boundedDrawable);
			boundedDrawables.push_back(boundedDrawable);
			this->minBoundedX = min(this->minBoundedX, boundedDrawable->minX);
			this->minBoundedY = min(this->minBoundedY, boundedDrawable->minY);
			this->maxBoundedX = max(this->maxBoundedX, boundedDrawable->maxX);
			this->maxBoundedY = max(this->maxBoundedY, boundedDrawable->maxY);
		}
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
	dc->DrawRectangle(unitXToPixelX(x), unitYToPixelY(y),
		xAxis->pixelLength(x, width), yAxis->pixelLength(y, height));
}

void
Canvas::rectanglePixel(double x, double y, int ndx, int pdx, int ndy, int pdy)
{
	updateMinMax(x, y, x, y);
	dc->DrawRectangle(unitXToPixelX(x) + ndx, unitYToPixelY(y) + ndy, pdx - ndx,
		ndy - pdy);
}

void Canvas::circle(double x, double y, double radius)
{
	updateMinMax(x - radius, y - radius, x + radius, y + radius);
	dc->DrawEllipticArc(unitXToPixelX(x + radius), unitYToPixelY(y + radius),
		xAxis->pixelLength(x + radius, radius * 2),
		yAxis->pixelLength(y - radius, radius * 2), 0, 360);
}

void Canvas::ellipse(double x, double y, double xRadius, double yRadius)
{
	updateMinMax(x - xRadius, y - yRadius, x + xRadius, y + yRadius);
	dc->DrawEllipticArc(unitXToPixelX(x - xRadius), unitYToPixelY(y + yRadius),
		xAxis->pixelLength(x - xRadius, xRadius * 2),
		yAxis->pixelLength(y - yRadius, yRadius * 2), 0, 360);
}

void Canvas::ellipsePixel(double x, double y, int xRadius, int yRadius)
{
	updateMinMax(x, y, x, y);
	dc->DrawEllipticArc(unitXToPixelX(x) - xRadius, unitYToPixelY(y) + yRadius,
		xRadius * 2, yRadius * 2, 0, 360);
}

void Canvas::lineSegment(double x1, double y1, double x2, double y2)
{
	updateMinMax(min(x1, x2), min(y1, y2), max(x1, x2), max(y1, y2));
	dc->DrawLine(unitXToPixelX(x1), unitYToPixelY(y1), unitXToPixelX(x2),
		unitYToPixelY(y2));
}

void
Canvas::lineSegmentPixel(double x, double y, int ndx, int pdx, int ndy, int pdy)
{
	updateMinMax(x, y, x, y);
	dc->DrawLine(unitXToPixelX(x) + ndx, unitYToPixelY(y) + ndy,
		unitXToPixelX(x) + pdx, unitYToPixelY(y) + pdy);
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
	updateMinMax(x, y, x, y);
	dc->DrawText(wxString::FromUTF8(str), unitXToPixelX(x), unitYToPixelY(y));
}

void Canvas::string(double x, double y, const std::string str)
{
	wxSize textSize = dc->GetTextExtent(str);
	updateMinMax(x, y, x, y);
	dc->DrawText(wxString::FromUTF8(str.c_str()), unitXToPixelX(x),
		unitYToPixelY(y));
}

int Canvas::unitXToPixelX(double x)
{
	return xAxis->unitToPixel(x);
}

int Canvas::unitYToPixelY(double y)
{
	return yAxis->unitToPixel(y);
}

inline double Canvas::pixelXToUnitX(int x)
{
	return xAxis->pixelToUnit(x);
}

inline double Canvas::pixelYToUnitY(int y)
{
	return yAxis->pixelToUnit(y);
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

void
Canvas::addBoundsListener(std::function<void(double, double, double, double)> f)
{
	boundsNotifier.addListener(f);
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
	xAxis->setScale(scale);
	yAxis->setScale(scale);
}

void
Canvas::updateMinMax(const double& minX, const double& minY, const double& maxX,
	const double& maxY)
{
	xAxis->minmax({minX, maxX});
	yAxis->minmax({minY, maxY});
}

void Canvas::zoomFit()
{
	if (!drawnOnce)
		zoomFitPending = true;
	else
	{
		zoomFitPending = false;
		xAxis->zoomFit();
		yAxis->zoomFit();
		notifyBoundsChanged();
	}

	Refresh();
}

void Canvas::notifyBoundsChanged()
{
	double minX, minY, maxX, maxY;
	getBounds(minX, maxX, minY, maxY);
	boundsNotifier.notify(minX, maxX, minY, maxY);
}

const wxColour& Canvas::getBackgroundColor() const
{
	return backgroundColor;
}

const wxColour& Canvas::getMajorDivisionColor() const
{
	return majorDivisionColor;
}

const wxColour& Canvas::getMinorDivisionColor() const
{
	return minorDivisionColor;
}

const wxColour& Canvas::getMajorDivisionLabelColor() const
{
	return majorDivisionLabelColor;
}

bool Canvas::isShowGridLabels() const
{
	return showGridLabels;
}

void Canvas::setShowGridLabels(bool showGridLabels)
{
	Canvas::showGridLabels = showGridLabels;
	Refresh();
}

void Canvas::setBackgroundColor(const wxColour& backgroundColor)
{
	Canvas::backgroundColor = backgroundColor;
}

void Canvas::setMajorDivisionColor(const wxColour& majorDivisionColor)
{
	Canvas::majorDivisionColor = majorDivisionColor;
}

void Canvas::setMinorDivisionColor(const wxColour& minorDivisionColor)
{
	Canvas::minorDivisionColor = minorDivisionColor;
}

void Canvas::setMajorDivisionLabelColor(const wxColour& majorDivisionLabelColor)
{
	Canvas::majorDivisionLabelColor = majorDivisionLabelColor;
}

Drawable* Canvas::getClosest(double x, double y)
{
	//TODO: Move to Tree
	//TODO: Check if there are >0 in tree
	//TODO: Fix possible infinite loop when >10 points are equidistant
	std::list<Drawable*> closests;
	double               minRadius = 0;
	double               maxRadius = 1;
	while (true)
	{
		double radius = (maxRadius + minRadius) / 2;
		closests = drawableTree
			.getDrawables(x - radius, y - radius, x + radius, y + radius);
		if (closests.size() == 0)
		{
			minRadius = radius;
			maxRadius *= 2;
		}
		else if (closests.size() > 10)
			maxRadius = radius;
		else
			break;
	}
	double               range     = numeric_limits<double>::infinity();
	Drawable     * closest = nullptr;
	for (Drawable* d : closests)
	{
		BoundedDrawable* bd = (BoundedDrawable*) d;
		double dx   = (bd->maxX + bd->minX) / 2;
		double dy   = (bd->maxY + bd->minY) / 2;
		double dist = flabs::hypot(x - dx, y - dy);
		if (dist < range)
		{
			range   = dist;
			closest = d;
		}
	}
	return closest;
}

void Canvas::setLabel(double x, double y, std::string label)
{
	labelX = x;
	labelY = y;
	this->label = label;
	Refresh();
}

Drawable* Canvas::getClosestPixel(int x, int y)
{
	//TODO: Move to Tree
	double range = numeric_limits<double>::infinity();
	Drawable     * closest = nullptr;
	for (Drawable* d : boundedDrawables)
	{
		BoundedDrawable* bd = (BoundedDrawable*) d;
		double dx   = (bd->maxX + bd->minX) / 2;
		double dy   = (bd->maxY + bd->minY) / 2;
		double dist =
				   flabs::hypot(x - unitXToPixelX(dx), y - unitYToPixelY(dy));
		if (dist < range)
		{
			range   = dist;
			closest = d;
		}
	}
	return closest;
}

Axis* Canvas::getXAxis() const
{
	return xAxis;
}

Axis* Canvas::getYAxis() const
{
	return yAxis;
}
}
