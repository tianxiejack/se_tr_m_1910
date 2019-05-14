/*
 * EventManager.hpp
 *
 *  Created on: 2019年5月8日
 *      Author: d
 */
#ifndef EVENTMANAGER_HPP_
#define EVENTMANAGER_HPP_
#include "eventParsing.hpp"
#include "StateManger.h"
#include "State.h"

class CEventManager
{
public:
	CEventManager();
	~CEventManager();
private:
	MSGDRIV_Handle _Handle;
	CMessage* _Msg;
	StateManger* _StateManager;
	State* _state;
	static CEventManager* pThis;
	void MSG_register();

private:
	static void MSG_Trk(void* p);
	static void MSG_SwitchSensor(void* p);
	static void MSG_ZoomLong(void* p);
	static void MSG_ZoomShort(void* p);
	static void MSG_TrkSearch(void* p);
	static void MSG_CaptureMode(void* p);
	static void MSG_IrisAndFocus(void* p);
	static void MSG_WorkMode(void* p);
	static void MSG_JosPos(void* p);
	static void MSG_PovPosX(void* p);
	static void MSG_PovPosY(void* p);
};



#endif /* EVENTMANAGER_HPP_ */
