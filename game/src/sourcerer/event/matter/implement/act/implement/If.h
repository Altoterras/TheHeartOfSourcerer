/***********************************************************************//**
 *	If.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_IF_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_IF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	条件分岐 IF Act
 *	
**//*---------------------------------------------------------------------*/
class If : public Act
{
public:
	u16				_evidJump;		// ［必須］ジャンプ先イベント ID
	u16				_situfc01;		// ［オプション］シチュエーション フラグ カテゴリ 01
	s8				_lang;			// ［オプション］言語
	s32				_sfid;			// ［オプション］セーブフラグ ID
	u16				_storyidAbove;	// ［オプション］指定ストーリー ID 以上
	u16				_storyidUnder;	// ［オプション］指定ストーリー ID 以下
	u8				_queststat;		// ［オプション］クエスト状態（クエスト用）
	u8				_questgval;		// ［オプション］クエスト汎用値識別番号（クエスト用；１以上の値の場合は IF 条件合格）
	bool			_negative;		// ［オプション］条件判定を反転させる

public:
	If()
		: Act(ACT_IF)
		, _evidJump(0)
		, _situfc01(0)
		, _lang(-1)
		, _sfid(0)
		, _storyidAbove(0)
		, _storyidUnder(0)
		, _queststat(0)
		, _questgval(0)
		, _negative(false)
	{}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_IF_H_
