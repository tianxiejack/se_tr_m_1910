
#include "State.h"

BoxCapture::BoxCapture()
{

}

BoxCapture::~BoxCapture()
{

}

void BoxCapture::OperationChangeState(StateManger* con)
{
	ipcParam.intPrm[0] = 5;
	m_ipc->IPCSendMsg(workmode, ipcParam.intPrm, 4);
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
	printf("Box Move \n");
}

