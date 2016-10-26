/*
 * CanvasFrame.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: Madison
 */

#include "CanvasFrame.h"

namespace flabs
{
	CanvasFrame::CanvasFrame() :
			wxFrame(NULL, wxID_ANY, ""), canvas(this, wxID_ANY)
	{
		SetSize(500, 500);
		SetTitle("Canvas");
		Center();
		Connect(wxID_EXIT, wxCommandEventHandler(CanvasFrame::OnExit));
	}

	CanvasFrame::~CanvasFrame()
	{
	}

	void CanvasFrame::OnExit(wxCommandEvent& event)
	{
		Close(true);
	}
}