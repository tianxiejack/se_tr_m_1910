/*
 * state_AutoMtd.cpp
 *
 *  Created on: 2019年5月16日
 *      Author: d
 */
#include "State.h"

StateAuto_Mtd::StateAuto_Mtd():m_ready4trk(false),m_haveobj(false)
{

}


StateAuto_Mtd::~StateAuto_Mtd()
{

}


void StateAuto_Mtd::OperationChangeState(StateManger* con)
{
	ipcParam.intPrm[0] = 2;
	m_ipc->IPCSendMsg(workmode, ipcParam.intPrm, 4);
	OperationInterface(con);
	return ;
}


int StateAuto_Mtd::curStateInterface()
{
	if(curState != STATE_AUTOMTD)
		curState = STATE_AUTOMTD;
	return curState;
}


void StateAuto_Mtd::switchSensor(char chid)
{

}


void StateAuto_Mtd::TrkCtrl(char Enable)
{
	return ;
}



void StateAuto_Mtd::mtdhandle(int arg)
{
	if(arg)
		m_haveobj = true;
	else
		m_haveobj = false;


	
	if(arg){	
		if(!cfg_value[CFGID_RTS_trken])
		{	
			ipcParam.intPrm[0] = 3;
			m_ipc->IPCSendMsg(mtdSelect, ipcParam.intPrm, 4);		
		}
	}
	
	return ;
}


void StateAuto_Mtd::autoMtdMainloop()
{
	struct timeval tmp;
	_ptz->runToPrepos();
	_ptz->ptzStop();
	tmp.tv_sec = 0;
	tmp.tv_usec = 300*1000;
	select(0, NULL, NULL, NULL, &tmp);	
	ipcParam.intPrm[0] = 1;
	m_ipc->IPCSendMsg(mtd, ipcParam.intPrm, 4);
	m_ready4trk = true;
	return ;
}


