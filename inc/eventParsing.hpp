/*
 * eventParsing.hpp
 *
 *  Created on: 2019年5月7日
 *      Author: d
 */

#ifndef EVENTPARSING_HPP_
#define EVENTPARSING_HPP_
#include <iostream>
#include "js_usb.hpp"
#include "CMessage.hpp"

using namespace std;

#define AXIS_X 0
#define AXIS_Y 1
#define POV_BUTTON 5
#define BUTTON 6

typedef enum
{
	/*button1234 ==>data[5]
	 * other ===> data[6]
	 * */
	js_1 = 0x1f,
	js_2 = 0x2f,
	js_3 = 0x4f,
	js_4 = 0x8f,
	js_5 = 0x01,
	js_6 = 0x02,
	js_7 = 0x04,
	js_8 = 0x08,
	js_9 = 0x10,
	js_10 = 0x20,
	js_11 = 0x40,
	js_12 = 0x80,
}js_button;

typedef enum
{
	/*pov ==> data[5]*/
	js_pov_reset = 0x0f,
	js_pov_up = 0x00,
	js_pov_rightUp = 0x01,
	js_pov_right = 0x02,
	js_pov_rightDown = 0x03,
	js_pov_down = 0x04,
	js_pov_leftDown = 0x05,
	js_pov_left = 0x06,
	js_pov_leftUp = 0x07,
}js_pov;

typedef enum _sys_msg_id_ {
    MSGID_SYS_INIT  = 0,
    MSGID_SYS_RESET,
    MSGID_EXT_INPUT_TRACKCTRL,
    MSGID_EXT_INPUT_SwitchSensor,
    MSGID_EXT_INPUT_ZOOMLONGCTRL,
    MSGID_EXT_INPUT_ZOOMSHORTCTRL,
    MSGID_EXT_INPUT_TRACKSEARCHCTRL,
    MSGID_EXT_INPUT_captureMode,
    MSGID_EXT_INPUT_IrisAndFocusAndExit,
    MSGID_EXT_INPUT_workMode,
    MSGID_EXT_INPUT_JOSPOS,
    MSGID_EXT_INPUT_POVPOSX,
    MSGID_EXT_INPUT_POVPOSY,
    MSGID_EXT_INPUT_MTDMode,
    MSGID_EXT_INPUT_IRISUPCTRL,
    MSGID_EXT_INPUT_IRISDOWNCTRL,
    MSGID_EXT_INPUT_FOCUSFARCHCTRL,
    MSGID_EXT_INPUT_FOCUSNEARCTRL,
    MSGID_EXT_INPUT_IMGENHCTRL,
    MSGID_EXT_INPUT_MMTCRTL,
    MSGID_EXT_INPUT_MMTSelect,
    MSGID_EXT_INPUT_MtdPreset,
    MSGID_EXT_INPUT_configWrite,
    MSGID_EXT_INPUT_configWrite_Save,
    MSGID_EXT_INPUT_config_Read,
    MSGID_EXT_INPUT_kboard,
    MSGID_EXT_INPUT_OSD,
    MSGID_EXT_INPUT_PRMBACK,
    MSGID_EXT_INPUT_zoomSpeed,
    MSGID_EXT_INPUT_sceneTrk,
    MSGID_EXT_INPUT_MtdAreaBox,
    MSGID_EXT_INPUT_CallPreset,
    MSGID_EXT_INPUT_setPan,
    MSGID_EXT_INPUT_setTilt,
    MSGID_EXT_INPUT_setZoom,
    MSGID_EXT_INPUT_queryPan,
    MSGID_EXT_INPUT_queryTilt,
    MSGID_EXT_INPUT_queryZoom,
    MSGID_EXT_INPUT_setPreset,
    MSGID_EXT_INPUT_goToPreset,
    test_ptz_left,
    test_ptz_right,
    test_ptz_stop,

    MSGID_IPC_INPUT_TRACKCTRL,
    MSGID_IPC_INPUT_MTDCTRL,
    MSGID_IPC_INPUT_MMTCRTL,
    MSGID_IPC_INPUT_IMGENHCTRL,
    MSGID_IPC_INPUT_TRCKBOXSIZECTRL,
    MSGID_IPC_MainElectronicZoom,
    MSGID_IPC_Config,
    MSGID_IPC_UserOSD,
    MSGID_IPC_BoxOSD,
    MSGID_IPC_OSDTEXT,
    MSGID_IPC_UTC,
    MSGID_IPC_LOSTTIME,
    MSGID_IPC_WORDFONT,
    MSGID_IPC_WORDSIZE,
    MSGID_IPC_Camera,
    MSGID_IPC_Channel_binding,
    MSGID_IPC_AxisMove,
    MSGID_IPC_saveAxis,
    MSGID_IPC_picp,
    MSGID_IPC_chooseVideoChannel,
    MSGID_IPC_frameCtrl,
    MSGID_IPC_compression_quality,
    MSGID_IPC_word,
    MSGID_IPC_BoresightPos,
    MSGID_IPC_Algosdrect,
    MSGID_IPC_switchSensor,
    MSGID_IPC_AutoMtd,
    MSGID_IPC_Mtdoutput,
    MSGID_IPC_Mtdpolar,
    MSGID_IPC_QueryPos,
    MSGID_IPC_INPUT_Pos,
    MSGID_IPC_INPUT_PosAndZoom,
    MSGID_IPC_INPUT_NameAndPos,
    MSGID_EXT_INPUT_fontsize,
    MSGID_IPC_INPUT_reset_swtarget_timer,
    MSGID_IPC_INPUT_ballparam,
    MAX_MSG_NUM
}eSysMsgId, MSG_PROC_ID;

class CEventParsing
{
public:
	CEventParsing();
	~CEventParsing();
	CMessage* _Msg;
	static void thread_jsEvent();

private:
	static CEventParsing* pThis;
	bool exit_jsParsing;
	void parsingJostickEvent();

};




#endif /* EVENTPARSING_HPP_ */
