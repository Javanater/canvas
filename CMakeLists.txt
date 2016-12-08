cmake_minimum_required(VERSION 3.5)
project(Canvas)
set(SOURCE_FILES
		Canvas.cpp
		Canvas.hpp
		CanvasFrame.cpp
		CanvasFrame.hpp
		Drawable.cpp
		Drawable.hpp DrawableLine.cpp DrawableLine.hpp DrawableCircle.cpp DrawableCircle.hpp DrawableMarker.cpp DrawableMarker.hpp DrawableTree.cpp DrawableTree.hpp BoundedDrawable.hpp BoundedDrawable.cpp DrawableText.cpp DrawableText.hpp Axis.cpp Axis.hpp LinearAxis.cpp LinearAxis.hpp)
add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
target_link_libraries(${PROJECT_NAME} Utilities)