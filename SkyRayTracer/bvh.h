#pragma once
#include "hittable.h"
#include "hittableList.h"
#include <algorithm>

inline bool BoxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis)
{
	Aabb boxa, boxb;

	if (!a->boundingbox(0, 0, boxa) || !b->boundingbox(0, 0, boxb))
	{
		std::cerr << "No bounding box in bvh_node constructor.\n";
	}
	return boxa.min()[axis] < boxb.min()[axis];
}

bool BoxCompareX(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
	return BoxCompare(a, b, 0);
}
bool BoxCompareY(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
	return BoxCompare(a, b, 1);
}
bool BoxCompareZ(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
	return BoxCompare(a, b, 2);
}

class BVHNode : public Hittable
{
public:
	BVHNode();
	BVHNode(const HittableList& list, float t0, float t1) :BVHNode(list.objects, 0, list.objects.size(), t0, t1) {}

	BVHNode(const std::vector<std::shared_ptr<Hittable>>& srcObjects, size_t start, size_t end, float t0, float t1);

	virtual bool hit(const Ray& r, float mint, float maxt, HitRecord& rec) const override;
	virtual bool boundingbox(float t0, float t1, Aabb& outputBox) const override;
public:
	std::shared_ptr<Hittable> left;
	std::shared_ptr<Hittable> right;
	Aabb box;
};

bool BVHNode::boundingbox(float t0, float t1, Aabb& outputBox) const
{
	outputBox = box;
	return true;
}

//光线和当前BVH节点的AABB求交
bool BVHNode::hit(const Ray& r, float mint, float maxt, HitRecord& rec) const
{
	if (!box.hit(r, mint, maxt))
	{
		return false;
	}

	bool hitleft = left->hit(r, mint, maxt, rec);
	bool hitright = right->hit(r, mint, hitleft ? rec.t : maxt, rec);

	return hitleft || hitright;
}

BVHNode::BVHNode(const std::vector<std::shared_ptr<Hittable>>& srcObjects, size_t start, size_t end, float t0, float t1)
{
	auto objects = srcObjects;
	
	int axis = Randomi(0, 2);
	auto comparator = (axis == 0) ? BoxCompareX : (axis == 1) ? BoxCompareY : BoxCompareZ;

	size_t objnum = end - start;
	if (objnum == 1)
	{
		left = right = objects[start];
	}
	if (objnum == 2)
	{
		if (comparator(objects[start], objects[end]))
		{
			left = objects[start];
			right = objects[start+1];
		}
		else
		{
			left = objects[start + 1];
			right = objects[start];
		}
	}
	else
	{
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		auto mid = start + objnum / 2;
		left = std::make_shared<BVHNode>(objects, start, mid, t0, t1);
		right = std::make_shared<BVHNode>(objects, mid, end, t0, t1);

	}

	Aabb boxleft, boxright;

	if (!left->boundingbox(t0, t1, boxleft) || !right->boundingbox(t0, t1, boxright))
	{
		std::cerr << "No bounding box in bvh_node constructor.\n";
	}

	box = SurroundingBox(boxleft, boxright);
}