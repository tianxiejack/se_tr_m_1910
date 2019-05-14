#include "globalDate.h"

CGlobalDate* CGlobalDate::_Instance = 0;

vector<int>  CGlobalDate::defConfigBuffer;

vector<Read_config_buffer>  CGlobalDate::readConfigBuffer;


CGlobalDate::CGlobalDate()
{
	resetType = false;
	jos_ctrl = host_ctrl = 0;
	joystick_flag = 0;
	workMode = captureMode = 0;
	outputMode = 3;
	target = time_start = respupgradefw_stat = respupgradefw_perc = respexpconfig_stat = respexpconfig_len = ImgMtdStat = mtdMode = ThreeMode_bak = 0;
	memset(respexpconfig_buf, 0, sizeof(respexpconfig_buf));
	Mtd_Limit = recv_AutoMtdDate;
	Mtd_Moitor = Mtd_Moitor_X = Mtd_Moitor_Y = 0;
	Mtd_ipc_target = 0;
	Mtd_Manual_Limit = 0;
	Mtd_ExitMode = 10;
	rcv_zoomValue = 2849;
	MtdAutoLoop = false;
	stateChange = false;
	memset(&mtdconfig, 0, sizeof(mtdconfig));
	//memset(&mainProStat, 0, sizeof(mainProStat));
	for(int i =0; i < 5; i++)
		host_osdBoxShow[i] = host_CROSS_draw_show[i] = true;
	mtdconfig.preset = 1;
	manual_mtd_filter = correctValue;
}

CGlobalDate* CGlobalDate::Instance()
{
	if(_Instance == 0)
		_Instance =  new CGlobalDate();
	return _Instance;
}
