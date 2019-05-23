
#include "State.h"

StateAuto_Mtd* StateAuto_Mtd::pThis = NULL;

StateAuto_Mtd::StateAuto_Mtd():m_haveobj(false)
{
	timeAutoMtd = m_timer->createTimer();
	m_timer->registerTimer(timeAutoMtd, TimeCallback, &timeAutoMtd);
}


StateAuto_Mtd::~StateAuto_Mtd()
{
	m_timer->Dx_destroyTimer();
}


void StateAuto_Mtd::OperationChangeState(StateManger* con)
{
	ipcParam.intPrm[0] = 2;
	m_ipc->IPCSendMsg(workmode, ipcParam.intPrm, 4);
	OperationInterface(con);
	autoMtdMainloop();
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

	while(curState == STATE_AUTOMTD)
	{
		tmp.tv_sec = 0;
		tmp.tv_usec = 100*1000;
		select(0, NULL, NULL, NULL, &tmp);	
		if(m_haveobj){
			if(!cfg_value[CFGID_RTS_trken])
			{	
				ipcParam.intPrm[0] = 3;
				m_ipc->IPCSendMsg(mtdSelect, ipcParam.intPrm, 4);		
			}
			break;
		}
	}
	
	return ;
}


void StateAuto_Mtd::outTrk()
{
	if(cfg_value[CFGID_RTS_trken])
	{
		ipcParam.intPrm[0] = 0;
		m_ipc->IPCSendMsg(trk, ipcParam.intPrm, 4);		
	}
	autoMtdMainloop();
	return ;
}


void StateAuto_Mtd::TimeCallback(void* p)
{
	int tid = *(int *)p;

	if(pThis->timeAutoMtd == tid)
	{
		pThis->m_timer->stopTimer(pThis->timeAutoMtd);
		pThis->outTrk();
	}
	return ;
}


void StateAuto_Mtd::recvTrkmsg(void* p)
{
	m_timer->registerTimer(timeAutoMtd, TimeCallback, &timeAutoMtd);

	m_timer->startTimer(timeAutoMtd,5000);
	
	return;
}

