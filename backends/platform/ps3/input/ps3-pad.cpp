#include "ps3-pad.h"
#include <cell/pad.h>

PS3Pad::PS3Pad()
{
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
	_xPos=x;
	_yPos=y;
	_xPosKnown=x;
	_yPosKnown=y;
}

bool PS3Pad::pollEvent(Common::Event &event)
{
	if(((int16)_xPos)!=_xPosKnown || ((int16)_yPos)!=_yPosKnown)
	{
		event.type=Common::EVENT_MOUSEMOVE;
		event.mouse.x=(int16)_xPos;
		event.mouse.y=(int16)_yPos;
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

	static CellPadData data;
	int ret;
	for(int i=0;i<7;i++)
	{
		ret=cellPadGetData(0,&data);
		if(ret==CELL_PAD_OK && data.len>0)
		{
			if(wasSet==false)
			{
				_xPosPad=0;
				_yPosPad=0;
			}
			//net_send("PS3Pad::pollEvent() x: %d\n",data.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X]);
			//net_send("PS3Pad::pollEvent() y: %d\n",data.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_Y]);
			int16 pad_raw_x=((int16)data.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X])-127;
			int16 pad_raw_y=((int16)data.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_Y])-127;
			if(ABS(pad_raw_x)>20)
				_xPosPad+=pad_raw_x;
			if(ABS(pad_raw_y)>20)
				_yPosPad+=pad_raw_y;
			bState1|=data.button[CELL_PAD_BTN_OFFSET_DIGITAL1];
			bState2|=data.button[CELL_PAD_BTN_OFFSET_DIGITAL2];
			wasSet=true;
		}
	}

	if(wasSet)
	{
		//net_send("PS3Pad::frame() - wasSet\n");
		_buttonState1=bState1;
		_buttonState2=bState2;
	}

	_xPos+=(_xPosPad>>4)/3.0f;
	_yPos+=(_yPosPad>>4)/3.0f;

	if(_xPos<0)_xPos=0;
	if(_xPos>_w)_xPos=_w;
	if(_yPos<0)_yPos=0;
	if(_yPos>_h)_yPos=_h;

}

bool PS3Pad::mapPS3ToEvent(Common::Event &event)
{
	if(_buttonState1==_buttonState1Known && _buttonState2==_buttonState2Known)
		return false;

	int16 diffstate1=_buttonState1^_buttonState1Known;
	int16 diffstate2=_buttonState2^_buttonState2Known;

	if(diffstate1&CELL_PAD_CTRL_LEFT)
	{
		if(_buttonState1&CELL_PAD_CTRL_LEFT)//pressed
		{
			event.type=Common::EVENT_KEYDOWN;
			_buttonState1Known|=CELL_PAD_CTRL_LEFT;
		}
		else//released
		{
			event.type=Common::EVENT_KEYUP;
			_buttonState1Known&=~CELL_PAD_CTRL_LEFT;
		}
		event.kbd.flags=0;
		event.kbd.keycode=Common::KEYCODE_LEFT;
		event.kbd.ascii=Common::KEYCODE_LEFT;
		
		return true;
	}
	if(diffstate1&CELL_PAD_CTRL_RIGHT)
	{
		if(_buttonState1&CELL_PAD_CTRL_RIGHT)//pressed
		{
			event.type=Common::EVENT_KEYDOWN;
			_buttonState1Known|=CELL_PAD_CTRL_RIGHT;
		}
		else//released
		{
			event.type=Common::EVENT_KEYUP;
			_buttonState1Known&=~CELL_PAD_CTRL_RIGHT;
		}
		event.kbd.flags=0;
		event.kbd.keycode=Common::KEYCODE_RIGHT;
		event.kbd.ascii=Common::KEYCODE_RIGHT;
		return true;
	}
	if(diffstate1&CELL_PAD_CTRL_UP)
	{
		if(_buttonState1&CELL_PAD_CTRL_UP)//pressed
		{
			event.type=Common::EVENT_KEYDOWN;
			_buttonState1Known|=CELL_PAD_CTRL_UP;
		}
		else//released
		{
			event.type=Common::EVENT_KEYUP;
			_buttonState1Known&=~CELL_PAD_CTRL_UP;
		}
		event.kbd.flags=0;
		event.kbd.keycode=Common::KEYCODE_UP;
		event.kbd.ascii=Common::KEYCODE_UP;
		return true;
	}
	if(diffstate1&CELL_PAD_CTRL_DOWN)
	{
		if(_buttonState1&CELL_PAD_CTRL_DOWN)//pressed
		{
			event.type=Common::EVENT_KEYDOWN;
			_buttonState1Known|=CELL_PAD_CTRL_DOWN;
		}
		else//released
		{
			event.type=Common::EVENT_KEYUP;
			_buttonState1Known&=~CELL_PAD_CTRL_DOWN;
		}
		event.kbd.flags=0;
		event.kbd.keycode=Common::KEYCODE_DOWN;
		event.kbd.ascii=Common::KEYCODE_DOWN;
		return true;
	}
	if(diffstate2&CELL_PAD_CTRL_CROSS)
	{
		if(_buttonState2&CELL_PAD_CTRL_CROSS)//pressed
		{
			event.type=Common::EVENT_LBUTTONDOWN;
			_buttonState2Known|=CELL_PAD_CTRL_CROSS;
		}
		else//released
		{
			event.type=Common::EVENT_LBUTTONUP;
			_buttonState2Known&=~CELL_PAD_CTRL_CROSS;
		}
		event.mouse.x=(int16)_xPos;
		event.mouse.y=(int16)_yPos;
		return true;
	}
	if(diffstate2&CELL_PAD_CTRL_CIRCLE)
	{
		if(_buttonState2&CELL_PAD_CTRL_CIRCLE)//pressed
		{
			event.type=Common::EVENT_RBUTTONDOWN;
			_buttonState2Known|=CELL_PAD_CTRL_CIRCLE;
		}
		else//released
		{
			event.type=Common::EVENT_RBUTTONUP;
			_buttonState2Known&=~CELL_PAD_CTRL_CIRCLE;
		}
		event.mouse.x=(int16)_xPos;
		event.mouse.y=(int16)_yPos;
		return true;
	}
	if(diffstate1&CELL_PAD_CTRL_START)
	{
		if(_buttonState1&CELL_PAD_CTRL_START)//pressed
		{
			event.type=Common::EVENT_KEYDOWN;
			_buttonState1Known|=CELL_PAD_CTRL_START;
		}
		else//released
		{
			event.type=Common::EVENT_KEYUP;
			_buttonState1Known&=~CELL_PAD_CTRL_START;
		}
		event.kbd.flags=0;
		event.kbd.keycode=Common::KEYCODE_F5;
		event.kbd.ascii=Common::KEYCODE_F5;
		return true;
	}
	if(diffstate1&CELL_PAD_CTRL_SELECT)
	{
		if(_buttonState1&CELL_PAD_CTRL_SELECT)//pressed
		{
			event.type=Common::EVENT_KEYDOWN;
			_buttonState1Known|=CELL_PAD_CTRL_SELECT;
		}
		else//released
		{
			event.type=Common::EVENT_KEYUP;
			_buttonState1Known&=~CELL_PAD_CTRL_SELECT;
		}
		event.kbd.flags=0;
		event.kbd.keycode=Common::KEYCODE_ESCAPE;
		event.kbd.ascii=Common::KEYCODE_ESCAPE;
		return true;
	}

	if(diffstate2&CELL_PAD_CTRL_R1)
	{
		if(_buttonState2&CELL_PAD_CTRL_R1)//pressed
		{
			event.type=Common::EVENT_MAINMENU;//hax :x
			_buttonState2Known|=CELL_PAD_CTRL_R1;
			return true;
		}
		else//released
		{
			//event.type=Common::EVENT_RBUTTONUP;
			_buttonState2Known&=~CELL_PAD_CTRL_R1;
		}
		return false;
	}

	_buttonState1=_buttonState1Known;
	_buttonState2=_buttonState2Known;
	return false;
}


