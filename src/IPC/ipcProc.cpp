#include <sys/time.h>
#include <assert.h>
#include <stdint.h>
#include "ipcProc.h"
#include "configtable.h"

CIPCProc* CIPCProc::pThis = 0;
//static float *sysConfig = NULL;
//static unsigned char *userConfig = NULL;

CIPCProc::CIPCProc()
	:m_thrRcvRun(0)
{
	pthread_mutex_init(&mutex,NULL);
	exitThreadIPCRcv = false;
}
CIPCProc::~CIPCProc()
{
	Destroy();

}

int CIPCProc::Create()
{
	pThis = this;
	if(m_thrRcvRun)
	{
		OSA_thrCreate(&thrHandlPCRcv,
				IPC_childdataRcvn,
		                  0,
		                  0,
		                  this);
	}
	 return 0;
}

int CIPCProc::Destroy()
{
	if(m_thrRcvRun)
	{
		exitThreadIPCRcv=true;
		OSA_thrJoin(&thrHandlPCRcv);
	}
	return 0;
}

int CIPCProc::IPCSendMsg(CMD_ID cmd, void* prm, int len)
{
	if(len >= PARAMLEN || (prm == NULL && len > 0))
		return -1;

	SENDST sendData;
	memset(sendData.param, 0, PARAMLEN);
	sendData.cmd_ID = cmd;
	if(len > 0)
		memcpy(sendData.param, prm, len);

	pthread_mutex_lock(&mutex);
	ipc_sendmsg(&sendData, IPC_TOIMG_MSG);
	pthread_mutex_unlock(&mutex);

	return 0;
}

int CIPCProc::IPCRecvMsg(void* prm)
{
	SENDST recvData;
	IPC_PRM_INT *pIn = (IPC_PRM_INT *)&recvData.param;
	ipc_recvmsg(&recvData,IPC_FRIMG_MSG);
	switch(recvData.cmd_ID)
	{
	case read_shm_block:
		{
			int configId = pIn->intPrm[0];
			int blkid = CFGID_blkId(configId);
			int feildId = CFGID_feildId(configId);
			/*if(blkid == CFGID_RTS_BKID)
			{
				if(prm != NULL)
					memcpy(prm, , );
			}*/
		}
		break;
	case read_shm_single:
		if(pIn->intPrm[0] == CFGID_RTS_trkstat)
			return (CFGID_RTS_trkstat);
		else if(pIn->intPrm[0] == CFGID_RTS_mtddet)
			return (CFGID_RTS_mtddet);
		break;

	default:
		break;
	}
	return 0;
}

void CIPCProc::getIPCMsgProc()
{
	SENDST recvData;
	IPC_PRM_INT *pIn = (IPC_PRM_INT *)&recvData.param;
	while(!exitThreadIPCRcv)
	{
		ipc_recvmsg(&recvData,IPC_FRIMG_MSG);
		//pthread_mutex_lock(&mutex);

		switch(recvData.cmd_ID)
		{
		default:
			break;
		}
		//pthread_mutex_unlock(&mutex);
	}
}

#if 0
int  CIPCProc::ipcTrackCtrl(volatile unsigned char AvtTrkStat)
{
    memset(test.param,0,PARAMLEN);
	test.cmd_ID = trk;
	test.param[0]=AvtTrkStat;
	ipc_sendmsg(&test,IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::ipcSceneTrkCtrl(volatile unsigned char senceTrkStat)
{
    memset(test.param,0,PARAMLEN);
	test.cmd_ID = sceneTrk;
	test.param[0]=senceTrkStat;
	ipc_sendmsg(&test,IPC_TOIMG_MSG);

	return 0;
}


int  CIPCProc::ipcMutilTargetDetecCtrl(volatile unsigned char ImgMmtStat)//1:open 0:close
{
    memset(test.param,0,PARAMLEN);
	test.cmd_ID = mmt;
	test.param[0]=ImgMmtStat;
	ipc_sendmsg(&test,IPC_TOIMG_MSG);

	return 0;
}

int  CIPCProc::ipcMutilTargetSelectCtrl(volatile unsigned char ImgMmtSelect)
{
    memset(test.param,0,PARAMLEN);
	test.cmd_ID = mmtselect;
	test.param[0]=ImgMmtSelect;
	//ipc_sendmsg(&test,IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IpcMmtLockCtrl(int mmt_Select)
{
	memset(test.param,0,PARAMLEN);
	test.cmd_ID = mmtLock;
	ipc_sendmsg(&test,IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::ipcImageEnhanceCtrl(volatile unsigned char ImgEnhStat) //1open 0close
{
    memset(test.param,0,PARAMLEN);
	test.cmd_ID = enh;
	test.param[0]=ImgEnhStat;
	ipc_sendmsg(&test,IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::ipcMoveTatgetDetecCtrl(volatile unsigned char ImgMtdStat)
{
    memset(test.param,0,PARAMLEN);
	test.cmd_ID = mtd;
	test.param[0]=ImgMtdStat;
	ipc_sendmsg(&test,IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::ipcSecTrkCtrl(selectTrack *m_selcTrak)
{
	CMD_SECTRK cmd_sectrk;
    memset(test.param,0,PARAMLEN);
	test.cmd_ID = sectrk;

	cmd_sectrk.SecAcqStat = m_selcTrak->SecAcqStat;
	cmd_sectrk.ImgPixelX =m_selcTrak->ImgPixelX;
	cmd_sectrk.ImgPixelY =m_selcTrak->ImgPixelY;
	memcpy(test.param, &cmd_sectrk, sizeof(cmd_sectrk));
	ipc_sendmsg(&test,IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::IpcSensorSwitch(volatile unsigned char ImgSenchannel)
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = sensor;
	test.param[0] = ImgSenchannel;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::IpcpinpCtrl(volatile unsigned char ImgPipStat)
{
	CMD_PinP cmd_pip;
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = pinp;
	cmd_pip.ImgPicp = ImgPipStat;
	cmd_pip.PicpSensorStat = 1;
	cmd_pip.PicpZoomStat = 1;
	memcpy(test.param, &cmd_pip, sizeof(cmd_pip));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::IpcAcqDoorCtrl(AcqBoxSize *BoxSize)
{
	memset(test.param, 0, PARAMLEN);
	AcqBoxWH m_acqBoxWH;
	test.cmd_ID = acqBox;
	{
		m_acqBoxWH.AimW = BoxSize->AcqBoxW[0];
		m_acqBoxWH.AimH = BoxSize->AcqBoxH[0];
		memcpy(test.param, &m_acqBoxWH, sizeof(m_acqBoxWH));
		ipc_sendmsg(&test, IPC_TOIMG_MSG);
	}
	return 0;
}

int CIPCProc::IpcIrisAndFocus(osd_triangle* osd_tri, char sign)
{
	memset(test.param, 0, PARAMLEN);
	CMD_triangle cmd_triangel;
	switch(sign)
	{
	case Exit:
		test.cmd_ID = exit_IrisAndFocus;
		break;
	case iris:
		test.cmd_ID = Iris;
		break;
	case Focus:
		test.cmd_ID = focus;
		break;
	}
	cmd_triangel.dir = osd_tri->dir;
	cmd_triangel.alpha = osd_tri->alpha;
	memcpy(test.param, &cmd_triangel, sizeof(cmd_triangel));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IpcFuncMenu(char sign)
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = menu;
	test.param[0] = sign;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IpcTrkPosMoveCtrl(POSMOVE * avtMove)
{
	memset(test.param, 0, PARAMLEN);
	CMD_POSMOVE cmd_posMove;
	test.cmd_ID = posmove;
	cmd_posMove.AvtMoveX = avtMove->AvtMoveX;
	cmd_posMove.AvtMoveY = avtMove->AvtMoveY;
	memcpy(test.param, &cmd_posMove, sizeof(cmd_posMove));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::IpcConfig()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = read_shm_config;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IpcConfigOSD()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = read_shm_osd;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::IpcConfigOSDTEXT()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = read_shm_osdtext;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IpcConfigUTC()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = read_shm_utctrk;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::IpcSendLosttime()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = ipclosttime;
	memcpy(&test.param[0],&losttime , 4);
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IPCSendMtdFrame()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = mtdFrame;
	pMtd = &Mtd_Frame;
	memcpy(&test.param, pMtd, sizeof(Mtd_Frame));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::IPCSendPos(int pan, int tilt, int zoom)
{
	LinkagePos cmd_linkagePos;
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = querypos;
	cmd_linkagePos.panPos = pan;
	cmd_linkagePos.tilPos = tilt;
	cmd_linkagePos.zoom = zoom;	//0;
	memcpy(&test.param,&cmd_linkagePos , sizeof(cmd_linkagePos));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::refreshPtzParam(int pan, int tilt, int zoom)
{
	LinkagePos cmd_linkagePos;
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = refreshptz;
	cmd_linkagePos.panPos = pan;
	cmd_linkagePos.tilPos = tilt;
	cmd_linkagePos.zoom = zoom;	//0;
	memcpy(&test.param,&cmd_linkagePos , sizeof(cmd_linkagePos));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::IPCSendBallParam()
{
	memset(test.param, 0, PARAMLEN);
	int chid = _GlobalDate->chid_camera;
	test.cmd_ID = ballbaud;
	memcpy(test.param, &_GlobalDate->m_uart_params.balladdress, sizeof(_GlobalDate->m_uart_params.balladdress));
	memcpy(&test.param[4], &_GlobalDate->m_uart_params.baudrate, sizeof(_GlobalDate->m_uart_params.baudrate));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::IpcWordFont(char font)
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = ipcwordFont;
	test.param[0] = font;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IpcWordSize(char size)
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = ipcwordSize;
	test.param[0] = size;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IPCConfigCamera()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = read_shm_camera;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IPCMtdSwitch(volatile unsigned char ImgMtdStat, volatile unsigned char mtdMode)
{
	CMD_MTD cmd_mtd;
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = mtd;
	cmd_mtd.ImgMtdStat = ImgMtdStat;
	cmd_mtd.mtdMode = mtdMode;
	memcpy(test.param, &cmd_mtd, sizeof(cmd_mtd));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IPCMtdSelectCtrl(volatile unsigned char MtdSelect)
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = mtdSelect;
	test.param[0] = MtdSelect;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IpcSwitchTarget()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = switchtarget;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}
	
int CIPCProc::IpcElectronicZoom(int zoom)
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = elecZoom;
	test.param[0] = zoom;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IPCChannel_binding(int channel)
{
	memset(test.param, 0, PARAMLEN);
	return 0;
}

int CIPCProc::IPCAxisMove(int x, int y)
{
	memset(test.param, 0, PARAMLEN);
	CMD_POSMOVE cmd_axismove;
	test.cmd_ID = axismove;
	cmd_axismove.AvtMoveX = x;
	cmd_axismove.AvtMoveY = y;
	memcpy(test.param, &cmd_axismove, sizeof(cmd_axismove));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}


int CIPCProc::IPCpicp(int status, int pipChannel)
{
	memset(test.param, 0, PARAMLEN);
	CMD_PinP cmd_pip;
	test.cmd_ID = pinp;
	cmd_pip.ImgPicp = status;
	cmd_pip.PicpSensorStat = pipChannel;
	memcpy(test.param, &cmd_pip, sizeof(cmd_pip));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;

}
int CIPCProc::IPCAlgosdrect(int tmp)
{
	memset(test.param, 0, PARAMLEN);
	CMD_ALGOSDRECT cmd_algosdrect;
	test.cmd_ID = algosdrect;
	cmd_algosdrect.Imgalgosdrect = tmp;
	memcpy(test.param, &cmd_algosdrect, sizeof(cmd_algosdrect));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IPCchooseVideoChannel(int channel)
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = sensor;
	test.param[0] = channel;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::IPCOsdWord(volatile osdbuffer_t &inosd)
{	
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = osdbuffer;
	memcpy(&test.param[0],(const char*)&inosd,sizeof(osdbuffer_t));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IPCBoresightPosCtrl(unsigned int x,unsigned int y)
{
	memset(test.param, 0, PARAMLEN);
	CMD_BoresightPos cmd_BoresightPos;
	test.cmd_ID = BoresightPos;
	cmd_BoresightPos.BoresightPos_x = x;
	cmd_BoresightPos.BoresightPos_y = y;
	memcpy(test.param, &cmd_BoresightPos, sizeof(cmd_BoresightPos));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::ipcAcqBoxCtrl(AcqBoxPos_ipc* m_acqPos)
{
	CMD_AcqBoxPos cmd_acqPos;
    memset(test.param,0,PARAMLEN);
	test.cmd_ID = AcqPos;

	cmd_acqPos.AcqStat = m_acqPos->AcqStat;
	cmd_acqPos.BoresightPos_x = m_acqPos->BoresightPos_x;
	cmd_acqPos.BoresightPos_y = m_acqPos->BoresightPos_y;
	memcpy(test.param, &cmd_acqPos, sizeof(cmd_acqPos));
	ipc_sendmsg(&test,IPC_TOIMG_MSG);

	return 0;
}

int CIPCProc::IPCResolution(CMD_IPCRESOLUTION tmp)
{
	memset(test.param, 0, PARAMLEN);
	CMD_IPCRESOLUTION cmd_resolution;
	test.cmd_ID = ipcresolution;
	cmd_resolution.resolution[ipc_eSen_CH0] = tmp.resolution[ipc_eSen_CH0];
	cmd_resolution.resolution[ipc_eSen_CH1] = tmp.resolution[ipc_eSen_CH1];
	cmd_resolution.resolution[ipc_eSen_CH2] = tmp.resolution[ipc_eSen_CH2];
	cmd_resolution.resolution[ipc_eSen_CH3] = tmp.resolution[ipc_eSen_CH3];
	cmd_resolution.resolution[ipc_eSen_CH4] = tmp.resolution[ipc_eSen_CH4];
	cmd_resolution.outputresol = tmp.outputresol;
	memcpy(test.param, &cmd_resolution, sizeof(cmd_resolution));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IPCSendWorkMode(int workMode)
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = workmode;
	test.param[0] = workMode;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}
#endif
