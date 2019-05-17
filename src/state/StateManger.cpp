#include "StateManger.h"

StateManger::StateManger()
{

}

StateManger::StateManger(State* state)
{
	this->_state = state;
	curState = _state->curStateInterface();
}

StateManger::~StateManger()
{
	delete _state;
}

void* StateManger::returnIpc()
{
	return _state->m_ipc;
}

void StateManger::GetParams(float* p)
{
	_state->cfg_value = p;
}

void StateManger::OperationInterface()
{
	_state->OperationInterface(this);
}

bool StateManger::ChangeState(State* state)
{
	this->_state = state;
	return true;
}

void StateManger::OperationChangState()
{
	_state->OperationChangeState(this);
}

int StateManger::CurStateInterface()
{
	printf("_state->curState  = %d  \n", curState);
	return curState;
}

void StateManger::ToStateAuto_Mtd()
{
	curState = _state->State::ChangeState(this, STATE_AUTOMTD);
	_state->OperationChangeState(this);
}

void StateManger::ToStateSceneSelect()
{
	curState = _state->State::ChangeState(this, STATE_SCENETRK);
	_state->OperationChangeState(this);
}

void StateManger::ToPlatFormCapture()
{
	curState = _state->State::ChangeState(this, STATE_PTZ);
	_state->OperationChangeState(this);
}

void StateManger::ToBoxCapture()
{
	curState = _state->State::ChangeState(this, STATE_BOX);
	_state->OperationChangeState(this);
}

void StateManger::ToManualMtdCapture()
{
	curState = _state->State::ChangeState(this, STATE_MANUALMTD);
	_state->OperationChangeState(this);
}


void StateManger::inter_TrkCtrl(char Enable)
{
	_state->TrkCtrl(Enable);
}

void StateManger::inter_SwitchSensor(char chid)
{
	_state->switchSensor(chid);
}

void StateManger::inter_ZoomCtrl(char type)
{
	_state->ZoomCtrl(type);
}

void StateManger::inter_AxisPos(int x, int y)
{

}

void StateManger::inter_AxisMove(int x, int y)
{
	_state->axisMove(x, y);
}

void StateManger::inter_TrkSearch(int type, int x, int y)
{
	_state->trkSearch(type, x, y);
}

void StateManger::inter_Iris_FocusCtrl(int type, int dir)
{
	_state->Iris_FocusCtrl(type, dir);
}



