/*
 * state_AutoMtd.cpp
 *
 *  Created on: 2019年5月16日
 *      Author: d
 */
#include "State.h"

StateAuto_Mtd::StateAuto_Mtd()
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
	ipcParam.intPrm[0] = 1;
	m_ipc->IPCSendMsg(mtd, ipcParam.intPrm, 4);
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
	if(!cfg_value[CFGID_RTS_trken])
	{
		if(arg)
		{
			ipcParam.intPrm[0] = 3;
			m_ipc->IPCSendMsg(mtdSelect, ipcParam.intPrm, 4);	
		}
	}

	
	return ;
}

