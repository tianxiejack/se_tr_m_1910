/*
 * 803uart.cpp
 *
 *  Created on: 2019年7月31日
 *      Author: alex
 */

#include "803uart.h"
#include <math.h>
#include <opencv2/core/core.hpp>

using namespace cv;

IPC_PRM_INT gIpcParam;

static C803COM* gThis;
	
C803COM::C803COM(sendIpcMsgCallback pfunc):pCom1(NULL),pCom2(NULL),
					existRecvThread(false),m_cmdlength(9),m_trktime(3)
{
	memset(m_senddata,0,sizeof(m_senddata));
	memset(m_recvdata,0,sizeof(m_recvdata));
	OSA_mutexCreate(&m_com1mutex);
	m_rcvBuf.clear();
	gThis = this;
	pFunc_SendIpc = pfunc;
	readtrktime();
}

C803COM::~C803COM()
{
	if(pCom1 != NULL)
	{
		pCom1->cclose();
		delete pCom1;
	}

	if(pCom2 != NULL)
	{
		pCom1->cclose();
		delete pCom1;
	}
	
	OSA_mutexDelete(&m_com1mutex);
}


void C803COM::createPort()
{
	pCom1 = PortFactory::createProduct(1);
	pCom2 = PortFactory::createProduct(3);

	if(pCom1 != NULL)
		com1fd = pCom1->copen();
	if(pCom2 != NULL)
		com2fd = pCom2->copen();

	packageSendbaseData();

	return;
}

void C803COM::sendtrkerr(int chid,int status,float errx,float erry,int rendercount)
{
	int x,y;
	memset(m_senddata,0,sizeof(m_senddata));
	m_senddata[0] = 0x55;
	m_senddata[1] = 0xAA;

	if(1 == status || 2 == status)
		m_senddata[2] = 0x1;
	else
		m_senddata[2] = 0x0;
	
	x = (int)round(errx);
	m_senddata[3] = (abs(x)>>8)&(0xff);
	m_senddata[4] = abs(x)&(0xff);
	if(x<0)
		m_senddata[3] |= 0x80;

	y = (int)round(erry);
	m_senddata[5] = (abs(y)>>8)&(0xff);
	m_senddata[6] = abs(y)&(0xff);
	if(y<0)
		m_senddata[5] |= 0x80;
	
	if(chid == 0)
		m_senddata[7] = 1;
	else
		m_senddata[7] = 0;
	
	m_senddata[8] = 0x0;
	calcCheckNum();
	m_senddata[10] = 0x0d;
	m_senddata[11] = 0x0a;
	OSA_mutexLock(&m_com1mutex);

	pCom1->csend(com1fd, m_senddata, sizeof(m_senddata));
	OSA_mutexUnlock(&m_com1mutex);
	
	return;	
}

void C803COM::packageSendbaseData()
{
	m_senddata[0] = 0x55;
	m_senddata[1] = 0xAA;
	m_senddata[10] = 0x0D;
	m_senddata[11] = 0x0A;
	return;
}

void C803COM::calcCheckNum()
{
	int sum = 0;
	for(int i=3;i<=9;i++)
		sum += m_senddata[i-1];

	m_senddata[9] = sum&(0xff);
	return;	
}

void* C803COM::runUpExtcmd(void *)
{
	gThis->getExtcmd();
	return NULL;
}

void C803COM::getExtcmd()
{
	int sizercv;
	while(existRecvThread == false)
	{
		sizercv = pCom2->crecv(com2fd, m_recvdata,sizeof(m_recvdata));
		findValidData(m_recvdata,sizercv);
	}
}

void C803COM::findValidData(unsigned char *tmpRcvBuff, int sizeRcv)
{
	unsigned int uartdata_pos = 0;
	unsigned char frame_head[]={0xAA, 0x55};

	static struct data_buf
	{
		unsigned int len;
		unsigned int pos;
		unsigned char reading;
		unsigned char buf[1024];
	}swap_data = {0, 0, 0,{0}};

	if(sizeRcv>0)
	{
		for(int j=0;j<sizeRcv;j++)
		{
			printf(" %02x ",tmpRcvBuff[j]);
		}
		printf("\n");

		while (uartdata_pos < sizeRcv)
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
				if(swap_data.len == m_cmdlength)
				{
					for(int i=0;i<swap_data.len;i++)
					{
						m_rcvBuf.push_back(swap_data.buf[i]);
					}
					parsing();
					memset(&swap_data, 0, sizeof(struct data_buf));					
				}
			}
		}
	}
	return;
}

unsigned int C803COM::recvcheck_sum()
{
	unsigned int sum = 0;
	for(int i=2;i<=6;i++)
		sum += m_rcvBuf[i];
	return sum;
}


void C803COM::parsing()
{
	int ret =  -1;

	if(m_rcvBuf.size()<m_cmdlength)
		return;

	unsigned char checkSum = recvcheck_sum();

	if( ((checkSum>>8)&0xff) == m_rcvBuf[7] && ((checkSum&0xff) == m_rcvBuf[8]))
	{
		if(m_rcvBuf[2] != 0x0)
		{
			if(m_rcvBuf[2] == 0x1)
			{
				gIpcParam.intPrm[0] = 1;
				pFunc_SendIpc(changeSensor, gIpcParam.intPrm, 4);	
				printf("send sensor  1 \n");
			}
			else if(m_rcvBuf[2] == 0x2)
			{
				gIpcParam.intPrm[0] = 2;
				pFunc_SendIpc(changeSensor, gIpcParam.intPrm, 4);
				printf("send sensor  2 \n");
			}
		}

		if(m_rcvBuf[3] != 0x0)
		{
			if(m_rcvBuf[3] == 0x1)
			{
				gIpcParam.intPrm[0] = 0;
				pFunc_SendIpc(trk,gIpcParam.intPrm,4);
				printf("trk open \n");
			}
			else if(m_rcvBuf[3] == 0x2)
			{
				gIpcParam.intPrm[0] = 1;
				pFunc_SendIpc(trk,gIpcParam.intPrm,4);	
				printf("trk close \n");
			}
		}

		if(m_rcvBuf[4] != 0x0)
		{
			if(m_rcvBuf[4] == 0x1)
			{
				gIpcParam.intPrm[0] = 1;
				pFunc_SendIpc(mtd, gIpcParam.intPrm, 4);
				printf("mtd open \n");
			}
			else if(m_rcvBuf[4] == 0x2)
			{
				gIpcParam.intPrm[0] = 0;
				pFunc_SendIpc(mtd, gIpcParam.intPrm, 4);
				printf("mtd close \n");
			}		
		}

		if(m_rcvBuf[5] != 0x0)
		{
			if(m_rcvBuf[5] >= 0x1 && m_rcvBuf[5] <= 0x5 )
			{
				gIpcParam.intPrm[0] = m_rcvBuf[5];
				pFunc_SendIpc(trkMtdId, gIpcParam.intPrm, 4);
				printf("trk mtd id = %d \n" , gIpcParam.intPrm[0]);
			}	
		}

		if(m_rcvBuf[6] != 0x0)
		{
			if(m_rcvBuf[6] >= 0x1 && m_rcvBuf[6] <= 0xa )
			{
				m_trktime = m_rcvBuf[6];
				pFunc_SendIpc(settrktime, gIpcParam.intPrm, 4);
				saveTrktime();
				printf("trk time : %d \n", m_trktime);
			}	
		}
				

	}
	else
		printf("%s,%d, checksum error:,cal is %02x,recv is %02x\n",__FILE__,__LINE__,checkSum,((m_rcvBuf[5]<<8)|m_rcvBuf[6]));
	
	m_rcvBuf.erase(m_rcvBuf.begin(),m_rcvBuf.begin()+m_cmdlength);
	return ;
}


int C803COM::readtrktime()
{	
	string cfgAvtFile;
	char  cfg_avt[30] = "cfg_";
	cfgAvtFile = "Trktime.yml";
	FILE *fp = fopen(cfgAvtFile.c_str(), "rt");
	if(fp != NULL){
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);
		if(len < 10)
			return  -1;
		else
		{
			FileStorage fr(cfgAvtFile, FileStorage::READ);
			if(fr.isOpened())
			{
				sprintf(cfg_avt, "trktime");
				m_trktime = (int)fr[cfg_avt];	
			}
			else
			{
				printf("[ReadUartParams]open YML failed\n");
				return -1;
			}
		}
	}
	else
	{
		printf("[ReadUartParams] Can not find YML. Please put this file into the folder of execute file\n");
		return -1;
	}
	return 0;
}


void C803COM::saveTrktime()
{
	string cfgAvtFile;
	char  cfg_avt[30] = "cfg_avt_";
	cfgAvtFile = "Trktime.yml";
	int status = -1;

	FILE *fp = fopen(cfgAvtFile.c_str(), "rt+");

	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);
		
		if(len < 10)
			status  = 0;
		else
		{
			FileStorage fr(cfgAvtFile, FileStorage::WRITE);
			if(fr.isOpened())
			{
				sprintf(cfg_avt, "trktime");
				int value = m_trktime;
				fr<< cfg_avt << value;
				status  = 1;	
			}
			else
			{
				status  = 0;
			}
		}
	}
	else
	{
		fclose(fp);
		status  = 0;
	}	
}
