/*
 * EventManager.cpp
 *
 *  Created on: 2019年5月8日
 *      Author: d
 */
#include "EventManager.hpp"
CEventManager* CEventManager::pThis = NULL;

CEventManager::CEventManager()
{
	pThis = this;
	_Msg = CMessage::getInstance();
	_state = new StateConvention();
	_state->StateInit();
	_StateManager = new StateManger(_state);
	_Handle = _Msg->MSGDRIV_create();
	MSG_register();
}

CEventManager::~CEventManager()
{
	_Msg->MSGDRIV_destroy(_Handle);
	delete _Msg;
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
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SWITCHSENSOR, MSG_Com_SwitchSensor, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_WORKMODE, MSG_Com_WorkMode, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_CAPTUREMODE, MSG_Com_CaptureMode, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_TRACKCTRL, MSG_Com_TrkCtrl, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_TRKMOVE, MSG_Com_TrkMove, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SECTRKCTRL, MSG_Com_SecTrkCtrl, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SECTRKPOS, MSG_Com_SecTrkPos, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_MTDSELECT, MSG_Com_MtdSelect, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_GATEMOVE, MSG_Com_Gatemove, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_ZOOMCTRL, MSG_Com_ZoomCtrl, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_APERTURECTRL, MSG_Com_ApertureCtrl, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_FOCUSCTRL, MSG_Com_FocusCtrl, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETPLATSPEED, MSG_Com_SetPlatSpeed, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETPLATANGLE, MSG_Com_SetPlatAngle, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_PRESETPOS, MSG_Com_PrsetPos, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_SETZOOM, MSG_Com_SetZoom, NULL);
	_Msg->MSGDRIV_register(MSGID_COM_INPUT_GETPLATANGLE, MSG_Com_GetPlatAngle, NULL);
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
	pThis->_StateManager->ToStateTrk();
}
void CEventManager::MSG_SwitchSensor(void* p)
{

}
void CEventManager::MSG_ZoomLong(void* p)
{

}
void CEventManager::MSG_ZoomShort(void* p)
{

}
void CEventManager::MSG_TrkSearch(void* p)
{

}
void CEventManager::MSG_CaptureMode(void* p)
{
	static int state;
	if(state == 0)
		pThis->_StateManager->ToPlatFormCapture();
	else if(state == 1)
		pThis->_StateManager->ToBoxCapture();
	else if(state == 2)
		pThis->_StateManager->ToManualMtdCapture();
	state = (state + 1)%3;
}
void CEventManager::MSG_IrisAndFocus(void* p)
{

}
void CEventManager::MSG_WorkMode(void* p)
{
	static int state;
	if(state == 0)
		pThis->_StateManager->ToStateConvention();
	else if(state == 1)
		pThis->_StateManager->ToStateAuto_Mtd();
	else if(state == 2)
		pThis->_StateManager->ToStateSceneSelect();
	state = (state + 1)%3;
}
void CEventManager::MSG_JosPos(void* p)
{
	char* jos = (char*)p;
	printf("X = %x, Y = %x \n", jos[0], jos[1]);
#if 0
	if(jos[0] > 0 && jos[0] < 0x7a)
		pThis->_StateManager->inter_AxisMove(PTZ_MOVE_Left, jos[0]);
	else if(jos[0] > 0x90 && jos[0] < 0xff)
		pThis->_StateManager->inter_AxisMove(PTZ_MOVE_Right, jos[0]);
	else
		pThis->_StateManager->inter_AxisMove(PTZ_MOVE_Stop, 0);
#endif
}
void CEventManager::MSG_PovPosX(void* p)
{

}
void CEventManager::MSG_PovPosY(void* p)
{

}

void CEventManager::MSG_Com_SelfCheck(void* p)
{
	printf("MSG_Com_SelfCheck start\n");
}
void CEventManager::MSG_Com_SwitchSensor(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int displaychid = tmp->displaychid;
	int capturechid = tmp->capturechid;
	printf("displaychid=%d,capturechid=%d\n",displaychid,capturechid);
}
void CEventManager::MSG_Com_WorkMode(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int workmode = tmp->workmode;
	printf("workmode=%d\n",workmode);
}
void CEventManager::MSG_Com_CaptureMode(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int capturemode = tmp->capturemode;
	printf("capturemode=%d\n",capturemode);
}
void CEventManager::MSG_Com_TrkCtrl(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int trkctrl = tmp->trkctrl;
	printf("trkctrl=%d\n",trkctrl);
}
void CEventManager::MSG_Com_TrkMove(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int trkmove = tmp->trkmove;
	printf("trkmove=%d\n",trkmove);
}
void CEventManager::MSG_Com_SecTrkCtrl(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int sectrkctrl = tmp->sectrkctrl;
	printf("sectrkctrl=%d\n",sectrkctrl);
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
	ComParams_t *tmp = (ComParams_t *)p;
	int zoomctrl = tmp->zoomctrl;
	printf("zoomctrl=%d\n",zoomctrl);
}
void CEventManager::MSG_Com_ApertureCtrl(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int aperturectrl = tmp->aperturectrl;
	printf("aperturectrl=%d\n",aperturectrl);
}
void CEventManager::MSG_Com_FocusCtrl(void* p)
{
	ComParams_t *tmp = (ComParams_t *)p;
	int focusctrl = tmp->focusctrl;
	printf("focusctrl=%d\n",focusctrl);
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
	unsigned short platanglex = tmp->platanglex;
	unsigned short platangley = tmp->platangley;
	printf("platangle x,y=(%d,%d)\n",platanglex, platangley);
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
void CEventManager::MSG_Com_GetPlatAngle(void* p)
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
		tmp->defConfigQueue.erase(tmp->defConfigQueue.begin());
	}
}
void CEventManager::MSG_Com_SaveCfg(void* p)
{
	printf("MSG_Com_SaveCfg start\n");
}
