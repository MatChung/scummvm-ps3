#include "common/events.h"

#pragma once

class PS3Pad
{
public:
	PS3Pad();

	bool pollEvent(Common::Event &event);
	void setResolution(int16 w,int16 h);
	void frame();

private:
	uint16 _buttonState1;
	uint16 _buttonState2;
	uint16 _buttonState1Known;
	uint16 _buttonState2Known;

	int16 _xPos;
	int16 _yPos;
	int16 _xPosKnown;
	int16 _yPosKnown;

	int16 _xPosPad;
	int16 _yPosPad;

	int16 _w;
	int16 _h;


	bool mapPS3ToEvent(Common::Event &event);
};