#include "State.h"
#include <iostream>

using namespace std;
State* pThis = NULL;
State *st1 = NULL, *st2 = NULL, *st3 = NULL, *st4 = NULL, *st5 = NULL;

CPTZControl* State::_ptz = NULL;
AgreeMentBaseFormat* State::_agreement = NULL;
CPlatformInterface* State::m_Platform = NULL;
CPTZSpeedTransfer*  State::m_ptzSpeed = NULL;
CIPCProc* State::m_ipc = NULL;

State::State()
{
	if(pThis == NULL)
		pThis = this;
	if(_agreement == NULL)
		_agreement = new IPelcoDFormat();
	if(_ptz == NULL)
		_ptz = new CPTZControl(_agreement);
	if(m_Platform == NULL)
		m_Platform = new CplatFormControl();
	if(m_ptzSpeed == NULL)
		m_ptzSpeed = new CPTZSpeedTransfer();
	if(m_ipc == NULL)
		m_ipc = new CIPCProc();

	curState = STATE_PTZ;
	selectch = {1, 1, 1, 1, 1, 0};
	curValidChid = selectch.idx;
}

State::~State()
{

}

void State::platformCreate()
{
	viewParam = m_Platform->sensorParams();
	m_Platform->PlatformCtrl_sensor_Init(cfg_value);
	m_Platform->PlatformCtrl_CreateParams_Init(&m_cfgPlatParam, cfg_value, viewParam);
	OSA_assert(m_plt == NULL);
	m_plt = m_Platform->PlatformCtrl_Create(&m_cfgPlatParam);
}

void State::StateInit()
{
	if(st1 == NULL)
		st1 = new StateAuto_Mtd();
	if(st2 == NULL)
		st2 = new StateSceneSelect();
	if(st3 == NULL)
		st3 = new PlatFormCapture();
	if(st4 == NULL)
		st4 = new BoxCapture();
	if(st5 == NULL)
		st5 = new ManualMtdCapture();
	platformCreate();
}

int State::ChangeState(StateManger* con, char nextState)
{
	curState = nextState;
	switch(nextState)
	{
	case STATE_AUTOMTD:
		con->ChangeState(st1);
		break;

	case STATE_SCENETRK:
		con->ChangeState(st2);
		break;

	case STATE_PTZ:
		con->ChangeState(st3);
		break;

	case STATE_BOX:
		con->ChangeState(st4);
		break;

	case STATE_MANUALMTD:
		con->ChangeState(st5);
		break;
	}
	return curState;
}

void State::TrkCtrl(char Enable)
{
	//m_ipc->ipcTrackCtrl(Enable);
}

void State::switchSensor(char chid)
{

	int SensorStat; //= GetIpcAddress(Sensor);
	if(chid == 0xff)
	{
		if(1)//SensorStat != selectch.idx)
			selectch.idx = SensorStat;
		do{
				selectch.idx++;
				selectch.idx = selectch.idx%5;
		}while(!selectch.validCh[selectch.idx]);
		//m_GlobalDate->chid_camera = selectch.idx;
		//m_ipc->IpcSensorSwitch(selectch.idx);
	}
	else
	{
		if(selectch.validCh[chid] == 1)
		{
			selectch.idx = chid;
			//m_ipc->IpcSensorSwitch(selectch.idx);
		}
	}
	curValidChid = selectch.idx;
}

void State::ZoomCtrl(char type)
{
	if(type == 0x01)
		_ptz->m_iSetZoomSpeed = -1;
	else if(type == 0x02)
		_ptz->m_iSetZoomSpeed = 1;
	else
		_ptz->m_iSetZoomSpeed = 0;
}

void State::axisMove(int x, int y)
{
	m_Platform->PlatformCtrl_VirtualInput(m_plt, DevUsr_AcqJoystickXInput, x/jos_value);
	m_Platform->PlatformCtrl_VirtualInput(m_plt, DevUsr_AcqJoystickYInput, y/jos_value);

	m_pltInput.iTrkAlgState= 0;
	m_Platform->PlatformCtrl_TrackerInput(m_plt, &m_pltInput);
	m_Platform->PlatformCtrl_TrackerOutput(m_plt, &m_pltOutput);
}




