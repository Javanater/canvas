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
#include <Utilities/utilities.h>
#include <wx/dcbuffer.h>
#include <iterator>
#include <iostream>

namespace flabs
{
	class Canvas : public wxPanel
	{
		public:
			const static double MINIMUM_SCALE;
			const static double MAXIMUM_SCALE;

		private:
			wxWindow* parent;
			typedef std::multiset<Drawable*, lessPointer<Drawable*>>
						DrawableSet;
			DrawableSet drawables;
			wxBufferedPaintDC* dc;

			/**
			 * Units per pixel.
			 */
			double       scale;
			double       x;
			double       y;
			bool         dragging;
			int          lastMouseX;
			int          lastMouseY;
			wxColour     drawColor;
			wxBrushStyle brushStyle;
			wxColour     backgroundColor;
			wxColour     majorDivisionColor;
			wxColour     minorDivisionColor;
			wxColour     majorDivisionLabelColor;
			bool         showGrid;

		public:
			Canvas(wxWindow* parent, int id);

			virtual ~Canvas();

			void OnSize(wxSizeEvent& event);

			void OnPaint(wxPaintEvent& event);

			void OnMouseMoved(wxMouseEvent& event);

			void OnMouseButton(wxMouseEvent& event);

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

			template<class Iterator>
			void polygon(Iterator start, Iterator end)
			{
				size_t count = std::distance(start, end);
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

			inline double pixelsToUnits(int pixels);

			inline int unitsToPixels(double units);

			int unitXToPixelX(double x);

			int unitYToPixelY(double y);

			inline double pixelXToUnitX(int x);

			inline double pixelYToUnitY(int y);

			void
			getBounds(double& minX, double& maxX, double& minY, double& maxY);

			void setShowGrid(bool enable);
	};
}

#endif
