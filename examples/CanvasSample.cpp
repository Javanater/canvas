//
// Created by Madison on 10/11/2016.
//

#include <canvas/CanvasFrame.hpp>
#include <canvas/DrawableLine.hpp>
#include <canvas/DrawableCircle.hpp>
#include <canvas/DrawableMarker.hpp>
#include "DrawableReferenceFrame2d.hpp"

using namespace std;
using namespace flabs;

class CanvasSampleApp : public wxApp
{
private:
	CanvasFrame* frame;
	DrawableLine                      drawableLine;
	DrawableCircle                    drawableCircle;
	list <unique_ptr<DrawableMarker>> markers;
	list <unique_ptr<DrawableCircle>> circles;
	ReferenceFrame<2>                 world;
	ReferenceFrame<2>                 child1;
	ReferenceFrame<2>                 child2;
	DrawableReferenceFrame2d          dworld;
	DrawableReferenceFrame2d          dchild1;
	DrawableReferenceFrame2d          dchild2;
	thread                            rotateThread;
	bool                              keepRunning;

public:
	CanvasSampleApp();

	virtual bool OnInit();

	virtual int OnExit();

	void addMarker(Canvas::MouseEvent event);

	void rotate();
};

wxIMPLEMENT_APP(CanvasSampleApp);

CanvasSampleApp::CanvasSampleApp() :
	drawableLine(0, 0, 1, 1), drawableCircle(1, 1, .5), child1(1, 0, 0, &world),
	child2(1, 0, 0, &child1), dworld(world), dchild1(child1), dchild2(child2),
	keepRunning(true)
{
}

bool CanvasSampleApp::OnInit()
{
	frame = new CanvasFrame();
	frame->canvas.add(&drawableLine);
	frame->canvas.add(&drawableCircle);
	frame->canvas.add(&dworld);
	frame->canvas.add(&dchild1);
	frame->canvas.add(&dchild2);
	frame->canvas.addMouseLeftDownNotifier(
		bind(&CanvasSampleApp::addMarker, this, placeholders::_1));
	frame->Show(true);

	for (int i = 0; i < 100000; ++i)
	{
		DrawableCircle* circle = new DrawableCircle(i, 0, .5);
		circles.emplace_back(circle);
		frame->canvas.add(circle);
	}

	for (int i = 0; i < 100000; ++i)
	{
		DrawableMarker* marker = new DrawableMarker(0, i);
		markers.emplace_back(marker);
		frame->canvas.add(marker);
	}

	rotateThread = thread(&CanvasSampleApp::rotate, this);
	return true;
}

void CanvasSampleApp::addMarker(Canvas::MouseEvent event)
{
	DrawableMarker* marker = new DrawableMarker(event.x, event.y);
	markers.emplace_back(marker);
	frame->canvas.add(marker);
}

int CanvasSampleApp::OnExit()
{
	keepRunning = false;
	rotateThread.join();
	return wxAppBase::OnExit();
}

void CanvasSampleApp::rotate()
{
	while (keepRunning)
	{
		world.setYawOffset(world.getYawOffset() + M_PI / 360);
		child1.setXOffset(cos(world.getYawOffset()) + 1);
		child2.setYawOffset(child2.getYawOffset() + M_PI / 360);
		frame->canvas.Refresh();
		sleepMillis(5);
	}
}
