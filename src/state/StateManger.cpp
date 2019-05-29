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

void StateManger::GetParams(int* p)
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
	//printf("_state->curState  = %d  \n", curState);
	return curState;
}


void StateManger::ToStateAuto_Mtd()
{
	curState = _state->State::ChangeState(this, STATE_AUTOMTD);
	_state->_ptz->ptzStop();
	_state->OperationChangeState(this);
	return;
}


void StateManger::ToStateSceneSelect()
{
	curState = _state->State::ChangeState(this, STATE_SCENETRK);
	_state->_ptz->ptzStop();
	_state->OperationChangeState(this);
	return ;
}


void StateManger::ToPlatFormCapture()
{
	curState = _state->State::ChangeState(this, STATE_PTZ);
	_state->_ptz->ptzStop();
	_state->OperationChangeState(this);
	return ;
}


void StateManger::ToBoxCapture()
{
	curState = _state->State::ChangeState(this, STATE_BOX);
	_state->_ptz->ptzStop();
	_state->OperationChangeState(this);
	return ;
}


void StateManger::ToManualMtdCapture()
{
	curState = _state->State::ChangeState(this, STATE_MANUALMTD);
	_state->_ptz->ptzStop();
	_state->OperationChangeState(this);
	return ;
}


void StateManger::inter_TrkCtrl(char Enable)
{
	_state->TrkCtrl(Enable);
	return ;
}


void StateManger::inter_SwitchSensor(char chid)
{
	_state->switchSensor(chid);
	return ;
}


void StateManger::inter_ZoomCtrl(char type)
{
	_state->ZoomCtrl(type);
	return ;
}


void StateManger::inter_AxisPos(int x, int y)
{
}


void StateManger::inter_AxisMove(int x, int y)
{
	x -= (JOS_VALUE_MAX>>1);
	y -= (JOS_VALUE_MAX>>1);

	if( abs(x) < _state->m_delta )
		x = 0;
	else
	{
		if(x>= _state->m_delta)
			x -= _state->m_delta;
		else 
			x += _state->m_delta;
	}
	
	if( abs(y) <_state->m_delta )
		y = 0;
	else
	{
		if(y>= _state->m_delta)
			y -= _state->m_delta;
		else 
			y += _state->m_delta;
	}
	_state->axisMove(x, y);
	return ;
}


void StateManger::inter_TrkSearch(int type, int x, int y)
{
	_state->trkSearch(type, x, y);
	return; 
}


void StateManger::inter_Iris_FocusCtrl(int type, int dir)
{
	_state->Iris_FocusCtrl(type, dir);
	return ;
}



