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
	if(Enable && cfg_value[CFGID_RTS_mtddet])
	{	
		ipcParam.intPrm[0] = 3;
		m_ipc->IPCSendMsg(mtdSelect, ipcParam.intPrm, 4);
		if(m_plt != NULL)
			m_Platform->PlatformCtrl_reset4trk(m_plt);
		mtdStatus = false;
	}
	else if(!Enable)
	{
		State::TrkCtrl(Enable);
		if(!mtdStatus)
			openCloseMtd(true);
	}
	return ;
}



void ManualMtdCapture::axisMove(int x, int y)
{

	if( abs(x - (0xff>>1)) <= 20 && abs(y - (0xff>>1)) <= 20 )
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
		State::axisMove(x,y);
	}
	
	return ;
}

void ManualMtdCapture::switchSensor(char chid)
{

}

void ManualMtdCapture::openCloseMtd(bool flag)
{
	ipcParam.intPrm[0] = flag;
	m_ipc->IPCSendMsg(mtd, ipcParam.intPrm, 4);
	mtdStatus = flag;
	return ;
}



void ManualMtdCapture::pov_move(int x,int y)
{
	if(cfg_value[CFGID_RTS_trken])
		State::pov_move( x , y );
	else
	{
		ipcParam.intPrm[0] = x;
		m_ipc->IPCSendMsg(mtdSelect, ipcParam.intPrm, 4);
	}

	return ;
}

