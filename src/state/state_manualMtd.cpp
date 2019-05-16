/*
 * state_manualMtd.cpp
 *
 *  Created on: 2019年5月16日
 *      Author: d
 */
#include "State.h"

ManualMtdCapture::ManualMtdCapture()
{

}

ManualMtdCapture::~ManualMtdCapture()
{

}

void ManualMtdCapture::OperationInterface(StateManger* con)
{
	cout<<"5  ManualMtdCapture"<<endl;
}

void ManualMtdCapture::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

int ManualMtdCapture::curStateInterface()
{
	if(curState != STATE_MANUALMTD)
		curState = STATE_MANUALMTD;
	return curState;
}

void ManualMtdCapture::axisMove(int x, int y)
{

}




