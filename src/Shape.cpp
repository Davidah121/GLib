#include "Shape.h"

#pragma region SHAPE


	#pragma region ClassStuff
	const Class Shape::myClass = Class("Shape", {&Object::myClass});
	const Class* Shape::getClass()
	{
		return &Shape::myClass;
	}
	#pragma endregion


Shape::Shape()
{
}

Shape::~Shape()
{
}

void Shape::setPosition(Vec3f posPointer)
{
	position = posPointer;
}

Vec3f Shape::getPosition()
{
	return position;
}

void Shape::setScale(Vec3f posScale)
{
	scale = posScale;
}

Vec3f Shape::getScale()
{
	return scale;
}

void Shape::setRotation(Vec3f posRotation)
{
	rotation = posRotation;
}

Vec3f Shape::getRotation()
{
	return rotation;
}

void Shape::transform()
{
	
}

#pragma endregion

#pragma region POINT2D

	#pragma region ClassStuff
	const Class Point2D::myClass = Class("Point2D", { &Shape::myClass });
	const Class* Point2D::getClass()
	{
		return &Point2D::myClass;
	}
	#pragma endregion

Point2D::Point2D(Vec2f pos)
{
	position.x = pos.x;
	position.y = pos.y;
}

Point2D::~Point2D()
{
}

#pragma endregion

#pragma region Box2D

	#pragma region ClassStuff
	const Class Box2D::myClass = Class("Box2D", { &Shape::myClass });
	const Class* Box2D::getClass()
	{
		return &Box2D::myClass;
	}
	#pragma endregion

Box2D::Box2D(double leftBound, double topBound, double rightBound, double bottomBound)
{
	this->lBound = leftBound;
	this->tBound = topBound;
	this->rBound = rightBound;
	this->bBound = bottomBound;
}

Box2D::~Box2D()
{
}

void Box2D::setLeftBound(double lb)
{
	lBound = lb;
}

double Box2D::getLeftBound()
{
	return lBound;
}

void Box2D::setTopBound(double tb)
{
	tBound = tb;
}

double Box2D::getTopBound()
{
	return tBound;
}

void Box2D::setRightBound(double rb)
{
	rBound = rb;
}

double Box2D::getRightBound()
{
	return rBound;
}

void Box2D::setBottomBound(double bb)
{
	bBound = bb;
}

double Box2D::getBottomBound()
{
	return bBound;
}

#pragma endregion

#pragma region CIRCLE

	#pragma region ClassStuff
	const Class Circle::myClass = Class("Circle", { &Shape::myClass });
	const Class* Circle::getClass()
	{
		return &Circle::myClass;
	}
	#pragma endregion

Circle::Circle(double rad)
{
	radius = rad;
}

Circle::~Circle()
{
}

double Circle::getRadius()
{
	return radius;
}

void Circle::setRadius(double rad)
{
	radius = rad;
}

#pragma endregion

#pragma region LINE2D

	#pragma region ClassStuff
	const Class Line2D::myClass = Class("Line2D", { &Shape::myClass });
	const Class* Line2D::getClass()
	{
		return &Line2D::myClass;
	}
	#pragma endregion

Line2D::Line2D(double x1, double y1, double x2, double y2)
{
	v1.x = x1;
	v1.y = y1;
	v2.x = x2;
	v2.y = y2;
}

Line2D::Line2D(Vec2f p1, Vec2f p2)
{
	v1 = p1;
	v2 = p2;
}

Line2D::~Line2D()
{
}

void Line2D::setPoint1(Vec2f p)
{
	v1 = p;
}

void Line2D::setPoint1(double x, double y)
{
	v1.x = x;
	v1.y = y;
}

void Line2D::setPoint2(Vec2f p)
{
	v2 = p;
}

void Line2D::setPoint2(double x, double y)
{
	v2.x = x;
	v2.y = y;
}

Vec2f Line2D::getPoint1()
{
	return v1;
}

Vec2f Line2D::getPoint2()
{
	return v2;
}

double Line2D::getSlope()
{
	if (v2.x - v1.x != 0)
	{
		return (v2.y - v1.y) / (v2.x - v1.x);
	}
	return 0.0;
}

double Line2D::getSlopeRelativeY()
{
	if (v2.y - v1.y != 0)
	{
		return (v2.x - v1.x) / (v2.y - v1.y);
	}
	return 0.0;
}

double Line2D::getYInt()
{
	return v1.y - getSlope()*v1.x;
}

double Line2D::getXInt()
{
	return v1.x - getSlopeRelativeY() * v1.y;
}

double Line2D::getMinX()
{
	return MathExt::min(v1.x, v2.x);
}

double Line2D::getMaxX()
{
	return MathExt::max(v1.x, v2.x);
}

double Line2D::getMinY()
{
	return MathExt::min(v1.y, v2.y);
}

double Line2D::getMaxY()
{
	return MathExt::max(v1.y, v2.y);
}

#pragma endregion

#pragma region TRIANGLE2D

	#pragma region ClassStuff
	const Class Triangle2D::myClass = Class("Triangle2D", { &Shape::myClass });
	const Class* Triangle2D::getClass()
	{
		return &Triangle2D::myClass;
	}
	#pragma endregion

Triangle2D::Triangle2D(Vec2f p1, Vec2f p2, Vec2f p3)
{
	v1 = p1;
	v2 = p2;
	v3 = p3;
}

Triangle2D::Triangle2D(double x1, double y1, double x2, double y2, double x3, double y3)
{
	v1.x = x1;
	v1.y = y1;
	v2.x = x2;
	v2.y = y2;
	v3.x = x3;
	v3.y = y3;
}

Triangle2D::~Triangle2D()
{
}

void Triangle2D::setVertex1(Vec2f p)
{
	v1 = p;
}

void Triangle2D::setVertex1(double x, double y)
{
	v1.x = x;
	v1.y = y;
}

void Triangle2D::setVertex2(Vec2f p)
{
	v2 = p;
}

void Triangle2D::setVertex2(double x, double y)
{
	v2.x = x;
	v2.y = y;
}

void Triangle2D::setVertex3(Vec2f p)
{
	v3 = p;
}

void Triangle2D::setVertex3(double x, double y)
{
	v3.x = x;
	v3.y = y;
}

Vec2f Triangle2D::getVertex1()
{
	return v1;
}

Vec2f Triangle2D::getVertex2()
{
	return v2;
}

Vec2f Triangle2D::getVertex3()
{
	return v3;
}

#pragma endregion

#pragma region COLLISION_MASTER

//Static initialization
bool(*CollisionMaster::extCollision)(Shape* a, Shape* b) = nullptr;

//Functions
bool CollisionMaster::getCollision(Shape* a, Shape* b)
{
	std::string aClassName = a->getClass()->getClassName();
	std::string bClassName = b->getClass()->getClassName();

	if (aClassName == "Box2D")
	{
		if (bClassName == "Box2D")
		{
			return CollisionMaster::collisionMethod((Box2D*)a, (Box2D*)b);
		}
		else if (bClassName == "Point2D")
		{
			return CollisionMaster::collisionMethod((Point2D*)b, (Box2D*)a);
		}
		else if (bClassName == "Circle")
		{
			return CollisionMaster::collisionMethod((Box2D*)a, (Circle*)b);
		}
		else if (bClassName == "Triangle2D")
		{
			return CollisionMaster::collisionMethod((Box2D*)a, (Triangle2D*)b);
		}
		else if (bClassName == "Line2D")
		{
			return CollisionMaster::collisionMethod((Box2D*)a, (Line2D*)b);
		}

		//No premade collision method found. Trying the external
		if (extCollision != nullptr)
		{
			return extCollision(a, b);
		}
		else
		{
			//No external collision found
			//will return false
		}
	}
	else if (aClassName == "Point2D")
	{
		if (bClassName == "Box2D")
		{
			return CollisionMaster::collisionMethod((Point2D*)a, (Box2D*)b);
		}
		else if (bClassName == "Point2D")
		{
			return CollisionMaster::collisionMethod((Point2D*)a, (Point2D*)b);
		}
		else if (bClassName == "Circle")
		{
			return CollisionMaster::collisionMethod((Point2D*)a, (Circle*)b);
		}
		else if (bClassName == "Triangle2D")
		{
			return CollisionMaster::collisionMethod((Point2D*)a, (Triangle2D*)b);
		}
		else if (bClassName == "Line2D")
		{
			return CollisionMaster::collisionMethod((Point2D*)a, (Line2D*)b);
		}

		//No premade collision method found. Trying the external
		if (extCollision != nullptr)
		{
			return extCollision(a, b);
		}
		else
		{
			//No external collision found
			//will return false
		}
	}
	else if (aClassName == "Circle")
	{
		if (bClassName == "Box2D")
		{
			return CollisionMaster::collisionMethod((Box2D*)b, (Circle*)a);
		}
		else if (bClassName == "Point2D")
		{
			return CollisionMaster::collisionMethod((Point2D*)b, (Circle*)a);
		}
		else if (bClassName == "Circle")
		{
			return CollisionMaster::collisionMethod((Circle*)a, (Circle*)b);
		}
		else if (bClassName == "Triangle2D")
		{
			return CollisionMaster::collisionMethod((Circle*)a, (Triangle2D*)b);
		}
		else if (bClassName == "Line2D")
		{
			return CollisionMaster::collisionMethod((Circle*)a, (Line2D*)b);
		}

		//No premade collision method found. Trying the external
		if (extCollision != nullptr)
		{
			return extCollision(a, b);
		}
		else
		{
			//No external collision found
			//will return false
		}
	}
	else if (aClassName == "Line2D")
	{
		if (bClassName == "Box2D")
		{
			return CollisionMaster::collisionMethod((Box2D*)b, (Line2D*)a);
		}
		else if (bClassName == "Point2D")
		{
			return CollisionMaster::collisionMethod((Point2D*)b, (Line2D*)a);
		}
		else if (bClassName == "Circle")
		{
			return CollisionMaster::collisionMethod((Circle*)b, (Line2D*)a);
		}
		else if (bClassName == "Triangle2D")
		{
			return CollisionMaster::collisionMethod((Line2D*)a, (Triangle2D*)b);
		}
		else if (bClassName == "Line2D")
		{
			return CollisionMaster::collisionMethod((Line2D*)a, (Line2D*)b);
		}

		//No premade collision method found. Trying the external
		if (extCollision != nullptr)
		{
			return extCollision(a, b);
		}
		else
		{
			//No external collision found
			//will return false
		}
	}
	else if (aClassName == "Triangle2D")
	{
		if (bClassName == "Box2D")
		{
			return CollisionMaster::collisionMethod((Box2D*)b, (Triangle2D*)a);
		}
		else if (bClassName == "Point2D")
		{
			return CollisionMaster::collisionMethod((Point2D*)b, (Triangle2D*)a);
		}
		else if (bClassName == "Circle")
		{
			return CollisionMaster::collisionMethod((Circle*)b, (Triangle2D*)a);
		}
		else if (bClassName == "Triangle2D")
		{
			return CollisionMaster::collisionMethod((Triangle2D*)a, (Triangle2D*)b);
		}
		else if (bClassName == "Line2D")
		{
			return CollisionMaster::collisionMethod((Line2D*)b, (Triangle2D*)a);
		}

		//No premade collision method found. Trying the external
		if (extCollision != nullptr)
		{
			return extCollision(a, b);
		}
		else
		{
			//No external collision found
			//will return false
		}
	}
	else
	{
		if (extCollision != nullptr)
		{
			return extCollision(a, b);
		}
		else
		{
			//No external collision found
			//will return false
		}
	}

	return false;
}

bool CollisionMaster::collisionMethod(Point2D* a, Point2D* b)
{
	if (a->getPosition().x == b->getPosition().x)
	{
		if (a->getPosition().y == b->getPosition().y)
		{
			return true;
		}
	}
	return false;
}

bool CollisionMaster::collisionMethod(Point2D* a, Circle* b)
{
	double dX = MathExt::sqr(a->getPosition().x - b->getPosition().x);
	double dY = MathExt::sqr(a->getPosition().x - b->getPosition().x);

	if (dX + dY <= MathExt::sqr(b->getRadius()))
	{
		return true;
	}
	return false;
}

bool CollisionMaster::collisionMethod(Point2D* a, Box2D* b)
{
	if (a->getPosition().x >= b->getLeftBound() &&
		a->getPosition().x <= b->getRightBound())
	{
		if (a->getPosition().y >= b->getTopBound() &&
			a->getPosition().y <= b->getBottomBound())
		{
			return true;
		}
	}
	return false;
}

bool CollisionMaster::collisionMethod(Point2D* a, Triangle2D* b)
{
	Vec2f v1 = b->getVertex2() - b->getVertex1();
	Vec2f v2 = b->getVertex3() - b->getVertex1();

	Vec2f toPoint = Vec2f(a->getPosition().x - b->getVertex1().x, a->getPosition().y - b->getVertex1().y);
	//v1*v2 = v1*toPoint + v2*tPoint

	
	if (MathExt::dot(v1, v2) == MathExt::dot(v1, toPoint) + MathExt::dot(v2, toPoint))
	{
		//Possible collision
		v1 = -v1;
		v2 = Vec2f(b->getVertex3() - b->getVertex2());

		toPoint = Vec2f(a->getPosition().x - b->getVertex2().x, a->getPosition().y - b->getVertex2().y);

		if (MathExt::dot(v1, v2) == MathExt::dot(v1, toPoint) + MathExt::dot(v2, toPoint))
		{
			return true;
		}
	}

	return false;
}

bool CollisionMaster::collisionMethod(Point2D* a, Line2D* b)
{
	if (b->getMinX() == b->getMaxX())
	{
		//Vertical line
		if (a->getPosition().x == b->getMaxX())
		{
			if (a->getPosition().y >= b->getMinY() &&
				a->getPosition().y <= b->getMaxY())
			{
				return true;
			}
		}
	}
	else
	{
		double yNeeded = (a->getPosition().x * b->getSlope()) + b->getYInt();

		if (yNeeded == a->getPosition().y)
		{
			return true;
		}
	}
	
	return false;
}

bool CollisionMaster::collisionMethod(Box2D* a, Box2D* b)
{
	if (a->getLeftBound() <= b->getRightBound() && a->getRightBound() >= b->getLeftBound())
	{
		if (a->getTopBound() <= b->getBottomBound() && a->getBottomBound() >= b->getTopBound())
		{
			return true;
		}
	}

	if (b->getLeftBound() <= a->getRightBound() && b->getRightBound() >= a->getLeftBound())
	{
		if (b->getTopBound() <= a->getBottomBound() && b->getBottomBound() >= a->getTopBound())
		{
			return true;
		}
	}
	return false;
}

bool CollisionMaster::collisionMethod(Box2D* a, Circle* b)
{
	double curX = MathExt::clamp(b->getPosition().x, a->getLeftBound(), a->getRightBound());
	double curY = MathExt::clamp(b->getPosition().y, a->getTopBound(), a->getBottomBound());

	double length = MathExt::sqr(curX - b->getPosition().x) + MathExt::sqr(curY - b->getPosition().y);
	
	if (length <= MathExt::sqr(b->getRadius()))
	{
		return true;
	}
	return false;
}

bool CollisionMaster::collisionMethod(Box2D* a, Triangle2D* b)
{
	return false;
}

bool CollisionMaster::collisionMethod(Box2D* a, Line2D* b)
{
	if (b->getMinX() == b->getMaxX())
	{
		//Vertical line
		if (b->getMinX() >= a->getLeftBound() &&
			b->getMinX() <= a->getRightBound())
		{
			if (b->getMinY() <= a->getBottomBound() &&
				b->getMaxY() >= a->getTopBound())
			{
				return true;
			}
		}
	}
	else
	{
		double yNeeded1 = (a->getLeftBound() * b->getSlope()) + b->getYInt();
		double yNeeded2 = (a->getRightBound() * b->getSlope()) + b->getYInt();

		double maY = MathExt::max(yNeeded1, yNeeded2);
		double miY = MathExt::min(yNeeded1, yNeeded2);

		maY = MathExt::clamp(maY, b->getMinY(), b->getMaxY());
		miY = MathExt::clamp(miY, b->getMinY(), b->getMaxY());
		
		double maX = MathExt::clamp(a->getLeftBound(), b->getMinX(), b->getMaxX());
		double miX = MathExt::clamp(a->getRightBound(), b->getMinX(), b->getMaxX());

		if (a->getLeftBound() <= maX && a->getRightBound() >= miX)
		{
			if (a->getTopBound() <= maY && a->getBottomBound() >= miY)
			{
				return true;
			}
		}

		if (miX <= a->getRightBound() && maX >= a->getLeftBound())
		{
			if (miY <= a->getBottomBound() && maY >= a->getTopBound())
			{
				return true;
			}
		}
	}

	return false;
}

bool CollisionMaster::collisionMethod(Circle* a, Circle* b)
{
	double disX = MathExt::sqr(a->getPosition().x - b->getPosition().x);
	double disY = MathExt::sqr(a->getPosition().y - b->getPosition().y);
	double len1 = disX + disY;

	double len2 = MathExt::sqr(a->getRadius()) + MathExt::sqr(b->getRadius());

	if (len1 <= len2)
	{
		return true;
	}
	return false;
}

bool CollisionMaster::collisionMethod(Circle* a, Triangle2D* b)
{
	return false;
}

bool CollisionMaster::collisionMethod(Circle* a, Line2D* b)
{
	//Why this method works and why use this method.
	/*
		This method finds the point on the line with the minimum distance
		to some other point. That point may not be in our line segment though.
		We clamp it back to the range of possible values. When we clamp it, that
		point is the closest point in the line segment. This is due to the line
		being linear. We can safely say that an x or y value closer to the point
		we desire is the best we can get.

		As for why we chose this method, We don't care about the points of
		intersection as much as whether there was a collision at all. This method 
		also handles the offset of the circle very well. For physics applications,
		this version provides us with the point that will have collided first which
		informs us on where to apply the force.
	*/

	if (b->getMaxX() == b->getMinX())
	{
		//vertical line
		//similar to the x version
		// y = (py + slope*px - slope*b) / (1+slope*slope)

		//find the point with the minimum distance
		double y = (a->getPosition().y + b->getSlopeRelativeY() * (a->getPosition().x - b->getXInt())) / (1 + MathExt::sqr(b->getSlopeRelativeY()));

		//clamp it down to the max or min possible x value then solve for y
		y = MathExt::clamp(y, b->getMinY(), b->getMaxY());
		double x = y * b->getSlopeRelativeY() + b->getXInt();

		//now solve distance
		double disX = MathExt::sqr(a->getPosition().x - x);
		double disY = MathExt::sqr(a->getPosition().y - y);

		if (disX + disY <= MathExt::sqr(a->getRadius()))
		{
			//There is a collision
			return true;
		}
	}
	else
	{
		// 0 = -2*(px - x) + -2*slope*(py - slope*x - b)
		// 0 = -2px + 2x + 2*slope*slope*x -2*slope*py + 2*slope*b
		// 2px + 2*slope*py - 2*slope*b = 2*x*(1+slope*slope)
		// px + slope*py - slope*b = x*(1+slope*slope)
		// x = (px + slope*py - slope*b) / (1 + slope * slope)

		//find the point with the minimum distance
		double x = (a->getPosition().x + b->getSlope() * (a->getPosition().y - b->getYInt())) / (1 + MathExt::sqr(b->getSlope()));

		//clamp it down to the max or min possible x value then solve for y
		x = MathExt::clamp(x, b->getMinX(), b->getMaxX());
		double y = x * b->getSlope() + b->getYInt();

		//now solve distance
		double disX = MathExt::sqr(a->getPosition().x - x);
		double disY = MathExt::sqr(a->getPosition().y - y);

		if (disX + disY <= MathExt::sqr(a->getRadius()))
		{
			//There is a collision
			return true;
		}
	}
	return false;
}

bool CollisionMaster::collisionMethod(Line2D* a, Line2D* b)
{
	//We want to look for the point of intersection
	//We have special cases to deal with though.
	//Those are vertical lines.
	//case 1: both are normal
	//case 2: both are vertical
	//case 3: a is vertical
	//case 4: b is vertical

	if (a->getMinX() != a->getMaxX())
	{
		//a is a normal line
		if (b->getMinX() != b->getMinX())
		{
			//b is a normal line
			
			//y=mx+b
			//y=m2x+b2
			//mx+b = m2x+b2
			//x(m-m2) = b2-b
			//x = (b2-b)/(m-m2)

			double x = (b->getYInt() - a->getYInt()) / (a->getSlope() - b->getSlope());
			double y = (a->getSlope() * x) + a->getYInt();

			//now check that the value is in both a's and b's domain

			if (x >= a->getMinX() && x <= a->getMaxX())
			{
				if (x >= b->getMinX() && x <= b->getMaxX())
				{
					return true;
				}
			}
		}
		else
		{
			//b is vertical
			double x = b->getMinX();

			if (x >= a->getMinX() && x <= a->getMaxX())
			{
				double y = a->getSlope() * x + a->getYInt();
				if (y >= b->getMinY() && y <= b->getMaxY())
				{
					return true;
				}
			}
		}
	}
	else
	{
		//a is vertical
		if (b->getMinX() != b->getMinX())
		{
			//b is a normal line
			double x = a->getMinX();

			if (x >= b->getMinX() && x <= b->getMaxX())
			{
				double y = b->getSlope() * x + b->getYInt();
				if (y >= a->getMinY() && y <= a->getMaxY())
				{
					return true;
				}
			}
		}
		else
		{
			//b is vertical
			//almost exactly like an AABB
			if (a->getMinX() == b->getMinX())
			{
				if (a->getMinY() <= b->getMaxY() && a->getMaxY() >= b->getMinY())
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CollisionMaster::collisionMethod(Line2D* a, Triangle2D* b)
{
	return false;
}

bool CollisionMaster::collisionMethod(Triangle2D* a, Triangle2D* b)
{
	return false;
}

void CollisionMaster::setExternalCollisionFunction(bool(*extFunction)(Shape* a, Shape* b))
{
	extCollision = extFunction;
}

#pragma endregion

