/***********************************************************************//**
 *	SysConf.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SysConf.h"

// Friends
#include "../event/EventSys.h"
#include "../body/Game.h"
#include "../gui/GameGui.h"
#include "../gui/window/implement/SubtitlesWindow.h"

// External
#include "../../tfw/Tfw.h"
#include "../../tfw/file/FileBase.h"
#include "../../tfw/gcmn/View.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SysConf メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	画面向きの設定
**//*---------------------------------------------------------------------*/
void SysConf::setScreenOrientation(bool isPlus90)
{
	// 向きを設定（システムがまだ存在しない／終了している場合はあきらめる）
	if(_isCreated)
	{
		u32 rotz;
		#if 0
			if(isPlus90)
			{
				#if defined(_WINDOWS)
					rotz = TFW_DISPF_RANG_180;
				#elif defined(_IOS) || defined(_ANDROID)
					rotz = TFW_DISPF_RANG_270;
				#endif
			}
			else
			{
				#if defined(_WINDOWS)
					rotz = TFW_DISPF_RANG_0;
				#elif defined(_IOS) || defined(_ANDROID)
					rotz = TFW_DISPF_RANG_90;
				#endif
			}
		#else
			if(isPlus90)
			{
				rotz = TFW_DISPF_RANG_180;
			}
			else
			{
				rotz = TFW_DISPF_RANG_0;
			}
		#endif
		View* view = Game::getGame()->getTfw()->getView();
		if(view != 0L)
		{
			view->setDisplayFlags((view->getDisplayFlags() & ~TFW_DISPF_RANG_MASK) | rotz, true);
		}
	}

	// セーブストラクチャに保存
	u32 disporNew = isPlus90 ? 1 : 0;
	if(_svst._dispor != disporNew)
	{
		_svst._dispor = disporNew;
		_isChanged = true;
	}
}

/*---------------------------------------------------------------------*//**
	言語の設定
**//*---------------------------------------------------------------------*/
void SysConf::setLanguage(LangId lang)
{
	// 言語を設定（システムがまだ存在しない／終了している場合はあきらめる）
	if(_isCreated)
	{
		Game::getGame()->setLangId(lang);
	}

	// セーブストラクチャに保存
	u8 langNew = (u8)lang;
	if(_svst._lang != langNew)
	{
		_svst._lang = langNew;
		_isChanged = true;
	}
}

#if 0	// Ver2.0.1 ～
/*---------------------------------------------------------------------*//**
	メッセージ送りの設定
**//*---------------------------------------------------------------------*/
void SysConf::setEnableWaitMovieMessage(bool isEnable)
{
	// ムービースキップ設定
	if(TFW_IS_FLAG(_svst._flags, F_WAIT_MOVIE_MSG) != isEnable)
	{
		// システムに反映（システムがまだ存在しない／終了している場合はあきらめる）
		if(_isCreated)
		{
			EventSys* evsys = Game::getGame()->getEventSys();
			if(evsys != 0L)
			{
				evsys->setEnableWaitMovieMessage(isEnable);
				SubtitlesWindow* wndMsg = Game::getGame()->getGui()->getSubtitlesWindow();
				if(wndMsg != 0L)
				{
					wndMsg->setWaitSayMode(isEnable);
				}
			}
		}

		// セーブストラクチャに保存
		_svst._flags = isEnable ? F_WAIT_MOVIE_MSG : 0;
		_isChanged = true;
	}
}
#endif

/*---------------------------------------------------------------------*//**
	省メモリマルチタスク切り替えの設定
**//*---------------------------------------------------------------------*/
void SysConf::setEcoMultiTasking(bool ecoMode)
{
	TFW_SET_FLAG(_svst._flags, F_MULTITASKING_ECO, ecoMode);
}

/*---------------------------------------------------------------------*//**
	セーブのためのデータを収集する
**//*---------------------------------------------------------------------*/
void SysConf::collectDataForSave()
{
	ASSERT(_isCreated);
	
	// 値を収集してくる
	// ※ 現状では、全ての値が _svst なので、何も収集しない
}

/*---------------------------------------------------------------------*//**
	セーブ
**//*---------------------------------------------------------------------*/
bool SysConf::save(FileBase* file)
{
	// ファイルに書き込む
	file->write(&_svst, sizeof(SaveStructure));

	_isChanged = false;	// 変更フラグをオフに

	return true;
}

/*---------------------------------------------------------------------*//**
	ロード
**//*---------------------------------------------------------------------*/
bool SysConf::load(FileBase* file)
{
	ASSERT(_isCreated);

	// ファイルから読み込む
	SaveStructure svst;
	file->read(&svst, sizeof(SaveStructure));

	// バージョン補正
	#if 0	// Ver2.0.1 ～
		#if 1	// Ver1.2.x ～
			TFW_SET_FLAG(svst._flags, F_WAIT_MOVIE_MSG, true);
		#else	// ～ Ver1.1.x 【2010/12/24 r-kishi modified】
			if(svst._version < SaveStructure::VER_SYSCONF_SAVE_STRUCTURE)
			{
				TFW_SET_FLAG(svst._flags, F_WAIT_MOVIE_MSG, true);
			}
		#endif
	#endif

	// 各種設定を判定
	{
		// SysConf がデータマスターの設定
		#if 0	// 無効化．iOS の方は shouldAutorotateToInterfaceOrientation に任せることになった ［2012/07/30 r-kishi］
			setScreenOrientation(svst._dispor == 1);
		#endif
		setLanguage((LangId)svst._lang);
		#if 0	// Ver2.0.1 ～
			setEnableWaitMovieMessage(TFW_IS_FLAG(svst._flags, F_WAIT_MOVIE_MSG));
		#endif

		// メンバに反映
		TFW_COPY_MEMORY(&_svst, &svst, sizeof(SaveStructure));
		// 構造体のバージョン番号再設定
		_svst._version = SaveStructure::VER_SYSCONF_SAVE_STRUCTURE;
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SysConf::SysConf()
	: _isChanged(false)
	, _isCreated(false)
{
	TRACE("{SysConf::SysConf} sizeof(SysConf::SaveStructure)=%d\n", sizeof(SaveStructure));
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	// 構造体のバージョン番号
	_svst._version = SaveStructure::VER_SYSCONF_SAVE_STRUCTURE;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SysConf::~SysConf()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SysConf::create()
{
	// 現在の言語設定を得ておく
	_svst._lang = (u8)Env_getLangId();

	// 現在の画面向きを得ておく
	View* view = Game::getGame()->getTfw()->getView();
	if(view != 0L)
	{
		u32 rotz = view->getRotZ();
		bool isPlus90 =
			#if defined(_WINDOWS)
				(rotz == TFW_DISPF_RANG_180);
			#elif defined(_IOS) || defined(_ANDROID)
				(rotz == TFW_DISPF_RANG_270);
			#endif
		_svst._dispor = isPlus90 ? 1 : 0;
	}
	
	#if 0	// Ver2.0.1 ～
		// デフォルトはメッセージポーズ有効
		setEnableWaitMovieMessage(true);
	#endif

	_isCreated = true;
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SysConf::destroy()
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
