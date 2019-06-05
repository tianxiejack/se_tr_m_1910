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

	int IPCSendMsg(CMD_ID cmd, void* prm, int len);
	int IPCRecvMsg(void* prm);

private:
	static CIPCProc* pThis;
	pthread_mutex_t mutex;

};
#endif
