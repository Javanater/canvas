cmake_minimum_required(VERSION 3.5)

project(Canvas)

set(SOURCE_FILES
	src/Canvas.cpp
	include/canvas/Canvas.hpp
	src/CanvasFrame.cpp
	include/canvas/CanvasFrame.hpp
	src/Drawable.cpp
	include/canvas/Drawable.hpp
	src/DrawableLine.cpp
	include/canvas/DrawableLine.hpp
	src/DrawableCircle.cpp
	include/canvas/DrawableCircle.hpp
	src/DrawableMarker.cpp
	include/canvas/DrawableMarker.hpp
	src/DrawableTree.cpp
	include/canvas/DrawableTree.hpp
	include/canvas/BoundedDrawable.hpp
	src/BoundedDrawable.cpp
	src/DrawableText.cpp
	include/canvas/DrawableText.hpp
	src/Axis.cpp
	include/canvas/Axis.hpp
	src/LinearAxis.cpp
	include/canvas/LinearAxis.hpp
    )

add_library(${PROJECT_NAME} ${SOURCE_FILES})

add_subdirectory(../utilities utilities)
add_subdirectory(../math math)

find_package(wxWidgets REQUIRED core base)
include(${wxWidgets_USE_FILE})

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${wxWidgets_LIBRARIES}
    utilities
    Math
    )

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(${PROJECT_NAME}_example
    examples/CanvasSample.cpp
    examples/DrawableReferenceFrame2d.cpp
    examples/DrawableReferenceFrame2d.hpp
    )

target_link_libraries(${PROJECT_NAME}_example
    Canvas
    ${wxWidgets_LIBRARIES}
    )

