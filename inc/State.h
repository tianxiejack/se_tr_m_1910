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
const int JOS_VALUE_MAX = (0xFF>>1);
class StateManger;

class State
{
public:
	State();
	virtual void StateInit();
	void create();
	virtual ~State();
	virtual void OperationInterface(StateManger* con);
	virtual void OperationChangeState(StateManger* con) = 0;
	virtual int curStateInterface() = 0;
	virtual int ChangeState(StateManger* con, char nextState);

public:
	int curState;
	OSA_SemHndl m_sem;
	static View* viewParam;
	IPC_PRM_INT ipcParam;
	static HPLTCTRL  m_plt;
	PLATFORMCTRL_TrackerInput m_pltInput;
	PLATFORMCTRL_Output m_pltOutput;
	PlatformCtrl_CreateParams m_cfgPlatParam;

public:
	virtual void TrkCtrl(char Enable);
	virtual void switchSensor(char chid);
	virtual void ZoomCtrl(char type);
	virtual void axisMove(int x, int y);
	virtual void trkSearch(int type, int x, int y);
	virtual void Iris_FocusCtrl(int type, int dir);

public:
	void switchSensor_interface(int chid);
	void axisMove_interface(int x, int y);
	void Ctrl_Iris(int dir);
	void Ctrl_Focus(int dir);
	void init_sigaction();
	void init_time();
	void uninit_time();
	static void detectionFunc (int sign);
	void trkMovControl();

public:
	static CIPCProc* m_ipc;
	static CPTZControl* _ptz;
	static AgreeMentBaseFormat* _agreement;
	static CPlatformInterface* m_Platform;
	static CPTZSpeedTransfer*  m_ptzSpeed;
	static float* cfg_value;
	char curValidChid;
	selectCh_t selectch;

private:
	void platformCreate();

	static State *m_st1, *m_st2, *m_st3, *m_st4, *m_st5;

};



class StateAuto_Mtd:public State
{
public:
	StateAuto_Mtd();
	virtual ~StateAuto_Mtd();
private:
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
	virtual void TrkCtrl(char Enable){};
	virtual void axisMove(int x, int y){};
	virtual void ZoomCtrl(char type){};
	virtual void trkSearch(int type, int x, int y){};
	virtual void switchSensor(char chid);
};



class StateSceneSelect:public State
{
public:
	StateSceneSelect();
	virtual ~StateSceneSelect();
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
	virtual void TrkCtrl(char Enable);
	virtual void trkSearch(int type, int x, int y){};

private:
	bool m_sceneflag;

	
};



class PlatFormCapture:public State
{
public:
	PlatFormCapture();
	virtual ~PlatFormCapture();
private:
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
};



class BoxCapture:public State
{
public:
	BoxCapture();
	virtual ~BoxCapture();
private:
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
	virtual void axisMove(int x, int y);
};



class ManualMtdCapture:public State
{
public:
	ManualMtdCapture();
	virtual ~ManualMtdCapture();
private:
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
	virtual void TrkCtrl(char Enable);
	virtual void axisMove(int x, int y);
	virtual void switchSensor(char chid);
	virtual void ZoomCtrl(char type);
	virtual void trkSearch(int type, int x, int y){};

private:
	void openCloseMtd(bool flag);

	bool mtdStatus;
	
};


#endif /* STATE_H_ */
