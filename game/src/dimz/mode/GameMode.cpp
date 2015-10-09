/***********************************************************************//**
 *	GameMode.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/06/29.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GameMode.h"

// Friends
//#include "ingame/IngameMode.h"
//#include "startgame/StartGameMode.h"
//#include "title/TitleMode.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

DIMZ_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GameMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// プロパティ

/*---------------------------------------------------------------------*//**
	末端のゲームモード種別を予約されたものも評価して取得する
**//*---------------------------------------------------------------------*/
GameMode::ModeKind GameMode::getChildEndModeKindConsiderReserved() const
{
	if(_modekindReserveEnd != 0)
	{
		return _modekindReserveEnd;
	}

	return getChildEndModeKind();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	末端のモード変更
**//*---------------------------------------------------------------------*/
bool GameMode::changeEndMode(ModeKind modek)
{
	// 自分が担当している範囲のモードの場合は自分で変更して true を返す
	if(changeModeDive(modek) == false)
	{
		// モードツリーのトップである GameMode には必ず、
		// changeModeDive は true が返ってくるはずである。
		// false が返ってくるということは、
		// 存在しないモードを指定したということだろう･･･
		return false;
	}

	_modekindReserveEnd = modek;
	return true;
}

/*---------------------------------------------------------------------*//**
	末端のモードを戻す
	※　末端のモードのひとつ親のモードの reservePervMode() を利用するため、
		親を超えては基本的には戻れない．
		戦闘やイベントなどの一時的なモード変更から戻るためのメソッドである
**//*---------------------------------------------------------------------*/
bool GameMode::changePrevEndMode()
{
	if(_modekindReserveEnd != MODE_NULL)	// モード予約済み
	{
		clearReservedMode(true);	// モード予約クリア
		_modekindReserveEnd = MODE_NULL;
	}

	Mode* modeEnd = getChildEndModeDive(this);
	if(modeEnd == 0L)		{	return false;	}
	Mode* modeParent = modeEnd->getParentMode();
	if(modeParent == 0L)	{	return false;	}

	// 予約
	_modekindReserveEnd = (ModeKind)modeParent->getPrevMode();
	modeParent->reserveMode(_modekindReserveEnd);
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
GameMode::GameMode()
	: _modeCurChildEnd(0L)
	, _modekindCurChildEnd(MODE_NULL)
	, _modekindReserveEnd(MODE_NULL)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
GameMode::~GameMode()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool GameMode::create()
{
	if(!GameModeBase::create(MODE_GAME__START_, MODE_GAME__END_, 0L))
	{
		return false;
	}

	// 汎用パラメータセット
	setParameter(&_param);

	// モード登録
//	registerMode((s16)MODE_GAME_STARTGAME, new StartGameMode());
//	registerMode((s16)MODE_GAME_INGAME, new IngameMode());
//	registerMode((s16)MODE_GAME_TITLE, new TitleMode());
#if defined(_DEBUG)
//	registerMode((s16)MODE_GAME_MODELVW, new ModelViewMode());
#endif

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void GameMode::destroy()
{
	// モードクリア
	setMode(MODE_NULL, false);

	Mode::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void GameMode::exec(ExecRes* res, const ExecCtx* ec)
{
	Mode::exec(res, ec);

	// 現在の末端モードを得ておく
	_modeCurChildEnd = getChildEndModeDive(this);
	_modekindCurChildEnd = (_modeCurChildEnd != 0L) ? (ModeKind)_modeCurChildEnd->getSelfModeId() : MODE_NULL;
}

/*---------------------------------------------------------------------*//**
	再帰処理で末端のモードを取得する
**//*---------------------------------------------------------------------*/
Mode* GameMode::getChildEndModeDive(const Mode* mode) const
{
	Mode* modeChild = mode->getCurrentMode();
	if(modeChild != 0L)
	{
		Mode* modeRet = getChildEndModeDive(modeChild);
		if(modeRet != 0L)
		{
			return modeRet;
		}
	}
	return modeChild;
}

/*---------------------------------------------------------------------*//**
	モード変更通知
**//*---------------------------------------------------------------------*/
void GameMode::notifyChildModeChanged(short modeid, void* objParamRef)
{
	_modekindReserveEnd = MODE_NULL;
}

////////////////////////////////////////////////////////////////////////////

DIMZ_END_NS
