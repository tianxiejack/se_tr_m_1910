#include "State.h"
#include <iostream>

using namespace std;
State* pThis = NULL;
State *st1 = NULL, *st2 = NULL, *st3 = NULL, *st4 = NULL, *st5 = NULL;

CPTZControl* State::_ptz = NULL;
AgreeMentBaseFormat* State::_agreement = NULL;
CPlatformInterface* State::m_Platform = NULL;

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
	curState = STATE_PTZ;
	selectch = {1, 1, 1, 1, 1, 0};
	curValidChid = selectch.idx;
}

State::~State()
{

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

void State::axisMove(int iDirection, int speed)
{
	_ptz->ptzMove(iDirection, speed);
}

void State::ZoomCtrl(char type)
{
	if(type == 0x01)
		_ptz->ptzZoomTele();
	else if(type == 0x02)
		_ptz->ptzZoomWide();
	else
		_ptz->ptzZoomStop();
}





/*=====================================*/


StateAuto_Mtd::StateAuto_Mtd()
{

}

StateAuto_Mtd::~StateAuto_Mtd()
{

}

void StateAuto_Mtd::OperationInterface(StateManger*con)
{
	cout<<"1  StateAuto_Mtd......"<<endl;
}

void StateAuto_Mtd::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

int StateAuto_Mtd::curStateInterface()
{
	if(curState != STATE_AUTOMTD)
		curState = STATE_AUTOMTD;
	return curState;
}

StateSceneSelect::StateSceneSelect()
{

}

StateSceneSelect::~StateSceneSelect()
{

}

void StateSceneSelect::OperationInterface(StateManger* con)
{
	cout<<"2  StateSceneSelect"<<endl;
}

void StateSceneSelect::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

int StateSceneSelect::curStateInterface()
{
	if(curState != STATE_SCENETRK)
		curState = STATE_SCENETRK;
	return curState;
}

PlatFormCapture::PlatFormCapture()
{

}

PlatFormCapture::~PlatFormCapture()
{

}

void PlatFormCapture::OperationInterface(StateManger* con)
{
	cout<<"3  PlatFormCapture"<<endl;
}

void PlatFormCapture::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

int PlatFormCapture::curStateInterface()
{
	if(curState != STATE_PTZ)
		curState = STATE_PTZ;
	return curState;
}

BoxCapture::BoxCapture()
{

}

BoxCapture::~BoxCapture()
{

}

void BoxCapture::OperationInterface(StateManger* con)
{
	cout<<"4  BoxCapture"<<endl;
}

void BoxCapture::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

int BoxCapture::curStateInterface()
{
	if(curState != STATE_BOX)
		curState = STATE_BOX;
	return curState;
}

ManualMtdCapture::ManualMtdCapture()
{

}

ManualMtdCapture::~ManualMtdCapture()
{

}

void ManualMtdCapture::OperationInterface(StateManger* con)
{
	cout<<"5  ManualMtdCapture"<<endl;
}

void ManualMtdCapture::OperationChangeState(StateManger* con)
{
	OperationInterface(con);
}

int ManualMtdCapture::curStateInterface()
{
	if(curState != STATE_MANUALMTD)
		curState = STATE_MANUALMTD;
	return curState;
}
