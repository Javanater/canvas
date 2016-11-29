//
// Created by Madison on 11/28/2016.
//

#ifndef PROJECTS_DRAWABLETREE_HPP
#define PROJECTS_DRAWABLETREE_HPP

#include <array>
#include <list>
#include "BoundedDrawable.hpp"

namespace flabs
{
class DrawableTree
{
private:
	struct Node
	{
		std::array<Node*, 4>        children;
		double                      x;
		double                      y;
		double                      size;
		std::list<BoundedDrawable*> drawables;

		Node(double x, double y, double size);

		inline bool contains(const BoundedDrawable* drawable)
		{
			return contains(drawable->minX, drawable->minY, drawable->maxX,
				drawable->maxY);
		}

		inline bool
		contains(const double& minX, const double& minY, const double& maxX,
			const double& maxY) const
		{
			return x <= minX && x + size > maxX && y <= minY && y + size > maxY;
		}

		inline bool
		intersects(const double& minX, const double& minY, const double& maxX,
			const double& maxY) const
		{
			return x <= maxX && x + size >= minX && y <= maxY &&
				y + size >= minY;
		}

		void insert(BoundedDrawable* drawable);

		void
		getDrawables(const double& minX, const double& minY, const double& maxX,
			const double& maxY, std::list<Drawable*>& drawables);
	};

	Node* root;

public:
	DrawableTree();

	void insert(BoundedDrawable* drawable);

	std::list<Drawable*>
	getDrawables(double minX, double minY, double maxX, double maxY);

private:
	void expand(BoundedDrawable* drawable);
};
}

#endif //PROJECTS_DRAWABLETREE_HPP
