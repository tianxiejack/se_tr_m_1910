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
	printf("mtd is open \n");
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


