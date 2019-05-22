/*
 * state_manualMtd.cpp
 *
 *  Created on: 2019年5月16日
 *      Author: d
 */
#include "State.h"

ManualMtdCapture::ManualMtdCapture():mtdStatus(false)
{

}

ManualMtdCapture::~ManualMtdCapture()
{

}

void ManualMtdCapture::OperationChangeState(StateManger* con)
{
	ipcParam.intPrm[0] = 6;
	m_ipc->IPCSendMsg(workmode, ipcParam.intPrm, 4);
	OperationInterface(con);
	openCloseMtd(true);
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

	if( abs(x) <= 20 && abs(y) <= 20 )
	{
		_ptz->ptzStop();
		if(!mtdStatus){
			openCloseMtd(true);
		}
	}
	else
	{
		if(mtdStatus)
			openCloseMtd(false);
		axisMove_interface(x,y);
	}
	
	return ;
}

void ManualMtdCapture::switchSensor(char chid)
{

}

void ManualMtdCapture::ZoomCtrl(char type)
{

}


void ManualMtdCapture::openCloseMtd(bool flag)
{
	ipcParam.intPrm[0] = flag;
	m_ipc->IPCSendMsg(mtd, ipcParam.intPrm, 4);
	mtdStatus = flag;
	return ;
}

