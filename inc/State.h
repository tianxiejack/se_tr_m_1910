/*
 * state.h
 *
 *  Created on: 2019年3月7日
 *      Author: YD
 */
#ifndef STATE_H_
#define STATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <osa_sem.h>
#include "PTZ_control.h"
#include "StateManger.h"
#include "platformControl.h"
#include "ipcProc.h"
#include "ptz_onvif_control.hpp"

typedef enum {
	Exit = 0,
	iris,
	Focus
}IrisAndFocusAndExit;

typedef struct
{
	bool validCh[5];
	char idx;
}selectCh_t;

typedef enum{
	ROUTINE_STATE,
	STATE_AUTOMTD,
	STATE_SCENETRK,
	STATE_PTZ,
	STATE_BOX,
	STATE_MANUALMTD,
}state_st;
const int JOS_VALUE_MAX = (0xFF);
class StateManger;

class State
{
public:
	State();
	void StateInit();
	void create();
	virtual ~State();
	virtual void OperationInterface(StateManger* con);
	virtual void OperationChangeState(StateManger* con) = 0;
	virtual int curStateInterface() = 0;
	virtual int ChangeState(StateManger* con, char nextState);
	
public:
	static int curState;
	static View* viewParam;
	IPC_PRM_INT ipcParam;
	static HPLTCTRL  m_plt;
	static PLATFORMCTRL_TrackerInput m_pltInput;
	static PLATFORMCTRL_Output m_pltOutput;
	static PlatformCtrl_CreateParams m_cfgPlatParam;
	static DxTimer* m_timer;

public:
	virtual void TrkCtrl(char Enable);
	virtual void switchSensor(char chid);
	virtual void ZoomCtrl(char type);
	virtual void axisMove(int x, int y);
	virtual void virtualAxisMove(int x, int y);
	virtual void trkSearch(int type, int x, int y);
	virtual void Iris_FocusCtrl(int type, int dir);
	virtual void pov_move(int x,int y);
	virtual void mtdhandle(int arg){};
	virtual void recvTrkmsg(int arg){};
	virtual void switchTarget(int arg){};
	virtual void moveAcqrect(int dir,int stepx,int stepy){};

public:
	void switchSensor_interface(int chid);
	void axisMove_interface(float x, float y);
	void Ctrl_Iris(int dir);
	void Ctrl_Focus(int dir);
	void init_sigaction();
	void init_time();
	void uninit_time();
	static void detectionFunc (int sign);
	void trkMovControl(int status,int errx,int erry);
	void PreposHandle(int arg);
	void getcurRate(float& curx , float& cury);
	static void notifyzoomchange(int zoom);
	static void* zoomServer(void* p);
	
public:
	static CIPCProc* m_ipc;
	static CptzInterface* _ptz;
	static AgreeMentBaseFormat* _agreement;
	static CPlatformInterface* m_Platform;
	static CPTZSpeedTransfer*  m_ptzSpeed;
	static int* cfg_value;
	char curValidChid;
	selectCh_t selectch;
	static int m_delta;

private:
	void platformCreate();
	static State *m_st1, *m_st2, *m_st3, *m_st4, *m_st5;
	static OSA_ThrHndl m_thrZoomServer;
	static bool exitThreadZoomserver;
};



class StateAuto_Mtd:public State
{
public:
	StateAuto_Mtd();
	virtual ~StateAuto_Mtd();
	
private:
	static StateAuto_Mtd* pThis;
	void OperationChangeState(StateManger* con);
	int curStateInterface();
	void axisMove(int x, int y){};
	void virtualAxisMove(int x, int y){};
	void ZoomCtrl(char type){};
	void trkSearch(int type, int x, int y){};
	void switchSensor(char chid);
	void TrkCtrl(char Enable);
	void mtdhandle(int arg);
	static void* autoMtdMainloop(void * p);
	void outTrk();
	void recvTrkmsg(int arg);
	static void TimeCallback(void* p);
	
private:
	bool m_haveobj;
	int timeAutoMtd;

};



class StateSceneSelect:public State
{
public:
	StateSceneSelect();
	virtual ~StateSceneSelect();
	void OperationChangeState(StateManger* con);
	int curStateInterface();
	void TrkCtrl(char Enable);
	void trkSearch(int type, int x, int y){};

private:
	bool m_sceneflag;

	
};


class StateLinkManual:public State
{
public:
	StateLinkManual();
	virtual ~StateLinkManual();
	void OperationChangeState(StateManger* con);
	int curStateInterface();



	
private:

};




class PlatFormCapture:public State
{
public:
	PlatFormCapture();
	virtual ~PlatFormCapture();
private:
	void OperationChangeState(StateManger* con);
	int curStateInterface();

};



class BoxCapture:public State
{
public:
	BoxCapture();
	virtual ~BoxCapture();
private:
	void OperationChangeState(StateManger* con);
	int curStateInterface();
	void virtualAxisMove(int x, int y);
	void axisMove(int x, int y);
	void TrkCtrl(char Enable);
	void moveAcqrect(int dir,int stepx,int stepy);

private:
	int m_winx,m_winy;
};



class ManualMtdCapture:public State
{
public:
	ManualMtdCapture();
	virtual ~ManualMtdCapture();
private:
	void OperationChangeState(StateManger* con);
	int curStateInterface();
	void TrkCtrl(char Enable);
	void virtualAxisMove(int x, int y) ;
	void axisMove(int x, int y);
	void switchSensor(char chid);
	void trkSearch(int type, int x, int y){};
	void pov_move(int x,int y);
	void switchTarget(int arg);
	void mtdhandle(int arg);
	
private:
	void openCloseMtd(bool flag);
	bool m_haveobj;
	bool mtdStatus;
};


#endif /* STATE_H_ */
