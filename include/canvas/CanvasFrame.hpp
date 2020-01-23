/*
 * CanvasFrame.h
 *
 *  Created on: Mar 10, 2016
 *      Author: Madison
 */

#ifndef CANVASFRAME_H_
#define CANVASFRAME_H_

#include <wx/wx.h>
#include "Canvas.hpp"

namespace flabs
{
	class CanvasFrame: public wxFrame
	{
		public:
			Canvas canvas;

		public:
			CanvasFrame();
			virtual ~CanvasFrame();

		private:
			void OnExit(wxCommandEvent& event);
	};
}

#endif
