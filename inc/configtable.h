#ifndef _CONFIG_TABLE_H_
#define _CONFIG_TABLE_H_

#define AIM_WIDTH					64
#define AIM_HEIGHT					64

#define AVT_TRK_AIM_SIZE			2

typedef void (*notifyZoom)(int); 

typedef enum {
	eSen_CH0	= 0x00,
	eSen_CH1	= 0x01,
	eSen_CH2   = 0x02,
	eSen_CH3 	= 0x03,
	eSen_CH4   = 0x04,
	eSen_CH5	= 0x05,
	eSen_Max   = 0x06
}eSenserStat;

enum devvideo{
	video_gaoqing0=0,
	video_gaoqing,
	video_gaoqing2,
	video_gaoqing3,
	video_pal,
	MAX_CHAN,
};

#define MAIN_CHID					video_gaoqing0
#define PAL_VIRCHID					0

#define VIDEO_DIS_WIDTH		1920
#define VIDEO_DIS_HEIGHT		1080

#define BALL_CHID			video_gaoqing
#define GUN_CHID			video_gaoqing0

#define min_width_ratio 0.2
#define max_width_ratio 0.8
#define min_height_ratio 0.2
#define max_height_ratio 0.8

extern int vcapWH[5][2];
extern int vdisWH[5][2];

static int ShowDPI[MAX_CHAN][2] = {{1920, 1080},{1920, 1080},{1920, 1080},{1920, 1080},{720, 576}};

////////////////////////////////////////////////////
// sys config table part
#define SYSCONFIG_VERSION     100
#define CFGID_BKFEILD_MAX	16
#define CFGID_USEROSD_MAX	32
#define USEROSD_LENGTH	128

#define CFGID_BUILD( blkId, feildId )   ( ( (( blkId) << 4 ) & 0xFF0 ) | ( feildId ) )
#define CFGID_blkId( cfgId )            ( ( (cfgId) >> 4 ) & 0xFF )
#define CFGID_feildId( cfgId )          ( cfgId & 0xF )

typedef enum 
{
	CFGID_JOS_BKID = 0,
	CFGID_PTZ_BKID = 1,
	CFGID_RTS_BKID = 2,
	CFGID_TRK_BKID = 3,		// 3block
	CFGID_OSD_BKID = 6,	// part1 16block
	CFGID_INPUT1_BKID = 22,	// 7block
	CFGID_OSD2_BKID = 29,	// part2 16block
	CFGID_OUTPUT_BKID = 50,
	CFGID_SYS_BKID = 51,
	CFGID_MTD_BKID = 53,	// 2block
	CFGID_PREPOS_BKID = 54,	// 1block
	CFGID_INPUT2_BKID = 55,	// 7block
	CFGID_INPUT3_BKID = 62,	// 7block
	CFGID_INPUT4_BKID = 69,	// 7block
	CFGID_INPUT5_BKID = 76,	// 7block
	CFGID_PID_BKID = 100,	// 2block
	CFGID_PLATFM_BKID = 102,

	CFGID_BKID_MAX = 128
}CFGID_BKID;

typedef enum 
{
	CFGID_JOS_BASE = CFGID_BUILD( CFGID_JOS_BKID, 0 ),
	CFGID_JOS_type = CFGID_BUILD( CFGID_JOS_BKID, 0 ),
	CFGID_JOS_deadx = CFGID_BUILD( CFGID_JOS_BKID, 1 ),
	CFGID_JOS_deady = CFGID_BUILD( CFGID_JOS_BKID, 2 ),
	CFGID_JOS_inputgainx1 = CFGID_BUILD( CFGID_JOS_BKID, 3 ),
	CFGID_JOS_inputgainy1 = CFGID_BUILD( CFGID_JOS_BKID, 4 ),
	CFGID_JOS_inputgainx2 = CFGID_BUILD( CFGID_JOS_BKID, 5 ),
	CFGID_JOS_inputgainy2 = CFGID_BUILD( CFGID_JOS_BKID, 6 ),
	CFGID_JOS_cutpointx1 = CFGID_BUILD( CFGID_JOS_BKID, 9 ),
	CFGID_JOS_cutpointy1 = CFGID_BUILD( CFGID_JOS_BKID, 10 ),
	CFGID_JOS_cutpointx2 = CFGID_BUILD( CFGID_JOS_BKID, 11 ),
	CFGID_JOS_cutpointy2 = CFGID_BUILD( CFGID_JOS_BKID, 12 ),
}CFGID_JOS;

typedef enum 
{
	CFGID_PTZ_BASE = CFGID_BUILD( CFGID_PTZ_BKID,  0 ),
	CFGID_PTZ_comm = CFGID_BUILD( CFGID_PTZ_BKID, 0 ),
	CFGID_PTZ_baud = CFGID_BUILD( CFGID_PTZ_BKID, 1 ),
	CFGID_PTZ_datbit = CFGID_BUILD( CFGID_PTZ_BKID, 2 ),
	CFGID_PTZ_checkbit = CFGID_BUILD( CFGID_PTZ_BKID, 3 ),
	CFGID_PTZ_stopbit = CFGID_BUILD( CFGID_PTZ_BKID, 4 ),
	CFGID_PTZ_flowcon = CFGID_BUILD( CFGID_PTZ_BKID, 5 ),	
	CFGID_PTZ_netip = CFGID_BUILD( CFGID_PTZ_BKID, 6 ),
	CFGID_PTZ_protype = CFGID_BUILD( CFGID_PTZ_BKID, 8 ),
	CFGID_PTZ_feedbacktype = CFGID_BUILD( CFGID_PTZ_BKID, 15 ),
}CFGID_PTZ;

typedef enum 
{
	CFGID_RTS_BASE = CFGID_BUILD( CFGID_RTS_BKID, 0 ),
	CFGID_RTS_mainch = CFGID_BUILD( CFGID_RTS_BKID, 0 ),
	CFGID_RTS_mainch2 = CFGID_BUILD( CFGID_RTS_BKID, 1 ),
	CFGID_RTS_trken = CFGID_BUILD( CFGID_RTS_BKID, 3 ),
	CFGID_RTS_trkmode = CFGID_BUILD( CFGID_RTS_BKID, 4 ),
	CFGID_RTS_sectrkstat = CFGID_BUILD( CFGID_RTS_BKID, 5 ),
	CFGID_RTS_trkstat = CFGID_BUILD( CFGID_RTS_BKID, 6 ),
	CFGID_RTS_trkerrx = CFGID_BUILD( CFGID_RTS_BKID, 7 ),
	CFGID_RTS_trkerry = CFGID_BUILD( CFGID_RTS_BKID, 8 ),
	CFGID_RTS_mtden = CFGID_BUILD( CFGID_RTS_BKID, 11 ),
	CFGID_RTS_mtddet = CFGID_BUILD( CFGID_RTS_BKID, 12 ),	// MtdDetectStat
}CFGID_RTS;

typedef enum 
{
	CFGID_TRK_BASE = CFGID_BUILD( CFGID_TRK_BKID, 0 ),
	CFGID_TRK_assitime = CFGID_BUILD( CFGID_TRK_BKID, 15 ),

	CFGID_TRK_BASE2 = CFGID_BUILD( CFGID_TRK_BKID+1, 0 ),
	CFGID_TRK_BASE3 = CFGID_BUILD( CFGID_TRK_BKID+2, 0 ),
}CFGID_TRK;


typedef enum 
{
	CFGID_OUTPUT_BASE = CFGID_BUILD( CFGID_OUTPUT_BKID, 0 ),
	CFGID_OUTPUT1_res = CFGID_BUILD( CFGID_OUTPUT_BKID, 4 ),	// HDMI resolution
	CFGID_OUTPUT2_res = CFGID_BUILD( CFGID_OUTPUT_BKID, 5 ),	// SDI resolution
}CFGID_OUTPUT;

typedef enum 
{
	CFGID_SYS_BASE = CFGID_BUILD( CFGID_SYS_BKID, 0 ),
	CFGID_SYSOSD_biten = CFGID_BUILD( CFGID_SYS_BKID, 1 ),		// bit enable
	//CFGID_USROSD_biten = CFGID_BUILD( CFGID_SYS_BKID, 2 ),		// bit enable
	//CFGID_SYSOSD_color = CFGID_BUILD( CFGID_SYS_BKID, 3 ),
}CFGID_SYS;

typedef enum 
{
	CFGID_MTD_BASE = CFGID_BUILD( CFGID_MTD_BKID, 0 ),
	CFGID_MTD_areabox = CFGID_BUILD( CFGID_MTD_BKID, 0 ),
	CFGID_MTD_detnum = CFGID_BUILD( CFGID_MTD_BKID, 1 ),	// detectNum
	CFGID_MTD_upspd = CFGID_BUILD( CFGID_MTD_BKID, 2 ),	// tmpUpdateSpeed
	CFGID_MTD_maxpixel = CFGID_BUILD( CFGID_MTD_BKID, 3 ),
	CFGID_MTD_minpixel = CFGID_BUILD( CFGID_MTD_BKID, 4 ),
	CFGID_MTD_detspd = CFGID_BUILD( CFGID_MTD_BKID, 6 ),		// detectSpeed
	CFGID_MTD_trktime = CFGID_BUILD( CFGID_MTD_BKID, 7 ),
	CFGID_MTD_priority = CFGID_BUILD( CFGID_MTD_BKID, 8 ),
}CFGID_MTD;


typedef enum 
{
	CFGID_PREPOS_BASE = CFGID_BUILD( CFGID_PREPOS_BKID, 0 ),
	CFGID_PREPOS_preposx = CFGID_BUILD( CFGID_PREPOS_BKID, 0 ),
	CFGID_PREPOS_preposy = CFGID_BUILD( CFGID_PREPOS_BKID, 1 ),
	CFGID_PREPOS_prezoom = CFGID_BUILD( CFGID_PREPOS_BKID,2 ),
}CFGID_POS;



typedef enum 
{
	CFGID_PID_BASE = CFGID_BUILD( CFGID_PID_BKID, 0 ),
	CFGID_PID_KPX = CFGID_BUILD( CFGID_PID_BKID, 1 ),
	CFGID_PID_KIX = CFGID_BUILD( CFGID_PID_BKID, 2 ),
	CFGID_PID_KDX = CFGID_BUILD( CFGID_PID_BKID, 3 ),
	CFGID_PID_KX = CFGID_BUILD( CFGID_PID_BKID, 4 ),
	CFGID_PID_KPY = CFGID_BUILD( CFGID_PID_BKID, 5 ),
	CFGID_PID_KIY = CFGID_BUILD( CFGID_PID_BKID, 6 ),
	CFGID_PID_KDY = CFGID_BUILD( CFGID_PID_BKID, 7 ),
	CFGID_PID_KY = CFGID_BUILD( CFGID_PID_BKID, 8 ),

	CFGID_PID_RATIOX = CFGID_BUILD( (CFGID_PID_BKID+1), 0 ),
	CFGID_PID_RATIOY = CFGID_BUILD( (CFGID_PID_BKID+1), 1 ),
	CFGID_PID_ERRORX = CFGID_BUILD( (CFGID_PID_BKID+1), 2 ),
	CFGID_PID_ERRORY = CFGID_BUILD( (CFGID_PID_BKID+1), 3 ),
	CFGID_PID_TIME = CFGID_BUILD( (CFGID_PID_BKID+1), 4 ),
	CFGID_PID_LIMITX = CFGID_BUILD( (CFGID_PID_BKID+1), 5 ),
	CFGID_PID_LIMITY = CFGID_BUILD( (CFGID_PID_BKID+1), 6 ),
	
}CFGID_PID;

typedef enum 
{
	CFGID_PLATFM_BASE = CFGID_BUILD( CFGID_PLATFM_BKID, 0 ),
}CFGID_PLATFM;

// user osd part1 ID 0-15
#define CFGID_OSD_BASE(ID)	CFGID_BUILD( CFGID_OSD_BKID+ID, 0 )
#define CFGID_OSD_SHOWEN(ID)	CFGID_BUILD( CFGID_OSD_BKID+ID, 0 )
#define CFGID_OSD_POSX(ID)		CFGID_BUILD( CFGID_OSD_BKID+ID, 1 )
#define CFGID_OSD_POSY(ID)		CFGID_BUILD( CFGID_OSD_BKID+ID, 2 )
#define CFGID_OSD_CONTENT(ID)	CFGID_BUILD( CFGID_OSD_BKID+ID, 4 )
#define CFGID_OSD_COLOR(ID)	CFGID_BUILD( CFGID_OSD_BKID+ID, 5 )
#define CFGID_OSD_ALPHA(ID)	CFGID_BUILD( CFGID_OSD_BKID+ID, 6 )
#define CFGID_OSD_FONT(ID)		CFGID_BUILD( CFGID_OSD_BKID+ID, 7 )
#define CFGID_OSD_SIZE(ID)		CFGID_BUILD( CFGID_OSD_BKID+ID, 8 )
// user osd part2 ID 16-31
#define CFGID_OSD2_BASE(ID)	CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 0 )
#define CFGID_OSD2_SHOWEN(ID)	CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 0 )
#define CFGID_OSD2_POSX(ID)	CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 1 )
#define CFGID_OSD2_POSY(ID)	CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 2 )
#define CFGID_OSD2_CONTENT(ID)	CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 4 )
#define CFGID_OSD2_COLOR(ID)	CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 5 )
#define CFGID_OSD2_ALPHA(ID)	CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 6 )
#define CFGID_OSD2_FONT(ID)		CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 7 )
#define CFGID_OSD2_SIZE(ID)		CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 8 )

// input part1 BKID = CFGID_INPUT##CHID##_BKID (5 channels CFGID_INPUT1_BKID ~ CFGID_INPUT5_BKID)
#define CFGID_INPUT_BASE(BKID)	CFGID_BUILD( BKID, 0 )
#define CFGID_INPUT_CHNAME(BKID)	CFGID_BUILD( BKID, 1 )
#define CFGID_INPUT_CHVALID(BKID)	CFGID_BUILD( BKID, 3 )
#define CFGID_INPUT_CHRES(BKID)	CFGID_BUILD( BKID, 4 )	// resolution
#define CFGID_INPUT_FOVTYPE(BKID)	CFGID_BUILD( BKID, 5 )	// lv 0-n
#define CFGID_INPUT_AIMTYPE(BKID)	CFGID_BUILD( BKID, 7 )	// 0-2
#define CFGID_INPUT_SENISIVITY(BKID)	CFGID_BUILD( BKID, 9 )
#define CFGID_INPUT_DETX(BKID)	CFGID_BUILD( BKID, 10 )
#define CFGID_INPUT_DETY(BKID)	CFGID_BUILD( BKID, 11 )
#define CFGID_INPUT_DETW(BKID)	CFGID_BUILD( BKID, 12 )
#define CFGID_INPUT_DETH(BKID)	CFGID_BUILD( BKID, 13 )
#define CFGID_INPUT_RESX(BKID)	CFGID_BUILD( BKID, 14 )
#define CFGID_INPUT_RESY(BKID)	CFGID_BUILD( BKID, 15 )
#define CFGID_INPUT_FOVCLASS(BKID)	CFGID_BUILD( BKID+1, 0 )	
// input part2 BKID = CFGID_INPUT##CHID##_BKID (5 channels CFGID_INPUT1_BKID ~ CFGID_INPUT5_BKID)
#define CFGID_INPUT_AIMLV(BKID)	CFGID_BUILD( BKID+6, 0 )
#define CFGID_INPUT_FIXAIMW(BKID)	CFGID_BUILD( BKID+6, 2 )
#define CFGID_INPUT_FIXAIMH(BKID)	CFGID_BUILD( BKID+6, 3 )
#define CFGID_INPUT_SWAIMW(BKID, SWLV)	CFGID_BUILD( BKID+6, 2+(SWLV*2) )	// SWLV 1-3
#define CFGID_INPUT_SWAIMH(BKID, SWLV)	CFGID_BUILD( BKID+6, 3+(SWLV*2) )	// SWLV 1-3

#define CFGID_INPUT_CURBOREX(BKID)	CFGID_BUILD( BKID+6, 14 )	
#define CFGID_INPUT_CURBOREY(BKID)	CFGID_BUILD( BKID+6, 15 )	


#define CFGID_INPUT_CURFOVLV(BKID, SWLV)	CFGID_BUILD( BKID+1, 0 )
#define CFGID_INPUT_ZOOMFBS(BKID , NUM)	CFGID_BUILD( BKID+3, NUM+3)



//input_fov1x     
static inline int CFGID_INPUT_FOVX(int BKID , int NUM)
{
	int ret;
	if(NUM <= 7)
		ret = CFGID_BUILD( BKID+1, NUM*2 );
	else
		ret = CFGID_BUILD( BKID+2, (NUM-8)*2 );
	return ret;
}
//input_fov1y
static inline int CFGID_INPUT_FOVY(int BKID , int NUM)
{
	int ret;
	if(NUM <= 7)
		ret = CFGID_BUILD( BKID+1, NUM*2+1 );
	else
		ret = CFGID_BUILD( BKID+2, (NUM-8)*2 +1);
	return ret;
}
//input_boresight1X
static inline int CFGID_INPUT_boresightX(int BKID , int NUM)
{
	int ret;
	if(NUM <= 8)
		ret = CFGID_BUILD( BKID+4, (NUM-1)*2 );
	else
		ret = CFGID_BUILD( BKID+5, (NUM-9)*2);
	return ret;
}
//input_boresight1Y
static inline int CFGID_INPUT_boresightY(int BKID , int NUM)
{
	int ret;
	if(NUM <= 8)
		ret = CFGID_BUILD( BKID+4, (NUM-1)*2+1 );
	else
		ret = CFGID_BUILD( BKID+5, (NUM-9)*2 +1);
	return ret;
}

static inline int CFGID_INPUT_Feedback(int BKID , int NUM)
{
	int ret;

	ret = CFGID_BUILD( BKID+3, NUM + 2);

	return ret;
} 


// sys config table part end
////////////////////////////////////////////////////


#endif
