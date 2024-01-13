#include "NewGui.h"
#include "Input.h"
#include "ResourceManager.h"

namespace glib
{
    #pragma region GUI_MANAGER
	std::unordered_map<std::string, std::function<SmartMemory<GuiItem>(SimpleHashMap<std::string, std::string>&, SmartMemory<GuiManager>)> > GuiManager::elementLoadingFunctions;
	const RootClass GuiManager::globalClass = RootClass("GuiManager", {"Object"});

	void GuiManager::initDefaultLoadFunctions()
	{
		GuiManager::registerLoadFunction("Empty", GuiEmpty::loadFunction);
		GuiManager::registerLoadFunction("Text", GuiText::loadFunction);
		GuiManager::registerLoadFunction("TextBox", GuiTextBox::loadFunction);
		GuiManager::registerLoadFunction("Sprite", GuiSprite::loadFunction);
		GuiManager::registerLoadFunction("Button", GuiButton::loadFunction);
		
		GuiManager::registerLoadFunction("LayoutFixed", GuiLayoutFixed::loadFunction);
		GuiManager::registerLoadFunction("LayoutList", GuiLayoutList::loadFunction);
		GuiManager::registerLoadFunction("LayoutTable", GuiLayoutTable::loadFunction);
		// GuiRectangleButton::registerLoadFunction();
		// GuiTextBlock::registerLoadFunction();
		// GuiTextBox::registerLoadFunction();
		// GuiSprite::registerLoadFunction();
		// GuiContainer::registerLoadFunction();
		// GuiList::registerLoadFunction();
		// GuiGrid::registerLoadFunction();
		// GuiContextMenu::registerLoadFunction();
		// GuiDatePicker::registerLoadFunction();
	}

	void GuiManager::registerLoadFunction(std::string className, std::function<SmartMemory<GuiItem>(SimpleHashMap<std::string, std::string>&, SmartMemory<GuiManager>)> func)
	{
		elementLoadingFunctions[ StringTools::toLowercase(className) ] = func;
	}

	bool GuiManager::loadElement(XmlNode* node, SmartMemory<GuiItem> parent)
	{
		if(node == nullptr)
			return false;

		if(StringTools::equalsIgnoreCase<char>(node->getTitle(), "SpriteResource"))
		{
			//separate processing
			HashPair<std::string, std::string>* idPair = node->getAttribute("id");
			HashPair<std::string, std::string>* srcPair = node->getAttribute("src");
			if(idPair != nullptr && srcPair != nullptr )
			{
				//can probably load stuff
				if(!idPair->data.empty() && !srcPair->data.empty())
				{
					//Unless the file is incorrect, should be in the resource list something
					GuiResourceManager::getResourceManager().addSprite(GraphicsInterface::createSprite(srcPair->data, graphicsInterfaceMode), idPair->data, false, false);
					return true;
				}
			}
			StringTools::println("ERROR LOADING SPRITE RESOURCE: No 'src' or 'id' specified");
			return false;
		}
		else if(StringTools::equalsIgnoreCase<char>(node->getTitle(), "FontResource"))
		{
			//separate processing
			HashPair<std::string, std::string>* idPair = node->getAttribute("id");
			HashPair<std::string, std::string>* srcPair = node->getAttribute("src");
			if(idPair != nullptr && srcPair != nullptr )
			{
				//can probably load stuff
				if(!idPair->data.empty() && !srcPair->data.empty())
				{
					//Unless the file is incorrect, should be in the resource list something
					GuiResourceManager::getResourceManager().addFont(GraphicsInterface::createFont(srcPair->data, graphicsInterfaceMode), idPair->data, false, false);
					return true;
				}
			}
			StringTools::println("ERROR LOADING FONT RESOURCE: No 'src' or 'id' specified");
			return false;
		}
		else
		{
			SmartMemory<GuiItem> newIns = nullptr;

			auto it = elementLoadingFunctions.find( node->getTitle() );
			if(it != elementLoadingFunctions.end())
			{
				newIns = it->second(node->getRawAttributes(), SmartMemory<GuiManager>::createNoDelete(this)); //call load function for specific instance
			}

			if(newIns == nullptr)
				return false;
			
			//Add to parent if possible. If not possible to add as a child, add to root layout
			if(parent.getPointer() != nullptr)
			{
				if(parent.getRawPointer()->getType() == GuiItem::TYPE_LAYOUT)
				{
					((GuiLayout*)parent.getRawPointer())->addChild(newIns);
				}
				else
				{
					//add to root layout
					addElement(newIns);
				}
			}
			else
			{
				//add to root layout
				addElement(newIns);
			}
			
			setID(newIns, newIns.getRawPointer()->getNameID());
			addToDisposeList(newIns);

			for(ChildNode& c : node->getChildNodes())
			{
				if(c.type != ChildNode::TYPE_NODE)
					continue;
				
				bool successful = loadElement(c.node, newIns);
				if(!successful)
				{
					StringTools::println("ERROR LOADING NODE: %ls", c.node->getTitle().c_str());
					return false;
				}
			}

			return true;
		}
		
	}

	void GuiManager::loadElementsFromFile(File f)
	{
		SimpleXml xmlFile = SimpleXml(f);
		
		//Everything must be encapsulated in the tag <SimpleGUI> or something
		XmlNode* parentNode = nullptr;

		for(XmlNode* n : xmlFile.getNodes())
		{
			if(StringTools::equalsIgnoreCase<char>(n->getTitle(), "SimpleGUI"))
			{
				parentNode = n;
				break;
			}
		}

		if(parentNode != nullptr)
		{
			auto wAttrib = parentNode->getAttribute("width");
			auto hAttrib = parentNode->getAttribute("height");

			if(wAttrib != nullptr && hAttrib != nullptr)
			{
				int w = StringTools::toInt(wAttrib->data);
				int h = StringTools::toInt(hAttrib->data);
				resizeImage(w, h);
			}

			for(ChildNode& c : parentNode->getChildNodes())
			{
				if(c.type != ChildNode::TYPE_NODE)
					continue;
				
				bool successful = loadElement(c.node, nullptr);
				if(!successful)
				{
					StringTools::println("ERROR LOADING NODE: %ls", c.node->getTitle().c_str());
				}
			}
		}
		else
		{
			StringTools::println("Expected Root Node SimpleGUI was not found.");
		}
	}

	void GuiManager::loadElementsFromXML(SimpleXml& xmlFile)
	{
		//Everything must be encapsulated in the tag <SimpleGUI> or something
		XmlNode* parentNode = nullptr;

		for(XmlNode* n : xmlFile.getNodes())
		{
			if(StringTools::equalsIgnoreCase<char>(n->getTitle(), "SimpleGUI"))
			{
				parentNode = n;
				break;
			}
		}

		if(parentNode != nullptr)
		{
			auto wAttrib = parentNode->getAttribute("width");
			auto hAttrib = parentNode->getAttribute("height");

			if(wAttrib != nullptr && hAttrib != nullptr)
			{
				int w = StringTools::toInt(wAttrib->data);
				int h = StringTools::toInt(hAttrib->data);
				resizeImage(w, h);
			}
			
			for(ChildNode& c : parentNode->getChildNodes())
			{
				if(c.type != ChildNode::TYPE_NODE)
					continue;
				
				XmlNode* n = c.node;
				bool successful = loadElement(n, nullptr);
				if(!successful)
				{
					StringTools::println("ERROR LOADING NODE: %ls", n->getTitle().c_str());
				}
			}
		}
		else
		{
			StringTools::println("Expected Root Node SimpleGUI was not found.");
		}
	}

	GuiManager::GuiManager(unsigned char type)
	{
		setClass(globalClass);
		GraphicsInterface::setDefaultType(type);
		surf = GraphicsInterface::createSurface(320, 240);
		expectedSize = Vec2f(320, 240);
	}

	GuiManager::GuiManager(unsigned char type, int width, int height)
	{
		setClass(globalClass);
		GraphicsInterface::setDefaultType(type);
		surf = GraphicsInterface::createSurface(width, height);
		expectedSize = Vec2f(width, height);
	}

	GuiManager::~GuiManager()
	{
		clear();

		if(surf != nullptr)
			delete surf;
		surf = nullptr;
	}

	void GuiManager::clear()
	{
		//Because we are using smart pointers, any object that should be deleted will be deleted when
		//the smart pointer goes out of scope. This includes when we clear the list of objects to delete.
		//removes the work we need to do.
		
		objectInFocus = nullptr;
		rootLayout = GuiLayoutFixed(); //should be okay
		resetRenderValues();
		previousRectsDrawn.clear();
		knownRectsToDraw.clear();
		objectsByName.clear();
		shouldDelete.clear();
	}

	void GuiManager::addElement(SmartMemory<GuiItem> k)
	{
		if(k.getPointer() != nullptr)
		{
			rootLayout.addChild(k);
		}
	}
	
	void GuiManager::removeElement(SmartMemory<GuiItem> k)
	{
		if(k.getPointer() != nullptr)
		{
			rootLayout.removeChild(k);

			//also remove from name list
			objectsByName.remove(k.getRawPointer()->nameID, k);
			if(k.getDeleteRights())
			{
				//dispose of this object.
				k.~SmartMemory();
			}
		}
	}
	
	void GuiManager::resetRenderValues()
	{
		previousRectsDrawn = knownRectsToDraw;
		renderCounter = 0;
		currDepthCounter = 0;
		knownRectsToDraw.clear();
	}

	void GuiManager::updateRenderCounter()
	{
		renderCounter++;
	}

	uint32_t GuiManager::getNextDepthValue()
	{
		return currDepthCounter++;
	}

	void GuiManager::addNewDrawnArea(GRect r)
	{
		knownRectsToDraw.push_back(r);
	}

	std::vector<GRect>& GuiManager::getNewDrawingRects()
	{
		return knownRectsToDraw;
	}

	std::vector<GRect>& GuiManager::getOldDrawingRects()
	{
		return previousRectsDrawn;
	}

	void GuiManager::updateGuiElements()
	{
		//Assume that pollInput() was already called
		//update root elements
		rootLayout.layoutUpdate(0, 0, surf->getWidth(), surf->getHeight());
		rootLayout.preUpdate();
		rootLayout.update( SmartMemory<GuiManager>::createNoDelete(this) );
	}
	
	bool GuiManager::renderGuiElements()
	{
		resetRenderValues();

		int width = surf->getWidth();
		int height = surf->getHeight();
		
		GraphicsInterface::setOrthoProjection(width, height);
		GraphicsInterface::resetClippingPlane();
		GraphicsInterface::setBoundSurface(surf);
		GraphicsInterface::setColor(backgroundColor);
		GraphicsInterface::clear();

		rootLayout.doRender( SmartMemory<GuiManager>::createNoDelete(this) );
		
		GraphicsInterface::setBoundSurface(surf);
		GraphicsInterface::setColor(Vec4f(1,1,1,1));
		GraphicsInterface::drawToScreen();
		
		return renderCounter > 0;
	}

	GuiLayoutFixed& GuiManager::getRootLayout()
	{
		return rootLayout;
	}
	
	std::vector< HashPair<std::string, SmartMemory<GuiItem>>* > GuiManager::getItemsByName(std::string name)
	{
		return objectsByName.getAll(name);
	}

	SurfaceInterface* GuiManager::getSurface()
	{
		return surf;
	}

	void GuiManager::resizeImage(int width, int height)
	{
		bool wasBound = (surf == GraphicsInterface::getBoundSurface());
		
		if(surf != nullptr)
		{
			if(surf->getWidth() != width || surf->getHeight() != height)
			{
				delete surf;
				surf = GraphicsInterface::createSurface(width, height);
			}
		}
		else
		{
			surf = GraphicsInterface::createSurface(width, height);
		}

		if(wasBound)
			GraphicsInterface::setBoundSurface(surf);
	}

	void GuiManager::setWindowX(int v)
	{
		windowX = v;
	}

	void GuiManager::setWindowY(int v)
	{
		windowY = v;
	}

	int GuiManager::getWindowX()
	{
		return windowX;
	}

	int GuiManager::getWindowY()
	{
		return windowY;
	}

	int GuiManager::getMouseX()
	{
		double temp = 0;
		double scaling = 1;

		temp = (double)(Input::getMouseX() - getWindowX());
		if(surf != nullptr)
			scaling = expectedSize.x / surf->getWidth();

		return (int)(temp*scaling);
	}
	
	int GuiManager::getMouseY()
	{
		double temp = 0;
		double scaling = 1;
		temp = (double)(Input::getMouseY() - getWindowY());
		if(surf != nullptr)
			scaling = expectedSize.y / surf->getHeight();

		return (int)(temp*scaling);
	}

	bool GuiManager::getFocus()
	{
		return isInFocus;
	}

	void GuiManager::setFocus(bool v)
	{
		isInFocus = v;
	}
	
	int GuiManager::getGraphicsInterfaceMode()
	{
		return graphicsInterfaceMode;
	}

	void GuiManager::setID(SmartMemory<GuiItem> k, std::string s)
	{
		objectsByName.add(s, k);
	}

	void GuiManager::addToDisposeList(SmartMemory<GuiItem> k)
	{
		shouldDelete.push_back(k);
	}

	void GuiManager::setObjectInFocus(SmartMemory<GuiItem> k)
	{
		objectInFocus = k;
	}

	bool GuiManager::getObjectInFocus(SmartMemory<GuiItem> k)
	{
		return objectInFocus.getPointer() == k.getPointer();
	}
	
    void GuiManager::setBackgroundColor(Color c)
	{
		backgroundColor = c;
	}


	#pragma endregion
};