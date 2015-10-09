/***********************************************************************//**
 *	EnterConfront.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_ENTER_CONFRONT_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_ENTER_CONFRONT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	戦闘へ入る
 *	
**//*---------------------------------------------------------------------*/
class EnterConfront : public Act
{
private:
	static const u32 OPTF_HAS_BGM_ID			= 0x01;
	static const u32 OPTF_HAS_EVID_AFTER_WIN	= 0x02;
	static const u32 OPTF_HAS_EVID_AFTER_LOSE	= 0x04;
	static const u32 OPTF_HAS_IS_ALLOW_ESCAPE	= 0x08;

public:
	u16				_evidCtrlTrg;		// ［オプション（いずれか必須）］対象を指定する制御用イベント ID （優先）
	u16				_charid;			// ［オプション（いずれか必須）］対象キャラクタ ID
	u16				_bgmid;				// ［オプション］BGM ID
	u16				_evidCtrlConfront;	// ［オプション］対決制御用イベント ID
	u16				_evidAfterWin;		// ［オプション］勝利後に実行するイベント ID
	u16				_evidAfterLose;		// ［オプション］敗北後に実行するイベント ID
	bool			_isAllowEscape;		// ［オプション］逃げる許可
	u32				_optflags;			// オプション取得フラグ（PlacementObj と重複する可能性があるもの）

public:
	EnterConfront() : Act(ACT_ENTER_CONFRONT), _evidCtrlTrg(0), _charid(0), _bgmid(0), _evidCtrlConfront(0), _evidAfterWin(0), _evidAfterLose(0), _isAllowEscape(false), _optflags(0) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_ENTER_CONFRONT_H_
