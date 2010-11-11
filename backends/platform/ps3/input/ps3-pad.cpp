#include "ps3-pad.h"

#define PAD_OFFSET_BUTTONS_1 2
#define PAD_OFFSET_BUTTONS_2 3
#define PAD_OFFSET_STICK_R_X 4
#define PAD_OFFSET_STICK_R_Y 5
#define PAD_OFFSET_STICK_L_X 6
#define PAD_OFFSET_STICK_L_Y 7

#define PAD_MASK_BTN1_LEFT		(1<<7)
#define PAD_MASK_BTN1_DOWN		(1<<6)
#define PAD_MASK_BTN1_RIGHT		(1<<5)
#define PAD_MASK_BTN1_UP		(1<<4)
#define PAD_MASK_BTN1_START		(1<<3)
#define PAD_MASK_BTN1_R3		(1<<2)
#define PAD_MASK_BTN1_L3		(1<<1)
#define PAD_MASK_BTN1_SELECT	(1<<0)

#define PAD_MASK_BTN2_SQUARE	(1<<7)
#define PAD_MASK_BTN2_CROSS		(1<<6)
#define PAD_MASK_BTN2_CIRCLE	(1<<5)
#define PAD_MASK_BTN2_TRIANGLE	(1<<4)
#define PAD_MASK_BTN2_R1		(1<<3)
#define PAD_MASK_BTN2_L1		(1<<2)
#define PAD_MASK_BTN2_R2		(1<<1)
#define PAD_MASK_BTN2_L2		(1<<0)


PS3Pad::PS3Pad()
{
	printf("PS3Pad::PS3Pad()\n");
	PadInfo padinfo;
	ioPadInit(7);

	ioPadGetInfo(&padinfo);

	_buttonState1=0;
	_buttonState2=0;
	_buttonState1Known=0;
	_buttonState2Known=0;

	_xPos=0;
	_yPos=0;
	_xPosKnown=0;
	_yPosKnown=0;

	_w=320;
	_h=200;
}

void PS3Pad::setResolution(int16 w,int16 h)
{
	_w=w;
	_h=h;
}

void PS3Pad::setPosition(int16 x,int16 y)
{
	_xPos=x/(float)_w;
	_yPos=y/(float)_h;
	_xPosKnown=x;
	_yPosKnown=y;
}

bool PS3Pad::pollEvent(Common::Event &event)
{
	if(((int16)(_xPos*_w))!=_xPosKnown || ((int16)(_yPos*_h))!=_yPosKnown)
	{
		event.type=Common::EVENT_MOUSEMOVE;
		event.mouse.x=getMappedX();
		event.mouse.y=getMappedY();
		_xPosKnown=event.mouse.x;
		_yPosKnown=event.mouse.y;

		return true;
	}

	return mapPS3ToEvent(event);
}

void PS3Pad::frame()
{
	uint16 bState1=0;
	uint16 bState2=0;
	bool wasSet=false;

	PadData data;
	int ret;
	for(int i=0;i<7;i++)
	{
		ret=ioPadGetData(0,&data);
		if(ret==0 && data.len>0)
		{
			if(wasSet==false)
			{
				_xPosPad=0;
				_yPosPad=0;
			}
			//net_send("PS3Pad::pollEvent() x: %d\n",data.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X]);
			//net_send("PS3Pad::pollEvent() y: %d\n",data.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_Y]);
			int16 pad_raw_x=((int16)data.button[PAD_OFFSET_STICK_L_X])-127;
			int16 pad_raw_y=((int16)data.button[PAD_OFFSET_STICK_L_Y])-127;
			if(ABS(pad_raw_x)>20)
				_xPosPad+=pad_raw_x;
			if(ABS(pad_raw_y)>20)
				_yPosPad+=pad_raw_y;
			bState1|=data.button[PAD_OFFSET_BUTTONS_1];
			bState2|=data.button[PAD_OFFSET_BUTTONS_2];
			wasSet=true;
		}
	}

	if(wasSet)
	{
		//net_send("PS3Pad::frame() - wasSet\n");
		_buttonState1=bState1;
		_buttonState2=bState2;
	}

	_xPos+=(_xPosPad>>4)/1000.0f;
	_yPos+=(_yPosPad>>4)/1000.0f;

	if(_xPos<0)_xPos=0;
	if(_xPos>1)_xPos=1;
	if(_yPos<0)_yPos=0;
	if(_yPos>1)_yPos=1;

}

bool PS3Pad::mapPS3ToEvent(Common::Event &event)
{
	if(_buttonState1==_buttonState1Known && _buttonState2==_buttonState2Known)
		return false;

	int16 diffstate1=_buttonState1^_buttonState1Known;
	int16 diffstate2=_buttonState2^_buttonState2Known;

	if(diffstate1&PAD_MASK_BTN1_LEFT)
	{
		if(_buttonState1&PAD_MASK_BTN1_LEFT)//pressed
		{
			event.type=Common::EVENT_KEYDOWN;
			_buttonState1Known|=PAD_MASK_BTN1_LEFT;
		}
		else//released
		{
			event.type=Common::EVENT_KEYUP;
			_buttonState1Known&=~PAD_MASK_BTN1_LEFT;
		}
		event.kbd.flags=0;
		event.kbd.keycode=Common::KEYCODE_LEFT;
		event.kbd.ascii=Common::KEYCODE_LEFT;

		return true;
	}
	if(diffstate1&PAD_MASK_BTN1_RIGHT)
	{
		if(_buttonState1&PAD_MASK_BTN1_RIGHT)//pressed
		{
			event.type=Common::EVENT_KEYDOWN;
			_buttonState1Known|=PAD_MASK_BTN1_RIGHT;
		}
		else//released
		{
			event.type=Common::EVENT_KEYUP;
			_buttonState1Known&=~PAD_MASK_BTN1_RIGHT;
		}
		event.kbd.flags=0;
		event.kbd.keycode=Common::KEYCODE_RIGHT;
		event.kbd.ascii=Common::KEYCODE_RIGHT;
		return true;
	}
	if(diffstate1&PAD_MASK_BTN1_UP)
	{
		if(_buttonState1&PAD_MASK_BTN1_UP)//pressed
		{
			event.type=Common::EVENT_KEYDOWN;
			_buttonState1Known|=PAD_MASK_BTN1_UP;
		}
		else//released
		{
			event.type=Common::EVENT_KEYUP;
			_buttonState1Known&=~PAD_MASK_BTN1_UP;
		}
		event.kbd.flags=0;
		event.kbd.keycode=Common::KEYCODE_UP;
		event.kbd.ascii=Common::KEYCODE_UP;
		return true;
	}
	if(diffstate1&PAD_MASK_BTN1_DOWN)
	{
		if(_buttonState1&PAD_MASK_BTN1_DOWN)//pressed
		{
			event.type=Common::EVENT_KEYDOWN;
			_buttonState1Known|=PAD_MASK_BTN1_DOWN;
		}
		else//released
		{
			event.type=Common::EVENT_KEYUP;
			_buttonState1Known&=~PAD_MASK_BTN1_DOWN;
		}
		event.kbd.flags=0;
		event.kbd.keycode=Common::KEYCODE_DOWN;
		event.kbd.ascii=Common::KEYCODE_DOWN;
		return true;
	}
	if(diffstate2&PAD_MASK_BTN2_CROSS)
	{
		if(_buttonState2&PAD_MASK_BTN2_CROSS)//pressed
		{
			event.type=Common::EVENT_LBUTTONDOWN;
			_buttonState2Known|=PAD_MASK_BTN2_CROSS;
		}
		else//released
		{
			event.type=Common::EVENT_LBUTTONUP;
			_buttonState2Known&=~PAD_MASK_BTN2_CROSS;
		}
		event.mouse.x=getMappedX();
		event.mouse.y=getMappedY();
		return true;
	}
	if(diffstate2&PAD_MASK_BTN2_CIRCLE)
	{
		if(_buttonState2&PAD_MASK_BTN2_CIRCLE)//pressed
		{
			event.type=Common::EVENT_RBUTTONDOWN;
			_buttonState2Known|=PAD_MASK_BTN2_CIRCLE;
		}
		else//released
		{
			event.type=Common::EVENT_RBUTTONUP;
			_buttonState2Known&=~PAD_MASK_BTN2_CIRCLE;
		}
		event.mouse.x=getMappedX();
		event.mouse.y=getMappedY();
		return true;
	}
	if(diffstate1&PAD_MASK_BTN1_START)
	{
		if(_buttonState1&PAD_MASK_BTN1_START)//pressed
		{
			event.type=Common::EVENT_KEYDOWN;
			_buttonState1Known|=PAD_MASK_BTN1_START;
		}
		else//released
		{
			event.type=Common::EVENT_KEYUP;
			_buttonState1Known&=~PAD_MASK_BTN1_START;
		}
		event.kbd.flags=0;
		event.kbd.keycode=Common::KEYCODE_F5;
		event.kbd.ascii=Common::KEYCODE_F5;
		return true;
	}
	if(diffstate1&PAD_MASK_BTN1_SELECT)
	{
		if(_buttonState1&PAD_MASK_BTN1_SELECT)//pressed
		{
			event.type=Common::EVENT_KEYDOWN;
			_buttonState1Known|=PAD_MASK_BTN1_SELECT;
		}
		else//released
		{
			event.type=Common::EVENT_KEYUP;
			_buttonState1Known&=~PAD_MASK_BTN1_SELECT;
		}
		event.kbd.flags=0;
		event.kbd.keycode=Common::KEYCODE_ESCAPE;
		event.kbd.ascii=Common::KEYCODE_ESCAPE;
		return true;
	}

	/*if(diffstate2&PAD_MASK_BTN2_R1)
	{
		if(_buttonState2&PAD_MASK_BTN2_R1)//pressed
		{
			event.type=Common::EVENT_MAINMENU;//hax :x
			_buttonState2Known|=PAD_MASK_BTN2_R1;
			return true;
		}
		else//released
		{
			//event.type=Common::EVENT_RBUTTONUP;
			_buttonState2Known&=~PAD_MASK_BTN2_R1;
		}
		return false;
	}*/

	_buttonState1=_buttonState1Known;
	_buttonState2=_buttonState2Known;
	return false;
}

int16 PS3Pad::getMappedX()
{
	return (int16)(_xPos*_w);
}

int16 PS3Pad::getMappedY()
{
	return (int16)(_yPos*_h);
}
