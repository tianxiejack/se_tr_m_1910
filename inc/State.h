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
	virtual void TrkCtrl(char Enable);
	virtual void switchSensor(char chid);
	virtual void ZoomCtrl(char type);
	virtual void axisMove(int iDirection, int speed);
public:
	static CPTZControl* _ptz;
	static AgreeMentBaseFormat* _agreement;
	static CPlatformInterface* m_Platform;
	int curState;
	char curValidChid;
	selectCh_t selectch;
};

class StateAuto_Mtd:public State
{
public:
	StateAuto_Mtd();
	virtual ~StateAuto_Mtd();
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
};

class StateSceneSelect:public State
{
public:
	StateSceneSelect();
	virtual ~StateSceneSelect();
	virtual  void OperationInterface(StateManger* con );
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
};

class PlatFormCapture:public State
{
public:
	PlatFormCapture();
	virtual ~PlatFormCapture();
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
};

class BoxCapture:public State
{
public:
	BoxCapture();
	virtual ~BoxCapture();
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
};

class ManualMtdCapture:public State
{
public:
	ManualMtdCapture();
	virtual ~ManualMtdCapture();
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
	virtual int curStateInterface();
};


#endif /* STATE_H_ */
