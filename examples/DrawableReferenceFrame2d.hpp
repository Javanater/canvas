//
// Created by Madison on 11/11/2016.
//

#ifndef PROJECTS_DRAWABLEREFERENCEFRAME2D_HPP
#define PROJECTS_DRAWABLEREFERENCEFRAME2D_HPP

#include <canvas/Canvas.hpp>
#include <math/geometry/ReferenceFrame.hpp>

namespace flabs
{
	class DrawableReferenceFrame2d : public Drawable
	{
		private:
			ReferenceFrame<2>& referenceFrame;
			wxColour color;

		public:
			DrawableReferenceFrame2d(ReferenceFrame<2>& referenceFrame,
				wxColour color = wxColor(0, 255, 0), int level = 0);

			virtual ~DrawableReferenceFrame2d();

			virtual void draw(Canvas& canvas);
	};
}

#endif //PROJECTS_DRAWABLEREFERENCEFRAME2D_HPP
