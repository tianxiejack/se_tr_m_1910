#ifndef _CONFIG_TABLE_H_
#define _CONFIG_TABLE_H_

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

static int ShowDPI[MAX_CHAN][2] = {{1920, 1080},{1920, 1080},{1920, 1080},{1920, 1080},{720, 576}};

////////////////////////////////////////////////////
// sys config table part
#define SYSCONFIG_VERSION     100
#define CFGID_BKFEILD_MAX	16
#define CFGID_USEROSD_MAX	32

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
}CFGID_JOS;

typedef enum 
{
	CFGID_PTZ_BASE = CFGID_BUILD( CFGID_PTZ_BKID, 0 ),
	CFGID_PTZ_comm = CFGID_BUILD( CFGID_PTZ_BKID, 0 ),
}CFGID_PTZ;

typedef enum 
{
	CFGID_RTS_BASE = CFGID_BUILD( CFGID_RTS_BKID, 0 ),
	CFGID_RTS_mainch = CFGID_BUILD( CFGID_RTS_BKID, 0 ),
	CFGID_RTS_trken = CFGID_BUILD( CFGID_RTS_BKID, 1 ),
	CFGID_RTS_trkalg = CFGID_BUILD( CFGID_RTS_BKID, 2 ),
	CFGID_RTS_trkstat = CFGID_BUILD( CFGID_RTS_BKID, 3 ),
	CFGID_RTS_trkerrx = CFGID_BUILD( CFGID_RTS_BKID, 4 ),
	CFGID_RTS_trkerry = CFGID_BUILD( CFGID_RTS_BKID, 5 ),
	CFGID_RTS_mtden = CFGID_BUILD( CFGID_RTS_BKID, 6 ),
	CFGID_RTS_mtddet = CFGID_BUILD( CFGID_RTS_BKID, 7 ),	// MtdDetectStat
}CFGID_RTS;

typedef enum 
{
	CFGID_TRK_BASE = CFGID_BUILD( CFGID_TRK_BKID, 0 ),
	CFGID_TRK_assitime = CFGID_BUILD( CFGID_TRK_BKID, 15 ),
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
	CFGID_USROSD_biten = CFGID_BUILD( CFGID_SYS_BKID, 2 ),		// bit enable
}CFGID_SYS;

typedef enum 
{
	CFGID_MTD_BASE = CFGID_BUILD( CFGID_MTD_BKID, 0 ),
	CFGID_MTD_areabox = CFGID_BUILD( CFGID_MTD_BKID, 0 ),
	CFGID_MTD_detnum = CFGID_BUILD( CFGID_MTD_BKID, 1 ),	// detectNum
	CFGID_MTD_upspd = CFGID_BUILD( CFGID_MTD_BKID, 2 ),	// tmpUpdateSpeed
	CFGID_MTD_maxpixel = CFGID_BUILD( CFGID_MTD_BKID, 3 ),
	CFGID_MTD_minpixel = CFGID_BUILD( CFGID_MTD_BKID, 4 ),
	CFGID_MTD_senisivity = CFGID_BUILD( CFGID_MTD_BKID, 5 ),	// sensitivityThreshold
	CFGID_MTD_detspd = CFGID_BUILD( CFGID_MTD_BKID, 6 ),		// detectSpeed
	CFGID_MTD_priority = CFGID_BUILD( CFGID_MTD_BKID, 8 ),
}CFGID_MTD;

typedef enum 
{
	CFGID_PID_BASE = CFGID_BUILD( CFGID_PID_BKID, 0 ),


	
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
#define CFGID_OSD_COLOR(ID)	CFGID_BUILD( CFGID_OSD_BKID+ID, 5 )
#define CFGID_OSD_ALPHA(ID)	CFGID_BUILD( CFGID_OSD_BKID+ID, 6 )
#define CFGID_OSD_FONT(ID)		CFGID_BUILD( CFGID_OSD_BKID+ID, 7 )
#define CFGID_OSD_SIZE(ID)		CFGID_BUILD( CFGID_OSD_BKID+ID, 8 )
// user osd part2 ID 16-31
#define CFGID_OSD2_BASE(ID)	CFGID_BUILD( CFGID_OSD2_BKID+ID, 0 )
#define CFGID_OSD2_SHOWEN(ID)	CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 0 )
#define CFGID_OSD2_POSX(ID)	CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 1 )
#define CFGID_OSD2_POSY(ID)	CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 2 )
#define CFGID_OSD2_COLOR(ID)	CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 5 )
#define CFGID_OSD2_ALPHA(ID)	CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 6 )
#define CFGID_OSD2_FONT(ID)		CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 7 )
#define CFGID_OSD2_SIZE(ID)		CFGID_BUILD( CFGID_OSD2_BKID+(ID-16), 8 )

// input part1 BKID = CFGID_INPUT##CHID##_BKID (5 channels CFGID_INPUT1_BKID ~ CFGID_INPUT5_BKID)
#define CFGID_INPUT_BASE(BKID)	CFGID_BUILD( BKID, 0 )
#define CFGID_INPUT_CHNAME(BKID)	CFGID_BUILD( BKID, 1 )
#define CFGID_INPUT_CHVALID(BKID)	CFGID_BUILD( BKID, 3 )
#define CFGID_INPUT_CHRES(BKID)	CFGID_BUILD( BKID, 4 )	// resolution
#define CFGID_INPUT_FOVTYPE(BKID)	CFGID_BUILD( BKID, 5 )
#define CFGID_INPUT_AIMTYPE(BKID)	CFGID_BUILD( BKID, 7 )
#define CFGID_INPUT_PRIORITY(BKID)	CFGID_BUILD( BKID, 8 )
#define CFGID_INPUT_SENISIVITY(BKID)	CFGID_BUILD( BKID, 9 )
#define CFGID_INPUT_DETX(BKID)	CFGID_BUILD( BKID, 10 )
#define CFGID_INPUT_DETY(BKID)	CFGID_BUILD( BKID, 11 )
#define CFGID_INPUT_DETW(BKID)	CFGID_BUILD( BKID, 12 )
#define CFGID_INPUT_DETH(BKID)	CFGID_BUILD( BKID, 13 )
// input part2 BKID = CFGID_INPUT##CHID##_BKID (5 channels CFGID_INPUT1_BKID ~ CFGID_INPUT5_BKID)
#define CFGID_INPUT_AIMLV(BKID)	CFGID_BUILD( BKID+6, 0 )
#define CFGID_INPUT_FIXAIMW(BKID)	CFGID_BUILD( BKID+6, 2 )
#define CFGID_INPUT_FIXAIMH(BKID)	CFGID_BUILD( BKID+6, 3 )
#define CFGID_INPUT_SWAIMW(BKID, SWLV)	CFGID_BUILD( BKID+6, 2+(SWLV*2) )	// SWLV 1-3
#define CFGID_INPUT_SWAIMH(BKID, SWLV)	CFGID_BUILD( BKID+6, 3+(SWLV*2) )	// SWLV 1-3

#define CFGID_INPUT_CURFOVLV(BKID, SWLV)	CFGID_BUILD( BKID+1, 0 )
#define CFGID_INPUT_ZOOMFBS(BKID , NUM)	CFGID_BUILD( BKID+3, NUM+3)



//input_fov1x     
static inline int CFGID_INPUT_FOVX(int BKID , int NUM)
{
	int a;
	if(NUM <= 7)
		a = CFGID_BUILD( BKID+1, NUM*2 );
	else
		a = CFGID_BUILD( BKID+2, (NUM-8)*2 );
	return a;
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
		ret = CFGID_BUILD( BKID+4, NUM*2 );
	else
		ret = CFGID_BUILD( BKID+5, (NUM-9)*2);
	return ret;
}
//input_boresight1Y
static inline int CFGID_INPUT_boresightY(int BKID , int NUM)
{
	int ret;
	if(NUM <= 8)
		ret = CFGID_BUILD( BKID+4, NUM*2+1 );
	else
		ret = CFGID_BUILD( BKID+5, (NUM-9)*2 +1);
	return ret;
}



// sys config table part end
////////////////////////////////////////////////////


#endif