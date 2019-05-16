/*
 * state_scene.cpp
 *
 *  Created on: 2019年5月16日
 *      Author: d
 */
#include "State.h"

StateSceneSelect::StateSceneSelect()
{

}

StateSceneSelect::~StateSceneSelect()
{

}

void StateSceneSelect::OperationInterface(StateManger* con)
{
	cout<<"2  StateSceneSelect"<<endl;
}

void StateSceneSelect::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

int StateSceneSelect::curStateInterface()
{
	if(curState != STATE_SCENETRK)
		curState = STATE_SCENETRK;
	return curState;
}

void StateSceneSelect::axisMove(int x, int y)
{

}


