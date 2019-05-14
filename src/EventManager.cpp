/*
 * EventManager.cpp
 *
 *  Created on: 2019年5月8日
 *      Author: d
 */
#include "EventManager.hpp"
CEventManager* CEventManager::pThis = NULL;

CEventManager::CEventManager()
{
	pThis = this;
	_Msg = CMessage::getInstance();
	_state = new StateConvention();
	_state->StateInit();
	_StateManager = new StateManger(_state);
	_Handle = _Msg->MSGDRIV_create();
	MSG_register();
}

CEventManager::~CEventManager()
{
	_Msg->MSGDRIV_destroy(_Handle);
	delete _Msg;
}

void CEventManager::MSG_register()
{
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_TRACKCTRL, MSG_Trk, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_SwitchSensor, MSG_SwitchSensor, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_ZOOMLONGCTRL, MSG_ZoomLong, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_ZOOMSHORTCTRL, MSG_ZoomShort, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_TRACKSEARCHCTRL, MSG_TrkSearch, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_captureMode, MSG_CaptureMode, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_IrisAndFocusAndExit, MSG_IrisAndFocus, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_workMode, MSG_WorkMode, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_JOSPOS, MSG_JosPos, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_POVPOSX, MSG_PovPosX, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_POVPOSY, MSG_PovPosY, NULL);
}

void CEventManager::MSG_Trk(void* p)
{
	pThis->_StateManager->ToStateTrk();
}
void CEventManager::MSG_SwitchSensor(void* p)
{

}
void CEventManager::MSG_ZoomLong(void* p)
{

}
void CEventManager::MSG_ZoomShort(void* p)
{

}
void CEventManager::MSG_TrkSearch(void* p)
{

}
void CEventManager::MSG_CaptureMode(void* p)
{
	static int state;
	if(state == 0)
		pThis->_StateManager->ToPlatFormCapture();
	else if(state == 1)
		pThis->_StateManager->ToBoxCapture();
	else if(state == 2)
		pThis->_StateManager->ToManualMtdCapture();
	state = (state + 1)%3;
}
void CEventManager::MSG_IrisAndFocus(void* p)
{

}
void CEventManager::MSG_WorkMode(void* p)
{
	static int state;
	if(state == 0)
		pThis->_StateManager->ToStateConvention();
	else if(state == 1)
		pThis->_StateManager->ToStateAuto_Mtd();
	else if(state == 2)
		pThis->_StateManager->ToStateSceneSelect();
	state = (state + 1)%3;
}
void CEventManager::MSG_JosPos(void* p)
{
	char* jos = (char*)p;
	printf("X = %x, Y = %x \n", jos[0], jos[1]);
#if 0
	if(jos[0] > 0 && jos[0] < 0x7a)
		pThis->_StateManager->inter_AxisMove(PTZ_MOVE_Left, jos[0]);
	else if(jos[0] > 0x90 && jos[0] < 0xff)
		pThis->_StateManager->inter_AxisMove(PTZ_MOVE_Right, jos[0]);
	else
		pThis->_StateManager->inter_AxisMove(PTZ_MOVE_Stop, 0);
#endif
}
void CEventManager::MSG_PovPosX(void* p)
{

}
void CEventManager::MSG_PovPosY(void* p)
{

}

