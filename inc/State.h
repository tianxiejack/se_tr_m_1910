/*
 * state.h
 *
 *  Created on: 2019年3月7日
 *      Author: YD
 */
#include <stdio.h>
#include <stdlib.h>

#ifndef STATE_H_
#define STATE_H_

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
	virtual void OperationInterface(StateManger* ) = 0;
	virtual void OperationChangeState(StateManger*) = 0;
	virtual int ChangeState(StateManger* con, char nextState);
	int curState;
protected:
private:
};

class StateConvention:public State
{
public:
	StateConvention();
	virtual void StateInit();
	virtual ~StateConvention();
	virtual  void OperationInterface(StateManger* );
	virtual  void OperationChangeState(StateManger*);
protected:

private:
	int ChangeState(StateManger* con, char nextState);
};


class StateAuto_Mtd:public State
{
public:
	StateAuto_Mtd();
	virtual ~StateAuto_Mtd();
	virtual  void OperationInterface(StateManger* );
	virtual  void OperationChangeState(StateManger*);
protected:

private:
	int ChangeState(StateManger* con, char nextState);
};

class StateSceneSelect:public State
{
public:
	StateSceneSelect();
	virtual ~StateSceneSelect();
	virtual  void OperationInterface(StateManger* );
	virtual  void OperationChangeState(StateManger*);
protected:

private:
	int ChangeState(StateManger* con, char nextState);
};

class StateTrk:public State
{
public:
	StateTrk();
	virtual ~StateTrk();
	virtual  void OperationInterface(StateManger* );
	virtual  void OperationChangeState(StateManger*);
protected:

private:
	int ChangeState(StateManger* con, char nextState);
};


class StateAcq:public State
{
public:
	StateAcq();
	virtual ~StateAcq();
	virtual  void OperationInterface(StateManger* );
	virtual  void OperationChangeState(StateManger*);
protected:

private:
	int ChangeState(StateManger* con, char nextState);
};


class PlatFormCapture:public StateConvention
{
public:
	PlatFormCapture();
	virtual ~PlatFormCapture();
	virtual  void OperationInterface(StateManger* );
	virtual  void OperationChangeState(StateManger*);
protected:

private:
	int ChangeState(StateManger* con, char nextState);
};

class BoxCapture:public StateConvention
{
public:
	BoxCapture();
	virtual ~BoxCapture();
	virtual  void OperationInterface(StateManger* );
	virtual  void OperationChangeState(StateManger*);
protected:

private:
	int ChangeState(StateManger* con, char nextState);
};

class ManualMtdCapture:public StateConvention
{
public:
	ManualMtdCapture();
	virtual ~ManualMtdCapture();
	virtual  void OperationInterface(StateManger* );
	virtual  void OperationChangeState(StateManger*);
protected:

private:
	int ChangeState(StateManger* con, char nextState);
};


#endif /* STATE_H_ */
