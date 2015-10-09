// Number of files = 39

const unsigned int	SYSTEM_GUI01_SD_JP_PNG = 202; // Gui01_sd_jp.png
const unsigned int	SYSTEM_GUI01_HD_JP_PNG = 212; // Gui01_hd_jp.png
const unsigned int	SYSTEM_GUI01_RD_JP_PNG = 222; // Gui01_rd_jp.png
const unsigned int	SYSTEM_FONT_EN01_SD_PNG = 1111; // Font_en01_sd.png
const unsigned int	SYSTEM_FONT_EN01_HD_PNG = 1121; // Font_en01_hd.png
const unsigned int	SYSTEM_FONT_EN01_RD_PNG = 1131; // Font_en01_rd.png
const unsigned int	SYSTEM_FONT_JP01G_SD_PNG = 1211; // Font_jp01g_sd.png
const unsigned int	SYSTEM_FONT_JP01G_SD_PTBL = 1212; // Font_jp01g_sd.ptbl
const unsigned int	SYSTEM_FONT_JP01NGB_HD_PNG = 1221; // Font_jp01ngb_hd.png
const unsigned int	SYSTEM_FONT_JP01NGB_HD_PTBL = 1222; // Font_jp01ngb_hd.ptbl
const unsigned int	SYSTEM_FONT_JP01NGB_RD_PNG = 1231; // Font_jp01ngb_rd.png
const unsigned int	SYSTEM_FONT_JP01NGB_RD_PTBL = 1232; // Font_jp01ngb_rd.ptbl
const unsigned int	SYSTEM_FONTJPCHARDEF_DAT = 1901; // FontJpCharDef.dat
const unsigned int	SYSTEM_LOADING_PNG = 4111; // Loading.png
const unsigned int	SYSTEM_TITLE_SD_PNG = 5111; // Title_sd.png
const unsigned int	SYSTEM_TITLE_HD_PNG = 5121; // Title_hd.png
const unsigned int	SYSTEM_TITLE_RD_PNG = 5131; // Title_rd.png
const unsigned int	SYSTEM_TITLE_WD_PNG = 5141; // Title_wd.png
const unsigned int	PARTICLE_PTCL001_TBMB = 820001; // Ptcl001.tbmb
const unsigned int	PARTICLE_PTCL002_TBMB = 820002; // Ptcl002.tbmb
const unsigned int	PARTICLE_PARTICLEDEF01_DAT = 821991; // ParticleDef01.dat
const unsigned int	AR_HELP_PAGE1_SD_PNG = 100100111; // help_page1_sd.png
const unsigned int	AR_HELP_PAGE1_HD_PNG = 100100121; // help_page1_hd.png
const unsigned int	AR_HELP_PAGE1_RD_PNG = 100100131; // help_page1_rd.png
const unsigned int	AR_HELP_PAGE2_SD_PNG = 100100211; // help_page2_sd.png
const unsigned int	AR_HELP_PAGE2_HD_PNG = 100100221; // help_page2_hd.png
const unsigned int	AR_HELP_PAGE2_RD_PNG = 100100231; // help_page2_rd.png
const unsigned int	AR_GETMARKER_PAGE1_SD_PNG = 100101111; // getmarker_page1_sd.png
const unsigned int	AR_GETMARKER_PAGE1_HD_PNG = 100101121; // getmarker_page1_hd.png
const unsigned int	AR_GETMARKER_PAGE1_RD_PNG = 100101131; // getmarker_page1_rd.png
const unsigned int	AR_GETMARKER_PAGE2_SD_PNG = 100101211; // getmarker_page2_sd.png
const unsigned int	AR_GETMARKER_PAGE2_HD_PNG = 100101221; // getmarker_page2_hd.png
const unsigned int	AR_GETMARKER_PAGE2_RD_PNG = 100101231; // getmarker_page2_rd.png
const unsigned int	AR_GOSUPPORT_PHONE_NO_SD_PNG = 100102111; // gosupport_phone-no_sd.png
const unsigned int	AR_GOSUPPORT_PHONE_NO_HD_PNG = 100102121; // gosupport_phone-no_hd.png
const unsigned int	AR_GOSUPPORT_PHONE_NO_RD_PNG = 100102131; // gosupport_phone-no_rd.png
const unsigned int	AR_GOSUPPORT_PHONE_YES_SD_PNG = 100102211; // gosupport_phone-yes_sd.png
const unsigned int	AR_GOSUPPORT_PHONE_YES_HD_PNG = 100102221; // gosupport_phone-yes_hd.png
const unsigned int	AR_GOSUPPORT_PHONE_YES_RD_PNG = 100102231; // gosupport_phone-yes_rd.png


// File Path for Development-Mode

#if defined(_TBF_PACK_DEVMODE)

	#if !defined(TBF_PACK_DEV_MODE_FILE_LIST)
		typedef const char TBF_PACK_DEV_MODE_FILE_LIST;
	#endif

	extern TBF_PACK_DEV_MODE_FILE_LIST** g_filedefTbfPack;

#endif
