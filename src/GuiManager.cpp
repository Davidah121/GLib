#include "InternalGuiManagerHeader.h"

namespace glib
{
	#pragma region GUI_MANAGER

	const Class GuiManager::myClass = Class("GuiManager", {&Object::myClass});
	const Class* GuiManager::getClass()
	{
		return &GuiManager::myClass;
	}

	GuiManager::GuiManager()
	{
	}

	GuiManager::GuiManager(Image* img, bool ownership)
	{
		ownsImage = ownership;
		surf = img;
	}

	GuiManager::GuiManager(const GuiManager& k)
	{
		ownsImage = false;
		surf = k.surf;
		objects = std::vector<GuiInstance*>(k.objects);
	}

	GuiManager::~GuiManager()
	{
		for(GuiInstance* ins : objects)
		{
			if(ins!=nullptr)
				ins->manager = nullptr;
		}
		
		objects.clear();

		if(ownsImage)
		{
			if(surf!=nullptr)
			{
				delete surf;
			}
		}

		surf = nullptr;
	}

	void GuiManager::addElement(GuiInstance* k)
	{
		if(k!=nullptr)
		{
			if(k->getManager() == nullptr)
			{
				k->setManager(this);
				objects.push_back(k);

				for(int i=0; i<k->getChildren().size(); i++)
				{
					GuiInstance* c = k->getChildren().at(i);
					if(c->getManager() == nullptr)
					{
						addElement(c);
					}
				}
			}
		}
	}

	void GuiManager::deleteElement(GuiInstance* k)
	{
		if(k->manager == this)
		{
			int startIndex = -1;
			int orgSize = getSize();

			for (int i = 0; i < orgSize; i++)
			{
				if (objects[i] == k)
				{
					startIndex = i;
					break;
				}
			}

			if (startIndex >= 0)
			{
				for (int i = startIndex; i < orgSize - 1; i++)
				{
					objects[i] = objects[i + 1];
				}
				objects.pop_back();
			}

			k->manager = nullptr;

			//delete children as well
			for(GuiInstance* o : k->getChildren())
			{
				deleteElement(o);
			}
		}
	}

	void GuiManager::sortElements()
	{
		Sort::insertionSort<GuiInstance*>(objects.data(), objects.size(), [](GuiInstance* a, GuiInstance* b)->bool{
			return a->getPriority() < b->getPriority();
		});
	}

	void GuiManager::updateGuiElements()
	{
		//Assume that pollInput() was already called
		sortElements();

		//remove objects that are not active or whose parents aren't active
		std::vector<GuiInstance*> currActiveObjects = std::vector<GuiInstance*>();
		for(int i=0; i<objects.size(); i++)
		{
			if (objects[i] != nullptr)
			{
				if(objects[i]->getActive())
				{
					currActiveObjects.push_back(objects[i]);
				}
			}
		}

		//UPDATE
		for(GuiInstance* obj : currActiveObjects)
		{
			obj->baseUpdate();
			obj->update();
		}
	}

	void GuiManager::renderGuiElements()
	{
		std::vector<GuiInstance*> currVisibleObjects = std::vector<GuiInstance*>();
		for(int i=0; i<objects.size(); i++)
		{
			if (objects[i] != nullptr)
			{
				if(objects[i]->getVisible())
				{
					currVisibleObjects.push_back(objects[i]);
				}
			}
		}

		//RENDER
		if (surf != nullptr)
		{
			Graphics::setColor(backgroundColor);
			surf->clearImage();
		}

		for (GuiInstance* obj : currVisibleObjects)
		{
			obj->baseRender();

			if(obj->canvas != nullptr)
			{
				obj->render(obj->canvas);
			}
			else
			{
				if(surf!=nullptr)
					obj->render(surf);
			}
		}
	}

	void GuiManager::setImage(Image* surface)
	{
		surf = surface;
	}

	Image* GuiManager::getImage()
	{
		return surf;
	}

	size_t GuiManager::getSize()
	{
		return objects.size();
	}

	void GuiManager::resizeImage(int width, int height)
	{
		if(ownsImage)
		{
			if(surf!=nullptr)
			{
				delete surf;
			}
			surf = new Image(width, height);
		}
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

	Color GuiManager::getBackgroundColor()
	{
		return backgroundColor;
	}

	void GuiManager::setBackgroundColor(Color c)
	{
		backgroundColor = c;
	}

	#pragma endregion

} //NAMESPACE glib END