/*
 * 803uart.h
 *
 *  Created on: 2019年7月31日
 *      Author: alex
 */

#ifndef FILE803UART_H_
#define FILE803UART_H_

#include "ipc_custom_head.hpp"
#include <vector>
#include "PortFactory.hpp"
#include "osa_mutex.h"

typedef int (*sendIpcMsgCallback)(CMD_ID cmd, void* prm, int len);

class C803COM
{
public:
	C803COM(sendIpcMsgCallback pfunc);
	~C803COM();
	
	void createPort();
	void sendtrkerr(int chid,int status,float errx,float erry,int rendercount);
	void getExtcmd();
	static void* runUpExtcmd(void*);
	void sendmtdprm(IPC_MTD_COORD_T inPrm);

protected:
	void packageSendbaseData();
	void findValidData(unsigned char *tmpRcvBuff, int sizeRcv);
	void parsing();
	unsigned int recvcheck_sum();
	void calcCheckNum();
	int readtrktime();
	void saveTrktime();
	void calcCheckNumMtdprm();
	int parsingComEvent();
	unsigned char recvcheck_sum(int len_t);


private:
	CPortInterface* pCom1,*pCom2;
	int com1fd,com2fd;
	unsigned char m_senddata[12];
	unsigned char m_recvdata[1024];
	unsigned char m_sendMtdPrm[26];

	bool existRecvThread;
	OSA_MutexHndl m_com1mutex;
	std::vector<unsigned char>  m_rcvBuf;
	int m_cmdlength;
	unsigned int m_trktime;
	sendIpcMsgCallback pFunc_SendIpc;

	std::vector<unsigned char>  rcvBufQue;

};




#endif /* 803UART_H_ */
