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
#include "PTZ_control.h"
#include "StateManger.h"
#include "platformControl.h"

typedef struct
{
	bool validCh[5];
	char idx;
}selectCh_t;

typedef enum{
	STATE_AUTOMTD,
	STATE_SCENETRK,
	STATE_PTZ,
	STATE_BOX,
	STATE_MANUALMTD,
}state_st;

class StateManger;

class State
{
public:
	State();
	virtual void StateInit();
	void create();
	virtual ~State();
	virtual void OperationInterface(StateManger* con) = 0;
	virtual void OperationChangeState(StateManger* con) = 0;
	virtual int curStateInterface() = 0;
	virtual int ChangeState(StateManger* con, char nextState);

public:
	const int jos_value = 0xFF;
	int curState;
	float* cfg_value;
	View* viewParam;
	HPLTCTRL  m_plt;
	PLATFORMCTRL_TrackerInput m_pltInput;
	PLATFORMCTRL_Output m_pltOutput;
	PlatformCtrl_CreateParams m_cfgPlatParam;

public:
	virtual void TrkCtrl(char Enable);
	virtual void switchSensor(char chid);
	virtual void ZoomCtrl(char type);
	virtual void axisMove(int x, int y);
private:
	static CPTZControl* _ptz;
	static AgreeMentBaseFormat* _agreement;
	static CPlatformInterface* m_Platform;
	static CPTZSpeedTransfer*  m_ptzSpeed;
	char curValidChid;
	selectCh_t selectch;

private:
	void platformCreate();
};

class StateAuto_Mtd:public State
{
public:
	StateAuto_Mtd();
	virtual ~StateAuto_Mtd();
private:
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
	virtual void axisMove(int x, int y){};
	virtual void ZoomCtrl(char type){};
};

class StateSceneSelect:public State
{
public:
	StateSceneSelect();
	virtual ~StateSceneSelect();
	virtual  void OperationInterface(StateManger* con );
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
	virtual void axisMove(int x, int y);
};

class PlatFormCapture:public State
{
public:
	PlatFormCapture();
	virtual ~PlatFormCapture();
private:
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
	virtual void axisMove(int x, int y);
};

class BoxCapture:public State
{
public:
	BoxCapture();
	virtual ~BoxCapture();
private:
	virtual  void OperationInterface(StateManger* con);
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
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
	virtual void axisMove(int x, int y);
};


#endif /* STATE_H_ */
