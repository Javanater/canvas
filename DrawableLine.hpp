//
// Created by Madison on 10/26/2016.
//

#ifndef PROJECTS_DRAWABLELINE_HPP
#define PROJECTS_DRAWABLELINE_HPP

#include "Canvas.hpp"

namespace flabs
{
	class DrawableLine : public Drawable
	{
		private:
			double   x, y, nx, ny;
			wxColour color;

		public:
			DrawableLine(double x, double y, double nx, double ny,
				wxColour color = wxColor(0, 0, 255), int level = 0);

			virtual ~DrawableLine();

			virtual void draw(Canvas& canvas);
	};
}

#endif //PROJECTS_DRAWABLELINE_HPP
