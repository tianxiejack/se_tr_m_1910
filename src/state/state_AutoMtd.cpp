
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

	OSA_ThrHndl tmpHandle;
	OSA_thrCreate(&tmpHandle, autoMtdMainloop, 0, 0, 0);
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


void* StateAuto_Mtd::autoMtdMainloop(void* p)
{
	OSA_thrDetach();
	static bool exist = false ;
	if(exist)
		return NULL;

	struct timeval tmp;
	pThis->_ptz->runToPrepos(1);
	while(pThis->curState == STATE_AUTOMTD)
	{
		if(pThis->_ptz->runToPrepos(2))
			break;
	}

	if(pThis->curState != STATE_AUTOMTD)
	{
		exist = false;
		return NULL;
	}
	
	pThis->_ptz->ptzStop();
	tmp.tv_sec = 0;
	tmp.tv_usec = 300*1000;
	select(0, NULL, NULL, NULL, &tmp);	
	pThis->ipcParam.intPrm[0] = 1;
	pThis->m_ipc->IPCSendMsg(mtd, pThis->ipcParam.intPrm, 4);

	while(pThis->curState == STATE_AUTOMTD)
	{		
		tmp.tv_sec = 0;
		tmp.tv_usec = 10*1000;
		select(0, NULL, NULL, NULL, &tmp);	
		if(pThis->m_haveobj){
			if(!pThis->cfg_value[CFGID_RTS_trken])
			{	
				pThis->ipcParam.intPrm[0] = 3;
				pThis->m_ipc->IPCSendMsg(mtdSelect, pThis->ipcParam.intPrm, 4);	
				pThis->m_Platform->PlatformCtrl_reset4trk(pThis->m_plt);
				pThis->ipcParam.intPrm[0] = 0;
				pThis->m_ipc->IPCSendMsg(mtd, pThis->ipcParam.intPrm, 4);
			}
			break;
		}
	}
	
	if(pThis->curState != STATE_AUTOMTD)
		printf(" have changed the state :%d \n" ,pThis->curState);

	exist = false;
	return NULL;
}


void StateAuto_Mtd::outTrk()
{
	if(cfg_value[CFGID_RTS_trken])
	{
		ipcParam.intPrm[0] = 0;
		m_ipc->IPCSendMsg(trk, ipcParam.intPrm, 4);		
	}
	OSA_ThrHndl tmpHandle;
	OSA_thrCreate(&tmpHandle, autoMtdMainloop, 0, 0, 0);
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
		m_timer->startTimer(timeAutoMtd,cfg_value[CFGID_MTD_trktime]*1000);
	else if(3 == arg){
		m_timer->stopTimer(timeAutoMtd);
		outTrk();
	}
	return;
}

