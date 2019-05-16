/*
 * shrun.h
 *
 *  Created on: Mar 20, 2019
 *      Author: yd
 */

#ifndef EXTENSIONFORMAT_H_
#define EXTENSIONFORMAT_H_

#include "AgreeMentBase.h"
#include "globalDate.h"
#include "osa_thr.h"
#include <time.h>
#include <osa_mutex.h>


class shrun : public AgreeMentBaseFormat
{
public:
	shrun();
	~shrun();

	virtual void MakeMoveX();
	virtual void MakeMoveY();
	virtual void MakeStop();
	virtual void MakeFocusFar(agreement Ppkt, UINT8 byAddress = 0);
	virtual void MakeFocusNear(agreement Ppkt, UINT8 byAddress = 0);
	virtual void MakeFocusStop(agreement Ppkt, UINT8 byAddress = 0);

private:
	static CGlobalDate* _GlobalDate;
	void move_up();
	void move_down();
	void move_right();
	void move_left();
	void move_stop();
	void PktFormat();
	void speed_x(float p);
	void speed_y(float p);

};


class PanoramicMirror : public AgreeMentBaseFormat
{
public:
	PanoramicMirror();
	~PanoramicMirror();

	virtual void MakeMoveX();
	virtual void MakeMoveY();
	virtual void MakeStop();
	virtual void SetSpeedX();
	virtual void SetSpeedY();
	virtual void MakeFocusFar(agreement Ppkt, UINT8 byAddress = 0);
	virtual void MakeFocusNear(agreement Ppkt, UINT8 byAddress = 0);
	virtual void MakeFocusStop(agreement Ppkt, UINT8 byAddress = 0);
	virtual void MakeSetPanPos(agreement Ppkt, UINT16 byValue, UINT8 byAddress);
	virtual void SendDataLen(int len);

private:
	static CGlobalDate* _GlobalDate;
	bool FocusNear_s, FocusFar_s;
	int recvLen, resetLen;
	void creat();
	void PktFormat();
	void move_up();
	void move_down();
	void move_right();
	void move_left();
	void move_stop();
	void speed_x(int p);
	void speed_y(int p);
	void DataParsing();

	OSA_ThrHndl thrHndlDatarecv;
	volatile bool exitRecv = false;
	static void* datarecv(void* p)
	{
		PanoramicMirror* pThis = (PanoramicMirror*) p;
		struct timeval tmp;
		for(; pThis->exitRecv == false;)
		{
			tmp.tv_sec = 0;
			tmp.tv_usec = 500;
			select(0, NULL, NULL, NULL, &tmp);
			pThis->DataParsing();
		}
	}

};


#endif /* EXTENSIONFORMAT_H_ */
