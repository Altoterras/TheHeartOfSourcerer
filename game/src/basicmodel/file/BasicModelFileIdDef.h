/***********************************************************************//**
 *	BasicModelFileIdDef.h
 *	Enlight BasicModel Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _BASICMODEL_FILE_BASICMODEL_FILE_ID_DEF_H_
#define _BASICMODEL_FILE_BASICMODEL_FILE_ID_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/file/IdFile.h"

////////////////////////////////////////////////////////////////////////////
// 定数

// 各ファイルの ID
#if defined(_DEBUG)
	#define _TBF_PACK_DEVMODE
#endif

BASICMODEL_BEGIN_NS

#include "FileIdDefineHeader.h"

BASICMODEL_END_NS

// ファイル ID の範囲
#define FILEID_CHARACTER	(300000)
#define FILEID_PARTICLE		(820000)
#define FILEID_SE			(1000000)
#define FILEID_BGM			(2100000)
#define FILEID_MAP			(200000000)
#define FILEID_STORY_EVENT	(300000000)
#define FILEID_QUEST_EVENT	(400000000)
#define FILEID_TMP_EVENT	(500000000)

////////////////////////////////////////////////////////////////////////////
// マクロ

#define FILEID_CONV_LANG(fid_)					((fid_) + Env_getLangId())
#define FILEID_CONV_PLATFORM(fid_)				((fid_) + Env_getPlatformId())
#define FILEID_CONV_HD(fid_)					((fid_) + (Env_isHdResolution() ? 10 : 0))
#define FILEID_CONV_RD(fid_)					((fid_) + (Env_isRdResolution() ? 20 : Env_isHdResolution() ? 10 : 0))

#define FILEID_FROM_PARTICLE_MODEL(ptclid_)		(FILEID_PARTICLE + (ptclid_))
#define FILEID_FROM_MAP_MODEL(mapid_)			(FILEID_MAP + ((mapid_) * 100) + 1)
#define FILEID_FROM_CHAR_MODEL(charid_)			(FILEID_CHARACTER + ((charid_) * 10) + 1)
#define FILEID_FROM_CHAR_EYE_MODEL(charid_)		(FILEID_CHARACTER + ((charid_) * 10) + 2)
#define FILEID_FROM_CHAR_MOUTH_MODEL(charid_)	(FILEID_CHARACTER + ((charid_) * 10) + 3)
#define FILEID_FROM_SE(seid_)					FILEID_CONV_PLATFORM(FILEID_SE + ((seid_) * 10) + 1)
#define FILEID_FROM_BGM(bgmid_, stereo_, caf_)	(FILEID_BGM + ((bgmid_) * 100) + ((stereo_) ? 20 : 10) + ((caf_) ? 2 : 1))
#define FILEID_FROM_MAP_EVENT_XML(mapid_)		(FILEID_MAP + ((mapid_) * 100) + 2)
#define FILEID_FROM_MAP_EVENT_MSGD(mapid_)		FILEID_CONV_LANG( FILEID_MAP + ((mapid_) * 100) + 11 )
#define FILEID_FROM_STORY_EVENT_XML(storyid_)	(FILEID_STORY_EVENT + ((storyid_) * 100) + 1)
#define FILEID_FROM_STORY_EVENT_MSGD(storyid_)	FILEID_CONV_LANG( FILEID_STORY_EVENT + ((storyid_) * 100) + 11 )
#define FILEID_FROM_QUEST_EVENT_XML(questid_)	(FILEID_QUEST_EVENT + ((questid_) * 100) + 1)
#define FILEID_FROM_QUEST_EVENT_MSGD(questid_)	FILEID_CONV_LANG( FILEID_QUEST_EVENT + ((questid_) * 100) + 11 )
#define FILEID_FROM_TMP_EVENT_XML(tmpevid_)		(FILEID_TMP_EVENT + ((tmpevid_) * 100) + 1)
#define FILEID_FROM_TMP_EVENT_MSGD(tmpevid_)	FILEID_CONV_LANG( FILEID_TMP_EVENT + ((tmpevid_) * 100) + 11 )

////////////////////////////////////////////////////////////////////////////

#endif	// _BASICMODEL_FILE_BASICMODEL_FILE_ID_DEF_H_
