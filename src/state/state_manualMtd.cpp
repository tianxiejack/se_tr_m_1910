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

void ManualMtdCapture::TrkCtrl(char Enable)
{
	if(cfg_value[CFGID_RTS_mtddet] == 1 && Enable == 1)
	{
		ipcParam.intPrm[0] = Enable;
		m_ipc->IPCSendMsg(trk, ipcParam.intPrm, 4);
	}
	else if(cfg_value[CFGID_RTS_trken] == 1 && Enable == 0)
	{
		ipcParam.intPrm[0] = Enable;
		m_ipc->IPCSendMsg(trk, ipcParam.intPrm, 4);
	}
}

void ManualMtdCapture::axisMove(int x, int y)
{

}




