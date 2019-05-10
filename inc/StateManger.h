/*
 * StateManger.h
 *
 *  Created on: 2019年3月7日
 *      Author: YD
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

class State;
class StateManger
{
public:
	StateManger();
	StateManger(State* state);
	~StateManger();
	int CurStateInterface();
	void ToStateConvention();
	void ToStateAuto_Mtd();
	void ToStateSceneSelect();
	void ToPlatFormCapture();
	void ToBoxCapture();
	void ToManualMtdCapture();
	void ToStateTrk();
	void ToStateAcq();

protected:

private:
	friend class State;
	bool ChangeState(State* state);
	void OperationInterface();
	void OperationChangState();

private:
	State* _state;
	int curState;
};
#endif /*~_CONTEXT_H_*/

