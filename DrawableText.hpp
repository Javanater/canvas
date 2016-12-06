//
// Created by Madison on 12/5/2016.
//

#ifndef PROJECTS_DRAWABLETEXT_HPP
#define PROJECTS_DRAWABLETEXT_HPP

#include "Canvas.h"

namespace flabs
{
class DrawableText : public BoundedDrawable
{
private:
	double      x, y;
	std::string text;
	wxColour    color;

public:
	DrawableText(double x, double y, const std::string& text,
		wxColour color = wxColour(0, 255, 0));

	virtual ~DrawableText() = default;

	virtual void draw(Canvas& canvas);
};
}

#endif //PROJECTS_DRAWABLETEXT_HPP
