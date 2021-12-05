#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_TEXTBLOCK_CLASS

	const Class GuiTextBlock::myClass = Class("GuiTextBlock", {&GuiInstance::myClass});
	const Class* GuiTextBlock::getClass()
	{
		return &GuiTextBlock::myClass;
	}

	GuiTextBlock::GuiTextBlock(int x, int y, int width, int height)
	{
		setBaseX(x);
		setBaseY(y);
		this->maxWidth = width;
		this->maxHeight = height;
	}

	GuiTextBlock::~GuiTextBlock()
	{
		
	}

	void GuiTextBlock::update()
	{
		Font* currFont = textFont;
		if(currFont == nullptr)
			currFont = SimpleGraphics::getFont();

		int totalWidth = currFont->getWidthOfString(text);

	}

	void GuiTextBlock::render(Image* surf)
	{
		if(surf!=nullptr)
		{
			Font* oldFont = SimpleGraphics::getFont();

			if(textFont!=nullptr)
				SimpleGraphics::setFont(textFont);

			SimpleGraphics::setColor(textColor);

			int actualMaxW = (maxWidth <= 0) ? 0xFFFF : maxWidth;
			int actualMaxH = (maxHeight <= 0) ? 0xFFFF : maxHeight;
			

			SimpleGraphics::setClippingRect( Box2D(renderX, renderY, renderX+actualMaxW, renderY+actualMaxH) );
			if(shouldHighlight)
				SimpleGraphics::drawTextLimitsHighlighted(text, renderX+offsetX, renderY+offsetY, actualMaxW-offsetX, actualMaxH-offsetY, allowLineBreaks, startHighlight, endHighlight, highlightColor, surf);
			else
				SimpleGraphics::drawTextLimits(text, renderX+offsetX, renderY+offsetY, actualMaxW-offsetX, actualMaxH-offsetY, allowLineBreaks, surf);
			
			SimpleGraphics::resetClippingPlane();
			SimpleGraphics::setFont(oldFont);
		}
	}

	void GuiTextBlock::setTextColor(Color c)
	{
		textColor = c;
	}

	Color GuiTextBlock::getTextColor()
	{
		return textColor;
	}

	std::wstring GuiTextBlock::getText()
	{
		return text;
	}

	std::wstring& GuiTextBlock::getTextRef()
	{
		return text;
	}

	void GuiTextBlock::setText(std::wstring s)
	{
		text = s;
	}

	void GuiTextBlock::setText(std::string s)
	{
		text = StringTools::toWideString(s);
	}

	void GuiTextBlock::setFont(Font* f)
	{
		textFont = f;
	}

	Font* GuiTextBlock::getFont()
	{
		return textFont;
	}

	void GuiTextBlock::setMaxWidth(int v)
	{
		maxWidth = v;
	}
	void GuiTextBlock::setMaxHeight(int v)
	{
		maxHeight = v;
	}

	int GuiTextBlock::getMaxWidth()
	{
		return maxWidth;
	}
	int GuiTextBlock::getMaxHeight()
	{
		return maxHeight;
	}

	void GuiTextBlock::setHighlightColor(Color c)
	{
		highlightColor = c;
	}
	Color GuiTextBlock::getHighlightColor()
	{
		return highlightColor;
	}

	void GuiTextBlock::setShouldHighlightText(bool v)
	{
		shouldHighlight = v;
	}
	bool GuiTextBlock::getShouldHighlightText()
	{
		return shouldHighlight;
	}

	void GuiTextBlock::setHighlightStart(int v)
	{
		startHighlight = v;
	}
	int GuiTextBlock::getHighlightStart()
	{
		return startHighlight;
	}
	void GuiTextBlock::setHighlightEnd(int v)
	{
		endHighlight = v;
	}
	int GuiTextBlock::getHighlightEnd()
	{
		return endHighlight;
	}

	void GuiTextBlock::setAllowLineBreaks(bool v)
	{
		allowLineBreaks = v;
	}

	bool GuiTextBlock::getAllowLineBreaks()
	{
		return allowLineBreaks;
	}

	void GuiTextBlock::setOffsetX(int x)
	{
		offsetX = x;
	}

	void GuiTextBlock::setOffsetY(int y)
	{
		offsetY = y;
	}

	void GuiTextBlock::loadDataFromXML(std::unordered_map<std::wstring, std::wstring>& attributes)
	{
		GuiInstance::loadDataFromXML(attributes);

		std::vector<std::wstring> possibleNames = { L"maxwidth", L"maxheight", L"textcolor", L"highlightcolor", L"allowhighlight", L"allowlinebreaks", L"highlightstart", L"highlightend", L"offsetx", L"offsety", L"text" };

		for(int i=0; i<possibleNames.size(); i++)
		{
			auto it = attributes.find(possibleNames[i]);
			if(it != attributes.end())
			{
				if(possibleNames[i] == L"maxwidth")
				{
					this->maxWidth = StringTools::toInt(it->second);
				}
				else if(possibleNames[i] == L"maxheight")
				{
					this->maxHeight = StringTools::toInt(it->second);
				}
				else if(possibleNames[i] == L"textcolor")
				{
					//define as color name or rgba
					this->textColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(possibleNames[i] == L"highlightcolor")
				{
					//define as color name or rgba
					this->highlightColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(possibleNames[i] == L"allowhighlight")
				{
					this->shouldHighlight = StringTools::equalsIgnoreCase<wchar_t>(it->second, L"true");
				}
				else if(possibleNames[i] == L"allowlinebreaks")
				{
					this->allowLineBreaks = StringTools::equalsIgnoreCase<wchar_t>(it->second, L"true");
				}
				else if(possibleNames[i] == L"highlightstart")
				{
					this->startHighlight = StringTools::toInt(it->second);
				}
				else if(possibleNames[i] == L"highlightend")
				{
					this->endHighlight = StringTools::toInt(it->second);
				}
				else if(possibleNames[i] == L"offsetx")
				{
					this->offsetX = StringTools::toInt(it->second);
				}
				else if(possibleNames[i] == L"offsety")
				{
					this->offsetY = StringTools::toInt(it->second);
				}
				else if(possibleNames[i] == L"text")
				{
					this->text = it->second;
				}

				attributes.erase(possibleNames[i]);
			}
		}
	}

	GuiInstance* GuiTextBlock::loadFunction(std::unordered_map<std::wstring, std::wstring>& attributes)
	{
		GuiTextBlock* ins = new GuiTextBlock(0, 0);
		ins->loadDataFromXML(attributes);

		return ins;
	}

	void GuiTextBlock::registerLoadFunction()
	{
		GuiManager::registerLoadFunction(L"TextBlock", GuiTextBlock::loadFunction);
	}


	#pragma endregion

} //NAMESPACE glib END