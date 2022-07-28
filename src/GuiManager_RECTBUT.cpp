#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_RECTANGLE_BUTTON

	const Class GuiRectangleButton::globalClass = Class("GuiRectangleButton", {&GuiInstance::globalClass});

	GuiRectangleButton::GuiRectangleButton(int x, int y, int width, int height) : GuiInstance()
	{
		setClass(globalClass);
		setBaseX(x);
		setBaseY(y);
		this->width = width;
		this->height = height;
		onClickFunction = nullptr;
		onClickHoldFunction = nullptr;
		onClickReleaseFunction = nullptr;
		includeChildrenInBounds = true;

		boundingBox = Box2D(x, y, x+width, y+height);
	}

	GuiRectangleButton::GuiRectangleButton(const GuiRectangleButton& other) : GuiInstance(other)
	{
		copy(other);
	}

	void GuiRectangleButton::operator=(const GuiRectangleButton& other)
	{
		GuiInstance::copy(other);
		copy(other);
	}

	void GuiRectangleButton::copy(const GuiRectangleButton& other)
	{
		setClass(globalClass);
		onClickFunction = other.onClickFunction;
		onClickHoldFunction = other.onClickHoldFunction;
		onClickReleaseFunction = other.onClickReleaseFunction;

		width = other.width;
		height = other.height;

		hover = other.hover;

		backgroundColor = other.backgroundColor;
		hoverColor = other.hoverColor;
		focusBackgroundColor = other.focusBackgroundColor;

		outlineColor = other.outlineColor;
		focusOutlineColor = other.focusOutlineColor;
	}

	GuiRectangleButton::~GuiRectangleButton()
	{
	}

	void GuiRectangleButton::update()
	{
		int mouseX;
		int mouseY;

		bool oldHover = hover;
		bool oldFocus = getFocus();
		
		if(getManager()!=nullptr)
		{
			mouseX = getManager()->getMouseX();
			mouseY = getManager()->getMouseY();
		}

		
		if (mouseX >= x && mouseX <= x + width)
		{
			if (mouseY >= y && mouseY <= y + height)
			{
				hover = true;

				if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
				{
					setFocus(true);
					if (onClickFunction != nullptr)
						onClickFunction(this);
				}
			}
			else
			{
				hover = false;
			}
		}
		else
		{
			hover = false;
		}

		if(Input::getMouseUp(Input::LEFT_MOUSE_BUTTON))
		{
			if(hover && getFocus())
			{
				if(onClickReleaseFunction != nullptr)
					onClickReleaseFunction(this);
			}
			setFocus(false);
		}

		if(getFocus())
		{
			if (onClickHoldFunction != nullptr)
				onClickHoldFunction(this);
		}

		if(getFocus() != oldFocus)
			setShouldRedraw(true);
		
		if(hover != oldHover)
			setShouldRedraw(true);
		
		boundingBox = Box2D(x, y, x+width, y+height);
	}

	void GuiRectangleButton::render()
	{
		GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();
		
		if(!getFocus())
		{
			if(!hover)
				graphicsInterface->setColor(backgroundColor);
			else
				graphicsInterface->setColor(hoverColor);
		}
		else
		{
			graphicsInterface->setColor(focusBackgroundColor);
		}
		
		graphicsInterface->drawRect(x, y, x + width, y + height, false);

		if (getFocus() == false)
			graphicsInterface->setColor(outlineColor);
		else
			graphicsInterface->setColor(focusOutlineColor);

		graphicsInterface->drawRect(x, y, x + width, y + height, true);
	}

	void GuiRectangleButton::setOnClickFunction(std::function<void(GuiInstance*)> func)
	{
		onClickFunction = func;
	}

	void GuiRectangleButton::setOnClickHoldFunction(std::function<void(GuiInstance*)> func)
	{
		onClickHoldFunction = func;
	}

	void GuiRectangleButton::setOnClickReleaseFunction(std::function<void(GuiInstance*)> func)
	{
		onClickReleaseFunction = func;
	}

	void GuiRectangleButton::setBackgroundColor(Color c)
	{
		backgroundColor = c;
		setShouldRedraw(true);
	}

	void GuiRectangleButton::setOutlineColor(Color c)
	{
		outlineColor = c;
		setShouldRedraw(true);
	}

	void GuiRectangleButton::setFocusOutlineColor(Color c)
	{
		focusOutlineColor = c;
		setShouldRedraw(true);
	}

	void GuiRectangleButton::setFocusBackgroundColor(Color c)
	{
		focusBackgroundColor = c;
		setShouldRedraw(true);
	}

	void GuiRectangleButton::setHoverColor(Color c)
	{
		hoverColor = c;
		setShouldRedraw(true);
	}

	void GuiRectangleButton::setWidth(int v)
	{
		width = v;
		boundingBox = Box2D(x, y, x+width, y+height);
		setShouldRedraw(true);
	}
	void GuiRectangleButton::setHeight(int v)
	{
		height = v;
		boundingBox = Box2D(x, y, x+width, y+height);
		setShouldRedraw(true);
	}
	int GuiRectangleButton::getWidth()
	{
		return width;
	}
	int GuiRectangleButton::getHeight()
	{
		return height;
	}

	void GuiRectangleButton::solveBoundingBox()
	{
		boundingBox = Box2D(x, y, x+width, y+height);
	}

	void GuiRectangleButton::loadDataFromXML(std::unordered_map<std::string, std::string>& attribs, GuiGraphicsInterface* inter)
	{
		GuiInstance::loadDataFromXML(attribs, inter);
		std::vector<std::string> possibleNames = { "width", "height", "backgroundcolor", "outlinecolor", "focusoutlinecolor", "hovercolor", "focusbackgroundcolor"};

		for(int i=0; i<possibleNames.size(); i++)
		{
			auto it = attribs.find(possibleNames[i]);
			if(it != attribs.end())
			{
				if(it->first == "width")
				{
					width = std::abs(StringTools::toInt(it->second));
				}
				else if(it->first == "height")
				{
					height = std::abs(StringTools::toInt(it->second));
				}
				else if(it->first == "backgroundcolor")
				{
					//define as color name or rgba
					backgroundColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(it->first == "outlinecolor")
				{
					//define as color name or rgba
					outlineColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(it->first == "focusoutlinecolor")
				{
					//define as color name or rgba
					focusOutlineColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(it->first == "focusbackgroundcolor")
				{
					//define as color name or rgba
					focusBackgroundColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(it->first == "hovercolor")
				{
					//define as color name or rgba
					hoverColor = ColorNameConverter::NameToColor(it->second);
				}
				
				attribs.erase(possibleNames[i]);
			}
		}
		boundingBox = Box2D(x, y, x+width, y+height);
	}

	void GuiRectangleButton::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiRectangleButton", GuiRectangleButton::loadFunction);
	}

	GuiInstance* GuiRectangleButton::loadFunction(std::unordered_map<std::string, std::string>& attributes, GuiGraphicsInterface* inter)
	{
		GuiRectangleButton* ins = new GuiRectangleButton(0,0,0,0);
		ins->loadDataFromXML(attributes, inter);
		
		return ins;
	}

	#pragma endregion

} //NAMESPACE glib END