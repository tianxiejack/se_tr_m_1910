/*
 * EventManager.cpp
 *
 *  Created on: 2019年5月8日
 *      Author: d
 */
#include "EventManager.hpp"
#include "configtable.h"
#include <opencv2/core/core.hpp>

CEventManager* CEventManager::pThis = NULL;
extern ACK_ComParams_t ACK_ComParams;
extern OSA_SemHndl  m_semHndl;
extern OSA_SemHndl m_semHndl_s;
int profileNum = (CFGID_BKID_MAX-1)*16;

using namespace cv;

CEventManager::CEventManager()
{
	pThis = this;
	_Msg = CMessage::getInstance();
	_state = new PlatFormCapture();
	_StateManager = new StateManger(_state);
	_Handle = _Msg->MSGDRIV_create();
	MSG_register();
	m_ipc = (CIPCProc*)_StateManager->returnIpc();
	IPC_Creat();
	ReadConfigFile();
	_StateManager->GetParams(cfg_value);
	_state->StateInit();
}

CEventManager::~CEventManager()
{
	_Msg->MSGDRIV_destroy(_Handle);
	delete _Msg;
}

void CEventManager::IPC_Creat()
{
    	Ipc_init();
   	int ret = Ipc_create();
    	if(ret == -1)
    	{
    		printf("[%s] %d ipc create error \n", __func__, __LINE__);
    		return;
    	}
	
	cfg_value = (float *)ipc_getSharedMem(IPC_IMG_SHA);
	usr_value = ipc_getSharedMem(IPC_USER_SHA);
}

void CEventManager::MSG_register()
{
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_TRACKCTRL, MSG_Trk, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_SwitchSensor, MSG_SwitchSensor, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_ZOOMLONGCTRL, MSG_ZoomLong, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_ZOOMSHORTCTRL, MSG_ZoomShort, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_TRACKSEARCHCTRL, MSG_TrkSearch, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_captureMode, MSG_CaptureMode, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_IrisAndFocusAndExit, MSG_IrisAndFocus, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_workMode, MSG_WorkMode, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_JOSPOS, MSG_JosPos, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_POVPOSX, MSG_PovPosX, NULL);
	_Msg->MSGDRIV_register(MSGID_EXT_INPUT_POVPOSY, MSG_PovPosY, NULL);

	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SELFCHECK, MSG_Com_SelfCheck, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_TRKMOVE, MSG_Com_TrkMove, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SECTRKPOS, MSG_Com_SecTrkPos, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_MTDSELECT, MSG_Com_MtdSelect, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_GATEMOVE, MSG_Com_Gatemove, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_FOCUSCTRL, MSG_Com_FocusCtrl, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_APERTURECTRL, MSG_Com_IrisCtrl, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_ZOOMCTRL, MSG_Com_ZoomCtrl, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETPLATSPEED, MSG_Com_SetPlatSpeed, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETPLATANGLE, MSG_Com_SetPlatAngle, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_PRESETPOS, MSG_Com_PrsetPos, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETZOOM, MSG_Com_SetZoom, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_QUERYPTZPOS, MSG_Com_QueryPtzPos, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_GETZOOM, MSG_Com_GetZoom, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_TRKOUTPUT, MSG_Com_TrkOutput, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETCFG, MSG_Com_SetCfg, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_GETCFG, MSG_Com_GetCfg, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETOSD, MSG_Com_SetOsd, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_DEFAULTCFG, MSG_Com_DefaultCfg, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SAVECFG, MSG_Com_SaveCfg, NULL);

}

void CEventManager::MSG_Trk(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	if(tmp->trkctrl == 0)
		tmp->trkctrl = !pThis->cfg_value[CFGID_RTS_trkstat];
	if(tmp->trkctrl == 2)
		tmp->trkctrl = 0;
	pThis->_StateManager->inter_TrkCtrl(tmp->trkctrl);
}
void CEventManager::MSG_SwitchSensor(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int displaychid = tmp->displaychid;
	int capturechid = tmp->capturechid;
	printf("displaychid=%d,capturechid=%d\n",displaychid,capturechid);
	pThis->_StateManager->inter_SwitchSensor(capturechid);
}
void CEventManager::MSG_ZoomLong(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	printf(" 3 => long = %d \n", tmp->zoomctrl);
	pThis->_StateManager->inter_ZoomCtrl(tmp->zoomctrl);
}
void CEventManager::MSG_ZoomShort(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	printf(" 4 => short = %d \n", tmp->zoomctrl);
	pThis->_StateManager->inter_ZoomCtrl(tmp->zoomctrl);
}
void CEventManager::MSG_TrkSearch(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	printf(" 1111  tmp->sectrkctrl = %d \n", tmp->sectrkctrl);
}
void CEventManager::MSG_CaptureMode(void* p)
{
	static int captureModeBack;
	ComParams_t *tmp = (ComParams_t *)p;
	if(tmp->workmode == ROUTINE_STATE)
	{
		if(tmp->capturemode == 0)
			pThis->_StateManager->ToPlatFormCapture();
		else if(tmp->capturemode == 1)
			pThis->_StateManager->ToBoxCapture();
		else if(tmp->capturemode == 2)
			pThis->_StateManager->ToManualMtdCapture();
		captureModeBack = tmp->capturemode;
	}
	else
	{
		tmp->capturemode = captureModeBack;
	}

}
void CEventManager::MSG_IrisAndFocus(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	printf("tmp->irisctrl = %d , tmp->focusctrl = %d \n", tmp->irisctrl, tmp->focusctrl);
}
void CEventManager::MSG_WorkMode(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	if(tmp->workmode == ROUTINE_STATE)
	{
		if(tmp->capturemode == 0)
			pThis->_StateManager->ToPlatFormCapture();
		else if(tmp->capturemode == 1)
			pThis->_StateManager->ToBoxCapture();
		else if(tmp->capturemode == 2)
			pThis->_StateManager->ToManualMtdCapture();
	}
	else if(tmp->workmode == STATE_AUTOMTD)
		pThis->_StateManager->ToStateAuto_Mtd();
	else if(tmp->workmode == STATE_SCENETRK)
		pThis->_StateManager->ToStateSceneSelect();
}
void CEventManager::MSG_JosPos(void* p)
{
#if 0
	ComParams_t *tmp = (ComParams_t *)p;
	int curState = pThis->_StateManager->CurStateInterface();
	switch(curState)
	{
	case STATE_PTZ:
		if(tmp->sectrkctrl == 0x01)
			printf("ptz search\n");
		else if(tmp->irisctrl == 0x03)
			printf("ptz iris \n");
		else if(tmp->focusctrl == 0x03)
			printf("ptz  focus \n");
		else
			printf("ptz jos move \n");
		break;
	case STATE_BOX:
		if(tmp->sectrkctrl == 0x01)
			printf("Box search\n");
		else if(tmp->irisctrl == 0x03)
			printf("Box iris \n");
		else if(tmp->focusctrl == 0x03)
			printf("Box  focus \n");
		else
			printf("Box jos move \n");
		break;
	case STATE_MANUALMTD:
		if(tmp->irisctrl == 0x03)
			printf("manual iris \n");
		else if(tmp->focusctrl == 0x03)
			printf("manual  focus \n");
		else
			printf("manual Mtd jos move \n");
		break;
	case STATE_SCENETRK:
		if(tmp->irisctrl == 0x03)
			printf("scene iris \n");
		else if(tmp->focusctrl == 0x03)
			printf("scene  focus \n");
		else
			printf("scene jos move\n");
		break;
	}
	//if(tmp->sectrkctrl);
	//MSG_Com_ApertureCtrl(p);
	//MSG_Com_FocusCtrl(p);
	//MSG_Com_Gatemove(p);
#endif
#if 0
	if(tmp->platspeedx > 0 &&tmp->platspeedx < 0x7a)
		pThis->_StateManager->inter_AxisMove(PTZ_MOVE_Left, tmp->platspeedx);
	else if(tmp->platspeedx > 0x90 &&tmp->platspeedx < 0xff)
		pThis->_StateManager->inter_AxisMove(PTZ_MOVE_Right,tmp->platspeedx);
	else
		pThis->_StateManager->inter_AxisMove(PTZ_MOVE_Stop, 0);
#endif
}
void CEventManager::MSG_PovPosX(void* p)
{
	MSG_Com_TrkMove(p);
	MSG_Com_MtdSelect(p);
}
void CEventManager::MSG_PovPosY(void* p)
{
	MSG_Com_TrkMove(p);
}

void CEventManager::MSG_Com_SelfCheck(void* p)
{
	printf("MSG_Com_SelfCheck start\n");
}

void CEventManager::MSG_Com_TrkMove(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int trkmove = tmp->trkmove;
	printf("trkmove=%d\n",trkmove);
}

void CEventManager::MSG_Com_SecTrkPos(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	short sectrkx = tmp->sectrkx;
	short sectrky = tmp->sectrky;
	printf("sectrk x,y(%d,%d)\n",sectrkx, sectrky);
}
void CEventManager::MSG_Com_MtdSelect(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int mtdselect = tmp->mtdselect;
	printf("mtdselect=%d\n",mtdselect);
}
void CEventManager::MSG_Com_Gatemove(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int gatemove = tmp->gatemove;
	unsigned short gatestepx = tmp->gatestepx;
	unsigned short gatestepy = tmp->gatestepy;
	printf("gatemove=%d, gatestepx=%d, gatestepy=%d\n",gatemove, gatestepx, gatestepy);
}
void CEventManager::MSG_Com_ZoomCtrl(void* p)
{
	//MSG_ZoomLong(p);
	//MSG_ZoomShort(p);

}
void CEventManager::MSG_Com_IrisCtrl(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	printf("aperturectrl=%d\n",tmp->irisctrl);
}
void CEventManager::MSG_Com_FocusCtrl(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	printf("focusctrl=%d\n",tmp->focusctrl);
}
void CEventManager::MSG_Com_SetPlatSpeed(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	short platspeedx = tmp->platspeedx;
	short platspeedy = tmp->platspeedy;
	printf("platspeed x,y=(%d,%d)\n",platspeedx, platspeedy);
}
void CEventManager::MSG_Com_SetPlatAngle(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	unsigned short platPan = tmp->platPan;
	unsigned short platTilt = tmp->platTilt;
	printf("platangle x,y=(%d,%d)\n",platPan, platTilt);
}
void CEventManager::MSG_Com_PrsetPos(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	unsigned short presetpos = tmp->presetpos;
	printf("presetpos=%d\n", presetpos);
}
void CEventManager::MSG_Com_SetZoom(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	unsigned short setzoom = tmp->setzoom;
	printf("setzoom=%d\n", setzoom);
}
void CEventManager::MSG_Com_QueryPtzPos(void* p)
{
	printf("MSG_Com_GetPlatAngle start\n");
}
void CEventManager::MSG_Com_GetZoom(void* p)
{
	printf("MSG_Com_GetZoom start\n");
}
void CEventManager::MSG_Com_TrkOutput(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	unsigned short trkoutput = tmp->trkoutput;
	printf("trkoutput=%d\n", trkoutput);
}
void CEventManager::MSG_Com_SetCfg(void* p)
{
	Set_config_t tmpcfg;
	ComParams_t *tmp = (ComParams_t *)p;
	while(tmp->setConfigQueue.size()){
		tmpcfg = tmp->setConfigQueue[0];
		printf("setcfg block,field,value(%d,%d,%f)\n", tmpcfg.block,tmpcfg.field,tmpcfg.value);
		pThis->SetConfig( tmpcfg.block, tmpcfg.field, tmpcfg.value, NULL);
		tmp->setConfigQueue.erase(tmp->setConfigQueue.begin());
	}
}
void CEventManager::MSG_Com_GetCfg(void* p)
{
	Get_config_t tmpcfg;
	ComParams_t *tmp = (ComParams_t *)p;
	while(tmp->getConfigQueue.size()){
		tmpcfg = tmp->getConfigQueue[0];
		printf("getcfg block,field(%d,%d)\n", tmpcfg.block,tmpcfg.field);
		OSA_semWait(&m_semHndl_s, OSA_TIMEOUT_FOREVER);
		pThis->GetConfig(tmp->fd, tmpcfg.block, tmpcfg.field);
		tmp->getConfigQueue.erase(tmp->getConfigQueue.begin());
	}
}
void CEventManager::MSG_Com_SetOsd(void* p)
{
	osdbuffer_t tmpcfg;
	ComParams_t *tmp = (ComParams_t *)p;
	tmpcfg = tmp->setosd;
	printf("osdid:%d, type:%d, buf=%s\n",tmpcfg.osdID, tmpcfg.type, tmpcfg.buf);
}
void CEventManager::MSG_Com_DefaultCfg(void* p)
{
	int status , block;
	ComParams_t *tmp = (ComParams_t *)p;
	while(tmp->defConfigQueue.size()){
		block = tmp->defConfigQueue.at(0);
		printf("default block=%d\n", block);
		pThis->DefaultConfig(tmp->fd, block);
		tmp->defConfigQueue.erase(tmp->defConfigQueue.begin());
	}
}
void CEventManager::MSG_Com_SaveCfg(void* p)
{
	printf("MSG_Com_SaveCfg start\n");
	pThis->SaveConfig();
}

int  CEventManager::ReadConfigFile()
{	
	string cfgAvtFile;
	int configId_Max = profileNum;
	char  cfg_avt[30] = "cfg_avt_";
	cfgAvtFile = "Profile.yml";
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
				for(int i=0; i<configId_Max; i++)
				{
					sprintf(cfg_avt, "cfg_avt_%d", i);
					float value = fr[cfg_avt];
					//printf("read value[%d]=%f\n", i, value);
					cfg_value[i] = value;
				}
			}
			else
			{
				printf("[get params]open YML failed\n");
				exit(-1);
			}
		}
	}
	else
	{
		printf("[get params] Can not find YML. Please put this file into the folder of execute file\n");
		exit (-1);
	}
	m_ipc->IPCSendMsg(read_shm_config, NULL, 0);
	return 0;
}

int CEventManager::SetConfig(int block, int field, float value,char *inBuf)
{
	int cfgid = CFGID_BUILD(block-1, field);
	cfg_value[cfgid] = value;
	m_ipc->IPCSendMsg(read_shm_single, &cfgid, 4);

	return 0;
}
int CEventManager::GetConfig(int fd, int block, int field)
{
	int cfgid = CFGID_BUILD(block-1, field);
	float value = cfg_value[cfgid];
	
	Set_config_t tmp = {block, field, value};
	ACK_ComParams.fd = fd;
	ACK_ComParams.cmdid  = ACK_GetConfig;
	ACK_ComParams.getConfigQueue.push_back(tmp);
	OSA_semSignal(&m_semHndl);

	return 0;
}
int CEventManager::DefaultConfig(int fd, int blockId)
{
	string cfgAvtFile;
	int configId_Max =profileNum;
	char  cfg_avt[20] = "cfg_avt_";
	cfgAvtFile = "Profile.yml";
	FILE *fp = fopen(cfgAvtFile.c_str(), "rt");

	if(fp != NULL)
	{
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
				for(int i=0; i<configId_Max; i++)
				{
					sprintf(cfg_avt, "cfg_avt_%d", i);
					
					int block = CFGID_blkId(i) + 1;
					if(0 == blockId)
						cfg_value[i] = (float)fr[cfg_avt];
					else if(block == blockId)
						cfg_value[i] = (float)fr[cfg_avt];
				}
			}
			else
			{
				printf("[back to default] YML open failed\n");
			}
		}
	}
	else
	{
		printf("[back to default] Can not find YML. Please put this file into the folder of execute file\n");
	}
	return 0;
}
int CEventManager::SaveConfig()
{
	printf("-----------save config------\n");
	string cfgAvtFile;
	int configId_Max = profileNum;
	char  cfg_avt[30] = "cfg_avt_";
	cfgAvtFile = "Profile.yml";

	FILE *fp = fopen(cfgAvtFile.c_str(), "rt+");

	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);
		
		if(len < 10)
			return  -1;
		else
		{
			FileStorage fr(cfgAvtFile, FileStorage::WRITE);
			if(fr.isOpened())
			{
				for(int i=0; i<configId_Max; i++)
				{
					sprintf(cfg_avt, "cfg_avt_%d", i);
					float value = cfg_value[i];
					fr<< cfg_avt << value;
				}
			}
			else
				return -1;
		}
	}
	else
		fclose(fp);
}
