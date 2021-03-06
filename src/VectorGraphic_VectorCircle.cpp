#include "VectorGraphic.h"
#include "Graphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

#pragma region VectorCircle

const Class VectorCircle::myClass = Class("VectorCircle", {&VectorShape::myClass});
const Class* VectorCircle::getClass()
{
	return &VectorCircle::myClass;
}

VectorCircle::VectorCircle() : VectorShape()
{
}

VectorCircle::~VectorCircle()
{
}

void VectorCircle::draw(Image* buffer, int globalWidth, int globalHeight)
{
	double preCX, preCY, preR;
	preCX = cx;
	preCY = cy;
	preR = radius;

	if(Mat3f::getIdentity() != getTransform())
	{
		//A interesting note for transforms is that it does not have to
		//change into a path. An Ellipse could also be used in some situations
		//and translation does not require a change. Skew, and in some situations,
		//rotation and scale require a change
		
		//Note that this function draws a path version of the shape
		drawTransformed(buffer, globalWidth, globalHeight);
		return;
	}

	//first, calc bounding box
	int x1 = cx-radius;
	int x2 = cx+radius;
	int y1 = cy-radius;
	int y2 = cy+radius;

	x1 = MathExt::clamp(x1, 0, globalWidth);
	y1 = MathExt::clamp(y1, 0, globalHeight);
	x2 = MathExt::clamp(x2, 0, globalWidth);
	y2 = MathExt::clamp(y2, 0, globalHeight);

	double radSqr = MathExt::sqr(radius);
	double strokeRad = MathExt::sqr(radius-this->getStrokeWidth());

	for(int j=y1; j<y2; j++)
	{
		for(int i=x1; i<x2; i++)
		{
			double thisPointDis = MathExt::sqr(i-cx) + MathExt::sqr(j-cy);
			if(thisPointDis<radSqr)
			{
				if(thisPointDis>strokeRad)
				{
					//stroke
					buffer->drawPixel(i, j, getStrokeColor());
				}
				else
				{
					//fill
					buffer->drawPixel(i, j, getFillColor());
				}
			}
		}
	}

	cx = preCX;
	cy = preCY;
	radius = preR;
}

void VectorCircle::drawStroke(Image* buffer, int globalWidth, int globalHeight)
{
	
}

void VectorCircle::setX(double x)
{
	this->cx = x;
}

double VectorCircle::getX()
{
	return cx;
}

void VectorCircle::setY(double y)
{
	this->cy = y;
}

double VectorCircle::getY()
{
	return cy;
}

void VectorCircle::setRadius(double r)
{
	this->radius = r;
}

double VectorCircle::getRadius()
{
	return this->radius;
}

void VectorCircle::applyTransform()
{
	Vec3f pos = Vec3f(cx, cy, 1.0);
	Vec3f r = Vec3f(radius, radius, 0.0);
	
	Vec3f newPos = getTransform() * pos;
	Vec3f newR = getTransform() * r;

	cx = newPos.x;
	cy = newPos.y;

	//deal with radius later
}

void VectorCircle::drawTransformed(Image* buffer, int globalWidth, int globalHeight)
{
	//convert to path then transform
	VectorPath p = VectorPath();

	p.setFillColor(getFillColor());
	p.setFillMethod(getFillMethod());
	p.setStrokeWidth(getStrokeWidth());
	p.setStrokeColor(getStrokeColor());
	p.setTransform(getTransform());
	p.setLineCap(getLineCap());
	p.setLineJoin(getLineJoin());

	//
	p.addMoveTo(cx-radius, cy);
	p.addArcTo(radius, radius, 0, false, false, cx+radius, cy);

	p.addMoveTo(cx-radius, cy);
	p.addArcTo(radius, radius, 0, false, true, cx+radius, cy);

	p.draw(buffer, globalWidth, globalHeight);
}


#pragma endregion