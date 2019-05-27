#include "PTZ_control.h"
#include "string.h"


#ifndef min
#define min(a, b) ((a)<(b)?(a):(b))
#endif

#define DATAIN_TSK_PRI              (2)
#define DATAIN_TSK_STACK_SIZE       (0)
unsigned char trackBuf[10];
CPTZControl* CPTZControl::pThis = 0;
notifyZoom CPTZControl::notifyUpdateZoom = NULL;


CPTZControl::CPTZControl(AgreeMentBaseFormat* _imp , notifyZoom ptr):pCom(NULL), 
	exitQuery_X(false), exitQuery_Y(false), exitQueryzoom(false),
	m_Mtd_Moitor(0),m_Mtd_Moitor_X(0),m_Mtd_Moitor_Y(0),	m_rcv_zoomValue(2849),
	m_sync_pos(false),m_sync_fovComp(false),m_stateChange(false),m_iZoomPosBk(0)
{

	m_bStopZoom = false;
	m_circle = 0;
	uiCurRecvLen = 0;
	m_nWait = 0;
	m_tResponse = PELCO_RESPONSE_Null;

	m_iCurPanSpeed = m_iCurTiltSpeed = m_iCurZoomSpeed = m_iCurIrisSpeed = m_iCurFocusNearSpeed = m_iCurFocusFarSpeed = m_iSetPreset = 0;
	m_iSetPanSpeed = m_iSetTiltSpeed = m_iSetZoomSpeed = m_iSetIrisSpeed =m_iSetFocusNearSpeed =  m_iSetFocusFarSpeed = m_iSetPreset =  0;
	m_bChangePanSpeed = m_bChangeTiltSpeed = m_bChangeZoomSpeed = m_bChangeIrisSpeed = \
	m_bChangeFocusNearSpeed = m_bChangeFocusFarSpeed = m_bChangePreset = false;
	m_iPanPos = m_iTiltPos = m_iZoomPos = m_iIrisPos= m_iFocusPos = 0;
	m_isetZoom = 0;

	m_changeZoom = false;

	m_bQuryZoomPos = false;
	m_fZoomLimit = 32.0;

	m_cmd1Code = 0;
	m_cmd2Code = 0;
	m_data1Code = 0;
	m_data2Code = 0;
	pThis = this;
	_GlobalDate = CGlobalDate::Instance();
	_agreeMent = _imp;
	memset(&sendBuffer1, 0, sizeof(sendBuffer1));
	memset(&recvBuffer, 0, sizeof(recvBuffer));
	memset(&sendBuffer, 0, sizeof(sendBuffer));
	Create();
	notifyUpdateZoom = ptr;
}

CPTZControl::~CPTZControl()
{
	Destroy();
}

int CPTZControl::Create()
{
	m_byAddr = balladdress;
	m_pReq = (LPPELCO_D_REQPKT)sendBuffer;
	m_pReqMove = (LPPELCO_D_REQPKT)sendBuffer1;
	m_pResp = (LPPELCO_D_RESPPKT)recvBuffer;

	pCom = PortFactory::createProduct(3);
	fd_ptz = pCom->copen();

	if(pCom != NULL)
	{
		OSA_mutexCreate(&m_mutex);
		SELF_semCreate(&m_sem);
		exitDataInThread = false;
		OSA_thrCreate(&thrHandleDataIn, port_dataInFxn,  DATAIN_TSK_PRI, DATAIN_TSK_STACK_SIZE, this);
		exitThreadMove = false;
		OSA_thrCreate(&thrHandleMove,MoveThrdFxn, DATAIN_TSK_PRI, DATAIN_TSK_STACK_SIZE, this);
		DXTimerCreat();
	}
    if(_GlobalDate->outputMode == 0)
    {
    	outputMode = 3;
    	return 0;
    }
    outputMode = _GlobalDate->outputMode;
	
	return 0;
}

void CPTZControl::DXTimerCreat()
{
	breakQuery_X = dtimer.createTimer();
	dtimer.registerTimer(breakQuery_X, Tcallback_QueryX, &breakQuery_X);

	breakQuery_Y = dtimer.createTimer();
	dtimer.registerTimer(breakQuery_Y, Tcallback_QueryY, &breakQuery_Y);
}


void CPTZControl::Tcallback_QueryX(void* p)
{
    int tid = *(int *)p;
	if(pThis->breakQuery_X == tid)
	{
		pThis->exitQuery_X = true;
		pThis->dtimer.stopTimer(pThis->breakQuery_X);
	}
}

void CPTZControl::Tcallback_QueryY(void* p)
{
    int tid = *(int *)p;
	if(pThis->breakQuery_Y == tid)
	{
		pThis->exitQuery_Y = true;
		pThis->dtimer.stopTimer(pThis->breakQuery_Y);
	}
}

void CPTZControl::Destroy()
{
	if(pCom != NULL){
		OSA_mutexLock(&m_mutex);
		exitDataInThread = true;
		do{
			OSA_waitMsecs(5);
		}while(exitDataInThread);
		OSA_thrDelete(&thrHandleDataIn);

		exitThreadMove = true;
		do{
			OSA_waitMsecs(5);
		}while(exitThreadMove);
		OSA_thrDelete(&thrHandleMove);

		delete pCom;

		OSA_mutexUnlock(&m_mutex);
		OSA_mutexDelete(&m_mutex);
		SELF_semDelete(&m_sem);
	}

}

void CPTZControl::dataInThrd()
{
    Int32 result;
    fd_set rd_set;
    Int32 rlen;
    Int32 i;
    struct timeval timeout;

    memset(&timeout, 0, sizeof(timeout));

    _agreeMent->buffer = (unsigned char*)malloc(1024);

    OSA_assert(_agreeMent->buffer != NULL);

    while(!exitDataInThread && pCom != NULL && fd_ptz != -1)
    {
        FD_ZERO(&rd_set);
        FD_SET(fd_ptz, &rd_set);
#if 1
        timeout.tv_sec   = 0;
        timeout.tv_usec = 200*1000;
        result = select(fd_ptz+1, &rd_set, NULL, NULL, &timeout);
        if(result == -1 || exitDataInThread )
            break;

        if(result == 0)
        {
            //OSA_waitMsecs(1);
        	//OSA_printf("%s: timeout.", __func__);
            continue;
        }
#endif
        if(FD_ISSET(fd_ptz, &rd_set))
        {

        	rlen =  pCom->crecv(fd_ptz, _agreeMent->buffer, 1024);

            switch(_agreeMent->ptzType)
            {
            case pelco_d:

            break;
            case Panoramic_mirror:
            	_agreeMent->SendDataLen(rlen);
            break;
            }

			for(i=0; i<rlen; i++)
				RecvByte(_agreeMent->buffer[i]);

        }
    }

    free(_agreeMent->buffer);

    exitDataInThread = false;
}

void CPTZControl::RecvByte(unsigned char byRecv)
{
	static bool sync_pan = false, sync_Tilt = false, sync_zoom= false;
	if(uiCurRecvLen == 0){

		if(byRecv == 0xFF){
			if(m_tResponse == PELCO_RESPONSE_Null)
				return ;
			recvBuffer[uiCurRecvLen++] = byRecv;
			if(m_tResponse == PELCO_RESPONSE_Query)
				m_nWait = 18;
			else if(m_tResponse == PELCO_RESPONSE_Extended)
				m_nWait = 7;
			else
				m_nWait = 4;
		}
	}else{
		recvBuffer[uiCurRecvLen++] = byRecv;
		if(uiCurRecvLen == m_nWait){
			if(recvBuffer[3] != 0x59 && recvBuffer[3] != 0x5B
				&& recvBuffer[3] != 0x5D && recvBuffer[3] != 0x63){
#if 0
				OSA_printf("PTZ Recv > %d %02X %02X %02X %02X %02X %02X %02X\n", uiCurRecvLen,
					recvBuffer[0], recvBuffer[1], recvBuffer[2], recvBuffer[3],
					recvBuffer[4], recvBuffer[5], recvBuffer[6]);
#endif
			}
			int diffValue;
			switch(recvBuffer[3])
			{
			case 0x59:
				m_iPanPos = recvBuffer[4];
				m_iPanPos <<= 8;
				m_iPanPos += recvBuffer[5];
				if(posx_bak == m_iPanPos)
				{
					exitQuery_X = true;
					posx_bak =0;
				}
				if(m_Mtd_Moitor == 1)
				{
					m_Mtd_Moitor_X = m_iPanPos;
					m_iPanPos = 0;
				}
				//fprintf(stdout, "INFO: m_iPanPos is %d\n",m_iPanPos);
				sync_pan = true;
				break;
			case 0x5B:
				m_iTiltPos = recvBuffer[4];
				m_iTiltPos <<= 8;
				m_iTiltPos += recvBuffer[5];
				if(posy_bak == m_iTiltPos)
				{
					exitQuery_Y = true;
					posy_bak = 0;
				}
				if(m_Mtd_Moitor == 1)
				{
					m_Mtd_Moitor_Y  = m_iTiltPos;
					m_iTiltPos = 0;
				}
				sync_Tilt = true;
				//printf("INFO: m_iTiltPos is %d\n",m_iTiltPos);
				break;
			case 0x5D:
				m_iZoomPos = recvBuffer[4];
				m_iZoomPos <<= 8;
				m_iZoomPos += recvBuffer[5];
				m_rcv_zoomValue = (unsigned short)m_iZoomPos;
				diffValue = zoom_bak - m_iZoomPos;					
				if(abs(diffValue) < 700)
				{
					exitQueryzoom = true;
					zoom_bak = 0;
				}
				if(m_Mtd_Moitor == 1)
				{
					m_Mtd_Moitor_Zoom  = m_iZoomPos;
					m_iZoomPos =m_Mtd_Moitor = 0;
				}
				if(m_iZoomPos != m_iZoomPosBk)
				{
					if(notifyUpdateZoom != NULL)
						notifyUpdateZoom(m_iZoomPos);
					m_iZoomPosBk = m_iZoomPos;
				}
				sync_zoom = true;
				//printf("INFO: zoompos is %d\n",m_iZoomPos);
				break;
			case 0x63:
				m_iMagnification = recvBuffer[4];
				m_iMagnification <<= 8;
				m_iMagnification += recvBuffer[5];
				//printf("INFO: Magnification is %d\n",m_iMagnification);
				break;
			}

			if(m_tResponse != PELCO_RESPONSE_Null)
				SELF_semSignal(&m_sem);
			uiCurRecvLen = 0;
			m_nWait = 0;

			if(sync_pan &&  sync_Tilt){
				m_sync_pos= false;
				sync_pan = false;
				sync_Tilt = false;
			}
			
			if(sync_zoom){
				m_sync_fovComp = false;
				sync_zoom = false;
				
			}
		}
	}
}


int CPTZControl::sendCmd(LPPELCO_D_REQPKT pCmd, PELCO_RESPONSE_t tResp /* = PELCO_RESPONSE_General */)
{
	int iRet = OSA_SOK;
	unsigned char *buffer = (unsigned char *)pCmd;

	OSA_mutexLock(&m_mutex);
	m_tResponse = tResp;

	int len = pCom->csend(fd_ptz, (unsigned char *)pCmd, sizeof(PELCO_D_REQPKT));

	if(pCom == NULL ||  len != sizeof(PELCO_D_REQPKT) || len == 0)
	{
		OSA_printf("%s: send error! \n", __func__);
		OSA_mutexUnlock(&m_mutex);
		return OSA_EFAIL;
	}
	if(tResp != PELCO_RESPONSE_Null)
		iRet = SELF_semWait(&m_sem, 200);
	
	static int sign;
	if( iRet != OSA_SOK ){
		OSA_printf("[%s] [line %d] -> PTZ send msg time out!\n", __func__, __LINE__);
		sign++;
		if(sign == 3)
		{
			m_sync_pos = false;
			m_sync_fovComp = false;
			sign = 0;
		}
		iRet = -1;
	}
	else
		sign = 0;
	OSA_mutexUnlock(&m_mutex);
	return iRet;
}

bool mask = true;

int CPTZControl::MoveSync()
{
	int iRet = 0;
	//OSA_mutexLock(&m_mutex);

	if(m_changeZoom || m_iSetZoomSpeed)
	{
		int Pan  = abs((int)(m_iCurPanSpeed));
		int Tilt   = abs((int)m_iCurTiltSpeed);

		m_cmd1Code=0;
		m_cmd2Code=0;

		if(m_iCurFocusNearSpeed > 0)
			m_cmd2Code |= 0x80;
		else if(m_iCurFocusFarSpeed < 0)
			m_cmd1Code |= 0x01;

		if(m_iCurZoomSpeed > 0)
			m_cmd2Code |= 0x20;
		else if(m_iCurZoomSpeed <0)
			m_cmd2Code |= 0x40;
		
		if(m_iCurIrisSpeed > 0)
			m_cmd1Code |= 0x02;
		else if(m_iCurIrisSpeed <0)
			m_cmd1Code |= 0x04;

		if(m_iCurPanSpeed>0)
			m_cmd2Code |= 0x02;
		else if(m_iCurPanSpeed<0)
			m_cmd2Code |= 0x04;
		
		if(m_iCurTiltSpeed > 0)
			m_cmd2Code |= 0x10;
		else if(m_iCurTiltSpeed < 0)
			m_cmd2Code |= 0x08;

		m_data1Code = min(Pan, 63);
		m_data2Code = min(Tilt, 63);

		switch(outputMode)
		{
		case 3:
			_agreeMent->PktFormat(m_pReqMove, m_cmd1Code, m_cmd2Code, m_data1Code, m_data2Code, m_byAddr);
			iRet = sendCmd(m_pReqMove, PELCO_RESPONSE_Null);
			break;

		case 1:
		case 2:
			trackErrOutput();
			iRet = SendtrackErr();
			break;
		}
		m_changeZoom = false;
	}

	return iRet;
}

int CPTZControl::Move()
{

	int iRet = 0;
	if(m_iCurPanSpeed == m_iSetPanSpeed
		&& m_iCurTiltSpeed == m_iSetTiltSpeed
		&& m_iCurZoomSpeed == m_iSetZoomSpeed
		&& m_iCurIrisSpeed == m_iSetIrisSpeed
		&& m_iCurFocusNearSpeed == m_iSetFocusNearSpeed
		&&m_iCurFocusFarSpeed == m_iSetFocusFarSpeed)
		return 0;

	m_changeZoom = 1;
	
	m_iCurPanSpeed = m_iSetPanSpeed;
	m_iCurTiltSpeed = m_iSetTiltSpeed;
	m_iCurZoomSpeed = m_iSetZoomSpeed;
	m_iCurIrisSpeed = m_iSetIrisSpeed;
	m_iCurFocusFarSpeed = m_iSetFocusFarSpeed;
	m_iCurFocusNearSpeed = m_iSetFocusNearSpeed;

	return 0;
}

int CPTZControl::Preset(int nCtrlType, int iPresetNum)
{
	_agreeMent->MakePresetCtrl(m_pReq, nCtrlType, iPresetNum, m_byAddr);
	return sendCmd(m_pReq, PELCO_RESPONSE_General);
}

int CPTZControl::Pattern(int nCtrlType, int iPatternNum)
{
	_agreeMent->MakePatternCtrl(m_pReq, nCtrlType, iPatternNum, m_byAddr);
	OSA_printf("Pattern type = %d\n", nCtrlType);
	return sendCmd(m_pReq, PELCO_RESPONSE_General);
}

int CPTZControl::Query(int iQueryType)
{
	_agreeMent->MakeExtCommand(m_pReq, iQueryType, 0, 0, m_byAddr);
	return sendCmd(m_pReq, PELCO_RESPONSE_Extended);
}

int CPTZControl::Dummy()
{
	_agreeMent->MakeDummy(m_pReq, m_byAddr);
	return sendCmd(m_pReq, PELCO_RESPONSE_Null);
}

int CPTZControl::ptzMove(INT32 iDirection, UINT8 bySpeed)
{
	_agreeMent->MakeMove(m_pReq,iDirection,bySpeed,1,m_byAddr);
	return sendCmd(m_pReq, PELCO_RESPONSE_Null);
}


void CPTZControl::resetPtz()
{	
	m_iSetPanSpeed = m_iSetTiltSpeed = 0;
	m_iSetZoomSpeed = m_iSetIrisSpeed = 0;
	m_iSetFocusNearSpeed = m_iSetFocusFarSpeed = 0;
	return ;
}


int CPTZControl::ptzStop()
{
	resetPtz();
	_agreeMent->PktFormat(m_pReq, 0, 0, 0, 0, m_byAddr);
	sendCmd(m_pReq, PELCO_RESPONSE_Null);
}

void CPTZControl::ptzZoomWide()
{
	_agreeMent->MakeZoomWide(m_pReq, m_byAddr);
	sendCmd(m_pReq, PELCO_RESPONSE_Null);
}

void CPTZControl::ptzZoomTele()
{
	_agreeMent->MakeZoomTele(m_pReq, m_byAddr);
	sendCmd(m_pReq, PELCO_RESPONSE_Null);
}

void CPTZControl::ptzZoomStop()
{
	_agreeMent->MakeZoomStop(m_pReq, m_byAddr);
	sendCmd(m_pReq, PELCO_RESPONSE_Null);
}


void CPTZControl::ptzSetPos(Uint16 posx, Uint16 posy)
{
	timeout = 0;
	posx_bak = posx;
	posy_bak = posy;
	_agreeMent->MakeSetPanPos(m_pReq,posx,m_byAddr);
	sendCmd(m_pReq, PELCO_RESPONSE_Null);
	struct timeval tmp;
	dtimer.startTimer(breakQuery_X, 3000);
	for(; exitQuery_X == false;)
	{
		if(m_stateChange)
		{
			ptzStop();
			break;
		}
		tmp.tv_sec = 0;
		tmp.tv_usec = 30*1000;
		select(0, NULL, NULL, NULL, &tmp);
		QueryPanPos();
	}
	exitQuery_X = false;
	pThis->dtimer.stopTimer(pThis->breakQuery_X);

	_agreeMent->MakeSetTilPos(m_pReq,posy,m_byAddr);
	sendCmd(m_pReq, PELCO_RESPONSE_Null);
	dtimer.startTimer(breakQuery_Y, 3000);
	for(; exitQuery_Y == false;)
	{
		if(m_stateChange)
		{
			ptzStop();
			break;
		}
		tmp.tv_sec = 0;
		tmp.tv_usec = 30*1000;
		select(0, NULL, NULL, NULL, &tmp);
		QueryTiltPos();
	}
	exitQuery_Y = false;
	pThis->dtimer.stopTimer(pThis->breakQuery_Y);
}

void CPTZControl::setZoomPos(Uint16 value)
{
	if(value < 2849)
		value = 2849;
	else if (value > 65535)
		value = 65535;
	zoom_bak = value;
	_agreeMent->MakeSetZoomPos(m_pReq,value,m_byAddr);
	sendCmd(m_pReq, PELCO_RESPONSE_Null);
	struct timeval tmp;
	for(; exitQueryzoom == false;)
	{
		if(m_stateChange)
		{
			ptzStop();
			break;
		}
		queryZoom();
	}
	exitQueryzoom = false;

}


void CPTZControl::simpleQueryPos()
{	
	QueryPanPos();
	struct timeval tmp;
	tmp.tv_sec = 0;
	tmp.tv_usec = 30*1000;
	select(0, NULL, NULL, NULL, &tmp);
	QueryTiltPos();
	tmp.tv_sec = 0;
	tmp.tv_usec = 30*1000;
	select(0, NULL, NULL, NULL, &tmp);
	return ;
}


void CPTZControl::queryZoom()
{
	_agreeMent->QueryZoomPos(m_pReq, m_byAddr);
	sendCmd(m_pReq, PELCO_RESPONSE_Extended);
	struct timeval tmp;
	tmp.tv_sec = 0;
	tmp.tv_usec = 30*1000;
	select(0, NULL, NULL, NULL, &tmp);

}

void CPTZControl::QueryPanPos()
{
	_agreeMent->QueryPanPos(m_pReq, m_byAddr);
	sendCmd(m_pReq, PELCO_RESPONSE_Extended);
}

void CPTZControl::QueryTiltPos()
{
	_agreeMent->QueryTiltPos(m_pReq, m_byAddr);
	sendCmd(m_pReq, PELCO_RESPONSE_Extended);
}

u_int8_t CPTZControl:: sendCheck_sum(uint len, u_int8_t *tmpbuf)
{
	u_int8_t  ckeSum=0;
	for(int n=0;n<len;n++)
		ckeSum ^= tmpbuf[n];
	return ckeSum;
}

u_int8_t CPTZControl:: package_frame(uint len, u_int8_t *tmpbuf)
{
	tmpbuf[0] = 0xEB;
	tmpbuf[1] = 0x53;
	tmpbuf[2] = len&0xff;
	tmpbuf[3] = (len>>8)&0xff;
	tmpbuf[len+4]= sendCheck_sum(len+3,tmpbuf+1);
	return 0;
}

void  CPTZControl::trackErrOutput()
{
	int msg_length = 5;
	u_int8_t  retrackErrOutput[msg_length+5];
	retrackErrOutput[4] = 0x08;
	retrackErrOutput[5]= _GlobalDate->errorOutPut[0];
	retrackErrOutput[5] = retrackErrOutput[5]&0xff;

	retrackErrOutput[6]= _GlobalDate->errorOutPut[0];
	retrackErrOutput[6] = (retrackErrOutput[6]>>8)&0xff;

	retrackErrOutput[7]= _GlobalDate->errorOutPut[1];
	retrackErrOutput[7] = retrackErrOutput[7]&0xff;

	retrackErrOutput[8]= _GlobalDate->errorOutPut[1];
	retrackErrOutput[8] = (retrackErrOutput[8]>>8)&0xff;

	package_frame(msg_length, retrackErrOutput);
	memcpy(trackBuf,retrackErrOutput,msg_length+5);
}

int CPTZControl::SendtrackErr()
{
	int iRet = OSA_SOK;

	OSA_mutexLock(&m_mutex);
	if(pCom == NULL || pCom->csend(fd_ptz, (unsigned char *)trackBuf, sizeof(trackBuf)) != sizeof(trackBuf))
	{
		OSA_printf("%s: send trackErr error! \n", __func__);
		OSA_mutexUnlock(&m_mutex);
		return OSA_EFAIL;
	}
	else
	{
		OSA_mutexUnlock(&m_mutex);
		return iRet;
	}
}

void CPTZControl::shRunOutput()
{
	_agreeMent->MakeMoveX();
	pCom->csend(fd_ptz, _agreeMent->sendBuf, 12);

	_agreeMent->MakeMoveY();
	pCom->csend(fd_ptz, _agreeMent->sendBuf, 12);

	_agreeMent->MakeStop();
	pCom->csend(fd_ptz, _agreeMent->sendBuf, 8);

}

void CPTZControl::PanoramicMirrorOutput()
{
#if 0

	static bool sign = true;
	if(sign)
	{
		PanoramicMirror_Init();
		sign = false;
	}
#endif
	struct timeval tmp;
	//printf("_GlobalDate->PanoramicMirror[0] = %d , _GlobalDate->PanoramicMirror[1] = %d \n",\
	//		_GlobalDate->PanoramicMirror[0], _GlobalDate->PanoramicMirror[1]);

	if(_GlobalDate->PanoramicMirror_value[0] != 0)
	{
		_agreeMent->MakeMoveX();
		pCom->csend(fd_ptz, _agreeMent->sendBuf, 10);
		tmp.tv_sec = 0;
		tmp.tv_usec = 5000;
		select(0, NULL, NULL, NULL, &tmp);
	}

	if(_GlobalDate->PanoramicMirror_value[1] != 0)
	{
		_agreeMent->MakeMoveY();
		pCom->csend(fd_ptz, _agreeMent->sendBuf, 10);
	}

	if(_GlobalDate->PanoramicMirror_value[0] == 0 && _GlobalDate->PanoramicMirror_value[1] == 0){
		_agreeMent->MakeStop();
		pCom->csend(fd_ptz, _agreeMent->sendBuf, 10);
	}

	for(int i=0; i<10; i++)
		printf("buf[%d]= %x ", i, _agreeMent->sendBuf[i]);
	putchar(10);


}

void CPTZControl::PanoramicMirror_Init()
{
	_agreeMent->SetSpeedX();
	/*
	for(int i=0; i<6; i++)
		printf("buf[%d]= %x ", i, _agreeMent->sendBuf[i]);
	putchar(10);
	*/
	pCom->csend(fd_ptz, _agreeMent->sendBuf, 6);
	_agreeMent->SetSpeedY();
	/*
	for(int i=0; i<6; i++)
		printf("buf[%d]= %x ", i, _agreeMent->sendBuf[i]);
	putchar(10);
	*/
	pCom->csend(fd_ptz, _agreeMent->sendBuf, 6);
}

void CPTZControl::MakeFoucusFar()
{
	_agreeMent->MakeFocusFar(NULL, 0);
	if(_GlobalDate->ptzType == Panoramic_mirror)
		pCom->csend(fd_ptz, _agreeMent->sendBuf, 10);
	else if(_GlobalDate->ptzType == sheenrun)
		pCom->csend(fd_ptz, _agreeMent->sendBuf, 8);
}

void CPTZControl::MakeFocusNear()
{
	_agreeMent->MakeFocusNear(NULL, 0);
	if(_GlobalDate->ptzType == Panoramic_mirror)
		pCom->csend(fd_ptz, _agreeMent->sendBuf, 10);
	else if(_GlobalDate->ptzType == sheenrun)
		pCom->csend(fd_ptz, _agreeMent->sendBuf, 8);
}

void CPTZControl::MakeFocusStop()
{
	_agreeMent->MakeFocusStop(NULL, 0);
	pCom->csend(fd_ptz, _agreeMent->sendBuf, 10);
}

void CPTZControl::test_left()
{
	_agreeMent->sendBuf[0] = 0x02;
	_agreeMent->sendBuf[1] = 0x91;
	_agreeMent->sendBuf[2] = 0x00;
	_agreeMent->sendBuf[3] = 0x01;
	_agreeMent->sendBuf[4] = 0x00;
	_agreeMent->sendBuf[5] = 0x02;
	_agreeMent->sendBuf[6] = 0x1E;
	_agreeMent->sendBuf[7] = 0x01;
	_agreeMent->sendBuf[8] = 0x00;
	_agreeMent->sendBuf[9] = 0x00;
	pCom->csend(fd_ptz, _agreeMent->sendBuf, 10);
}


void CPTZControl::test_stop()
{
	_agreeMent->MakeStop();
	pCom->csend(fd_ptz, _agreeMent->sendBuf, 10);
}


void CPTZControl::setPrepos(int& preposx,int& preposy)
{
	Preset(0x03, 20);
	queryPos();
	m_Mtd_Moitor_X = m_iPanPos;
	m_Mtd_Moitor_Y = m_iTiltPos;
	preposx = m_Mtd_Moitor_X;
	preposy = m_Mtd_Moitor_Y;
	return ;
}

int CPTZControl::runToPrepos(int arg)
{
	int ret = 0;
	switch(arg)
	{
		case 0:
			Preset(0x07, 20);
			judgepos(arg);
			break;
		case 1:
			Preset(0x07, 20);
			break;
		case 2:
			ret = judgepos(arg);
			break;
		default:
			break;
	}
	return ret;
}


void CPTZControl::queryPos()
{
	m_sync_pos = true;
	while(m_sync_pos)
		simpleQueryPos();
	return ;
}

bool CPTZControl::judgePanTilpos()
{
	if( m_Mtd_Moitor_X == m_iPanPos && m_Mtd_Moitor_Y == m_iTiltPos )
		return true;
	return false;
}

int CPTZControl::judgepos(int arg)
{
	int ret = 0;
	do
	{
		queryPos();
		if(judgePanTilpos())
		{	
			ret = 1;
			break;
		}
	}while(!arg);
	
	return ret;
}


void CPTZControl::getpos(int& pan, int& til)
{
	pan = m_iPanPos;
	til = m_iTiltPos;
	return;
}


void CPTZControl::getzoom(int& zoom)
{
	zoom = m_iZoomPos;
	return ;
}

