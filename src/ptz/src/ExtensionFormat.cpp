#include "ExtensionFormat.h"
#include "osa.h"
#include <math.h>
const int MAXNUM = 1024;

CGlobalDate* shrun::_GlobalDate = NULL;
CGlobalDate* PanoramicMirror::_GlobalDate = NULL;

shrun::shrun()
{
	_GlobalDate = _GlobalDate->Instance();
	ptzType  = sheenrun;
}
shrun::~shrun()
{

}

void shrun::PktFormat()
{
	sendBuf[0] = 0x6E;
	sendBuf[1] = 0x01;
	sendBuf[2] = 0x00;

	sendBuf[4] = 0x05;
	sendBuf[5] = 0x02;

	sendBuf[10] = 0x00;
	sendBuf[11] = 0xE6;
}

void shrun::move_up()
{
	sendBuf[3] = 0x06;
}

void shrun::move_down()
{
	sendBuf[3] = 0x07;
}

void shrun::move_left()
{
	sendBuf[3] = 0x08; /*方向*/
}

void shrun::move_right()
{
	sendBuf[3] = 0x09;
}

void shrun::move_stop()
{
	sendBuf[3] = 0x0a;
	sendBuf[4] = 0x01;
	sendBuf[5] = 0x00;
	sendBuf[6] = 0x00;
	sendBuf[7] = 0xE6;
}

void shrun::speed_x(float p)
{
	p = fabs(p);
	int x, x1, x2;
	x = *(int*)&p;

	x1 = (x>>16)&0xffff;
	x2 = x&0xffff;
	sendBuf[6] = (x1>>8)&0xff;
	sendBuf[7] = x1&0xff;
	sendBuf[8] = (x2>>8)&0xff;
	sendBuf[9] = x2&0xff;
}

void shrun::speed_y(float p)
{
	p = fabs(p);
	int y, y1, y2;
	y = *(int*)&p;

	y1 = (y>>16)&0xffff;
	y2 = y&0xffff;
	sendBuf[6] = (y1>>8)&0xff;
	sendBuf[7] = y1&0xff;
	sendBuf[8] = (y2>>8)&0xff;
	sendBuf[9] = y2&0xff;
}

void shrun::MakeMoveX()
{
	PktFormat();
	if(_GlobalDate->sheenrun[0] != 0)
		speed_x(_GlobalDate->sheenrun[0]);

	if(_GlobalDate->sheenrun[0] > 0)
		move_right();
	else if(_GlobalDate->sheenrun[0] < 0)
		move_left();
}

void shrun::MakeMoveY()
{
	PktFormat();
	if(_GlobalDate->sheenrun[1] != 0)
		speed_y(_GlobalDate->sheenrun[1]);

	if(_GlobalDate->sheenrun[1] > 0)
		move_down();
	else if(_GlobalDate->sheenrun[1] < 0)
		move_up();
}

void shrun::MakeStop()
{
	PktFormat();
	if(_GlobalDate->sheenrun[0] == 0 && _GlobalDate->sheenrun[1] == 0)
		move_stop();
}

void shrun::MakeFocusFar(agreement Ppkt, UINT8 byAddress)
{
	PktFormat();
	sendBuf[3] = 0x00;
	sendBuf[4] = 0x02;
	sendBuf[6] = sendBuf[7] = 0x00;
	sendBuf[8] = 0xE6;
}

void shrun::MakeFocusNear(agreement Ppkt, UINT8 byAddress)
{
	PktFormat();
	sendBuf[3] = 0x01;
	sendBuf[4] = 0x02;
	sendBuf[6] = sendBuf[7] = 0x00;
	sendBuf[8] = 0xE6;
}

void shrun::MakeFocusStop(agreement Ppkt, UINT8 byAddress)
{
	PktFormat();
	move_stop();
}

PanoramicMirror::PanoramicMirror()
{
	_GlobalDate = _GlobalDate->Instance();
	FocusNear_s = FocusFar_s = resetLen = false;
	ptzType = Panoramic_mirror;
	recvLen = 0;
	creat();
}

PanoramicMirror::~PanoramicMirror()
{
	OSA_thrDelete(&thrHndlDatarecv);
}

void PanoramicMirror::creat()
{
	cacheBuf[1024] = {0};
	OSA_thrCreate(&thrHndlDatarecv,datarecv, 0, 0, this);
}

void PanoramicMirror::PktFormat()
{
	sendBuf[0] = 0x02;
	sendBuf[1] = 0x91;

	if(sendBuf[2] == 0xff)
		sendBuf[2] = 0;
	else
	{
		sendBuf[2]++;
		static bool sign = true;
		if(sign)
		{
			sendBuf[2] = 0x00;
			sign = false;
		}
	}

	sendBuf[3] = 0x01;
	sendBuf[4] = 0x00;
	if(!FocusFar_s && !FocusNear_s)
		sendBuf[5] = 0x02;
}

void PanoramicMirror::move_up()
{
	sendBuf[6] = 0x00;
	sendBuf[7] = 0x00;
}

void PanoramicMirror::move_down()
{
	sendBuf[6] = 0x00;
	sendBuf[7] = 0x00;
}

void PanoramicMirror::move_right()
{
	sendBuf[8] = 0x00;
	sendBuf[9] = 0x00;
}

void PanoramicMirror::move_left()
{
	sendBuf[8] = 0x00;
	sendBuf[9] = 0x00;
}

void PanoramicMirror::move_stop()
{
	sendBuf[6] = sendBuf[7] = sendBuf[8] = sendBuf[9] = 0;
}

void PanoramicMirror::MakeMoveX()
{
	PktFormat();
	int x = _GlobalDate->PanoramicMirror_value[0];
	speed_x(x);
}

void PanoramicMirror::MakeMoveY()
{
	PktFormat();
	int y = _GlobalDate->PanoramicMirror_value[1];
	speed_y(y);
}

void PanoramicMirror::MakeStop()
{
	PktFormat();
	move_stop();

}

void PanoramicMirror::SetSpeedX()
{
	sendBuf[0] = 0x03;
	sendBuf[1] = 0xc8;
	sendBuf[2] = 0x00;
	sendBuf[3] = 0x00;
	sendBuf[4] = 0x48;
	sendBuf[5] = 0x43;
}

void PanoramicMirror::SetSpeedY()
{
	sendBuf[0] = 0x03;
	sendBuf[1] = 0xc9;
	sendBuf[2] = 0x00;
	sendBuf[3] = 0x00;
	sendBuf[4] = 0xA0;
	sendBuf[5] = 0x41;
}

void PanoramicMirror::speed_x(int p)
{

	if(p > 8191)
		p = 8191;
	if(p < -8192)
		p = -8192;
	sendBuf[6] = p&0xff;
	sendBuf[7] = (p>>8)&0xff;
}

void PanoramicMirror::speed_y(int p)
{
	if(p > 8191)
		p = 8191;
	if(p < -8192)
		p = -8192;
	p *= -1;
	sendBuf[8] = p&0xff;
	sendBuf[9] = (p>>8)&0xff;
}

void PanoramicMirror::MakeFocusFar(agreement Ppkt, UINT8 byAddress)
{
	FocusFar_s = true;
	PktFormat();
	sendBuf[5] = 0x12;
	sendBuf[6] = sendBuf[7] = sendBuf[8] = sendBuf[9] = 0;
}

void PanoramicMirror::MakeFocusNear(agreement Ppkt, UINT8 byAddress)
{
	FocusNear_s = true;
	PktFormat();
	sendBuf[5] = 0x22;
	sendBuf[6] = sendBuf[7] = sendBuf[8] = sendBuf[9] = 0;
}

void PanoramicMirror::MakeFocusStop(agreement Ppkt, UINT8 byAddress)
{
	FocusFar_s = FocusNear_s = false;
	PktFormat();
	sendBuf[6] = sendBuf[7] = sendBuf[8] = sendBuf[9] = 0;
}

void PanoramicMirror::DataParsing()
{
	static int pan, Tilt;
	int high, low, middle, high2, low2, middle2;
	static int readhead = 0, lastlen = 0, curlen = 0;
	unsigned char swapBuf[15];

	if(recvLen >= 1023)
		resetLen = true;

	if(resetLen == true)
	{
		lastlen = 0;
		readhead = 0;
		resetLen = false;
	}
	curlen = recvLen - lastlen;

	memcpy(cacheBuf+lastlen, buffer, curlen);
	lastlen = recvLen;
//static unsigned int tt;
	while(recvLen - readhead > 10)
	{
		if(cacheBuf[readhead] == 0x03)
		{
			if(cacheBuf[readhead + 1] == 0x01)
			{
				memcpy(swapBuf, cacheBuf+readhead, 10);
				low = swapBuf[4];
				middle = swapBuf[5];
				high = swapBuf[6];
				low2 = swapBuf[7];
				middle2 = swapBuf[8];
				high2 = swapBuf[9];

				pan = (high<<16) | (middle<<8) | low;
				Tilt = (high2<<16) | (middle2<<8) | low2;
				//printf("pan = %02x, Tilt = %02x [%d]\n",pan, Tilt, readhead);
				//printf("======time======= %d    \n", OSA_getCurTimeInMsec()-tt);
				//tt = OSA_getCurTimeInMsec();
				readhead += 10;
			}
			else
				readhead += 2;
		}
		else
			readhead += 2;
	}
}

void PanoramicMirror::SendDataLen(int len)
{
	static int ilen = 0;
	ilen += len;
	if(ilen >= 1023)
	{
		ilen = 0;
		resetLen = true;
		recvLen = GetDataLen(len);
	}
	recvLen = GetDataLen(ilen);
	//DataParsing();
}

void PanoramicMirror::MakeSetPanPos(agreement Ppkt, UINT16 byValue, UINT8 byAddress)
{

}
