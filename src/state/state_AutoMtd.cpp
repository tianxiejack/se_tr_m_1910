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

void StateAuto_Mtd::OperationInterface(StateManger*con)
{
	cout<<"1  StateAuto_Mtd......"<<endl;
}

void StateAuto_Mtd::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

int StateAuto_Mtd::curStateInterface()
{
	if(curState != STATE_AUTOMTD)
		curState = STATE_AUTOMTD;
	return curState;
}
