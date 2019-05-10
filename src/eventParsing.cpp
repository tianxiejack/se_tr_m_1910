/*
 * eventParsing.cpp
 *
 *  Created on: 2019年5月7日
 *      Author: d
 */
#include "eventParsing.hpp"

CEventParsing* CEventParsing::pThis = NULL;

CEventParsing::CEventParsing()
{
	js_data = NULL;
	pThis = this;
	exit_jsParsing = false;
	_Msg = CMessage::getInstance();
	_Js = new CjoyStick();
}

CEventParsing::~CEventParsing()
{
	exit_jsParsing = true;
	delete _Msg;
	delete pThis;
}

void CEventParsing::thread_jsEvent()
{
	while(!pThis->exit_jsParsing)
	{
		pThis->js_data = pThis->_Js->JoystickProcess();
		pThis->parsingJostickEvent(pThis->js_data);
	}
}

void CEventParsing::parsingJostickEvent(unsigned char* jos_data)
{
	static int povbak=0, butbak=0, xBak=0, yBak=0;

	/*joystick button event*/
	if(jos_data[POV_BUTTON] != povbak)
	{
		switch(jos_data[POV_BUTTON])
		{
		case js_pov_up:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_POVPOSY, jos_data);
			break;
		case js_pov_right:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_POVPOSX, jos_data);
			break;
		case js_pov_down:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_POVPOSY, jos_data);
			break;
		case js_pov_left:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_POVPOSX, jos_data);
			break;
		case js_1:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRACKCTRL, jos_data);
			break;
		case js_2:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_SwitchSensor, jos_data);
			break;
		case js_3:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_ZOOMLONGCTRL, jos_data);
			break;
		case js_4:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_ZOOMSHORTCTRL, jos_data);
			break;
		}
		povbak = jos_data[POV_BUTTON];
	}

	if(jos_data[BUTTON] != butbak)
	{
		switch(jos_data[BUTTON])
		{
		case js_5:
			//_Msg->MSGDRIV_send(, jos_data);
			break;
		case js_6:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRACKSEARCHCTRL, jos_data);
			break;
		case js_7:
			//_Msg->MSGDRIV_send(, jos_data);
			break;
		case js_8:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_captureMode, jos_data);
			break;
		case js_9:
			//_Msg->MSGDRIV_send(, jos_data);
			break;
		case js_10:
			//_Msg->MSGDRIV_send(, jos_data);
			break;
		case js_11:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_IrisAndFocusAndExit, jos_data);
			break;
		case js_12:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_workMode, jos_data);
			break;
		}
		butbak = jos_data[BUTTON];
	}

	if(jos_data[AXIS_X] != xBak)
	{
		_Msg->MSGDRIV_send(MSGID_EXT_INPUT_JOSPOS, jos_data);
		xBak = jos_data[AXIS_X];
	}

	if(jos_data[AXIS_Y] != yBak)
	{
		_Msg->MSGDRIV_send(MSGID_EXT_INPUT_JOSPOS, jos_data);
		yBak = jos_data[AXIS_Y];
	}
}
