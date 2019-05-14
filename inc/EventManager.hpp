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

	static void MSG_Com_SelfCheck(void* p);
	static void MSG_Com_SwitchSensor(void* p);
	static void MSG_Com_WorkMode(void* p);
	static void MSG_Com_CaptureMode(void* p);
	static void MSG_Com_TrkCtrl(void* p);
	static void MSG_Com_TrkMove(void* p);
	static void MSG_Com_SecTrkCtrl(void* p);
	static void MSG_Com_SecTrkPos(void* p);
	static void MSG_Com_MtdSelect(void* p);
	static void MSG_Com_Gatemove(void* p);
	static void MSG_Com_ZoomCtrl(void* p);
	static void MSG_Com_ApertureCtrl(void* p);
	static void MSG_Com_FocusCtrl(void* p);
	static void MSG_Com_SetPlatSpeed(void* p);
	static void MSG_Com_SetPlatAngle(void* p);
	static void MSG_Com_PrsetPos(void* p);
	static void MSG_Com_SetZoom(void* p);
	static void MSG_Com_GetPlatAngle(void* p);
	static void MSG_Com_GetZoom(void* p);
	static void MSG_Com_TrkOutput(void* p);
	static void MSG_Com_SetCfg(void* p);
	static void MSG_Com_GetCfg(void* p);
	static void MSG_Com_SetOsd(void* p);
	static void MSG_Com_DefaultCfg(void* p);
	static void MSG_Com_SaveCfg(void* p);
	
};



#endif /* EVENTMANAGER_HPP_ */
