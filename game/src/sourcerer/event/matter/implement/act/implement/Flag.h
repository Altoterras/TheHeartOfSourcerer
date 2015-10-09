/***********************************************************************//**
 *	Flag.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/28.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_FLAG_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_FLAG_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	フラグ制御 Act
 *	
**//*---------------------------------------------------------------------*/
class Flag : public Act
{
public:
	s32				_sfid;			// ［オプション（いずれか必須）］セーブフラグ ID
	u8				_questgval;		// ［オプション（いずれか必須）］クエスト汎用値識別番号（クエスト用）
	s32				_value;			// ［オプション］新しい値

public:
	Flag() : Act(ACT_FLAG), _sfid(0), _questgval(0), _value(0) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_FLAG_H_
