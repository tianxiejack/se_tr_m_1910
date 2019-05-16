/*
 * eventParsing.cpp
 *
 *  Created on: 2019年5月7日
 *      Author: d
 */
#include "eventParsing.hpp"
#include <unistd.h>
#include <arpa/inet.h>

CEventParsing* CEventParsing::pThis = NULL;

CEventParsing::CEventParsing()
{
	pThis = this;
	exit_jsParsing = false;
	_Msg = CMessage::getInstance();
	_Js = new CjoyStick();

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
		pThis->parsingframe(tmpRcvBuff, sizeRcv, pThis->comfd);
	}

}

void CEventParsing::thread_comsendEvent()
{
	char buf[128] = {0xEB,0x53,0x02,0x00,0x07,0x09, 0x0E};
	while(!pThis->exit_comParsing)
	{
		if(pThis->connetVector.size()>0)
		{
			pThis->pCom1->csend(pThis->pThis->connetVector[0]->connfd, buf, 7);
		}
		sleep(1);
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
		pThis->parsingframe(tmpRcvBuff, sizeRcv, pConnect->connfd);
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
void CEventParsing::parsingJostickEvent(unsigned char* jos_data)
{
	static int povbak=0, butbak=0, xBak=0, yBak=0;
	static int Iris_Focus;
	/*joystick button event*/
	if(jos_data[POV_BUTTON] != povbak)
	{
		switch(jos_data[POV_BUTTON])
		{
		case js_pov_up:
			ComParams.trkmove = 0x04;
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_POVPOSY, &ComParams);
			break;
		case js_pov_right:
			ComParams.trkmove = 0x02;
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_POVPOSX, &ComParams);
			break;
		case js_pov_down:
			ComParams.trkmove = 0x08;
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_POVPOSY, &ComParams);
			break;
		case js_pov_left:
			ComParams.trkmove = 0x01;
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_POVPOSX, &ComParams);
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
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_ZOOMLONGCTRL, &ComParams);
			break;
		case js_4:
			if(ComParams.zoomctrl == 0)
				ComParams.zoomctrl = 1;
			else
				ComParams.zoomctrl = 0;
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_ZOOMSHORTCTRL, &ComParams);
			break;
		}
		povbak = jos_data[POV_BUTTON];
	}

	if(jos_data[BUTTON] != butbak)
	{
		switch(jos_data[BUTTON])
		{
		case js_5:
			//_Msg->MSGDRIV_send(, jos_data);
			break;
		case js_6:
			ComParams.sectrkctrl = ComParams.sectrkctrl % 2 + 1;
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRACKSEARCHCTRL, &ComParams);
			break;
		case js_7:
			//_Msg->MSGDRIV_send(, jos_data);
			break;
		case js_8:
			ComParams.capturemode = (ComParams.capturemode + 1)%3;
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_captureMode, &ComParams);
			break;
		case js_9:
			//_Msg->MSGDRIV_send(, jos_data);
			break;
		case js_10:
			//_Msg->MSGDRIV_send(, jos_data);
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
		}
		butbak = jos_data[BUTTON];
	}

	if(jos_data[AXIS_X] != xBak)
	{
		ComParams.platspeedx = jos_data[AXIS_X];
		_Msg->MSGDRIV_send(MSGID_EXT_INPUT_JOSPOS, &ComParams);
		xBak = jos_data[AXIS_X];
	}

	if(jos_data[AXIS_Y] != yBak)
	{
		ComParams.platspeedy = jos_data[AXIS_Y];
		_Msg->MSGDRIV_send(MSGID_EXT_INPUT_JOSPOS, &ComParams);
		yBak = jos_data[AXIS_Y];
	}
}

void CEventParsing::parsingframe(unsigned char *tmpRcvBuff, int sizeRcv, int fd)
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
		printf("------------------(fd:%d)Uart start recv date---------------------\n",fd);
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
						parsingComEvent(fd);
						memset(&swap_data, 0, sizeof(struct data_buf));
					}
				}
			}
		}
	}
}

int CEventParsing::parsingComEvent(int fd)
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
    	unsigned char checkSum = check_sum(cmdLength);

    	if(checkSum== rcvBufQue.at(cmdLength-1))
    	{	
    		ComParams.fd = fd;
        		switch(rcvBufQue.at(4))
        		{
            		case 0x01:
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_SELFCHECK, NULL);
                			break;
			case 0x02:
				ComParams.displaychid = rcvBufQue.at(5);
				ComParams.capturechid = rcvBufQue.at(6);
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_SwitchSensor, &ComParams);
				break;
			case 0x03:
				ComParams.workmode = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_workMode, &ComParams);
				break;
			case 0x04:
				ComParams.capturemode = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_captureMode, &ComParams);
				break;
			case 0x05:
				ComParams.trkctrl = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRACKCTRL, &ComParams);
				break;
			case 0x06:
				ComParams.trkmove = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_TRKMOVE, &ComParams);
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
				ComParams.presetpos = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_PRESETPOS, &ComParams);
				break;
			case 0x12:
				ComParams.setzoom = rcvBufQue.at(5) |(rcvBufQue.at(6) << 8);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_SETZOOM, &ComParams);
				break;
			case 0x41:
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_QUERYPTZPOS, NULL);
				break;
			case 0x42:
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_GETZOOM, NULL);
				break;
			case 0x43:
				ComParams.trkoutput = rcvBufQue.at(5);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_TRKOUTPUT, &ComParams);
				break;
			case 0x51:
				settmp.block= rcvBufQue.at(5);
				settmp.field= rcvBufQue.at(6);
				for(int m = 7; m < 11; m++)
					tempbuf[m-7]  = rcvBufQue.at(m);
				memcpy(&settmp.value, tempbuf, 4);
				ComParams.setConfigQueue.push_back(settmp);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_SETCFG, &ComParams);
				break;
			case 0x52:
				gettmp.block = rcvBufQue.at(5);
				gettmp.field = rcvBufQue.at(6);
				ComParams.getConfigQueue.push_back(gettmp);
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_GETCFG, &ComParams);
				break;
			case 0x53:
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
				_Msg->MSGDRIV_send(MSGID_COM_INPUT_SAVECFG, NULL);
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

unsigned char CEventParsing::check_sum(int len_t)
{
    unsigned char cksum = 0;
    for(int n=4; n<len_t-1; n++)
    {
        cksum ^= rcvBufQue.at(n);
    }
    return  cksum;
}
