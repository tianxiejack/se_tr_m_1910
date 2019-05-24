
#include "State.h"

StateAuto_Mtd* StateAuto_Mtd::pThis = NULL;

StateAuto_Mtd::StateAuto_Mtd():m_haveobj(false)
{
	timeAutoMtd = m_timer->createTimer();
	m_timer->registerTimer(timeAutoMtd, TimeCallback, &timeAutoMtd);
	pThis = this;
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
	printf("reach at the prepos \n");
	_ptz->ptzStop();
	tmp.tv_sec = 0;
	tmp.tv_usec = 300*1000;
	select(0, NULL, NULL, NULL, &tmp);	
	ipcParam.intPrm[0] = 1;
	m_ipc->IPCSendMsg(mtd, ipcParam.intPrm, 4);
	printf("into the while wait the target  \n");
	while(curState == STATE_AUTOMTD)
	{		
		tmp.tv_sec = 0;
		tmp.tv_usec = 10*1000;
		select(0, NULL, NULL, NULL, &tmp);	
		if(m_haveobj){
			if(!cfg_value[CFGID_RTS_trken])
			{	
				ipcParam.intPrm[0] = 3;
				m_ipc->IPCSendMsg(mtdSelect, ipcParam.intPrm, 4);	
				m_Platform->PlatformCtrl_reset4trk(m_plt);
				ipcParam.intPrm[0] = 0;
				m_ipc->IPCSendMsg(mtd, ipcParam.intPrm, 4);
			}
			break;
		}
	}
	
	printf("out of the while  \n");
	if(curState != STATE_AUTOMTD)
		printf(" have changed the state :%d \n" ,curState);
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


void StateAuto_Mtd::recvTrkmsg(int arg)
{
	if(1 == arg)
		m_timer->startTimer(timeAutoMtd,5000);
	else if(3 == arg)
		m_timer->stopTimer(timeAutoMtd);
	return;
}

