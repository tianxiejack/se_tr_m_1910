/*
 * state_ptz.cpp
 *
 *  Created on: 2019年5月16日
 *      Author: d
 */
#include "State.h"

PlatFormCapture::PlatFormCapture()
{

}

PlatFormCapture::~PlatFormCapture()
{

}

void PlatFormCapture::OperationInterface(StateManger* con)
{
	cout<<"3  PlatFormCapture"<<endl;
}

void PlatFormCapture::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

int PlatFormCapture::curStateInterface()
{
	if(curState != STATE_PTZ)
		curState = STATE_PTZ;
	return curState;
}

void PlatFormCapture::axisMove(int x, int y)
{

}


