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

void PlatFormCapture::OperationChangeState(StateManger* con)
{
	ipcParam.intPrm[0] = 4;
	m_ipc->IPCSendMsg(workmode, ipcParam.intPrm, 4);
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


