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


class CEventManager
{
public:
	CEventManager();
	~CEventManager();
	static void thread_ipcEvent();
private:
	MSGDRIV_Handle _Handle;
	CMessage* _Msg;
	StateManger* _StateManager;
	State* _state;
	CIPCProc* m_ipc;
	static CEventManager* pThis;
	int *cfg_value;
	char *usr_value;
	bool exit_ipcthread;
	float winPos[2];
	int outtype;
	comtype_t outcomtype;
	void IPC_Creat();
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
	static void MSG_Com_TrkMove(void* p);
	static void MSG_Com_SecTrkPos(void* p);
	static void MSG_Com_MtdSelect(void* p);
	static void MSG_Com_Gatemove(void* p);
	static void MSG_Com_ZoomCtrl(void* p);
	static void MSG_Com_IrisCtrl(void* p);
	static void MSG_Com_FocusCtrl(void* p);
	static void MSG_Com_SetPlatSpeed(void* p);
	static void MSG_Com_SetPlatAngle(void* p);
	static void MSG_Com_PrsetPos(void* p);
	static void MSG_Com_SetZoom(void* p);
	static void MSG_Com_QueryPtzPos(void* p);
	static void MSG_Com_GetZoom(void* p);
	static void MSG_Com_TrkOutput(void* p);
	static void MSG_Com_SetCfg(void* p);
	static void MSG_Com_GetCfg(void* p);
	static void MSG_Com_SetOsd(void* p);
	static void MSG_Com_DefaultCfg(void* p);
	static void MSG_Com_SaveCfg(void* p);
	int  ReadConfigFile();
	int SetConfig(int block, int field, int value,char *inBuf);
	int GetConfig(comtype_t comtype, int block, int field);
	int DefaultConfig(comtype_t comtype, int blockId);
	int SaveConfig(comtype_t comtype);
	void signalFeedBack(int argnum ...);
	int is_float(int cfgid);
	string intip2string(unsigned int intip);
	unsigned int stringip2int(string str);
	vector<string> csplit(const string& str, const string& delim);
	
};



#endif /* EVENTMANAGER_HPP_ */
