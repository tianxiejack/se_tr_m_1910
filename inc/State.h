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


typedef struct
{
	bool validCh[5];
	char idx;
}selectCh_t;

typedef enum{
	STATE_CONVENTION = 0,
	STATE_AUTOMTD,
	STATE_SCENETRK,
	STATE_PTZ,
	STATE_BOX,
	STATE_MANUALMTD,
	STATE_TRK,
	STATE_ACQ
}state_st;

class StateManger;

class State
{
public:
	State();
	virtual void StateInit();
	virtual ~State();
	virtual void OperationInterface(StateManger* con) = 0;
	virtual void OperationChangeState(StateManger* con) = 0;
	virtual int ChangeState(StateManger* con, char nextState);
	virtual void switchSensor(char chid);
	virtual void axisMove(int iDirection, int speed);
private:
	CPTZControl* _ptz;
	AgreeMentBaseFormat* _agreement;
	int curState;
	char curValidChid;
	selectCh_t selectch;
};

class StateConvention:public State
{
public:
	StateConvention();
	virtual ~StateConvention();
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
};


class StateAuto_Mtd:public State
{
public:
	StateAuto_Mtd();
	virtual ~StateAuto_Mtd();
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
};

class StateSceneSelect:public State
{
public:
	StateSceneSelect();
	virtual ~StateSceneSelect();
	virtual  void OperationInterface(StateManger* con );
	virtual  void OperationChangeState(StateManger* con);
};

class StateTrk:public State
{
public:
	StateTrk();
	virtual ~StateTrk();
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
};


class StateAcq:public State
{
public:
	StateAcq();
	virtual ~StateAcq();
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
};


class PlatFormCapture:public StateConvention
{
public:
	PlatFormCapture();
	virtual ~PlatFormCapture();
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
};

class BoxCapture:public StateConvention
{
public:
	BoxCapture();
	virtual ~BoxCapture();
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
};

class ManualMtdCapture:public StateConvention
{
public:
	ManualMtdCapture();
	virtual ~ManualMtdCapture();
	virtual  void OperationInterface(StateManger* con);
	virtual  void OperationChangeState(StateManger* con);
};


#endif /* STATE_H_ */
