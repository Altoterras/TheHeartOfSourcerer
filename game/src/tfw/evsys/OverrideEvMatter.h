/***********************************************************************//**
 *	OverrideEvMatter.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2010/04/01.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_EVSYS_OVERRIDE_EV_MATTER_H_
#define _TFW_EVSYS_OVERRIDE_EV_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	置換イベント マター情報
 *
**//*---------------------------------------------------------------------*/
class OverrideEvMatter
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 種別を得る
	inline s32 getKind() const { return _kind; }
	// 置換前のイベント ID を得る
	inline u16 getOldEventId() const { return _evidOld; }
	// 置換後のイベント ID を得る
	inline u16 getNewEventId() const { return _evidNew; }
	// 識別キーを得る
	inline u16 getDescKey() const { return _desckey; }

	// 種別を設定する
	inline void setKind(s32 kind) { _kind = kind; }
	// 置換前のイベント ID を設定する
	inline void setOldEventId(u16 evidOld) { _evidOld = evidOld; }
	// 置換後のイベント ID を設定する
	inline void setNewEventId(u16 evidNew) { _evidNew = evidNew; }
	// 識別キーを設定する
	inline void setDescKey(u32 desckey) { _desckey = desckey; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool setup(s32 type, u16 evidOld, u16 evidNew, u32 desckey);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	OverrideEvMatter();

	//======================================================================
	// 変数
private:
	// 基本情報
	s32 _kind;
	u16 _evidOld;
	u16 _evidNew;
	u32 _desckey;		// 識別キー（外部管理の値）
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_EVSYS_OVERRIDE_EV_MATTER_H_
