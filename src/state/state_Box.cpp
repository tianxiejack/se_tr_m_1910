
#include "State.h"

BoxCapture::BoxCapture():m_winx(1920>>1),m_winy(1080>>1)
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


void BoxCapture::moveAcqrect(int dir,int stepx,int stepy)
{
	if(dir&(0x1<<0)){
		m_winx -+ stepx;
		if(m_winx < 20)
			m_winx = 20;
	}else if(dir & (0x1<<1)){
		m_winx += stepx;
		if(m_winx > 1900)
			m_winx = 1900;
	}

	if(dir&(0x1<<2)){
		m_winy -= stepy;
		if(m_winy <20)
			m_winy = 20;
	}else if(dir&(0x1<<3)){
		m_winy += stepy;
		if(m_winy > 1060)
			m_winy = 1060;
	}

	return ;
}


void BoxCapture::virtualAxisMove(int x, int y)
{
	if( x == 0 && y == 0)
	{
		m_winx = 1920>>1;
		m_winy = 1080>>1;
	}
	else
	{
		x += 32767;
		y += 32767;
		m_winx = (unsigned int)(1920 * ((float)x / 65535));
		m_winy = (unsigned int)(1080 * ((float)y / 65535));
	}
	ipcParam.intPrm[0] = 2;
	ipcParam.intPrm[1] = m_winx;
	ipcParam.intPrm[2] = m_winy;
	//m_ipc->IPCSendMsg(AcqPos, ipcParam.intPrm, 4*3);
	return ;
}


void BoxCapture::axisMove(int x, int y)
{
	if(  x == 0 && y == 0 )
	{
		m_winx = 1920>>1;
		m_winy = 1080>>1;
	}
	else
	{
		m_winx = (1920>>1) + ((1920>>1) * ((float)x / ((JOS_VALUE_MAX>>1)-m_delta)));
		m_winy = (1080>>1) + ((1080>>1) * ((float)y / ((JOS_VALUE_MAX>>1)-m_delta)));
	}
	ipcParam.intPrm[0] = 2;
	ipcParam.intPrm[1] = m_winx;
	ipcParam.intPrm[2] = m_winy;
	//m_ipc->IPCSendMsg(AcqPos, ipcParam.intPrm, 4*3);
	return ;
}


void BoxCapture::TrkCtrl(char Enable)
{
	if(Enable)
	{
		ipcParam.intPrm[0] = Enable;
		ipcParam.intPrm[1] = m_winx;
		ipcParam.intPrm[2] = m_winy;
		//m_ipc->IPCSendMsg(AcqPos, ipcParam.intPrm, 4*3);
		if(m_plt != NULL)
			m_Platform->PlatformCtrl_reset4trk(m_plt);
		m_winx = 1920>>1;
		m_winy = 1080>>1;
	}
	else
	{
		 State::TrkCtrl(Enable);	 
	}
	return ;
}

