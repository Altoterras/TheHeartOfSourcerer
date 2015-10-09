/***********************************************************************//**
 *	AnimChg.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_ANIM_CHG_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_ANIM_CHG_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	アニメーション変更 Act
 *	
**//*---------------------------------------------------------------------*/
class AnimChg : public Act
{
public:
	s32				_animid;		// ［必須］アニメーション ID
	s32				_charid;		// ［オプション］キャラクタ ID
	bool			_loop;			// ［オプション］ループフラグ

public:
	AnimChg() : Act(ACT_SHOW_OBJ), _animid(0), _charid(0), _loop(false) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_ANIM_CHG_H_
