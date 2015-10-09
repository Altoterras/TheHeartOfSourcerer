/***********************************************************************//**
 *	GameMode.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GameMode.h"

// Friends
#include "bookview/BookViewMode.h"
#include "ingame/IngameMode.h"
#include "purepuzzle/PurePuzzleMode.h"
#include "startgame/StartGameMode.h"
#include "title/TitleMode.h"
#include "../debug/mode/TestViewTerminatorMode.h"
#include "../debug/mode/modelview/ModelViewMode.h"
#include "../debug/mode/txteditor/TxtEditorMode.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GameMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	末端のゲームモード種別を予約されたものも評価して取得する
**//*---------------------------------------------------------------------*/
#if 0
GameMode::ModeKind GameMode::getChildEndModeKindConsiderReserved() const
{
	if(_modekindReserveEnd != 0)
	{
		return _modekindReserveEnd;
	}

	return getChildEndModeKind();
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	末端のモード変更
**//*---------------------------------------------------------------------*/
bool GameMode::changeEndMode(ModeKind modek)
{
	// 自分が担当している範囲のモードの場合は自分で変更して true を返す
	short modeidEnd = changeModeDive(modek);
	if(modeidEnd == 0)
	{
		// モードツリーのトップである GameMode には必ず、
		// changeModeDive は true が返ってくるはずである。
		// false が返ってくるということは、
		// 存在しないモードを指定したということだろう･･･
		return false;
	}

//	_modekindReserveEnd = modek;
	addHistory((ModeKind)modeidEnd);
	return true;
}

/*---------------------------------------------------------------------*//**
	末端のモードを戻す
	※　末端のモードのひとつ親のモードの reservePervMode() を利用するため、
		親を超えては基本的には戻れない．
		戦闘やイベントなどの一時的なモード変更から戻るためのメソッドである
**//*---------------------------------------------------------------------*/
bool GameMode::changePrevEndMode(ModeKind modekCurHint)
{
#if 1
	int itrg = 0;
	for(int i = 0; i < NUM_HISTORY; i++)
	{
		ModeKind modekPrev = MODE_NULL;

		// ヒントが MODE_NULL 指定の場合は無条件で一つ上のモードへ
		if(modekCurHint == GameMode::MODE_NULL)
		{
			itrg = 1;
			break;
		}

		// ヒントと完全に合致するもの
		if(modekCurHint == _arrEndModeHistory[i])
		{
			itrg = i + 1;
			break;
		}

		// 関係性が合致するもの
		const Mode* modeHead = findChildModeDive(_arrEndModeHistory[i]);
		if(modeHead != 0L)
		{
			Mode::Relation rel = modeHead->checkRelation((short)modekCurHint);
			switch(rel)
			{
			case REL_CHILD:
			case REL_DESCENDANT:
			case REL_PARENT:
			case REL_ANCESTOR:
				itrg = i + 1;
				break;
			default:
				i = NUM_HISTORY;
				break;
			}
		}
	}

	// ジャンプ先のモードを求める
	ModeKind modekJump = (itrg < NUM_HISTORY) ? _arrEndModeHistory[itrg] : MODE_NULL;
	for(int i = 0; (i < itrg) && (i < NUM_HISTORY); i++)
	{
		popHistory();
	}
	if(modekJump == MODE_NULL)
	{
		modekJump = MODE_GAME_INGAME;	// デフォルトのモード
	}

	return changeModeDive(modekJump) != 0;

#else
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

#endif
}

/*---------------------------------------------------------------------*//**
	履歴を追加する（changeEndMode を利用しない直接モード変更時）
**//*---------------------------------------------------------------------*/
void GameMode::addHistory(ModeKind modek)
{
	if(_arrEndModeHistory[0] != MODE_NULL)
	{
		// モード巻き戻し時など、同じモードに映るときは履歴追加しない
		if(_arrEndModeHistory[0] == modek)
		{
			return;
		}

		const Mode* modeHead = findChildModeDive(_arrEndModeHistory[0]);
		if(modeHead != 0L)
		{
			Mode::Relation rel = modeHead->checkRelation((short)modek);
			switch(rel)
			{
			case REL_CHILD:
			case REL_DESCENDANT:
				// 追加するモードが先頭モードの子孫である場合は子孫モードの ID に書換え
				_arrEndModeHistory[0] = modek;
				return;
			case REL_PARENT:
			case REL_ANCESTOR:
				// 追加するモードが先頭モードの先祖である場合は何もしないで抜ける
				return;
			}
		}
	}

	pushHistory(modek);
}

/*---------------------------------------------------------------------*//**
	履歴をクリアする
**//*---------------------------------------------------------------------*/
void GameMode::clearHistory()
{
	for(int i = 0; i < NUM_HISTORY; i++)
	{
		_arrEndModeHistory[i] = MODE_NULL;
	}
}

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	モードツリーを表示する
**//*---------------------------------------------------------------------*/
void GameMode::straceModeTree()
{
	straceModeTreeDive(0);
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
GameMode::GameMode()
	: _modeCurChildEnd(0L)
	, _modekindCurChildEnd(MODE_NULL)
//	, _modekindReserveEnd(MODE_NULL)
	, _arrEndModeHistory(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
GameMode::~GameMode()
{
	ASSERT(_arrEndModeHistory == 0L);
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
	registerMode((s16)MODE_GAME_STARTGAME, new StartGameMode());
	registerMode((s16)MODE_GAME_INGAME, new IngameMode());
	registerMode((s16)MODE_GAME_TITLE, new TitleMode());
#if defined(_DEBUG)
	registerMode((s16)MODE_GAME_MODELVW, new ModelViewMode());
	registerMode((s16)MODE_GAME_BOOKVW, new BookViewMode());
	registerMode((s16)MODE_GAME_TXTEDITOR, new TxtEditorMode());
	//registerMode((s16)MODE_GAME_MOVIE, new GameModeBase());
	registerMode((s16)MODE_GAME_TESTVIEWTERM, new TestViewTerminatorMode());
#endif
#if defined(_ENLIGHT_PRPZL01)
	registerMode((s16)MODE_GAME_PUREPUZZLE, new PurePuzzleMode());
#endif

	// 履歴テーブルの作成
	_arrEndModeHistory = new ModeKind[NUM_HISTORY];
	clearHistory();

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void GameMode::destroy()
{
	// モードクリア
	changeChildMode(MODE_NULL);

	// 履歴テーブルの削除
	delete[] _arrEndModeHistory;
	_arrEndModeHistory = 0L;

	Mode::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void GameMode::exec(ExecRes* res, const ExecCtx* ec)
{
	Mode::exec(res, ec);

	// 現在の末端モードを得ておく
	_modeCurChildEnd = getEndModeDive(this);
	_modekindCurChildEnd = (_modeCurChildEnd != 0L) ? (ModeKind)_modeCurChildEnd->getSelfModeId() : MODE_NULL;

	#if defined(_DEBUG) && 0
	{
		straceModeTree();
		for(int i = 0; i < NUM_HISTORY; i++)
		{
			if(_arrEndModeHistory[i] == 0) { break; }
			if(i != 0) { STRACE(" > "); }
			STRACE("%d", _arrEndModeHistory[i]);
			if(i == (NUM_HISTORY - 1)) { STRACE("\n"); }
		}
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	再帰処理で末端のモードを取得する
**//*---------------------------------------------------------------------*/
Mode* GameMode::getEndModeDive(const Mode* mode) const
{
	Mode* modeChild = mode->getCurrentMode();
	if(modeChild != 0L)
	{
		Mode* modeRet = getEndModeDive(modeChild);
		if(modeRet != 0L)
		{
			return modeRet;
		}
	}
	return modeChild;
}

/*---------------------------------------------------------------------*//**
	モード履歴に追加
**//*---------------------------------------------------------------------*/
void GameMode::pushHistory(ModeKind modek)
{
	for(int i = NUM_HISTORY - 2; i >= 0; i--)
	{
		_arrEndModeHistory[i + 1] = _arrEndModeHistory[i];
	}
	_arrEndModeHistory[0] = modek;
}

/*---------------------------------------------------------------------*//**
	モード履歴から取り出す
**//*---------------------------------------------------------------------*/
GameMode::ModeKind GameMode::popHistory()
{
	ModeKind modek = _arrEndModeHistory[0];
	for(int i = 0; i < NUM_HISTORY - 1; i++)
	{
		_arrEndModeHistory[i] = _arrEndModeHistory[i + 1];
	}
	_arrEndModeHistory[NUM_HISTORY - 1] = MODE_NULL;
	return modek;
}

/*---------------------------------------------------------------------*//**
	モード変更通知
**//*---------------------------------------------------------------------*/
void GameMode::notifyChildModeChanged(short modeid, void* objParamRef)
{
//	_modekindReserveEnd = MODE_NULL;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
