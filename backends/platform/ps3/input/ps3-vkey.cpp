#include "ps3-vkey.h"
#include <sysutil/sysutil_oskdialog.h>
#include "../netdbg/net.h"


#define MESSAGE	L"OSK Dialog"
#define INIT_TEXT	L""
#define OSK_DIALOG_MEMORY_CONTAINER_SIZE 1024*1024*7

PS3VKeyboard::PS3VKeyboard()
{
	_isShown=false;
	memset(_buffer,0,PS3_MAX_VKEY_CHARS*2);
	memset(_cbuffer,0,PS3_MAX_VKEY_CHARS);
	_sent=0;
}

void PS3VKeyboard::show()
{
	if(_isShown)
		return;

	net_send("PS3VKeyboard::show()\n");

	_sent=0;

	int ret;

	sys_memory_container_create( &_container_id, 
		OSK_DIALOG_MEMORY_CONTAINER_SIZE
		);
	/*E Input field information */
	CellOskDialogInputFieldInfo inputFieldInfo;
	/*E Text to be displayed at upper left of the on-screen keyboard dialog as a guide message */
	inputFieldInfo.message = (uint16_t*)MESSAGE;
	/*E Initial input text */
	inputFieldInfo.init_text = (uint16_t*)INIT_TEXT;
	/*E Length limitation for input text */
	inputFieldInfo.limit_length = CELL_OSKDIALOG_STRING_SIZE;

	/*E Initial display position of the on-screen keyboard dialog (x, y) */
	CellOskDialogPoint pos;
	pos.x = 0.0;
	pos.y = 0.0;

	/*E On-screen keyboard dialog utility activation parameters */ 
	CellOskDialogParam dialogParam;
	/*E Select panels to be used using flag(s) (alphabet input, hiragana input, etc.) */
	dialogParam.allowOskPanelFlg = CELL_OSKDIALOG_PANELMODE_ALPHABET_FULL_WIDTH |
		CELL_OSKDIALOG_PANELMODE_ALPHABET |
		CELL_OSKDIALOG_PANELMODE_NUMERAL_FULL_WIDTH |
		CELL_OSKDIALOG_PANELMODE_NUMERAL              |
		CELL_OSKDIALOG_PANELMODE_JAPANESE             |
		CELL_OSKDIALOG_PANELMODE_JAPANESE_KATAKANA  |
		CELL_OSKDIALOG_PANELMODE_ENGLISH;
	/*E Panel to be displayed first */
	dialogParam.firstViewPanel = CELL_OSKDIALOG_PANELMODE_ALPHABET;	
	/*E Initial display position of the on-screen keyboard dialog */
	dialogParam.controlPoint = pos;
	/*E Prohibited operation flag(s) (ex. CELL_OSKDIALOG_NO_SPACE) */
	dialogParam.prohibitFlgs = 0;

	//ret = cellSysutilRegisterCallback( 0, sysutil_callback, NULL );


	ret = cellOskDialogLoadAsync(_container_id, &dialogParam, &inputFieldInfo);
	/*
	while(1)
	{
	sys_timer_usleep( 16 * 1000 );

	ret = cellSysutilCheckCallback();
	assert(ret==0);
	}
	*/

	_isShown=true;
}

void PS3VKeyboard::finish()
{
	if(!_isShown)
		return;

	net_send("PS3VKeyboard::finish()\n");
	_isShown=false;
	CellOskDialogCallbackReturnParam OutputInfo;
	OutputInfo.numCharsResultString=PS3_MAX_VKEY_CHARS;
	OutputInfo.pResultString=(uint16_t*)_buffer;

	cellOskDialogUnloadAsync(&OutputInfo);
	sys_memory_container_destroy(_container_id);

	int num=wcstombs(_cbuffer,_buffer,PS3_MAX_VKEY_CHARS);
	_cbuffer[num]=0;

	net_send("    result: %d\n",OutputInfo.result);
	net_send("    numchars: %d\n",OutputInfo.numCharsResultString);
	net_send("    text: %S\n",(wchar_t*)OutputInfo.pResultString);
	net_send("    text: %s\n",_cbuffer);
}

void PS3VKeyboard::kill()
{
	if(!_isShown)
		return;

	net_send("PS3VKeyboard::kill()\n");
	cellOskDialogAbort();
}

bool PS3VKeyboard::pollEvent(Common::Event &event)
{
	net_send("PS3VKeyboard::pollEvent(%d,%d,%d)\n",_isShown,strlen(_cbuffer),_sent);

	if(_isShown)
		return false;

	int str=strlen(_cbuffer);
	if(str*2<=_sent)
		return false;

	char c=_cbuffer[_sent>>1];
	_sent++;

	if(_sent%2==1)
		event.type=Common::EVENT_KEYDOWN;
	else
		event.type=Common::EVENT_KEYUP;

	char lower=tolower(c);

	event.kbd.ascii=c;
	event.kbd.keycode=(Common::KeyCode)lower;
	if(lower!=c)
		event.kbd.flags=Common::KBD_SHIFT;
	else
		event.kbd.flags=0;

	net_send("    sending: %d,%d,%d,%d\n",event.type,event.kbd.ascii,event.kbd.flags,event.kbd.keycode);

	return true;
}
