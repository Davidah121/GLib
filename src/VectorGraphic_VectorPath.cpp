#include "VectorGraphic.h"
#include "Graphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

#pragma region VectorPath
const Class VectorPath::myClass = Class("VectorPath", {&VectorShape::myClass});
const Class* VectorPath::getClass()
{
	return &VectorPath::myClass;
}

VectorPath::VectorPath() : VectorShape()
{
	addMoveTo(0,0);
}

VectorPath::VectorPath(const VectorPath& other) : VectorShape()
{
	//StringTools::println("Copy Constructor");
	copy((VectorPath&)other);
}

void VectorPath::operator=(VectorPath& other)
{
	//StringTools::println("Assignment Operator");
	copy(other);
}

void VectorPath::copy(VectorPath& other)
{
	//StringTools::println("Copy Function");
	this->setFillColor( other.getFillColor() );
	this->setStrokeColor( other.getStrokeColor() );
	this->setFillMethod( other.getFillMethod() );
	this->setStrokeWidth( other.getStrokeWidth() );
	this->setTransform( other.getTransform() );
	this->setLineCap( other.getLineCap() );
	this->setLineJoin( other.getLineJoin() );

	for(PathCommand com : other.commands)
	{
		PathCommand q;
		q.c = com.c;
		switch ( std::tolower(com.c))
		{
		case 'm':
			q.points = new Vec2f[1]{ com.points[0] };
			break;
		case 'l':
			q.points = new Vec2f[1]{ com.points[0] };
			break;
		case 'h':
			q.points = new Vec2f[1]{ com.points[0] };
			break;
		case 'v':
			q.points = new Vec2f[1]{ com.points[0] };
			break;
		case 'q':
			q.points = new Vec2f[2]{ com.points[0], com.points[1] };
			break;
		case 't':
			q.points = new Vec2f[1]{ com.points[0] };
			break;
		case 'c':
			q.points = new Vec2f[3]{ com.points[0], com.points[1], com.points[2] };
			break;
		case 's':
			q.points = new Vec2f[2]{ com.points[0], com.points[1] };
			break;
		case 'a':
			q.points = new Vec2f[3]{ com.points[0], com.points[1], com.points[2] };
			break;
		case 'z':
			q.points = nullptr;
			break;
		default:
			break;
		}
		this->commands.push_back(q);
	}
}

VectorPath::~VectorPath()
{
	clear();
}

void VectorPath::draw(Image* img, int globalWidth, int globalHeight)
{
	//copy to old vectorpath
	VectorPath copyVal = VectorPath(*this);

	applyTransform();

	Vec2f currentPos;
	Vec2f extraPos;
	Vec2f closeTo;

	int minY = 0;
	int maxY = globalHeight-1;
	int yDis = 1+maxY-minY;

	bool endOfCommands=false;
	
	std::vector<criticalPoint>* scanLines = new std::vector<criticalPoint>[yDis]();
	std::vector<int>* strokeScanLines = new std::vector<int>[yDis]();

	for(int i=0; i<commands.size(); i++)
	{
		switch(commands[i].c)
		{
			case 'M':
				//no drawing
				currentPos = *(commands[i].points);
				closeTo = currentPos;
				break;
			case 'm':
				//no drawing
				currentPos += *(commands[i].points);
				closeTo = currentPos;
				break;
			case 'L':
				//record
				drawLineTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				currentPos = *(commands[i].points);
				break;
			case 'l':
				//record
				drawLineTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				currentPos += *(commands[i].points);
				break;
			case 'H':
				//no drawing
				//drawHorizontalTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				drawLineTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				currentPos += *(commands[i].points);
				break;
			case 'h':
				//no drawing
				//drawHorizontalTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				drawLineTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				currentPos += *(commands[i].points);
				break;
			case 'V':
				//record
				//drawVerticalTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				drawLineTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				currentPos += *(commands[i].points);
				break;
			case 'v':
				//record
				//drawVerticalTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				drawLineTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				currentPos += *(commands[i].points);
				break;
			case 'Q':
				//record
				drawQuadTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				extraPos = commands[i].points[0];
				currentPos = commands[i].points[1];
				break;
			case 'q':
				//record
				drawQuadTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				extraPos = currentPos+commands[i].points[0];
				currentPos += commands[i].points[1];
				break;
			case 'T':
				//record
				if(commands[i-1].c == 'Q' || commands[i-1].c == 'q' || commands[i-1].c == 'T' || commands[i-1].c == 't')
				{
					extraPos = currentPos + (currentPos-extraPos);
				}
				else
				{
					extraPos = currentPos;
				}
				drawQuadShortTo(currentPos, extraPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);

				currentPos = commands[i].points[0];
				break;
			case 't':
				if(commands[i-1].c == 'Q' || commands[i-1].c == 'q' || commands[i-1].c == 'T' || commands[i-1].c == 't')
				{
					extraPos = currentPos + (currentPos-extraPos);
				}
				else
				{
					extraPos = currentPos;
				}
				drawQuadShortTo(currentPos, extraPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				
				currentPos += commands[i].points[0];
				break;
			case 'C':
				//record
				drawCubicTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				extraPos = commands[i].points[1];
				currentPos = commands[i].points[2];
				break;
			case 'c':
				//record
				drawCubicTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				extraPos = currentPos + commands[i].points[1];
				currentPos += commands[i].points[2];
				break;
			case 'S':
				//record, check previous index as well
				if(commands[i-1].c == 'S' || commands[i-1].c == 's' || commands[i-1].c == 'C' || commands[i-1].c == 'c')
					extraPos = currentPos + (currentPos-extraPos);
				else
					extraPos = currentPos;

				drawCubicShortTo(currentPos, extraPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				extraPos = commands[i].points[0];
				currentPos = commands[i].points[1];
				break;
			case 's':
				//record, check previous index as well
				if(commands[i-1].c == 'S' || commands[i-1].c == 's' || commands[i-1].c == 'C' || commands[i-1].c == 'c')
					extraPos = currentPos + (currentPos-extraPos);
				else
					extraPos = currentPos;
					
				drawCubicShortTo(currentPos, extraPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				extraPos = currentPos + commands[i].points[0];
				currentPos += commands[i].points[1];
				break;
			case 'A':
				//record. Also difficult
				drawArcTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				currentPos = commands[i].points[2];
				break;
			case 'a':
				//record. Also difficult
				drawArcTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				currentPos += commands[i].points[2];
				break;
			case 'Z':
				//record
				drawCloseTo(currentPos, closeTo, minY, maxY, scanLines, strokeScanLines);
				currentPos = closeTo;
				break;
			default:
				endOfCommands=true;
				break;
		}

		if(endOfCommands)
			break;
	}


	for(int i=0; i<yDis; i++)
	{
		if(scanLines[i].size()>0)
		{
			Sort::insertionSort<criticalPoint>(scanLines[i].data(), scanLines[i].size(), [](criticalPoint a, criticalPoint b) -> bool {return a.xValue<b.xValue;});
			
			// StringTools::out << "Scanline " << i << ": ";

			// for(int k=0; k<scanLines[i].size(); k++)
			// {
			// 	StringTools::out << "(" << scanLines[i][k].xValue << ", " << scanLines[i][k].isYPositive << ") ";
			// }
			// StringTools::println("");
			
			//rule, can not be the same if you are filling to it.
			//different for even-odd and non-zero
			std::vector<criticalPoint> newScanLine = std::vector<criticalPoint>();

			if(getFillMethod() == VectorShape::EVEN_ODD_RULE)
			{
				bool lowestPoint = true;
				for(int j=1; j<scanLines[i].size(); j+=2)
				{
					criticalPoint p1 = scanLines[i][j-1];
					criticalPoint p2 = scanLines[i][j];
					bool oldDir = p1.isYPositive;
					bool newDir = p2.isYPositive;

					if(newDir!=oldDir)
					{
						newScanLine.push_back(p1);
						newScanLine.push_back(p2);
					}
					else
					{
						if(lowestPoint)
							newScanLine.push_back(p1);
						else
							newScanLine.push_back(p2);

						lowestPoint = !lowestPoint;
					}
				}
			}
			else
			{
				newScanLine.push_back(scanLines[i][0]);
				for(int j=1; j<scanLines[i].size(); j++)
				{
					if(scanLines[i][j].xValue != scanLines[i][j-1].xValue)
					{
						newScanLine.push_back(scanLines[i][j]);
					}
					else
					{
						bool newDir = scanLines[i][j].isYPositive;
						bool oldDir = scanLines[i][j-1].isYPositive;

						if(newDir != oldDir)
						{
							newScanLine.push_back(scanLines[i][j]);
						}
						
					}
				}
			}

			scanLines[i] = newScanLine;

			// StringTools::out << "Scanline " << i << ": ";

			// for(int k=0; k<scanLines[i].size(); k++)
			// {
			// 	StringTools::out << scanLines[i][k].xValue << ", ";
			// }
			// StringTools::println("");
		}

		
	}

	if(getFillMethod()==VectorShape::EVEN_ODD_RULE)
	{
		//even-odd rule. Fill between even to odd indicies
		//not odd to even indicies
		for(int j=0; j<yDis; j++)
		{
			if(scanLines[j].size()>1)
			{
				int actualSize = scanLines[j].size();
				if(actualSize%2 == 1)
				{
					actualSize-=2;
				}
				else
				{
					actualSize-=1;
				}
				
				for(int i=0; i<actualSize; i+=2)
				{
					//fill between spots
					double startX = scanLines[j][i].xValue;
					double endX = scanLines[j][i+1].xValue;

					int fillX = MathExt::floor(scanLines[j][i].xValue);
					double yVal = j+minY;
					int intYVal = (int)j+minY;

					Color col = getFillColor();
					
					img->drawPixel(startX, yVal, col);
					fillX++;
					while(fillX < endX)
					{
						img->drawPixel(fillX, intYVal, col);
						fillX++;
					}
					img->drawPixel(endX, yVal, col);
				}
			}
		}
		
	}
	else
	{
		//non-zero rule. Fill when positive only. Not when zero or
		//less.
		for(int j=0; j<yDis; j++)
		{
			int passCounter = 0;
			if(scanLines[j].size()>1)
			{
				for(int i=0; i<scanLines[j].size()-1; i++)
				{
					//fill between spots
					double startX = scanLines[j][i].xValue;
					double endX = scanLines[j][i+1].xValue;

					bool dir = scanLines[j][i].isYPositive;

					double yVal = j+minY;
					int intYVal = (int)j+minY;

					if(dir == true)
					{
						//positive y direction
						passCounter++;
					}
					else
					{
						//negative y direction
						passCounter--;
					}
					
					if(passCounter!=0)
					{
						int fillX = startX;
						Color col = getFillColor();
						
						img->drawPixel(startX, yVal, col);
						fillX++;
						while(fillX < endX)
						{
							img->drawPixel(fillX, intYVal, col);
							fillX++;
						}
						img->drawPixel(endX, yVal, col);
					}
				}
			}
		}
	}

	//strokes
	/*
	for(int j=0; j<yDis; j++)
	{
		if(scanLines[j].size()>1)
		{
			for(int i=0; i<scanLines[j].size()-1; i++)
			{
				double halfWidth = getStrokeWidth()/2;
				int startX = scanLines[j][i].xValue;
				int endX = scanLines[j][i+1].xValue;

				if(scanLines[j][i].horizontalLine==true && scanLines[j][i+1].horizontalLine==true)
				{
					int tempFillX = MathExt::ceil(startX-halfWidth);
					int tempEndX = MathExt::floor(endX+halfWidth);

					int tempYStart = MathExt::ceil(j+minY-halfWidth);
					int tempYEnd = MathExt::floor(j+minY+halfWidth);

					do
					{
						for(int y = tempYStart; y<=tempYEnd; y++)
						{
							img->drawPixel(tempFillX, y, getStrokeColor());
						}
						tempFillX++;
					} while (tempFillX <= tempEndX);
				}
				
				
				//end points stroke
				Vec2f strokeDir = MathExt::normalize(MathExt::inverseVec(scanLines[j][i].direction));
				Vec2f strokeDir2 = MathExt::normalize(MathExt::inverseVec(scanLines[j][i+1].direction));

				for(double strokeLocation=-halfWidth; strokeLocation<halfWidth; strokeLocation+=1.0)
				{
					int p1StartX = MathExt::ceil(startX+(strokeDir.x*strokeLocation));
					int p1EndX = MathExt::floor(endX+(strokeDir2.x*strokeLocation));

					int p1StartY = MathExt::ceil(j+minY+(strokeDir.y*strokeLocation));
					int p1EndY = MathExt::floor(j+minY+(strokeDir2.y*strokeLocation));

					img->drawPixel(p1StartX, p1StartY, getStrokeColor());
					img->drawPixel(p1EndX, p1EndY, getStrokeColor());
				}
			}
		}
		
	}
	*/

	copy(copyVal);
}

void VectorPath::drawStroke(Image* buffer, int globalWidth, int globalHeight)
{
}

#pragma region PATH_DRAW_FUNCTIONS

void VectorPath::drawLineTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	int actualMinY = 0;
	int actualMaxY = 0;

	BezierCurve b = BezierCurve();
	b.addPoint(currentPos);

	if(relative==false)
	{
		b.addPoint(*(command.points));

		actualMinY = MathExt::ceil(MathExt::min( currentPos.y, command.points->y));
		actualMaxY = MathExt::floor(MathExt::max( currentPos.y, command.points->y));
	}
	else
	{
		b.addPoint(currentPos + *(command.points));

		actualMinY = MathExt::ceil(MathExt::min( currentPos.y, currentPos.y + command.points->y));
		actualMaxY = MathExt::floor(MathExt::max( currentPos.y, currentPos.y + command.points->y));
	}

	// StringTools::out << "currentPos: (" << currentPos.x << ", " << currentPos.y << ")";
	// StringTools::out << "otherPos: (" << currentPos.x+(*(command.points)).x << ", " << currentPos.y +(*(command.points)).y<< ")" << StringTools::lineBreak;
	
	
	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

	for(int y=actualMinY; y<=actualMaxY; y++)
	{
		std::vector<double> times;
		//The line is not a horizontal line. Scanlines are handled separately
		if(actualMinY!=actualMaxY)
			times = b.findTimeForY(y);
		
		for(int t=0; t<times.size(); t++)
		{
			Vec2f v = b.getFuctionAt(times[t]);
			//because it is only 2 points, we can use get simple derivative and it will be accurate.
			Vec2f dir = b.getSimpleDerivativeAt(times[t]);
			
			scanLines[y].push_back({v.x, dir.y>0, 1});
		}
	}

	//Stroke drawing
	if(getStrokeWidth()>0)
	{
		double halfStrokeWidth = getStrokeWidth()/2;
		BezierCurve strokeCurve1 = BezierCurve();
		BezierCurve strokeCurve2 = BezierCurve();
		BezierCurve strokeCurveConn1 = BezierCurve();
		BezierCurve strokeCurveConn2 = BezierCurve();
		Vec2f invVec = MathExt::normalize(MathExt::inverseVec(b.getSimpleDerivativeAt(0))) * halfStrokeWidth;

		strokeCurve1.addPoint( b.getPoint(0) + invVec );
		strokeCurve2.addPoint( b.getPoint(0) - invVec );

		strokeCurve1.addPoint( b.getPoint(1) + invVec );
		strokeCurve2.addPoint( b.getPoint(1) - invVec );

		strokeCurveConn1.addPoint( strokeCurve1.getPoint(0) );
		strokeCurveConn1.addPoint( strokeCurve2.getPoint(0) );

		strokeCurveConn2.addPoint( strokeCurve1.getPoint(1) );
		strokeCurveConn2.addPoint( strokeCurve2.getPoint(1) );
		
		actualMinY = MathExt::ceil(MathExt::min( strokeCurve1.getPoint(0).y, strokeCurve1.getPoint(1).y));
		actualMaxY = MathExt::floor(MathExt::max( strokeCurve1.getPoint(0).y, strokeCurve1.getPoint(1).y));
	}
}

void VectorPath::drawVerticalTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	int actualMinY, actualMaxY;
	if(!relative)
	{
		actualMinY = MathExt::ceil(MathExt::min( currentPos.y, command.points->y));
		actualMaxY = MathExt::floor(MathExt::max( currentPos.y, command.points->y));
	}
	else
	{
		actualMinY = MathExt::ceil(MathExt::min( currentPos.y, currentPos.y+command.points->y));
		actualMaxY = MathExt::floor(MathExt::max( currentPos.y, currentPos.y+command.points->y));
	}
	
	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

	if(!relative)
	{
		for(int y=actualMinY; y<=actualMaxY; y++)
		{
			scanLines[y].push_back({currentPos.x, (command.points->y - currentPos.y) >0, 1});
		}
	}
	else
	{
		for(int y=actualMinY; y<=actualMaxY; y++)
		{
			scanLines[y].push_back({currentPos.x, command.points->y >0, 1});
		}
	}
	
}

void VectorPath::drawHorizontalTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	if(getStrokeWidth()>0)
	{
		double halfStrokeWidth = getStrokeWidth()/2;
		//stroke stuff
		int actualMinY = MathExt::clamp((int)MathExt::ceil(currentPos.y-halfStrokeWidth ), minY, maxY);
		int actualMaxY = MathExt::clamp((int)MathExt::floor(currentPos.y+halfStrokeWidth ), minY, maxY);

		if(!relative)
		{
			for(int y=actualMinY; y<=actualMaxY; y++)
			{
				strokeScanLines[y].push_back((int)MathExt::round(currentPos.x));
				strokeScanLines[y].push_back((int)MathExt::round(command.points->x));
			}
		}
		else
		{
			for(int y=actualMinY; y<=actualMaxY; y++)
			{
				strokeScanLines[y].push_back((int)MathExt::round(currentPos.x));
				strokeScanLines[y].push_back((int)MathExt::round(currentPos.x+command.points->x));
			}
		}
	}
}

void VectorPath::drawQuadTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	BezierCurve b = BezierCurve();
	b.addPoint(currentPos);

	int actualMinY = MathExt::ceil(currentPos.y);
	int actualMaxY = MathExt::floor(currentPos.y);

	for(int j=0; j<2; j++)
	{
		if(!relative)
		{
			b.addPoint(command.points[j]);
			if(command.points[j].y < actualMinY)
			{
				actualMinY = MathExt::ceil(command.points[j].y);
			}
			if(command.points[j].y > actualMaxY)
			{
				actualMaxY = MathExt::floor(command.points[j].y);
			}
		}
		else
		{
			b.addPoint(currentPos+command.points[j]);
			if(currentPos.y+command.points[j].y < actualMinY)
			{
				actualMinY = MathExt::ceil(currentPos.y+command.points[j].y);
			}
			if(currentPos.y+command.points[j].y > actualMaxY)
			{
				actualMaxY = MathExt::floor(currentPos.y+command.points[j].y);
			}
		}
	}

	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

	for(int y=actualMinY; y<=actualMaxY; y++)
	{
		std::vector<double> times = b.findTimeForY(y);
		for(int t=0; t<times.size(); t++)
		{
			Vec2f v = b.getFuctionAt(times[t]);
			Vec2f dir = b.getDerivativeAt(times[t]);
			
			scanLines[y].push_back({v.x, dir.y >0, 1});
		}
	}
}

void VectorPath::drawQuadShortTo(Vec2f currentPos, Vec2f extraPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	BezierCurve b = BezierCurve();
	b.addPoint(currentPos);

	if(extraPos!=currentPos)
	{
		b.addPoint(extraPos);
	}
	
	if(!relative)
		b.addPoint(command.points[0]);
	else
		b.addPoint(currentPos+command.points[0]);

	int actualMinY = MathExt::ceil(currentPos.y);
	int actualMaxY = MathExt::floor(currentPos.y);
	for(int j=0; j<b.size(); j++)
	{
		if(b.getPoint(j).y < actualMinY)
		{
			actualMinY = MathExt::ceil(b.getPoint(j).y);
		}
		if(b.getPoint(j).y > actualMaxY)
		{
			actualMaxY = MathExt::floor(b.getPoint(j).y);
		}
	}

	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);
	
	for(int y=actualMinY; y<=actualMaxY; y++)
	{
		std::vector<double> times = b.findTimeForY(y);
		for(int t=0; t<times.size(); t++)
		{
			Vec2f v = b.getFuctionAt(times[t]);
			Vec2f dir = b.getDerivativeAt(times[t]);
			
			scanLines[y].push_back({v.x, dir.y>0, 1});
		}
	}
}

void VectorPath::drawCubicTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	BezierCurve b = BezierCurve();
	b.addPoint(currentPos);

	int actualMinY = MathExt::ceil(currentPos.y);
	int actualMaxY = MathExt::floor(currentPos.y);

	for(int j=0; j<3; j++)
	{
		if(!relative)
		{
			b.addPoint(command.points[j]);
			if(command.points[j].y < actualMinY)
			{
				actualMinY = MathExt::ceil(command.points[j].y);
			}
			if(command.points[j].y > actualMaxY)
			{
				actualMaxY = MathExt::floor(command.points[j].y);
			}
		}
		else
		{
			b.addPoint(currentPos+command.points[j]);
			if(currentPos.y+command.points[j].y < actualMinY)
			{
				actualMinY = MathExt::ceil(currentPos.y+command.points[j].y);
			}
			if(currentPos.y+command.points[j].y > actualMaxY)
			{
				actualMaxY = MathExt::floor(currentPos.y+command.points[j].y);
			}
		}
	}

	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

	for(int y=actualMinY; y<=actualMaxY; y++)
	{
		std::vector<double> times = b.findTimeForY(y);
		
		for(int t=0; t<times.size(); t++)
		{
			Vec2f v = b.getFuctionAt(times[t]);
			Vec2f dir = b.getDerivativeAt(times[t]);
			
			scanLines[y].push_back({v.x, dir.y>0, 1});
		}
	}
}

void VectorPath::drawCubicShortTo(Vec2f currentPos, Vec2f extraPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	BezierCurve b = BezierCurve();
	b.addPoint(currentPos);
	if(extraPos!=currentPos)
	{
		b.addPoint(extraPos);
	}
	if(!relative)
	{
		b.addPoint(command.points[0]);
		b.addPoint(command.points[1]);
	}
	else
	{
		b.addPoint(currentPos+command.points[0]);
		b.addPoint(currentPos+command.points[1]);
	}

	int actualMinY = currentPos.y;
	int actualMaxY = currentPos.y;
	for(int j=1; j<b.size(); j++)
	{
		if(b.getPoint(j).y < actualMinY)
		{
			actualMinY = MathExt::ceil(b.getPoint(j).y);
		}
		if(b.getPoint(j).y > actualMaxY)
		{
			actualMaxY = MathExt::floor(b.getPoint(j).y);
		}
	}

	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

	for(int y=actualMinY; y<=actualMaxY; y++)
	{
		std::vector<double> times = b.findTimeForY(y);
		for(int t=0; t<times.size(); t++)
		{
			Vec2f v = b.getFuctionAt(times[t]);
			Vec2f dir = b.getDerivativeAt(times[t]);
			
			scanLines[y].push_back({v.x, dir.y>0, 1});
		}
	}
}

void VectorPath::drawArcTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	//first, parse information from command
	Vec2f arcRadi = command.points[0];
	double arcAngle = command.points[1].x;
	Vec2f arcEndPos = command.points[2];

	if(relative)
	{
		arcEndPos += currentPos;
	}

	double xDir = arcEndPos.x - currentPos.x;
	double midPointX = (currentPos.x + arcEndPos.x)/2;
	//solves arc system. Returns an array of vectors
	//arcInfo[0] is the final radi of the ellipse. If it did not need to be expanded, this will be the same
	//arcInfo[1] is one of the position for the 2 ellipses
	//arcInfo[2] is the other position for the 2 ellipses
	std::vector<Vec2f> arcInfo = getArcStuff(arcRadi, arcAngle, currentPos, arcEndPos);

	//isSweep determines whether you go clockwise or counter-clockwise
	//isLargeArc picks the larger of the 2 options you have
	Vec2f finalArcPosition;
	bool isLargeArc = command.points[1].y >= 2.0;
	bool isSweep = (command.points[1].y==1.0 || command.points[1].y==3.0);

	if(isSweep)
	{
		//counter-clockwise
		if(isLargeArc)
		{
			if(xDir>0)
			{
				if(arcInfo[1].x > midPointX)
					finalArcPosition = arcInfo[1];
				else
					finalArcPosition = arcInfo[2];
			}
			else
			{
				if(arcInfo[1].x < midPointX)
					finalArcPosition = arcInfo[1];
				else
					finalArcPosition = arcInfo[2];
			}
		}
		else
		{
			if(xDir>0)
			{
				if(arcInfo[1].x < midPointX)
					finalArcPosition = arcInfo[1];
				else
					finalArcPosition = arcInfo[2];
			}
			else
			{
				if(arcInfo[1].x > midPointX)
					finalArcPosition = arcInfo[1];
				else
					finalArcPosition = arcInfo[2];
			}
		}
	}
	else
	{
		//clockwise
		if(isLargeArc)
		{
			if(xDir>0)
			{
				if(arcInfo[1].x < midPointX)
					finalArcPosition = arcInfo[1];
				else
					finalArcPosition = arcInfo[2];
			}
			else
			{
				if(arcInfo[1].x > midPointX)
					finalArcPosition = arcInfo[1];
				else
					finalArcPosition = arcInfo[2];
			}
		}
		else
		{
			if(xDir>0)
			{
				if(arcInfo[1].x > midPointX)
					finalArcPosition = arcInfo[1];
				else
					finalArcPosition = arcInfo[2];
			}
			else
			{
				if(arcInfo[1].x < midPointX)
					finalArcPosition = arcInfo[1];
				else
					finalArcPosition = arcInfo[2];
			}
		}
	}

	int actualMinY, actualMaxY, radiY;

	//check if both solutions are the same
	if(arcInfo[1] == arcInfo[2])
	{
		//will always be a large arc
		if(isSweep)
		{
			actualMinY = (int)MathExt::ceil(finalArcPosition.y-MathExt::max(arcInfo[0].y, arcInfo[0].x));
			actualMaxY = (int)MathExt::floor(MathExt::max(currentPos.y, arcEndPos.y));
		}
		else
		{
			actualMinY = (int)MathExt::ceil(MathExt::min(currentPos.y, arcEndPos.y));
			actualMaxY = (int)MathExt::floor(finalArcPosition.y+MathExt::max(arcInfo[0].y, arcInfo[0].x));
		}
	}
	else
	{
		if(!isLargeArc)
		{
			actualMinY = (int)MathExt::ceil(MathExt::min(currentPos.y, arcEndPos.y));
			actualMaxY = (int)MathExt::floor(MathExt::max(currentPos.y, arcEndPos.y));
		}
		else
		{
			actualMinY = (int)MathExt::ceil(finalArcPosition.y-MathExt::max(arcInfo[0].y, arcInfo[0].x));
			actualMaxY = (int)MathExt::floor(finalArcPosition.y+MathExt::max(arcInfo[0].y, arcInfo[0].x));
		}
	}

	actualMinY = MathExt::clamp(actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp(actualMaxY, minY, maxY);

	BezierCurve separatingLine = BezierCurve();
	separatingLine.addPoint(currentPos);
	separatingLine.addPoint(arcEndPos);

	bool minSide = (arcEndPos - currentPos).y < 0;
	bool slopeOfSeparatingLine = !minSide;
	if(!isSweep)
	{
		minSide = !minSide;
	}

	//distance from the center of the arc to where we start.
	radiY = actualMinY - finalArcPosition.y;

	for(int y=actualMinY; y<=actualMaxY; y++, radiY++)
	{
		//get x points for the circle
		#pragma region Math_Behind_The_Constants
		// ((cos(0)*x + sin(0)*y)^2 / rx^2) + ((cos(0)*y - sin(0)*x)^2 / ry^2) = 1
		// ((c*x + A)^2 / rx^2) + ((B - s*x)^2 / ry^2) = 1
		// ((c^2*x^2 + 2cAx + A^2) / rx^2 ) + ((B^2 -2sBx + s^2*x^2) / ry^2) = 1
		// (c^2/rx^2 + s^2/ry^2)x^2 + (2cA/rx^2 - 2sB/ry^2)x + (A^2/rx^2 + B^2/ry^2 - 1) = 0
		//group into constants
		//C1 = cos^2(angle)/rx^2 + sin^2(angle)/ry^2
		//C2 = 2cos(angle)sin(angle)y/rx^2 - 2sin(angle)cos(angle)y/ry^2
		//C3 = (sin(angle)y)^2/rx^2 + (cos(angle)y)^2/ry^2 - 1
		#pragma endregion

		double sinValue = MathExt::sin(MathExt::toRad(arcAngle));
		double cosValue = MathExt::cos(MathExt::toRad(arcAngle));
		double C1 = MathExt::sqr(cosValue/arcRadi.x) + MathExt::sqr(sinValue/arcRadi.y);
		double C2 = 2.0*cosValue*sinValue*radiY * (1.0/MathExt::sqr(arcRadi.x) - 1.0/MathExt::sqr(arcRadi.y));
		double C3 = MathExt::sqr( sinValue*radiY / arcRadi.x ) + MathExt::sqr( cosValue*radiY / arcRadi.y ) - 1.0;

		std::vector<double> solutions = MathExt::solveQuadraticReal(C1, C2, C3);
		if(solutions.size()==2)
		{
			//fill from min to max
			//deal with direction and large arc

			double minX = finalArcPosition.x + MathExt::min(solutions[0], solutions[1]);
			double maxX = finalArcPosition.x + MathExt::max(solutions[0], solutions[1]);
			
			bool isMinXSlopePos = false;
			bool isMaxXSlopePos = false;

			//Since we only care if the y slope is greater than 0, this shortcut can be taken.
			//For all ellipses, if the x value is less than the center point, the y slope will be negative.
			//if the x value is greater than the center point, the y slope will be positive.
			
			std::vector<double> times = separatingLine.findTimeForY(y);

			if(minSide)
			{
				//change max x
				bool isPos = minX>finalArcPosition.x;
				
				if(times.size() > 0)
				{
					double solveX = separatingLine.getFuctionAt(times[0]).x;

					if((int)MathExt::ceil(minX) < solveX)
					{
						scanLines[y].push_back({minX, isPos, 1});
						scanLines[y].push_back({solveX, !isPos, 1});
					}
					else
					{
						scanLines[y].push_back({solveX, isPos, 1});
						scanLines[y].push_back({solveX, !isPos, 1});
					}
				}
				else
				{
					scanLines[y].push_back({minX, isPos, 1});
					scanLines[y].push_back({maxX, !isPos, 1});
				}
			}
			else
			{
				//change min x
				bool isPos = maxX>finalArcPosition.x;

				if(times.size() > 0)
				{
					double solveX = separatingLine.getFuctionAt(times[0]).x;

					if((int)MathExt::ceil(maxX) > solveX)
					{
						scanLines[y].push_back({maxX, isPos, 1});
						scanLines[y].push_back({solveX, !isPos, 1});
					}
					else
					{
						scanLines[y].push_back({solveX, isPos, 1});
						scanLines[y].push_back({solveX, !isPos, 1});
					}
				}
				else
				{
					scanLines[y].push_back({maxX, isPos, 1});
					scanLines[y].push_back({minX, !isPos, 1});
				}
			}
			
			/*
			if(!isSweep)
			{
				//change max x
				scanLines[y].push_back({(int)MathExt::ceil(minX), minX>finalArcPosition.x});
				if(times.size() > 0)
				{
					//maxX = MathExt::max(maxX, separatingLine.getFuctionAt(times[0]).x);
				}
				else
				{
					scanLines[y].push_back({(int)MathExt::floor(maxX), maxX>finalArcPosition.x});
				}
			}
			else
			{
				//change min x
				scanLines[y].push_back({(int)MathExt::floor(maxX), maxX>finalArcPosition.x});
				if(times.size() > 0)
				{
					//minX = MathExt::min(minX, separatingLine.getFuctionAt(times[0]).x);
				}
				else
				{
					scanLines[y].push_back({(int)MathExt::ceil(minX), minX>finalArcPosition.x});
				}
			}
			*/
			
		}
		else
		{
			//Outside of the bounds of the ellipse. Do nothing.
		}
	}				
}

void VectorPath::drawCloseTo(Vec2f currentPos, Vec2f closePoint, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines)
{
	BezierCurve b = BezierCurve();
	b.addPoint(currentPos);
	b.addPoint(closePoint);

	int actualMinY = MathExt::ceil(MathExt::min( currentPos.y, closePoint.y));
	int actualMaxY = MathExt::floor(MathExt::max( currentPos.y, closePoint.y));

	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

	for(int y=actualMinY; y<=actualMaxY; y++)
	{
		std::vector<double> times = b.findTimeForY(y);
		for(int t=0; t<times.size(); t++)
		{
			Vec2f v = b.getFuctionAt(times[t]);
			Vec2f dir = b.getSimpleDerivativeAt(times[t]);
			
			scanLines[y].push_back({v.x, dir.y>0, 1});
		}
	}
}
	

#pragma endregion

void VectorPath::addMoveTo(double x, double y)
{
	commands.push_back( {'M', new Vec2f[1]{Vec2f(x,y)} });
}

void VectorPath::addMoveTo(Vec2f p)
{
	commands.push_back( {'M', new Vec2f[1]{p} });
}

void VectorPath::addMoveToRel(double x, double y)
{
	commands.push_back( {'m', new Vec2f[1]{Vec2f(x,y)} });
}

void VectorPath::addMoveToRel(Vec2f p)
{
	commands.push_back( {'m', new Vec2f[1]{p} });
}

void VectorPath::addLineTo(double x, double y)
{
	commands.push_back( {'L', new Vec2f[1]{Vec2f(x,y)} });
}

void VectorPath::addLineTo(Vec2f p)
{
	commands.push_back( {'L', new Vec2f[1]{p} });
}

void VectorPath::addLineToRel(double x, double y)
{
	commands.push_back( {'l', new Vec2f[1]{Vec2f(x,y)} });
}

void VectorPath::addLineToRel(Vec2f p)
{
	commands.push_back( {'l', new Vec2f[1]{p} });
}

void VectorPath::addHorizontalTo(double x)
{
	commands.push_back( {'H', new Vec2f[1]{Vec2f(x,0)} });
}

void VectorPath::addHorizontalToRel(double x)
{
	commands.push_back( {'h', new Vec2f[1]{Vec2f(x,0)} });
}

void VectorPath::addVerticalTo(double y)
{
	commands.push_back( {'V', new Vec2f[1]{Vec2f(0,y)} });
}

void VectorPath::addVerticalToRel(double y)
{
	commands.push_back( {'v', new Vec2f[1]{Vec2f(0,y)} });
}

void VectorPath::addClosePath()
{
	commands.push_back( {'Z', nullptr});
}

void VectorPath::addQuadTo(double x1, double y1, double x2, double y2)
{
	commands.push_back( {'Q', new Vec2f[2]{Vec2f(x1,y1), Vec2f(x2,y2)} });
}

void VectorPath::addQuadTo(Vec2f p1, Vec2f p2)
{
	commands.push_back( {'Q', new Vec2f[2]{p1, p2} });
}

void VectorPath::addQuadToRel(double x1, double y1, double x2, double y2)
{
	commands.push_back( {'q', new Vec2f[2]{Vec2f(x1,y1), Vec2f(x2,y2)} });
}

void VectorPath::addQuadToRel(Vec2f p1, Vec2f p2)
{
	commands.push_back( {'q', new Vec2f[2]{p1, p2} });
}

void VectorPath::addQuadToShort(double x, double y)
{
	commands.push_back( {'T', new Vec2f[1]{Vec2f(x,y)} });
}

void VectorPath::addQuadToShort(Vec2f p)
{
	commands.push_back( {'T', new Vec2f[1]{p} });
}

void VectorPath::addQuadToShortRel(double x, double y)
{
	commands.push_back( {'t', new Vec2f[1]{Vec2f(x,y)} });
}

void VectorPath::addQuadToShortRel(Vec2f p)
{
	commands.push_back( {'t', new Vec2f[1]{p} });
}

void VectorPath::addCubicTo(double x1, double y1, double x2, double y2, double x3, double y3)
{
	commands.push_back( {'C', new Vec2f[3]{Vec2f(x1,y1), Vec2f(x2,y2), Vec2f(x3,y3)} });
}

void VectorPath::addCubicTo(Vec2f p1, Vec2f p2, Vec2f p3)
{
	commands.push_back( {'C', new Vec2f[3]{p1, p2, p3} });
}

void VectorPath::addCubicToRel(double x1, double y1, double x2, double y2, double x3, double y3)
{
	commands.push_back( {'c', new Vec2f[3]{Vec2f(x1,y1), Vec2f(x2,y2), Vec2f(x3,y3)} });
}

void VectorPath::addCubicToRel(Vec2f p1, Vec2f p2, Vec2f p3)
{
	commands.push_back( {'c', new Vec2f[3]{p1, p2, p3} });
}

void VectorPath::addCubicToShort(double x1, double y1, double x2, double y2)
{
	commands.push_back( {'S', new Vec2f[2]{Vec2f(x1,y1), Vec2f(x2,y2)} });
}

void VectorPath::addCubicToShort(Vec2f p1, Vec2f p2)
{
	commands.push_back( {'S', new Vec2f[2]{p1, p2} });
}

void VectorPath::addCubicToShortRel(double x1, double y1, double x2, double y2)
{
	commands.push_back( {'s', new Vec2f[2]{Vec2f(x1,y1), Vec2f(x2,y2)} });
}

void VectorPath::addCubicToShortRel(Vec2f p1, Vec2f p2)
{
	commands.push_back( {'s', new Vec2f[2]{p1, p2} });
}

void VectorPath::addArcTo(double rx, double ry, double angle, bool largeArcFlag, bool sweepFlag, double x, double y)
{
	//pack into 3 vectors
	double comboVal = (double)largeArcFlag*2 + (double)sweepFlag; //0.0=0,0 - 1.0=0,1 - 2.0=1,0 - 3.0=1,1
	commands.push_back( {'A', new Vec2f[3]{Vec2f(rx,ry), Vec2f(angle,comboVal), Vec2f(x,y)} });
}

void VectorPath::addArcTo(Vec2f radi, double angle, bool largeArcFlag, bool sweepFlag, Vec2f p)
{
	//pack into 3 vectors
	double comboVal = (double)largeArcFlag*2 + (double)sweepFlag; //0.0=0,0 - 1.0=0,1 - 2.0=1,0 - 3.0=1,1
	commands.push_back( {'A', new Vec2f[3]{radi, Vec2f(angle,comboVal), p} });
}

void VectorPath::addArcToRel(double rx, double ry, double angle, bool largeArcFlag, bool sweepFlag, double x, double y)
{
	//pack into 3 vectors
	double comboVal = (double)largeArcFlag*2 + (double)sweepFlag; //0.0=0,0 - 1.0=0,1 - 2.0=1,0 - 3.0=1,1
	commands.push_back( {'a', new Vec2f[3]{Vec2f(rx,ry), Vec2f(angle,comboVal), Vec2f(x,y)} });
}

void VectorPath::addArcToRel(Vec2f radi, double angle, bool largeArcFlag, bool sweepFlag, Vec2f p)
{
	//pack into 3 vectors
	double comboVal = (double)largeArcFlag*2 + (double)sweepFlag; //0.0=0,0 - 1.0=0,1 - 2.0=1,0 - 3.0=1,1
	commands.push_back( {'a', new Vec2f[3]{radi, Vec2f(angle,comboVal), p} });
}

void VectorPath::clear()
{
	for(int i=0; i<commands.size(); i++)
	{
		if(commands[i].points!=nullptr)
			delete[] commands[i].points;
	}
	commands.clear();
}

int VectorPath::size()
{
	return commands.size();
}

std::vector<Vec2f> VectorPath::getArcStuff(Vec2f radi, double angle, Vec2f p1, Vec2f p2)
{
	//also center a point at zero to make things easier
	//p1 will be zero by default
	//first determine if the intersections make a vertical line.
	//if so, a divide by zero error can occur and requires a swap of variable.
	//both radii must not be 0.

	if(radi.x !=0 && radi.y !=0 )
	{
		Vec2f dir = p2-p1;

		double cosValue = MathExt::dcos(angle);
		double sinValue = MathExt::dsin(angle);

		double A = (MathExt::sqr( cosValue ) / MathExt::sqr(radi.x)) +  (MathExt::sqr( sinValue ) / MathExt::sqr(radi.y));
		double B = 2.0*cosValue*sinValue*(1.0/MathExt::sqr(radi.x) - 1.0/MathExt::sqr(radi.y));
		double C = (MathExt::sqr(sinValue) / MathExt::sqr(radi.x)) + (MathExt::sqr(cosValue) / MathExt::sqr(radi.y));
		
		double l1 = 2.0*A*dir.x + B*dir.y;
		double l2 = B*dir.x + 2.0*C*dir.y;
		double l3 = -(A*MathExt::sqr(dir.x)+ B*dir.x*dir.y + C*MathExt::sqr(dir.y));

		if(l1!=0)
		{
			//solve with respect to y
			double Q = -l2/l1;
			double D = -l3/l1;

			double n1 = A*MathExt::sqr(Q) + B*Q + C;
			double n2 = 2*A*Q*D + B*D;
			double n3 = A*MathExt::sqr(D) - 1;

			//check if it can be solved. If not, the midpoint is the center of both ellipses and the radius must be expanded
			std::vector<double> solutions = MathExt::solveQuadraticReal(n1,n2,n3);
			if(solutions.size()==0)
			{
				//couldn't be solved must expand radius
				Vec2f midPoint = (p1+p2)/2;
				Vec2f solvePoint = p2-midPoint;
				double F1 = cosValue*solvePoint.x + sinValue*solvePoint.y;
				double F2 = cosValue*solvePoint.y - sinValue*solvePoint.x;
				double ratio = radi.y/radi.x;

				double finalRadius = MathExt::sqrt( MathExt::sqr(F1) + MathExt::sqr( F2/ratio ) );

				return { Vec2f(finalRadius, ratio*finalRadius), midPoint, midPoint };
			}
			else
			{
				//solved, find x values
				double x1 = Q*solutions[0] + D;
				double x2 = Q*solutions[1] + D;
				
				//The points represent how far we should move from 0,0
				//Here, the original value is added back to fix the offset that was made originally
				//for easier computation

				Vec2f newP1 = p1 + Vec2f(x1,solutions[0]);
				Vec2f newP2 = p1 + Vec2f(x2,solutions[1]);
				
				return {radi, newP1, newP2};
			}
		}
		else
		{
			//solve with respect to x
			double Q = -l1/l2;
			double D = -l3/l2;

			double n1 = A*MathExt::sqr(Q) + B*Q + C;
			double n2 = 2*A*Q*D + B*D;
			double n3 = A*MathExt::sqr(D) - 1;

			//check if it can be solved. If not, the midpoint is the center of both ellipses and the radius must be expanded
			std::vector<double> solutions = MathExt::solveQuadraticReal(n1,n2,n3);
			if(solutions.size()==0)
			{
				//couldn't be solved must expand radius
				Vec2f midPoint = (p1+p2)/2;
				Vec2f solvePoint = p2-midPoint;
				double F1 = cosValue*solvePoint.x + sinValue*solvePoint.y;
				double F2 = cosValue*solvePoint.y - sinValue*solvePoint.x;
				double ratio = radi.y/radi.x;

				double finalRadius = MathExt::sqrt( MathExt::sqr(F1) + MathExt::sqr( F2/ratio ) );

				return { Vec2f(finalRadius, ratio*finalRadius), midPoint, midPoint };
			}
			else
			{
				//solved, find y values
				double y1 = Q*solutions[0] + D;
				double y2 = Q*solutions[1] + D;
				
				//The points represent how far we should move from 0,0
				//Here, the original value is added back to fix the offset that was made originally
				//for easier computation

				Vec2f newP1 = p1 + Vec2f(solutions[0],y1);
				Vec2f newP2 = p1 + Vec2f(solutions[1],y2);
				
				return {radi, newP1, newP2};
			}
		}
	}
	
	return {};
}

void VectorPath::applyTransform()
{
	for(PathCommand com : commands)
	{
		Vec3f pos;
		double translationToo = 1.0;
		if( std::islower(com.c) )
			translationToo = 0.0;

		switch ( std::tolower(com.c) )
		{
		case 'm':
			pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
			com.points[0] = Vec2f(pos.x, pos.y);
			break;
		case 'l':
			pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
			com.points[0] = Vec2f(pos.x, pos.y);
			break;
		case 'h':
			pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
			com.points[0] = Vec2f(pos.x, pos.y);
			break;
		case 'v':
			pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
			com.points[0] = Vec2f(pos.x, pos.y);
			break;
		case 'q':
			pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
			com.points[0] = Vec2f(pos.x, pos.y);
			pos = (getTransform() * Vec3f( com.points[1].x, com.points[1].y, translationToo ));
			com.points[1] = Vec2f(pos.x, pos.y);
			break;
		case 't':
			pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
			com.points[0] = Vec2f(pos.x, pos.y);
			break;
		case 'c':
			pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
			com.points[0] = Vec2f(pos.x, pos.y);
			pos = (getTransform() * Vec3f( com.points[1].x, com.points[1].y, translationToo ));
			com.points[1] = Vec2f(pos.x, pos.y);
			pos = (getTransform() * Vec3f( com.points[2].x, com.points[2].y, translationToo ));
			com.points[2] = Vec2f(pos.x, pos.y);
			break;
		case 's':
			pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
			com.points[0] = Vec2f(pos.x, pos.y);
			pos = (getTransform() * Vec3f( com.points[1].x, com.points[1].y, translationToo ));
			com.points[1] = Vec2f(pos.x, pos.y);
			break;
		case 'a':
			//Do thing
			pos = (getTransform() * Vec3f(com.points[0].x, com.points[0].y, 0.0));
			com.points[0] = Vec2f(pos.x, pos.y);

			pos = (getTransform() * Vec3f(1.0, 0.0, 0.0));
			com.points[1] = Vec2f(MathExt::darctan2(pos.y, pos.x), com.points[1].y);
			
			pos = (getTransform() * Vec3f(com.points[2].x, com.points[2].y, translationToo));
			com.points[2] = Vec2f(pos.x, pos.y);
			break;
		case 'z':
			//Do nothing
			break;
		default:
			break;
		}

	}
}

#pragma endregion
