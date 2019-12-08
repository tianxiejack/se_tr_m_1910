#ifndef _AGREEMENTBASE_H
#define _AGREEMENTBASE_H

#include "pelcoFormat.h"
#include <iostream>
#include <string.h>

class AgreeMentBaseFormat
{
public:
	AgreeMentBaseFormat(){};
	virtual ~AgreeMentBaseFormat(){};
	
	int ptzType;
	int time1, time2, value;
	unsigned char sendBuf[15];
	unsigned char* buffer = NULL;
	unsigned char cacheBuf[1024];

	virtual void MakeSumCheck(agreement Ppkt){};
	virtual void PktFormat(agreement Ppkt, UINT8 cmd1, UINT8 cmd2, UINT8 data1, UINT8 data2, UINT8 byAddress = 0){};
	virtual void MakeCameraOn(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void MakeCameraOff(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void MakeMove(agreement Ppkt, INT32 iDirection, UINT8 bySpeed = 0x20,
		bool bClear = true, UINT8 byAddress = 0){};
	virtual void SetSpeedX(){};
	virtual void SetSpeedY(){};
	virtual void MakeMoveX(){};
	virtual void MakeMoveY(){};
	virtual void MakeStop(){};
	virtual void MakeFocusFar(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void MakeFocusNear(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void MakeFocusStop(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void MakeIrisOpen(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void MakeIrisClose(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void MakeZoomWide(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void MakeZoomTele(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void MakeZoomStop(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void MakeAutoScan(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void QueryZoomPos(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void QueryPanPos(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void QueryTiltPos(agreement Ppkt, UINT8 byAddress = 0){};


	// EXTENDED COMMANDS
	virtual void MakePresetCtrl(agreement Ppkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress = 0){};
	virtual void MakePatternCtrl(agreement Ppkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress = 0){};
	virtual void MakeSetZoomSpeed(agreement Ppkt, UINT8 byValue, UINT8 byAddress = 0){};
	virtual void MakeSetFocusSpeed(agreement Ppkt, UINT8 byValue, UINT8 byAddress = 0) {};
	virtual void MakeRemoteReset(agreement Ppkt, UINT8 byAddress = 0) {};
	virtual void MakeExtCommand(agreement Ppkt, INT32 nCmdType, UINT8 byValue, UINT8 byDataExt = 0, UINT8 byAddress = 0){};
	virtual void MakeDummy(agreement Ppkt, UINT8 byAddress = 0){};
	virtual void MakeSetPanPos(agreement Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */){};
	virtual void MakeSetTilPos(agreement Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */) {};
	virtual void MakeSetZoomPos(agreement Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */){};

	virtual void SendDataLen(int len){};
	virtual int GetDataLen(int len){return len;};
};


#endif
