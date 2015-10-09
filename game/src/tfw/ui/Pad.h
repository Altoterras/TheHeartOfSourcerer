/***********************************************************************//**
 *	Pad.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_UI_PAD_H_
#define _TFW_UI_PAD_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "ui.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class UiListener;

/*---------------------------------------------------------------------*//**
 *	パッド（コントローラー）
 *	
**//*---------------------------------------------------------------------*/
class Pad : public Ui
{
	//======================================================================
	// 定数
public:
	enum Key
	{
		KEY_OK			= 0,			// OK
		KEY_CANCEL		= 1,			// キャンセル
		KEY_LEFT		= 2,			// ←
		KEY_RIGHT		= 3,			// →
		KEY_UP			= 4,			// ↑
		KEY_DOWN		= 5,			// ↓
		_DEBUG_			= 6,			// デバッグモード
		NUM_FIXED_KEY	= 7,			// 固定キーの数
		_KEY_CUSTOM_	= NUM_FIXED_KEY	// カスタムキーを定義する場合の開始番号
	};
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	bool isOn(int key) const;
	bool isTrigger(int key) const;
	bool isRelease(int key) const;
	bool isRepeatTrigger(int key) const;
	bool isOnAny() const;

	void setOn(int key, bool is);
	void setRepeatCount(short cnt);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Pad();
	virtual ~Pad();

	bool create(int numKeyKind, short cntRepMax = 6);
	virtual void exec();
	
	//======================================================================
	// 変数
private:
	int			_numKeyKind;
	bool*		_swOnDirect;
	bool*		_swOnCur;
	bool*		_swOnPrev;
	bool*		_swTrgCur;
	bool*		_swTrgRelCur;
	bool		_swOnAny;
	short*		_cntRepOn;
	short		_cntRepMax;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_UI_PAD_H_