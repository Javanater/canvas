/*
 * Drawable.h
 *
 *  Created on: Mar 10, 2016
 *      Author: Madison
 */

#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include <wx/wx.h>

namespace flabs
{
	class Canvas;

	class Drawable
	{
		private:
			int level;

		public:
			Drawable(int level = 0);
			virtual ~Drawable();
			bool operator<(const Drawable& other) const;
			virtual void draw(Canvas& canvas) = 0;
	};
}

#endif
