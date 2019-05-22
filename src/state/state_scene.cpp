/*
 * state_scene.cpp
 *
 *  Created on: 2019年5月16日
 *      Author: d
 */
#include "State.h"

StateSceneSelect::StateSceneSelect():m_sceneflag(false)
{

}

StateSceneSelect::~StateSceneSelect()
{

}

void StateSceneSelect::OperationChangeState(StateManger* con)
{
	ipcParam.intPrm[0] = 3;
	m_ipc->IPCSendMsg(workmode, ipcParam.intPrm, 4);
	OperationInterface(con);
}

int StateSceneSelect::curStateInterface()
{
	if(curState != STATE_SCENETRK)
		curState = STATE_SCENETRK;
	return curState;
}

void StateSceneSelect::TrkCtrl(char Enable)
{
	m_sceneflag = Enable;
	ipcParam.intPrm[0] = m_sceneflag;
	m_ipc->IPCSendMsg(sceneTrk, ipcParam.intPrm, 4);
	return ;
}

