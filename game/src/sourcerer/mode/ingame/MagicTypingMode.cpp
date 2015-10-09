/***********************************************************************//**
 *	MagicTypingMode.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MagicTypingMode.h"

// Friends
#include "../../episode/char/pc/MySpirit.h"
#include "../../episode/party/MyParty.h"
#include "../../es/Es.h"
#include "../../body/Game.h"
#include "../../gui/FocusCursor.h"
#include "../../gui/GameGui.h"
#include "../../gui/window/implement/SourceEditWindow.h"
#include "../../gui/window/implement/SourceListWindow.h"
#include "../../magic/MagicSys.h"
#include "../../magic/MagicCluster.h"
#include "../../unit/char/pc/PcUnit.h"
#include "../../source/Source.h"
#include "../../source/SourceManager.h"

// External
#include "../../../etk/sg/SorSgm.h"
#include "../../../tfw/lib/Matrix.h"
#include "../../../tfw/lib/Vector.h"
#include "../../../tfw/g3d/sg/node/SgNodeChain.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MagicTypingMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MagicTypingMode::MagicTypingMode()
	: _smode(SMODE_TYPING)
	, _srckind(0)
	, _isDirectListMode(false)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MagicTypingMode::create()
{
	if(!GameModeBase::create(0, 0, 0L))
	{
		return false;
	}

	// 直接リストから選択し、実行する
	_isDirectListMode = true;

	return true;
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool MagicTypingMode::notifyBegin(void* objParamRef)
{
	// 背景を消す
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	sgm->showBg(false);

	if(_isDirectListMode)
	{
		// ソースリストウインドウを開く
		SourceListWindow* wndSourcelist = Game::getGame()->getGui()->getMagicListWindow(); ASSERT(wndSourcelist != 0L);
		wndSourcelist->setCase(SourceListWindow::SLWCASE_SELECT);
		wndSourcelist->showWindow(true);
		_smode = SMODE_SELLIST;
	}
	else
	{
		// ソース入力ウインドウを開く
		SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow(); ASSERT(wndSrcedt != 0L);
		wndSrcedt->showWindow(true);
		_smode = SMODE_TYPING;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void MagicTypingMode::notifyEnd(void* objParamRef)
{
	// まだソース編集中だったら強制終了
	SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();	ASSERT(wndSrcedt != 0L);
	if(wndSrcedt->isShowWindow())
	{
		wndSrcedt->showWindow(false);
	}
	SourceListWindow* wndMlist = Game::getGame()->getGui()->getMagicListWindow(); ASSERT(wndMlist != 0L);
	if(wndMlist->isShowWindow())
	{
		wndMlist->showWindow(false);
	}

	// 背景表示を戻す
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	sgm->showBg(true);
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void MagicTypingMode::exec(ExecRes* res, const ExecCtx* ec)
{
	GameModeBase::exec(res, ec);

	switch(_smode)
	{
	case SMODE_TYPING:
		{
			SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();	ASSERT(wndSrcedt != 0L);
			if(wndSrcedt->isDone())	// 完了検知
			{
				// 入力完了ならばマ法を放つ
				if(wndSrcedt->isInputDone())
				{
					if(Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY) != 0L)
					{
						// 自キャラを得る
						Spirit* sprt = Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY)->spirit();
						ASSERT(sprt != 0L);

						// ソースコードを得る
						VcString code;
						wndSrcedt->takeSourceText(&code);

						// ソウマ実行
						if(	(code.getLength() > 0) &&
							(_srckind != SourceDef::SRCKIND_NULL)	)
						{
							Spirit::SoumaBehaviorParam sbp(
								&code,						// ソースコードを設定する
								_srckind,					// 最終種別を適用
								false	);					// 現時点では不明
							sprt->startBehavior(Spirit::BEHV_SOUMA, &sbp);
						}

						// ソースエディトウインドウのソース文字列から不要なスペースや改行を削除する
						wndSrcedt->trimSourceText();

						// フォーカスカーサーの操作をしばらく禁止する
						FocusCursor* fccsr = Game::getGame()->getGui()->getFocusCursor(); ASSERT(fccsr != 0L);
						fccsr->setPreventTapCounter(30.0f);	// 30 フレーム待て
					}

					// 以前のモードへ戻る
					changePrevMode();
				}
				// リスト入力による完了ならばマ法リストウインドウを開く
				else if(wndSrcedt->isListClickDone())
				{
					// マ法リストウインドウを開く
					SourceListWindow* wndSourcelist = Game::getGame()->getGui()->getMagicListWindow(); ASSERT(wndSourcelist != 0L);
					wndSourcelist->setCase(SourceListWindow::SLWCASE_SELECT);
					wndSourcelist->showWindow(true);
					_smode = SMODE_SELLIST;
				}
				// キャンセル
				else
				{
					// 以前のモードへ戻る
					changePrevMode();
				}

				// マ法入力ウインドウを閉じる
				SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();	ASSERT(wndSrcedt != 0L);
				wndSrcedt->showWindow(false);
			}
		}
		break;
	case SMODE_SELLIST:
		{
			SourceListWindow* wndMlist = Game::getGame()->getGui()->getMagicListWindow();	ASSERT(wndMlist != 0L);
			if(wndMlist->isDone())	// 完了検知
			{
				// 選択したマ法をソースエディタにセット
				const Source* source = wndMlist->getSelectedSource();
				SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();	ASSERT(wndSrcedt != 0L);
				if(source != 0L)	// リストを選択した
				{
					// 種別を更新
					_srckind = source->getSourceKind();

					// 関数呼び出し補完文字列
					VcString strFuncCall;
					switch(_srckind)
					{
					case SourceDef::SRCKIND_JS:	strFuncCall.set("();");		break;
					case SourceDef::SRCKIND_SS:	strFuncCall.set(" a o.");	break;
					}

					// ソース入力ウインドウに設定する
					VcString strSource(*source->getName() + strFuncCall);
					wndSrcedt->setup(SourceEditWindow::ACTMODE_LIVE, &strSource);

					if(_isDirectListMode && (Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY) != 0L))
					{
						// 自キャラを得る
						Spirit* sprt = Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY)->spirit();
						ASSERT(sprt != 0L);

						// ソースコードを得る
						VcString code;
						wndSrcedt->takeSourceText(&code);

						// ソウマ実行
						Spirit::SoumaBehaviorParam sbp(
							&code,
							_srckind,
							source->isUserCustomized()	);
						sprt->startBehavior(Spirit::BEHV_SOUMA, &sbp);

						// フォーカスカーサーの操作をしばらく禁止する
						FocusCursor* fccsr = Game::getGame()->getGui()->getFocusCursor(); ASSERT(fccsr != 0L);
						fccsr->setPreventTapCounter(30.0f);	// 30 フレーム待て
					}
				}

				if(_isDirectListMode)
				{
					// 以前のモードへ戻る
					changePrevMode();
				}
				else
				{
					// ソース入力ウインドウを開く
					wndSrcedt->showWindow(true);
					_smode = SMODE_TYPING;
				}

				// ソースリストウインドウを閉じる
				wndMlist->showWindow(false);
			}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	以前のモードに戻る
**//*---------------------------------------------------------------------*/
void MagicTypingMode::changePrevMode()
{
#if 0
	Mode* modeParent = getParentMode();
	s16 modePrev = modeParent->getPrevMode();
	if(modePrev == GameMode::MODE_GAME_INGAME_WALKING_MAGICTYPING)
	{
		modePrev = GameMode::MODE_GAME_INGAME_WALKING_NORMAL;	// 救済策
	}
	modeParent->reserveMode(modePrev);	// モードを戻す
#else
	Game::getGame()->getGameMode()->changePrevEndMode(GameMode::MODE_NULL);
#endif
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
