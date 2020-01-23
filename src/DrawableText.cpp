//
// Created by Madison on 12/5/2016.
//

#include <canvas/DrawableText.hpp>

using namespace std;

namespace flabs
{
DrawableText::DrawableText(double x, double y, const string& text,
	wxColour color) : x(x), y(y), text(text), color(color)
{
}

void DrawableText::draw(Canvas& canvas)
{
}
}
