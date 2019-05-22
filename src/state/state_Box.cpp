
#include "State.h"

BoxCapture::BoxCapture():winx(1920>>1),winy(1080>>1)
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
	if( abs(x - (JOS_VALUE_MAX>>1)) <= 20 && abs(y - (JOS_VALUE_MAX>>1)) <= 20 )
	{
		winx = 1920>>1;
		winy = 1080>>1;
	}
	else
	{
		winx = (unsigned int)(1920 * ((float)x / JOS_VALUE_MAX));
		winy = (unsigned int)(1080 * ((float)y / JOS_VALUE_MAX));
	}
	ipcParam.intPrm[0] = 2;
	ipcParam.intPrm[1] = winx;
	ipcParam.intPrm[2] = winy;
	m_ipc->IPCSendMsg(AcqPos, ipcParam.intPrm, 4*3);
	
	return ;
}

void BoxCapture::TrkCtrl(char Enable)
{
	if(Enable)
	{
		ipcParam.intPrm[0] = Enable;
		ipcParam.intPrm[1] = winx;
		ipcParam.intPrm[2] = winy;
		m_ipc->IPCSendMsg(AcqPos, ipcParam.intPrm, 4*3);
		if(m_plt != NULL)
			m_Platform->PlatformCtrl_reset4trk(m_plt);
		winx = 1920>>1;
		winy = 1080>>1;
	}
	else
	{
		 State::TrkCtrl(Enable);	 
	}
	return ;
}

