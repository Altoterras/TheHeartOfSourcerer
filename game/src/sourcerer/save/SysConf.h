/***********************************************************************//**
 *	SysConf.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/02.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SAVE_SYS_CONF_H_
#define _SRCR_SAVE_SYS_CONF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../etk/body/Env.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

TFW_END_NS

SRCR_BEGIN_NS
	
class Game;

/*---------------------------------------------------------------------*//**
 *	システム コンフィグ
 *
**//*---------------------------------------------------------------------*/
class SysConf
{
	//======================================================================
	// 定数
private:
	// フラグ
	static const u32 F_WAIT_MOVIE_MSG		= 0x00000001;	// ムービー時の台詞待ち．廃止 -> 【2012/08/26 r-kishi Ver2.0.2～】Game::OPTF_AUTO_FLOW_MOVIE_MSG に移行
	static const u32 F_MULTITASKING_ECO		= 0x00000002;	// 省メモリ低速切り替えマルチタスキング

	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	システムコンフィグのセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class SaveStructure
	{
	public:
		// [  0]
		u8 _version;							// [  1] この構造体のバージョン
		u8 _lang;								// [  1] 言語
		u8 _dispor;								// [  1] 画面向き
		u8 _spdoptmz;							// [  1] 速度最適化
		u32 _flags;								// [  4] フラグ
		u8 __pad_0010[ 8];						// [  8]
		// [ 16]
		static const s32 SIZE_THIS = 16;
		static const u8 VER_SYSCONF_SAVE_STRUCTURE = 2;	// バージョン番号
		SaveStructure() { TFW_ZERO_MEMORY(this, sizeof(SaveStructure)); }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 画面向きを得る
	inline bool isScreenOrientationPlus90() const { return _svst._dispor == 1; }
	// 言語を得る
	inline LangId getLanguage() const { return (LangId)_svst._lang; }
#if 0	// Ver2.0.1 ～
	// ムービーをポーズするかどうかを得る
	inline bool isEnableWaitMovieMessage() const { return TFW_IS_FLAG(_svst._flags, F_WAIT_MOVIE_MSG); }
#endif
	// 省メモリ低速切り替えマルチタスキングかどうか
	inline bool isEcoMultiTasking() const { return TFW_IS_FLAG(_svst._flags, F_MULTITASKING_ECO); }

	// 変更されているかどうかを得る
	inline bool isChanged() const { return _isChanged; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setScreenOrientation(bool isPlus90);
	void setLanguage(LangId lang);
#if 0	// Ver2.0.1 ～
	void setEnableWaitMovieMessage(bool isEnable);
#endif
	void setEcoMultiTasking(bool ecoMode);

	void collectDataForSave();
	bool save(FileBase* file);
	bool load(FileBase* file);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SysConf();
	~SysConf();
	bool create();
	void destroy();

	//======================================================================
	// 変数
private:
	// 保存データ
	SaveStructure _svst;
	bool _isChanged;
	bool _isCreated;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SAVE_SYS_CONF_H_
