/*
 * eventParsing.cpp
 *
 *  Created on: 2019年5月7日
 *      Author: d
 */
#include "eventParsing.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include "osa_sem.h"

CEventParsing* CEventParsing::pThis = NULL;
ACK_ComParams_t ACK_ComParams;
OSA_SemHndl  m_semHndl;
OSA_SemHndl m_semHndl_s;

CEventParsing::CEventParsing()
{
	pThis = this;
	exit_jsParsing = false;
	_Msg = CMessage::getInstance();
	_Js = new CjoyStick();

	OSA_semCreate(&m_semHndl, 1, 0);
	OSA_semCreate(&m_semHndl_s, 1, 0);
	OSA_semSignal(&m_semHndl_s);
	memset(&ComParams, 0, sizeof(ComParams));
	exit_comParsing = false;
	pCom1 = PortFactory::createProduct(1);
	comfd = pCom1->copen();
	exit_netParsing = false;
	memset(&mutexConn,0,sizeof(mutexConn));
	OSA_mutexCreate(&mutexConn);
	pCom2 = PortFactory::createProduct(2);
	pCom2->copen();
}

CEventParsing::~CEventParsing()
{
	exit_jsParsing = true;
	delete _Msg;
	delete pThis;

	OSA_semDelete(&m_semHndl);
	OSA_semDelete(&m_semHndl_s);
	exit_comParsing = true;
	exit_netParsing = true;
	pCom1->cclose();
	pCom2->cclose();
	delete pCom1;
	delete pCom2;
}

void CEventParsing::thread_jsEvent()
{
	while(!pThis->exit_jsParsing)
	{
		pThis->parsingJostickEvent(pThis->_Js->JoystickProcess());
	}
}

void CEventParsing::thread_comrecvEvent()
{
	int sizeRcv;
 	uint8_t dest[1024]={0};
	int read_status = 0;
	int dest_cnt = 0;
	unsigned char  tmpRcvBuff[RECV_BUF_SIZE];
	memset(tmpRcvBuff,0,sizeof(tmpRcvBuff));

	while(!pThis->exit_comParsing)
	{
		sizeRcv= pThis->pCom1->crecv(pThis->comfd, (void *)tmpRcvBuff,RECV_BUF_SIZE);
		comtype_t comtype = {pThis->comfd, 1};
		pThis->parsingframe(tmpRcvBuff, sizeRcv, comtype);
	}
}

void CEventParsing::thread_comsendEvent()
{
	sendInfo repSendBuffer;
	
	while(!pThis->exit_comParsing)
	{
		OSA_semWait(&m_semHndl,OSA_TIMEOUT_FOREVER);
		pThis->getSendInfo(&repSendBuffer);
		if(1 == repSendBuffer.comtype.type)
		{
			pThis->pCom1->csend(repSendBuffer.comtype.fd, &repSendBuffer.sendBuff, repSendBuffer.byteSizeSend);
		}

		else if(2 == repSendBuffer.comtype.type)
		{
			OSA_mutexLock(&pThis->mutexConn);
			if((pThis->connetVector.size()>0) && (pThis->connetVector[0]->bConnecting))
			{
				pThis->pCom2->csend(repSendBuffer.comtype.fd, &repSendBuffer.sendBuff, repSendBuffer.byteSizeSend);
#if 1
				printf("net send %d bytes:\n", repSendBuffer.byteSizeSend);
				for(int i = 0; i < repSendBuffer.byteSizeSend; i++)
					printf("%02x ", repSendBuffer.sendBuff[i]);
				printf("\n");
#endif


			}
			OSA_mutexUnlock(&pThis->mutexConn);
		}
		
		if((ACK_GetConfig == ACK_ComParams.cmdid) ||(ACK_SetConfig == ACK_ComParams.cmdid) ||(ACK_GetOsd == ACK_ComParams.cmdid) ||(ACK_DefaultConfig == ACK_ComParams.cmdid))
			OSA_semSignal(&m_semHndl_s);
	}
}

void CEventParsing::thread_Getaccept()
{
	int iret;
	struct sockaddr_in connectAddr;
	memset(&connectAddr, 0, sizeof(connectAddr));
	int lenthConnect = sizeof(connectAddr);
	
    	while(!pThis->exit_netParsing)
    	{
    		int m_connectfd=pThis->pCom2->caccept((struct sockaddr*)&connectAddr,(socklen_t*)&lenthConnect);
		if(m_connectfd<0)
		{
			printf("ERR: Can not  accept  Client connect\r\n");
			continue;
		}
		else
		{
			char *connectIP = inet_ntoa(connectAddr.sin_addr);
			int connectPort = ntohs(connectAddr.sin_port);
			printf("INFO: Accept Connect IP : %s---- Port: %d,connfd:%d\r\n",connectIP, connectPort, m_connectfd);
			
			CConnectVECTOR *pConnect = (CConnectVECTOR *)malloc(sizeof(CConnectVECTOR));
			pConnect->connfd = m_connectfd;
			OSA_mutexLock(&pThis->mutexConn);
			pThis->connetVector.push_back(pConnect);
			OSA_mutexUnlock(&pThis->mutexConn);

			pConnect->bConnecting=true;
			iret = OSA_thrCreate(&pConnect->getDataThrID,
			        thread_netrecvEvent,
			        0,
			        0,
			        (void*)pConnect);
		}
	}
}

void *CEventParsing::thread_netrecvEvent(void *p)
{
	int sizeRcv;
 	uint8_t dest[1024]={0};
	int read_status = 0;
	int dest_cnt = 0;
	unsigned char  tmpRcvBuff[RECV_BUF_SIZE];
	memset(tmpRcvBuff,0,sizeof(tmpRcvBuff));

	CConnectVECTOR *pConnect = (CConnectVECTOR *)p;
	while(!pThis->exit_netParsing)
	{
		sizeRcv= pThis->pCom2->crecv(pConnect->connfd, (void *)tmpRcvBuff,RECV_BUF_SIZE);
		if(-2 == sizeRcv)
		{
			pConnect->bConnecting = false;
			break;
		}
		comtype_t comtype = {pConnect->connfd, 2};
		pThis->parsingframe(tmpRcvBuff, sizeRcv, comtype);
	}
}

int CEventParsing::thread_ReclaimConnect()
{
	while(!pThis->exit_netParsing)
	{
		OSA_mutexLock(&pThis->mutexConn);
		vector<CConnectVECTOR *>::iterator iter = pThis->connetVector.begin();
		for (iter; iter != pThis->connetVector.end();)
		{
			CConnectVECTOR *pConnect = *iter;
			if (!pConnect->bConnecting)
			{
				printf("INFO:  Reclainm connect !!\r\n");
				OSA_thrDelete(&pConnect->getDataThrID);
				pThis->connetVector.erase(iter);
				free(pConnect);
			}
			else
			{
				iter++;
			}
		}
		OSA_mutexUnlock(&pThis->mutexConn);
				
		struct timeval tv;
		int err;
		do{
			tv.tv_sec = TIME_FOR_THREAD_END;
			tv.tv_usec = 0;
			err = select(0, NULL, NULL, NULL, &tv);
		}while(err<0 && errno==EINTR);
	}

	if(pThis->exit_netParsing)
	{
		OSA_mutexLock(&pThis->mutexConn);
		vector<CConnectVECTOR *>::iterator iter = pThis->connetVector.begin();
		for (iter; iter != pThis->connetVector.end();)
		{
			CConnectVECTOR *pConnect = *iter;
			if (pConnect->bConnecting)
			{
				pConnect->bConnecting = false;
				OSA_thrDelete(&pConnect->getDataThrID);
				pThis->connetVector.erase(iter);
				printf("*pConnect=%p\n", pConnect);
				free(pConnect);
			}
			else
			{
				++iter;
			}
		}
		OSA_mutexUnlock(&pThis->mutexConn);
		
		struct timeval tv;
		int err;
		do{
			tv.tv_sec = TIME_FOR_THREAD_END;
			tv.tv_usec = 0;
			err = select(0, NULL, NULL, NULL, &tv);
		}while(err<0 && errno==EINTR);
			
	}
    	return 0;
}



void CEventParsing::buttonStopHandle(int index)
{
	switch(index)
	{
		case 3:
			if(ComParams.zoomctrl == 0)
				ComParams.zoomctrl = 2;
			else
				ComParams.zoomctrl = 0;
			_Msg->MSGDRIV_send(MSGID_COM_INPUT_ZOOMCTRL, &ComParams);
			break;
			
		case 4:
			if(ComParams.zoomctrl == 0)
				ComParams.zoomctrl = 1;
			else
				ComParams.zoomctrl = 0;
			_Msg->MSGDRIV_send(MSGID_COM_INPUT_ZOOMCTRL, &ComParams);
			break;

		case 6:
			ComParams.sectrkctrl = ComParams.sectrkctrl % 2 + 1;
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRACKSEARCHCTRL, &ComParams);
			break;
			
		default:
			break;
	}
	return ;
}



void CEventParsing::parsingJostickPovData(unsigned char* jos_data)
{
	static unsigned char povbak = 0;
	static int index = 0;
	if(jos_data[POV_BUTTON] != povbak)
	{		
		switch(jos_data[POV_BUTTON])
		{
			case js_pov_up:
				ComParams.trkmove = 4;
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRKMOVE, &ComParams);
				break;
			case js_pov_right:
				ComParams.trkmove = 2;
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRKMOVE, &ComParams);
				break;
			case js_pov_down:
				ComParams.trkmove = 8;
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRKMOVE, &ComParams);
				break;
			case js_pov_left:
				ComParams.trkmove = 1;
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRKMOVE, &ComParams);
				break;
			case js_1:
				ComParams.trkctrl = 0x00;
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRACKCTRL, &ComParams);
				break;
			case js_2:
				ComParams.displaychid = 0;
				ComParams.capturechid = 0xff;
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_SwitchSensor, &ComParams);
				break;
			case js_3:
				if(ComParams.zoomctrl == 0)
					ComParams.zoomctrl = 2;
				else
					ComParams.zoomctrl = 0;
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_ZOOMCTRL, &ComParams);
				index = 3;
				break;
			case js_4:
				if(ComParams.zoomctrl == 0)
					ComParams.zoomctrl = 1;
				else
					ComParams.zoomctrl = 0;
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_ZOOMCTRL, &ComParams);
				index = 4;
				break;
			default:
				buttonStopHandle(index);	
				index = 0;
				break;
		}
		povbak = jos_data[POV_BUTTON];
	}

	return;
}



void CEventParsing::parsingJostickButtonData(unsigned char* jos_data)
{
	static unsigned char butbak = 0;
	static int Iris_Focus = 0;
	static int index = 0;
	if(jos_data[BUTTON] != butbak)
	{
		switch(jos_data[BUTTON])
		{
			case js_5:
				break;
			case js_6:
				ComParams.sectrkctrl = ComParams.sectrkctrl % 2 + 1;
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRACKSEARCHCTRL, &ComParams);
				index = 6;
				break;
			case js_7:

				break;
			case js_8:
				ComParams.capturemode = (ComParams.capturemode + 1)%3;
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_captureMode, &ComParams);
				break;
			case js_9:
				ComParams.prepos = 0x2;
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_PREPOS, &ComParams);
				break;
			case js_10:
				ComParams.prepos = 0x1;
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_PREPOS, &ComParams);
				break;
			case js_11:
				Iris_Focus= (Iris_Focus + 1)%3;
				if(Iris_Focus == 1)
				{
					ComParams.irisctrl= 0x03;
					ComParams.focusctrl = 0;
				}
				else if(Iris_Focus == 2)
				{
					ComParams.focusctrl = 0x03;
					ComParams.irisctrl= 0;
				}
				else
					ComParams.irisctrl = ComParams.focusctrl = 0;
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_IrisAndFocusAndExit, &ComParams);
				break;
			case js_12:
				ComParams.workmode = (ComParams.workmode + 1)%3;
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_workMode, &ComParams);
				break;
			default:
				buttonStopHandle(index);
				index = 0;
				break;
		}
		butbak = jos_data[BUTTON];
	}
	
	return ;
}



void CEventParsing::parsingJostickAxisData(unsigned char* jos_data)
{
	static unsigned char xBak=0, yBak=0;
	bool flag = false;
	if(jos_data[AXIS_X] != xBak)
	{
		ComParams.platspeedx = jos_data[AXIS_X];
		xBak = jos_data[AXIS_X];
		flag = true;
	}

	if(jos_data[AXIS_Y] != yBak)
	{
		ComParams.platspeedy = jos_data[AXIS_Y];
		yBak = jos_data[AXIS_Y];
		flag = true;
	}

	if(flag)
		_Msg->MSGDRIV_send(MSGID_EXT_INPUT_JOSPOS, &ComParams);
	
	return ;
}



void CEventParsing::parsingJostickEvent(unsigned char* jos_data)
{
	/*joystick button event*/
	parsingJostickPovData(jos_data);
	parsingJostickButtonData(jos_data);
	parsingJostickAxisData(jos_data);
	return;
}

void CEventParsing::parsingframe(unsigned char *tmpRcvBuff, int sizeRcv, comtype_t comtype)
{
	unsigned int uartdata_pos = 0;
	unsigned char frame_head[]={0xEB, 0x53};
	
	static struct data_buf
	{
		unsigned int len;
		unsigned int pos;
		unsigned char reading;
		unsigned char buf[RECV_BUF_SIZE];
	}swap_data = {0, 0, 0,{0}};


	uartdata_pos = 0;
	if(sizeRcv>0)
	{
		printf("------------------(fd:%d)start recv date---------------------\n", comtype.fd);
		for(int j=0;j<sizeRcv;j++)
		{
			printf("%02x ",tmpRcvBuff[j]);
		}
		printf("\n");

		while (uartdata_pos< sizeRcv)
		{
	        		if((0 == swap_data.reading) || (2 == swap_data.reading))
	       		{
	            			if(frame_head[swap_data.len] == tmpRcvBuff[uartdata_pos])
	            			{
	                			swap_data.buf[swap_data.pos++] =  tmpRcvBuff[uartdata_pos++];
	                			swap_data.len++;
	                			swap_data.reading = 2;
	                			if(swap_data.len == sizeof(frame_head)/sizeof(char))
	                    				swap_data.reading = 1;
	            			}
		           		 else
		            		{
		                		uartdata_pos++;
		                		if(2 == swap_data.reading)
		                    		memset(&swap_data, 0, sizeof(struct data_buf));
		            		}
			}
		        	else if(1 == swap_data.reading)
			{
				swap_data.buf[swap_data.pos++] = tmpRcvBuff[uartdata_pos++];
				swap_data.len++;
				if(swap_data.len>=4)
				{
					if(swap_data.len==((swap_data.buf[2]|(swap_data.buf[3]<<8))+5))
					{

						for(int i=0;i<swap_data.len;i++)
						{
							rcvBufQue.push_back(swap_data.buf[i]);
						}
						parsingComEvent(comtype);
						memset(&swap_data, 0, sizeof(struct data_buf));
					}
				}
			}
		}
	}
}

int CEventParsing::parsingComEvent(comtype_t comtype)
{
	int ret =  -1;
	int cmdLength= (rcvBufQue.at(2)|rcvBufQue.at(3)<<8)+5;
	int block, field;
	float value;
	Set_config_t settmp;
	Get_config_t gettmp;
	osdbuffer_t osdtmp;
	unsigned char tempbuf[4];
	unsigned char contentBuff[128]={0};

	if(cmdLength<6)
	{
        	printf("Warning::Invaild frame\r\n");
        	rcvBufQue.erase(rcvBufQue.begin(),rcvBufQue.begin()+cmdLength);
        	return ret;
    	}
    	unsigned char checkSum = recvcheck_sum(cmdLength);

    	if(checkSum== rcvBufQue.at(cmdLength-1))
    	{	
    		ComParams.comtype = comtype;
        		switch(rcvBufQue.at(4))
        		{
            		case 0x01:
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_SELFCHECK, &ComParams);
                			break;
			case 0x02:
				ComParams.displaychid = rcvBufQue.at(5);
				ComParams.capturechid = rcvBufQue.at(6);
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_SwitchSensor, &ComParams);
				break;
			case 0x03:
				ComParams.workmode = rcvBufQue.at(5) - 1;
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_workMode, &ComParams);
				break;
			case 0x04:
				ComParams.capturemode = rcvBufQue.at(5) - 1;
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_captureMode, &ComParams);
				break;
			case 0x05:
				ComParams.trkctrl = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRACKCTRL, &ComParams);
				break;
			case 0x06:
				ComParams.trkmove = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRKMOVE, &ComParams);
				break;
			case 0x07:
				ComParams.sectrkctrl = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRACKSEARCHCTRL, &ComParams);
				break;
			case 0x08:
				ComParams.sectrkx = rcvBufQue.at(5) |(rcvBufQue.at(6) << 8);
				ComParams.sectrky = rcvBufQue.at(7) |(rcvBufQue.at(8) << 8);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_SECTRKPOS, &ComParams);
				break;
			case 0x09:
				ComParams.mtdselect = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_MTDSELECT, &ComParams);
				break;
			case 0x0a:
				ComParams.gatemove = rcvBufQue.at(5);
				ComParams.gatestepx = rcvBufQue.at(6) |(rcvBufQue.at(7) << 8);
				ComParams.gatestepy = rcvBufQue.at(8) |(rcvBufQue.at(9) << 8);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_GATEMOVE, &ComParams);
				break;
			case 0x0C:
				ComParams.zoomctrl = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_ZOOMCTRL, &ComParams);
				break;
			case 0x0D:
				ComParams.irisctrl = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_APERTURECTRL, &ComParams);
				break;
			case 0x0E:
				ComParams.focusctrl = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_FOCUSCTRL, &ComParams);
				break;
			case 0x0F:
				ComParams.platspeedx = rcvBufQue.at(5) |(rcvBufQue.at(6) << 8);
				ComParams.platspeedy = rcvBufQue.at(7) |(rcvBufQue.at(8) << 8);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_SETPLATSPEED, &ComParams);
				break;
			case 0x10:
				ComParams.platPan = rcvBufQue.at(5) |(rcvBufQue.at(6) << 8);
				ComParams.platTilt = rcvBufQue.at(7) |(rcvBufQue.at(8) << 8);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_SETPLATANGLE, &ComParams);
				break;
			case 0x11:
				ComParams.prepos = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_PREPOS, &ComParams);
				break;
			case 0x12:
				ComParams.setzoom = rcvBufQue.at(5) |(rcvBufQue.at(6) << 8);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_SETZOOM, &ComParams);
				break;
			case 0x41:
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_QUERYPTZPOS, &ComParams);
				break;
			case 0x42:
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_GETZOOM, &ComParams);
				break;
			case 0x43:
				ComParams.trkoutput = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_TRKOUTPUT, &ComParams);
				break;
			case 0x51:
				{
					settmp.block= rcvBufQue.at(5);
					settmp.field= rcvBufQue.at(6);
					for(int m = 7; m < 11; m++)
						tempbuf[m-7]  = rcvBufQue.at(m);

					memcpy(&settmp.value, tempbuf, 4);
					
					ComParams.setConfigQueue.push_back(settmp);
					_Msg->MSGDRIV_send(MSGID_COM_INPUT_SETCFG, &ComParams);
				}
				break;
			case 0x52:
				gettmp.block = rcvBufQue.at(5);
				gettmp.field = rcvBufQue.at(6);
				ComParams.getConfigQueue.push_back(gettmp);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_GETCFG, &ComParams);
				break;
			case 0x53:
				memset(&osdtmp, 0, sizeof(osdtmp));
				osdtmp.osdID = rcvBufQue.at(5);
				osdtmp.type = rcvBufQue.at(6);
				for(int i = 0; i < cmdLength -8; i++){
					osdtmp.buf[i]= rcvBufQue.at(7+i);
				}
				ComParams.setosd = osdtmp;
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_SETOSD, &ComParams);
				break;
			case 0x54:
				block = rcvBufQue.at(5);
				ComParams.defConfigQueue.push_back(block);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_DEFAULTCFG, &ComParams);
				break;
			case 0x57:
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_SAVECFG, &ComParams);
				break;
            		default:
               			 printf("INFO: Unknow  Control Command, please check!!!\r\n ");
                			ret =0;
                			break;
       		 }
    	}
    	else
        		printf("%s,%d, checksum error:,cal is %02x,recv is %02x\n",__FILE__,__LINE__,checkSum,rcvBufQue.at(cmdLength-1));
		
	rcvBufQue.erase(rcvBufQue.begin(),rcvBufQue.begin()+cmdLength);
	return 1;

}

unsigned char CEventParsing::recvcheck_sum(int len_t)
{
    unsigned char cksum = 0;
    for(int n=4; n<len_t-1; n++)
    {
        cksum ^= rcvBufQue.at(n);
    }
    return  cksum;
}

int  CEventParsing::getSendInfo(sendInfo * psendBuf)
{
	int respondId = ACK_ComParams.cmdid;
	switch(respondId){
		case ACK_Sensor:
			package_ACK_Sensor(psendBuf);
			break;
		case ACK_Workmode:
			package_ACK_Workmode(psendBuf);
			break;
		case ACK_CaptureMode:
			package_ACK_Capturemode(psendBuf);
			break;
		case ACK_TrkStat:
			package_ACK_TrkStat(psendBuf);
			break;
		case ACK_SectrkStat:
			package_ACK_SecTrkStat(psendBuf);
			break;
		case ACK_CtrlPos:
			package_ACK_CtrlPos(psendBuf);
			break;
		case ACK_SetZoom:
			package_ACK_SetZoom(psendBuf);
			break;
		case ACK_QueryPos:
			package_ACK_QueryPos(psendBuf);
			break;
		case ACK_QueryZoom:
			package_ACK_QueryZoom(psendBuf);
			break;
		case ACK_output:
			package_ACK_Output(psendBuf);
			break;
		case ACK_GetConfig:
			package_ACK_GetConfig(psendBuf);
			break;
		case ACK_SetConfig:
			package_ACK_SetConfig(psendBuf);
			break;
		case ACK_DefaultConfig:
			package_ACK_DefaultConfig(psendBuf);
			break;
		case ACK_saveconfig:
			package_ACK_SaveConfig(psendBuf);
			break;
		case ACK_GetOsd:
			package_ACK_GetOsd(psendBuf);
			break;
		default:
			break;
	}
	return 0;
}

int  CEventParsing::package_ACK_Sensor(sendInfo *psendBuf)
{
	int bodylen = 3;
	psendBuf->sendBuff[4] = ACK_Sensor;
	psendBuf->sendBuff[5] = ACK_ComParams.displaychid;
	psendBuf->sendBuff[6] = ACK_ComParams.capturechid;
	
	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_Workmode(sendInfo *psendBuf)
{
	int bodylen = 2;
	psendBuf->sendBuff[4] = ACK_Workmode;
	psendBuf->sendBuff[5] = ACK_ComParams.workmode;
	
	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_Capturemode(sendInfo *psendBuf)
{
	int bodylen = 2;
	psendBuf->sendBuff[4] = ACK_CaptureMode;
	psendBuf->sendBuff[5] = ACK_ComParams.capturemode;
	
	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_TrkStat(sendInfo *psendBuf)
{
	int bodylen = 2;
	psendBuf->sendBuff[4] = ACK_TrkStat;
	psendBuf->sendBuff[5] = ACK_ComParams.trkctrl;
	
	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_SecTrkStat(sendInfo *psendBuf)
{
	int bodylen = 2;
	psendBuf->sendBuff[4] = ACK_SectrkStat;
	psendBuf->sendBuff[5] = ACK_ComParams.sectrkctrl;
	
	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_CtrlPos(sendInfo *psendBuf)
{
	int bodylen = 5;
	psendBuf->sendBuff[4] = ACK_CtrlPos;
	psendBuf->sendBuff[5] = ACK_ComParams.ctrlpan & 0xff;
	psendBuf->sendBuff[6] = (ACK_ComParams.ctrlpan >> 8) & 0xff;
	psendBuf->sendBuff[7] = ACK_ComParams.ctrltil & 0xff;
	psendBuf->sendBuff[8] = (ACK_ComParams.ctrltil >> 8) & 0xff;

	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_SetZoom(sendInfo *psendBuf)
{
	int bodylen = 3;
	psendBuf->sendBuff[4] = ACK_QueryZoom;
	psendBuf->sendBuff[5] = ACK_ComParams.setzoom & 0xff;
	psendBuf->sendBuff[6] = (ACK_ComParams.setzoom >> 8) & 0xff;
	
	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_QueryPos(sendInfo *psendBuf)
{
	int bodylen = 5;
	psendBuf->sendBuff[4] = ACK_QueryPos;
	psendBuf->sendBuff[5] = ACK_ComParams.querypan & 0xff;
	psendBuf->sendBuff[6] = (ACK_ComParams.querypan >> 8) & 0xff;
	psendBuf->sendBuff[7] = ACK_ComParams.querytil & 0xff;
	psendBuf->sendBuff[8] = (ACK_ComParams.querytil >> 8) & 0xff;
	
	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_QueryZoom(sendInfo *psendBuf)
{
	int bodylen = 3;
	psendBuf->sendBuff[4] = ACK_QueryZoom;
	psendBuf->sendBuff[5] = ACK_ComParams.queryzoom & 0xff;
	psendBuf->sendBuff[6] = (ACK_ComParams.queryzoom >> 8) & 0xff;
	
	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_Output(sendInfo *psendBuf)
{

	int bodylen = 11;
	psendBuf->sendBuff[4] = ACK_output;
	psendBuf->sendBuff[5] = ACK_ComParams.trkstat;
	psendBuf->sendBuff[6] = ACK_ComParams.outtype;
	memcpy(psendBuf->sendBuff + 7, &ACK_ComParams.trkerrx, 4);
	memcpy(psendBuf->sendBuff + 11, &ACK_ComParams.trkerry, 4);

	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_GetConfig(sendInfo *psendBuf)
{
	int bodylen = 7;
	psendBuf->sendBuff[4] = ACK_GetConfig;
	psendBuf->sendBuff[5] = ACK_ComParams.getConfigQueue[0].block;
	psendBuf->sendBuff[6] = ACK_ComParams.getConfigQueue[0].field;
	memcpy(psendBuf->sendBuff+7, &ACK_ComParams.getConfigQueue[0].value, 4);
	ACK_ComParams.getConfigQueue.erase(ACK_ComParams.getConfigQueue.begin());
	
	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_SetConfig(sendInfo *psendBuf)
{
	int bodylen = 7;
	psendBuf->sendBuff[4] = ACK_SetConfig;
	psendBuf->sendBuff[5] = ACK_ComParams.setConfigQueue[0].block;
	psendBuf->sendBuff[6] = ACK_ComParams.setConfigQueue[0].field;
	memcpy(psendBuf->sendBuff+7, &ACK_ComParams.setConfigQueue[0].value, 4);
	ACK_ComParams.setConfigQueue.erase(ACK_ComParams.setConfigQueue.begin());
	
	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_DefaultConfig(sendInfo *psendBuf)
{
	int bodylen = 2;
	psendBuf->sendBuff[4] = ACK_DefaultConfig;
	psendBuf->sendBuff[5] = ACK_ComParams.defConfigQueue[0];
	ACK_ComParams.defConfigQueue.erase(ACK_ComParams.defConfigQueue.begin());
	
	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_SaveConfig(sendInfo *psendBuf)
{
	int bodylen = 2;
	psendBuf->sendBuff[4] = ACK_saveconfig;
	psendBuf->sendBuff[5] = ACK_ComParams.saveconfig;
	
	package_ACK_commondata(psendBuf, bodylen);
}
int  CEventParsing::package_ACK_GetOsd(sendInfo *psendBuf)
{
	int osdlength = strlen((char *)ACK_ComParams.getOsdQueue[0].buf)<128?strlen((char *)ACK_ComParams.getOsdQueue[0].buf):128;
	int bodylen = osdlength + 3;
	psendBuf->sendBuff[4] = ACK_GetConfig;
	psendBuf->sendBuff[5] = ACK_ComParams.getOsdQueue[0].block;
	psendBuf->sendBuff[6] = ACK_ComParams.getOsdQueue[0].field;
	memcpy(psendBuf->sendBuff+7, &ACK_ComParams.getOsdQueue[0].buf, osdlength);
	ACK_ComParams.getOsdQueue.erase(ACK_ComParams.getOsdQueue.begin());
	
	package_ACK_commondata(psendBuf, bodylen);
}


int  CEventParsing::package_ACK_commondata(sendInfo *psendBuf, int bodylen)
{
	psendBuf->sendBuff[0] = 0xEB;
	psendBuf->sendBuff[1] = 0x53;
	psendBuf->sendBuff[2] = bodylen&0xff;
	psendBuf->sendBuff[3] = (bodylen>>8)&0xff;

	unsigned char sumcheck=sendcheck_sum(psendBuf->sendBuff+4, bodylen);
	psendBuf->sendBuff[bodylen+4]=(sumcheck&0xff);
	psendBuf->byteSizeSend = bodylen + 5;
	psendBuf->comtype = ACK_ComParams.comtype;
}


unsigned char CEventParsing::sendcheck_sum(unsigned char *tmpbuf, int len)
{
	unsigned char ckeSum=0;
	for(int n=0; n < len; n++)
		ckeSum ^= tmpbuf[n];
	return ckeSum;
}
