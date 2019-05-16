/*
 * state_Box.cpp
 *
 *  Created on: 2019年5月16日
 *      Author: d
 */
#include "State.h"

BoxCapture::BoxCapture()
{

}

BoxCapture::~BoxCapture()
{

}

void BoxCapture::OperationInterface(StateManger* con)
{
	cout<<"4  BoxCapture"<<endl;
}

void BoxCapture::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

int BoxCapture::curStateInterface()
{
	if(curState != STATE_BOX)
		curState = STATE_BOX;
	return curState;
}

void BoxCapture::axisMove(int x, int y)
{

}

