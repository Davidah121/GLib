#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_TEXTBOX_CLASS

	const Class GuiTextBox::myClass = Class("GuiTextBox", {&GuiInstance::myClass});
	const Class* GuiTextBox::getClass()
	{
		return &GuiTextBox::myClass;
	}

	GuiTextBox::GuiTextBox(int x, int y, int width, int height) : GuiInstance()
	{
		setBaseX(x);
		setBaseY(y);
		this->width = width;
		this->height = height;
		onEnterPressedFunction = nullptr;
		onKeyPressFunction = nullptr;

		textElement = GuiTextBlock(0, 0, width, height);
		textElement.setShouldHighlightText(true);
		addChild( &textElement );
	}

	GuiTextBox::GuiTextBox(const GuiTextBox& other) : GuiInstance(other)
	{
		copy(other);
	}

	void GuiTextBox::operator=(const GuiTextBox& other)
	{
		GuiInstance::copy(other);
		copy(other);
	}

	void GuiTextBox::copy(const GuiTextBox& other)
	{
		onEnterPressedFunction = other.onEnterPressedFunction;
		onKeyPressFunction = other.onKeyPressFunction;

		width = other.width;
		height = other.height;

		backgroundColor = other.backgroundColor;
		outlineColor = other.outlineColor;
		focusOutlineColor = other.focusOutlineColor;

		textElement = other.textElement;

		removeChild((GuiInstance*)&other.textElement);
		addChild( &textElement );
	}

	GuiTextBox::~GuiTextBox()
	{
	}

	void GuiTextBox::setOnEnterPressedFunction(std::function<void(GuiInstance*)> func)
	{
		onEnterPressedFunction = func;
	}

	void GuiTextBox::setOnKeyPressFunction(std::function<void(GuiInstance*)> func)
	{
		onKeyPressFunction = func;
	}

	void GuiTextBox::setBackgroundColor(Color c)
	{
		backgroundColor = c;
		setShouldRedraw(true);
	}

	void GuiTextBox::setOutlineColor(Color c)
	{
		outlineColor = c;
		setShouldRedraw(true);
	}

	void GuiTextBox::setFocusOutlineColor(Color c)
	{
		focusOutlineColor = c;
		setShouldRedraw(true);
	}

	void GuiTextBox::setCursorBlinkColor(Color c)
	{
		cursorBlinkColor = c;
		setShouldRedraw(true);
	}

	void GuiTextBox::setCursorBlinkTimer(int timeInFrames)
	{
		cursorBlinkMaxTime = timeInFrames;
	}

	void GuiTextBox::setCursorWidth(int w)
	{
		cursorWidth = w;
		setShouldRedraw(true);
	}

	void GuiTextBox::render()
	{
		GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();

		//draw a rectangle
		graphicsInterface->setColor(backgroundColor);
		graphicsInterface->drawRect(renderX, renderY, renderX + width, renderY + height, false);

		if (getFocus() == false)
			graphicsInterface->setColor(outlineColor);
		else
			graphicsInterface->setColor(focusOutlineColor);
		
		graphicsInterface->drawRect(renderX, renderY, renderX + width, renderY + height, true);

		if(getFocus())
		{
			if(!cursorBlink)
			{
				//Font Stuff
				graphicsInterface->setColor(cursorBlinkColor);
				std::wstring testText = textElement.getTextRef().substr(startStringIndex, cursorLocation-startStringIndex);
				GuiFontInterface* fInt = (textElement.getFont() != nullptr) ? textElement.getFont() : graphicsInterface->getFont();
				Font* f = fInt->getFont();

				int startOfCursorLine = f->getWidthOfString(testText);

				graphicsInterface->drawRect(renderX+startOfCursorLine, renderY, renderX+startOfCursorLine+cursorWidth, renderY+f->getFontSize(), false);
			}
		}
	
	}

	void GuiTextBox::keyInput()
	{
		if (getFocus())
		{
			if (Input::getKeyChanged() && Input::getKeyPressed(Input::KEY_ENTER)!=true )
			{
				//General Key
				if (onKeyPressFunction != nullptr)
				{
					onKeyPressFunction(this);
				}
			}

			if (Input::getKeyPressed(Input::KEY_ENTER) || Input::getKeyPressed(Input::KEY_NUMPAD_ENTER))
			{
				//Enter key
				if (onEnterPressedFunction != nullptr)
				{
					onEnterPressedFunction(this);
				}
			}
			else if (Input::getKeyPressed(Input::KEY_LEFT))
			{
				//Move cursor back
				cursorLocation = MathExt::clamp(cursorLocation-1, 0, (int)textElement.getTextRef().size());
				selectStart = cursorLocation;
				selectEnd = cursorLocation;
			}
			else if (Input::getKeyPressed(Input::KEY_RIGHT))
			{
				//Move cursor forward
				cursorLocation = MathExt::clamp(cursorLocation+1, 0, (int)textElement.getTextRef().size());
				selectStart = cursorLocation;
				selectEnd = cursorLocation;
			}
			else if( Input::getKeyPressed(Input::KEY_BACKSPACE))
			{
				int minSelect = MathExt::min(selectStart, selectEnd);
				int maxSelect = MathExt::max(selectStart, selectEnd);
				if(minSelect==maxSelect)
					minSelect--;
				std::wstring prefix = textElement.getText().substr(0, minSelect);
				std::wstring suffix = textElement.getText().substr(maxSelect);

				textElement.setText(prefix+suffix);

				cursorLocation = minSelect;
				selectStart = cursorLocation;
				selectEnd = cursorLocation;
			}
			else if( Input::getKeyPressed(Input::KEY_DELETE))
			{
				int minSelect = MathExt::min(selectStart, selectEnd);
				int maxSelect = MathExt::max(selectStart, selectEnd);
				if(minSelect==maxSelect)
					maxSelect++;

				std::wstring prefix = textElement.getText().substr(0, minSelect);
				std::wstring suffix = textElement.getText().substr(maxSelect);

				textElement.setText(prefix+suffix);

				cursorLocation = minSelect;
				selectStart = cursorLocation;
				selectEnd = cursorLocation;
			}
			else if(Input::getKeyDown(Input::KEY_CONTROL))
			{
				if( Input::getKeyPressed('V'))
				{
					//PASTE
					std::wstring pasteText = System::pasteFromClipboard();

					int minSelect = MathExt::min(selectStart, selectEnd);
					int maxSelect = MathExt::max(selectStart, selectEnd);
					
					std::wstring prefix = textElement.getText().substr(0, minSelect);
					std::wstring suffix = textElement.getText().substr(maxSelect);

					textElement.setText( (prefix + pasteText) + suffix );
					cursorLocation = minSelect+pasteText.size();
					selectStart = cursorLocation;
					selectEnd = cursorLocation;
				}
				else if( Input::getKeyPressed('C'))
				{
					//COPY
					int minSelect = MathExt::min(selectStart, selectEnd);
					int maxSelect = MathExt::max(selectStart, selectEnd);
					std::wstring cpyText = textElement.getText().substr(minSelect, maxSelect);

					System::copyToClipboard(cpyText);
				}
				else if( Input::getKeyPressed('X'))
				{
					//CUT
					int minSelect = MathExt::min(selectStart, selectEnd);
					int maxSelect = MathExt::max(selectStart, selectEnd);
					std::wstring cpyText = textElement.getText().substr(minSelect, maxSelect);

					System::copyToClipboard(cpyText);

					std::wstring prefix = textElement.getText().substr(0, minSelect);
					std::wstring suffix = textElement.getText().substr(maxSelect);

					textElement.setText(prefix+suffix);
					cursorLocation = minSelect;
					selectStart = cursorLocation;
					selectEnd = cursorLocation;
				}
			}
			else
			{
				int lastKey = Input::getLastKeyPressed();
				int charVal = lastKey;

				//convert char to ascii
				int value = StringTools::convertKeyToAscii(lastKey, Input::getKeyDown(Input::KEY_SHIFT));

				if(value!=-1)
				{
					int minSelect = MathExt::min(selectStart, selectEnd);
					int maxSelect = MathExt::max(selectStart, selectEnd);
					
					std::wstring prefix = textElement.getText().substr(0, minSelect);
					std::wstring suffix = textElement.getText().substr(maxSelect);

					textElement.setText( (prefix + (wchar_t)value) + suffix );
					cursorLocation = minSelect+1;
					selectStart = cursorLocation;
					selectEnd = cursorLocation;
				}
			}
		}
	}

	void GuiTextBox::mouseInput()
	{
		GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();
		GuiFontInterface* fInt = (textElement.getFont() != nullptr) ? textElement.getFont() : graphicsInterface->getFont();
		Font* f = fInt->getFont();

		int mouseX = Input::getMouseX();
		int mouseY = Input::getMouseY();
		
		if(getManager()!=nullptr)
		{
			mouseX -= getManager()->getWindowX();
			mouseY -= getManager()->getWindowY();
		}
		
		if (Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
		{
			setFocus(false);

			selectEnd = -1;
			selectStart = -1;
			cursorLocation = 0;

			if (mouseX >= x && mouseX <= x + width)
			{
				if (mouseY >= y && mouseY <= y + height)
				{
					setFocus(true);
					hold = true;

					//set cursor pos and start selection
					int testX = mouseX-x;
					std::wstring testText = textElement.getText();

					int totalWidth = 0;
					bool found = false;
					for(int i=startStringIndex; i<testText.size(); i++)
					{
						FontCharInfo fci = f->getFontCharInfo(testText[i]);
						totalWidth += fci.horizAdv;
						if(testX<totalWidth)
						{
							cursorLocation = i;
							selectStart = i;
							selectEnd = i;
							found = true;
							break;
						}
					}

					if(!found)
					{
						cursorLocation = testText.size();
						selectStart = testText.size();
					}
				}
			}
		}

		if (Input::getMouseUp(Input::LEFT_MOUSE_BUTTON))
		{
			hold = false;
		}

		if(hold == true)
		{
			//selection stuff
			//set cursor pos and end selection
			int testX = mouseX-x;
			std::wstring testText = textElement.getText();

			int totalWidth = 0;
			bool found = false;
			for(int i=startStringIndex; i<testText.size(); i++)
			{
				FontCharInfo fci = f->getFontCharInfo(testText[i]);
				totalWidth += fci.horizAdv;
				if(testX<totalWidth)
				{
					cursorLocation = i;
					selectEnd = i;
					found = true;
					break;
				}
			}

			if(!found)
			{
				cursorLocation = testText.size();
				selectEnd = testText.size();
			}
		}
	}

	void GuiTextBox::selectionCleanup()
	{
		GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();
		GuiFontInterface* fInt = (textElement.getFont() != nullptr) ? textElement.getFont() : graphicsInterface->getFont();
		Font* f = fInt->getFont();
		
		textElement.setHighlightStart(selectStart);
		textElement.setHighlightEnd(selectEnd);

		if(startStringIndex>=cursorLocation)
		{
			startStringIndex = cursorLocation-1;
		}

		if(startStringIndex<0)
			startStringIndex = 0;

		std::wstring testStringWidth = textElement.getTextRef().substr(startStringIndex, cursorLocation-startStringIndex);
		int testWidth = f->getWidthOfString(testStringWidth);

		if(testWidth >= width)
		{
			startStringIndex++;
		}

		int offX = f->getWidthOfString( textElement.getTextRef().substr(0, startStringIndex) );
		textElement.setOffsetX(-offX);
	}

	void GuiTextBox::update()
	{
		int prevCursorLoc = cursorLocation;
		selectEnd = MathExt::clamp(selectEnd, 0, (int)textElement.getTextRef().size());
		selectStart = MathExt::clamp(selectStart, 0, (int)textElement.getTextRef().size());
		cursorLocation = MathExt::clamp(cursorLocation, 0, (int)textElement.getTextRef().size());
		startStringIndex = MathExt::clamp(startStringIndex, 0, (int)textElement.getTextRef().size());
		
		keyInput();
		mouseInput();
		selectionCleanup();

		if(getFocus())
		{
			cursorBlinkTimer++;
			if(cursorBlinkTimer>=cursorBlinkMaxTime)
			{
				cursorBlinkTimer = 0;
				cursorBlink = !cursorBlink;
				setShouldRedraw(true);
			}
		}

		if(prevCursorLoc != cursorLocation)
			setShouldRedraw(true);
		
		boundingBox = Box2D(x, y, x+width, y+height);
	}

	GuiTextBlock* GuiTextBox::getTextBlockElement()
	{
		return &textElement;
	}

	void GuiTextBox::setWidth(int v)
	{
		width = v;
		setShouldRedraw(true);
	}
	void GuiTextBox::setHeight(int v)
	{
		height = v;
		setShouldRedraw(true);
	}
	int GuiTextBox::getWidth()
	{
		return width;
	}
	int GuiTextBox::getHeight()
	{
		return height;
	}

	void GuiTextBox::loadDataFromXML(std::unordered_map<std::wstring, std::wstring>& attributes, GuiGraphicsInterface* inter)
	{
		GuiInstance::loadDataFromXML(attributes, inter);

		std::vector<std::wstring> possibleNames = { L"width", L"height", L"cursorblinktimer", L"cursorwidth", L"backgroundcolor", L"outlinecolor", L"focusoutlinecolor", L"cursorblinkcolor"};

		for(int i=0; i<possibleNames.size(); i++)
		{
			auto it = attributes.find(possibleNames[i]);
			if(it != attributes.end())
			{
				if(possibleNames[i] == L"width")
				{
					this->width = std::abs(StringTools::toInt(it->second));
				}
				else if(possibleNames[i] == L"height")
				{
					this->height = std::abs(StringTools::toInt(it->second));
				}
				else if(possibleNames[i] == L"cursorblinktimer")
				{
					this->setCursorBlinkTimer(StringTools::toInt(it->second));
				}
				else if(possibleNames[i] == L"cursorwidth")
				{
					this->cursorWidth = std::abs(StringTools::toInt(it->second));
				}
				else if(possibleNames[i] == L"backgroundcolor")
				{
					//define as color name or rgba
					this->backgroundColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(possibleNames[i] == L"outlinecolor")
				{
					//define as color name or rgba
					this->outlineColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(possibleNames[i] == L"focusoutlinecolor")
				{
					//define as color name or rgba
					this->focusOutlineColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(possibleNames[i] == L"cursorblinkcolor")
				{
					//define as color name or rgba
					this->cursorBlinkColor = ColorNameConverter::NameToColor(it->second);
				}

				attributes.erase(possibleNames[i]);
			}
		}

		textElement.loadDataFromXML(attributes, inter);
	}

	GuiInstance* GuiTextBox::loadFunction(std::unordered_map<std::wstring, std::wstring>& attributes, GuiGraphicsInterface* inter)
	{
		GuiTextBox* ins = new GuiTextBox(0, 0, 0, 0);
		ins->loadDataFromXML(attributes, inter);

		return ins;
	}

	void GuiTextBox::registerLoadFunction()
	{
		GuiManager::registerLoadFunction(L"GuiTextBox", GuiTextBox::loadFunction);
	}

	#pragma endregion

} //NAMESPACE glib END