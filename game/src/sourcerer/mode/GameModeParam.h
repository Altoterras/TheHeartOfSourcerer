/***********************************************************************//**
 *	GameModeParam.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/14.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_GAME_MODE_PARAM_H_
#define _SRCR_MODE_GAME_MODE_PARAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Unit;

/*---------------------------------------------------------------------*//**
 *	ゲームモード汎用パラメータ
 *
**//*---------------------------------------------------------------------*/
class GameModeParam
{
	//======================================================================
	// 定数
public:
	static const u32	F_RESUME_START				=	0x00000001;
	static const u32	F_PASTE_SAVES_BOOT			=	0x00000002;
	static const u32	F_DEBUG_MODEL_VIEW_FILE1	=	0x10000000;
	static const u32	F_DEBUG_MODEL_VIEW_FILE2	=	0x20000000;
	static const u32	F_DEBUG_MODEL_VIEW_FILE3	=	0x40000000;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// フラグを得る
	inline u32 getFlags() const { return _flags; }
	// フォーカスメニュー呼び出し時の対象ユニットを得る
	inline Unit* getFocusMenuUnit() const { return _unitFocusMenu; }

	// フラグを設定する
	inline void setFlags(u32 flag, bool isValid) { TFW_SET_FLAG(_flags, flag, isValid); }
	// フラグをクリアする
	inline void clearFlags() { _flags = 0; }
	// フォーカスメニュー呼び出し時の対象ユニットを設定する
	inline void setFocusMenuUnit(Unit* unit) { _unitFocusMenu = unit; }


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	GameModeParam() : _flags(0), _unitFocusMenu(0L) {}

	//======================================================================
	// 変数
private:
	u32	_flags;
	Unit* _unitFocusMenu;	// フォーカスメニュー呼び出し時の対象ユニット
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_GAME_MODE_PARAM_H_
