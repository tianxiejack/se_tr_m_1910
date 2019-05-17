/*
 * StateManger.h
 *
 *  Created on: 2019年3月7日
 *      Author: YD
 */
#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "State.h"


class State;
class StateManger
{
public:
	StateManger();
	StateManger(State* state);
	~StateManger();

	void* returnIpc();
	void GetParams(float* p);
	int CurStateInterface();

	void ToStateAuto_Mtd();
	void ToStateSceneSelect();
	void ToPlatFormCapture();
	void ToBoxCapture();
	void ToManualMtdCapture();

	void inter_TrkCtrl(char Enable);
	void inter_SwitchSensor(char chid);
	void inter_ZoomCtrl(char type);
	void inter_AxisPos(int x, int y);
	void inter_AxisMove(int x, int y);
	void inter_TrkSearch(int type, int x, int y);

protected:

private:
	friend class State;
	bool ChangeState(State* state);
	void OperationInterface();
	void OperationChangState();

private:
	State* _state;
	int curState;
	float* cfg_param;
};
#endif /*~_CONTEXT_H_*/

