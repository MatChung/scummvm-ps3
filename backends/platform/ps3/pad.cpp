#include "pad.h"
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

bool PS3Pad::pollEvent(Common::Event &event)
{
	static CellPadData data;
	int ret;
	
	uint16 bState1=0;
	uint16 bState2=0;
	bool wasSet=false;
	_xPosPad=0;
	_yPosPad=0;

	for(int i=0;i<7;i++)
	{
		ret=cellPadGetData(0,&data);
		if(ret==CELL_PAD_OK && data.len>0)
		{
			//printf("PS3Pad::pollEvent() got data: %d\n",i);
			_xPosPad+=data.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X]-127;
			_yPosPad+=data.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_Y]-127;
			bState1|=data.button[CELL_PAD_BTN_OFFSET_DIGITAL1];
			bState2|=data.button[CELL_PAD_BTN_OFFSET_DIGITAL2];
			wasSet=true;
		}
	}

	if(_xPos<0)_xPos=0;
	if(_xPos>_w)_xPos=_w;
	if(_yPos<0)_yPos=0;
	if(_yPos>_h)_yPos=_h;

	if(wasSet)
	{
		_buttonState1=bState1;
		_buttonState2=bState2;
	}

	if(_xPos!=_xPosKnown || _yPos!=_yPosKnown)
	{
		event.type=Common::EVENT_MOUSEMOVE;
		event.mouse.x=_xPos;
		event.mouse.y=_yPos;
		_xPosKnown=_xPos;
		_yPosKnown=_yPos;

		return true;
	}

	return mapPS3ToEvent(event);
}

void PS3Pad::frame()
{
	_xPos+=_xPosPad>>3;
	_yPos+=_yPosPad>>3;
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
		event.mouse.x=_xPos;
		event.mouse.y=_yPos;
		return true;
	}
	if(diffstate2&CELL_PAD_CTRL_CIRCLE)
	{
		if(_buttonState2&CELL_PAD_CTRL_LEFT)//pressed
		{
			event.type=Common::EVENT_LBUTTONDOWN;
			_buttonState2Known|=CELL_PAD_CTRL_CIRCLE;
		}
		else//released
		{
			event.type=Common::EVENT_LBUTTONUP;
			_buttonState2Known&=~CELL_PAD_CTRL_CIRCLE;
		}
		event.mouse.x=_xPos;
		event.mouse.y=_yPos;
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

	_buttonState1=_buttonState1Known;
	_buttonState2=_buttonState2Known;
	return false;
}

