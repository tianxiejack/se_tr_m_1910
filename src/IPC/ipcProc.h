#ifndef _IPCPROC_H_
#define _IPCPROC_H_

#include "osa_thr.h"
#include "ipc_custom_head.hpp"
#include <string>
#pragma once
using namespace std;

class  CIPCProc{

public:
	CIPCProc();
	~CIPCProc();
	int Create();
	int Destroy();

	int IPCSendMsg(CMD_ID cmd, void* prm, int len);
	int IPCRecvMsg(void* prm);

	int m_thrRcvRun;
	static void *IPC_childdataRcvn(void * prm){
		CIPCProc *pThis = (CIPCProc*)prm;
		pThis->getIPCMsgProc();
	};

private:
	static CIPCProc* pThis;
	bool exitThreadIPCRcv;
	OSA_ThrHndl thrHandlPCRcv;
	pthread_mutex_t mutex;
	void getIPCMsgProc();

};
#endif
