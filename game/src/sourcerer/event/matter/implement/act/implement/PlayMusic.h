/***********************************************************************//**
 *	PlayMusic.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_PLAY_MUSIC_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_PLAY_MUSIC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	BGM 再生 Act
 *	
**//*---------------------------------------------------------------------*/
class PlayMusic : public Act
{
public:
	static const s32	CTRLKEY_BGM_ACT	= 1;

public:
	u16				_bgmid;			// ［必須］BGM ID
	s32				_cntLoop;		// ［オプション］ループ回数
	f32				_volume;		// ［オプション］音量
	s32				_frameFadeOut;	// ［オプション］フェードアウトフレーム数
	s32				_frameFadeIn;	// ［オプション］フェードインフレーム数

public:
	PlayMusic();
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_PLAY_MUSIC_H_
