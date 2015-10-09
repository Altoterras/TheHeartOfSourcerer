/***********************************************************************//**
 *	Keyboard.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2010/05/25.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_UI_KEYBOARD_H_
#define _TFW_UI_KEYBOARD_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "ui.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class UiListener;

/*---------------------------------------------------------------------*//**
 *	キーボード
 *	
**//*---------------------------------------------------------------------*/
class Keyboard : public Ui
{
	//======================================================================
	// 定数
public:
	// システムキー（ASCII コードの非文字部分に割り当てる）
	enum SysKey
	{
		SYSKEY_NULL,
		SYSKEY_INS		= 0x05,		// Insert
		SYSKEY_HOME		= 0x06,		// Home
		SYSKEY_END		= 0x07,		// End
		SYSKEY_BS		= 0x08,		// BackSpace
		SYSKEY_SHIFT	= 0x10,		// Shift
		SYSKEY_CTRL		= 0x11,		// Ctrl
		SYSKEY_ALT		= 0x12,		// Alt
		SYSKEY_LEFT		= 0x13,		// ←
		SYSKEY_RIGHT	= 0x14,		// →
		SYSKEY_UP		= 0x15,		// ↑
		SYSKEY_DOWN		= 0x16,		// ↓
		SYSKEY_PGUP		= 0x17,		// PageUp
		SYSKEY_PGDOWN	= 0x18,		// PageDown
		SYSKEY_ESC		= 0x1b,		// Esc
		SYSKEY_DEL		= 0x7F,		// Delete
	};

	static const u32	NUM_KEY					= 128;

	static const short	REPCNT_FIRST_DEFAULT	= 16;
	static const short	REPCNT_AFTER_DEFAULT	= 2;

private:
	static const u8		SWF_ON_DIRECT			= 0x01;
	static const u8		SWF_ON_CUR				= 0x02;
	static const u8		SWF_ON_PREV				= 0x04;
	static const u8		SWF_TRG_CUR				= 0x08;
	static const u8		SWF_TRG_REL_CUR			= 0x10;

	static const u8		CF_ON_ONE_SHOT			= 0x01;	// １フレームのみの押下

	//======================================================================
	// クラス
private:
	class KeyInfo
	{
	public:
		u8 _swflags;	// スイッチ フラグ
		u8 _cflags;		// 設定フラグ
		s16 _repcnt;	// リピート カウンタ
		KeyInfo() : _swflags(0), _cflags(0), _repcnt(0) {}
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
	void setOnOnlyOneFrame(int key);
	void setRepeatCount(short cntFirst, short cntAfter);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Keyboard();
	virtual ~Keyboard();

	bool create();
	void destroy();
	virtual void exec();
	
	//======================================================================
	// 変数
private:
	KeyInfo		_ki[NUM_KEY];
	bool		_swOnAny;
	short		_cntRepFirst;
	short		_cntRepAfter;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_UI_KEYBOARD_H_