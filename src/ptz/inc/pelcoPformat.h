#ifndef PELCOPFORMAT_H_
#define PELCOPFORMAT_H_
#include <iostream>
#include"pelcoFormat.h"
#include "AgreeMentBase.h"

using namespace std;

class IPelcoPFormat : public AgreeMentBaseFormat
{
public:
	IPelcoPFormat(){
		ptzType = 0;//pelco_p;
	}
	 ~IPelcoPFormat(){}
	 void MakeSumCheck(agreement Ppkt);
	 void PktFormat(agreement Ppkt, UINT8 cmd1, UINT8 cmd2, UINT8 data1, UINT8 data2, UINT8 byAddress = 0);
	//
	 void MakeCameraOn(agreement Ppkt, UINT8 byAddress = 0);
	 void MakeCameraOff(agreement Ppkt, UINT8 byAddress = 0);
	 void MakeMove(agreement Ppkt, INT32 iDirection, UINT8 bySpeed = 0x20,
		bool bClear = true, UINT8 byAddress = 0);
	 void MakeFocusFar(agreement Ppkt, UINT8 byAddress = 0);
	 void MakeFocusNear(agreement Ppkt, UINT8 byAddress = 0);
	 void MakeFocusStop(agreement Ppkt, UINT8 byAddress = 0);
	 void MakeIrisOpen(agreement Ppkt, UINT8 byAddress = 0);
	 void MakeIrisClose(agreement Ppkt, UINT8 byAddress = 0);
	 void MakeZoomWide(agreement Ppkt, UINT8 byAddress = 0);
	 void MakeZoomTele(agreement Ppkt, UINT8 byAddress = 0);
	 void MakeZoomStop(agreement Ppkt, UINT8 byAddress = 0);
	 void MakeAutoScan(agreement Ppkt, UINT8 byAddress = 0);

	// EXTENDED COMMANDS
	 void MakePresetCtrl(agreement Ppkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress = 0);
	 void MakeSetZoomSpeed(agreement Ppkt, UINT8 byValue, UINT8 byAddress = 0);
	 void MakeSetFocusSpeed(agreement Ppkt, UINT8 byValue, UINT8 byAddress = 0);
	 void MakeRemoteReset(agreement Ppkt, UINT8 byAddress = 0);
	 void MakeExtCommand(agreement Ppkt, INT32 nCmdType, UINT8 byValue, UINT8 byDataExt = 0, UINT8 byAddress = 0);
	 void MakeDummy(agreement Ppkt, UINT8 byAddress = 0);
	 void MakeSetPanPos(agreement Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */);
	 void MakeSetTilPos(agreement Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */);
	 void MakePatternCtrl(agreement Ppkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress = 0);
};


#endif
