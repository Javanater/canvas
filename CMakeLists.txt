cmake_minimum_required(VERSION 3.5)
project(Canvas)
set(SOURCE_FILES
		Canvas.cpp
		Canvas.h
		CanvasFrame.cpp
		CanvasFrame.h
		Drawable.cpp
		Drawable.h)
add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})