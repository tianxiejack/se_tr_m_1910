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
	_State = new StateManger();
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
printf("11111111  \n");
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

}
void CEventManager::MSG_IrisAndFocus(void* p)
{

}
void CEventManager::MSG_WorkMode(void* p)
{

}
void CEventManager::MSG_JosPos(void* p)
{
	char* test = (char*)p;
	printf("a = %x, b = %x \n", test[0], test[1]);
}
void CEventManager::MSG_PovPosX(void* p)
{

}
void CEventManager::MSG_PovPosY(void* p)
{

}

