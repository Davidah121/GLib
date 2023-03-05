#include "ext/TouchSimulator.h"

namespace glib
{
    std::vector<POINTER_TOUCH_INFO> TouchSimulator::inputs = std::vector<POINTER_TOUCH_INFO>(TouchSimulator::INPUTS_ALLOWED); //10 inputs since 10 fingers

    void TouchSimulator::init()
    {
        InitializeTouchInjection(TouchSimulator::INPUTS_ALLOWED, TOUCH_FEEDBACK_DEFAULT);
        for(int i=0; i<inputs.size(); i++)
        {
            memset(&inputs[i], 0, sizeof(POINTER_TOUCH_INFO)); //Zero Memory

            inputs[i].touchFlags = TOUCH_FLAG_NONE;
            inputs[i].touchMask = TOUCH_MASK_CONTACTAREA; //Could add pressure
            
            inputs[i].pointerInfo.pointerType = PT_TOUCH;
            inputs[i].pointerInfo.pointerId = i;
        }
    }

    void TouchSimulator::injectDown(int inputID, int x, int y)
    {
        if(inputID >= 0 && inputID < TouchSimulator::INPUTS_ALLOWED)
        {
            inputs[inputID].pointerInfo.ptPixelLocation.x = x;
            inputs[inputID].pointerInfo.ptPixelLocation.y = y;
            inputs[inputID].rcContact.left = x-2;
            inputs[inputID].rcContact.right = x+2;
            inputs[inputID].rcContact.top = y-2;
            inputs[inputID].rcContact.bottom = y+2;
            

            inputs[inputID].pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
            InjectTouchInput(1, &inputs[inputID]);
        }
    }

    void TouchSimulator::injectUpdate(int inputID, int x, int y)
    {
        if(inputID >= 0 && inputID < TouchSimulator::INPUTS_ALLOWED)
        {
            inputs[inputID].pointerInfo.ptPixelLocation.x = x;
            inputs[inputID].pointerInfo.ptPixelLocation.y = y;
            inputs[inputID].rcContact.left = x-2;
            inputs[inputID].rcContact.right = x+2;
            inputs[inputID].rcContact.top = y-2;
            inputs[inputID].rcContact.bottom = y+2;
            

            inputs[inputID].pointerInfo.pointerFlags = POINTER_FLAG_UPDATE | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
            InjectTouchInput(1, &inputs[inputID]);
        }
    }

    void TouchSimulator::injectUp(int inputID, int x, int y)
    {
        if(inputID >= 0 && inputID < TouchSimulator::INPUTS_ALLOWED)
        {
            inputs[inputID].pointerInfo.ptPixelLocation.x = x;
            inputs[inputID].pointerInfo.ptPixelLocation.y = y;
            inputs[inputID].rcContact.left = x-2;
            inputs[inputID].rcContact.right = x+2;
            inputs[inputID].rcContact.top = y-2;
            inputs[inputID].rcContact.bottom = y+2;

            inputs[inputID].pointerInfo.pointerFlags = POINTER_FLAG_UP;
            InjectTouchInput(1, &inputs[inputID]);
        }
    }
};