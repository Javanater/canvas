//
// Created by Madison on 11/10/2016.
//

#ifndef PROJECTS_DRAWABLEMARKER_HPP
#define PROJECTS_DRAWABLEMARKER_HPP

#include "Canvas.hpp"

namespace flabs
{
class DrawableMarker : public BoundedDrawable
	{
		public:
			enum Type
			{
				STAR, CIRCLE, PLUS, X, SQUARE, DOT, BLOCK
			};

			static Type typeFromChar(char c);

			double   x, y;
			Type     type;
			wxColour color;

			DrawableMarker(double x = 0, double y = 0, Type type = STAR,
				wxColour color = wxColor(0, 0, 255), int level = 0);

			DrawableMarker(double x, double y, char type,
				wxColour color = wxColor(0, 0, 255), int level = 0);

	void updateBounds();

			virtual ~DrawableMarker();

			virtual void draw(Canvas& canvas);
	};
}

#endif //PROJECTS_DRAWABLEMARKER_HPP
