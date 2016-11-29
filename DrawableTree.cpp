//
// Created by Madison on 11/28/2016.
//

#include "DrawableTree.hpp"

namespace flabs
{
DrawableTree::Node::Node(double x, double y, double size) :
	x(x), y(y), size(size)
{
	children.fill(nullptr);
}

void DrawableTree::Node::insert(BoundedDrawable* drawable)
{
	bool added = false;

	for (uint8_t i = 0; i < 4; ++i)
	{
		Node*& child = children[i];

		if (!child)
			child = new Node(x + size / 2 * ((i >> 0) & 1),
				y + size / 2 * ((i >> 1) & 1), size / 2);

		if (child->contains(drawable))
		{
			child->insert(drawable);
			added = true;
		}
	}

	if (!added)
		drawables.push_back(drawable);
}

void DrawableTree::Node::getDrawables(const double& minX, const double& minY,
	const double& maxX, const double& maxY, std::list<Drawable*>& drawables)
{
	if (intersects(minX, minY, maxX, maxY))
	{
		for (Drawable* drawable : this->drawables)
			drawables.push_back(drawable);

		for (Node* child : children)
			if (child)
				child->getDrawables(minX, minY, maxX, maxY, drawables);
	}
}

DrawableTree::DrawableTree() : root(new Node(0, 0, 1))
{
}

void DrawableTree::insert(BoundedDrawable* drawable)
{
	while (!root->contains(drawable))
		expand(drawable);

	root->insert(drawable);
}

std::list<Drawable*>
DrawableTree::getDrawables(double minX, double minY, double maxX, double maxY)
{
	std::list<Drawable*> drawables;
	root->getDrawables(minX, minY, maxX, maxY, drawables);
	return drawables;
}

void DrawableTree::expand(BoundedDrawable* drawable)
{
	double  x          = root->x + root->size / 2;
	double  y          = root->y + root->size / 2;
	double  dx         = (drawable->minX + drawable->maxX) / 2;
	double  dy         = (drawable->minY + drawable->maxY) / 2;
	double  newX       = root->x;
	double  newY       = root->y;
	uint8_t childIndex = 0;

	if (dx < x)
	{
		newX -= root->size;
		childIndex |= 1;
	}

	if (dy < y)
	{
		newY -= root->size;
		childIndex |= 2;
	}

	Node* newRoot = new Node(newX, newY, root->size * 2);
	newRoot->children[childIndex] = root;
	root = newRoot;
}
}
