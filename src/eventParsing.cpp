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

	/*joystick button event*/
	if(jos_data[POV_BUTTON] != povbak)
	{
		switch(jos_data[POV_BUTTON])
		{
		case js_pov_up:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_POVPOSY, jos_data);
			break;
		case js_pov_right:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_POVPOSX, jos_data);
			break;
		case js_pov_down:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_POVPOSY, jos_data);
			break;
		case js_pov_left:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_POVPOSX, jos_data);
			break;
		case js_1:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRACKCTRL, jos_data);
			break;
		case js_2:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_SwitchSensor, jos_data);
			break;
		case js_3:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_ZOOMLONGCTRL, jos_data);
			break;
		case js_4:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_ZOOMSHORTCTRL, jos_data);
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
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_TRACKSEARCHCTRL, jos_data);
			break;
		case js_7:
			//_Msg->MSGDRIV_send(, jos_data);
			break;
		case js_8:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_captureMode, jos_data);
			break;
		case js_9:
			//_Msg->MSGDRIV_send(, jos_data);
			break;
		case js_10:
			//_Msg->MSGDRIV_send(, jos_data);
			break;
		case js_11:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_IrisAndFocusAndExit, jos_data);
			break;
		case js_12:
			_Msg->MSGDRIV_send(MSGID_EXT_INPUT_workMode, jos_data);
			break;
		}
		butbak = jos_data[BUTTON];
	}

	if(jos_data[AXIS_X] != xBak)
	{
		_Msg->MSGDRIV_send(MSGID_EXT_INPUT_JOSPOS, jos_data);
		xBak = jos_data[AXIS_X];
	}

	if(jos_data[AXIS_Y] != yBak)
	{
		_Msg->MSGDRIV_send(MSGID_EXT_INPUT_JOSPOS, jos_data);
		yBak = jos_data[AXIS_Y];
	}
}

void CEventParsing::parsingframe(unsigned char *tmpRcvBuff, int sizeRcv, int method)
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
		printf("------------------(fd:%d)Uart start recv date---------------------\n",method);
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
						parsingComEvent(method);
						memset(&swap_data, 0, sizeof(struct data_buf));
					}
				}
			}
		}
	}
}

int CEventParsing::parsingComEvent(int method)
{

	int ret =  -1;
	int cmdLength= (rcvBufQue.at(2)|rcvBufQue.at(3)<<8)+5;

	if(cmdLength<6)
	{
        	printf("Warning::Invaild frame\r\n");
        	rcvBufQue.erase(rcvBufQue.begin(),rcvBufQue.begin()+cmdLength);
        	return ret;
    	}
    	unsigned char checkSum = check_sum(cmdLength);

    	if(checkSum== rcvBufQue.at(cmdLength-1))
    	{	
        		switch(rcvBufQue.at(4))
        		{
            		case 0x01:
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
