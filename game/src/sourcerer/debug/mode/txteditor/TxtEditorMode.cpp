/***********************************************************************//**
 *	TxtEditorMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/11/26.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TxtEditorMode.h"

// Friends
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameRenderCtx.h"
#include "../../../gui/GameFontSet.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/SourceEditWindow.h"

// External
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/gui/txtbox/TxtBox.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define TEST_DIRECT_TB	(0)	// テキストエディタを直接テストする

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TxtEditorMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	カスタムイベント呼び出しハンドラ
**//*---------------------------------------------------------------------*/
void TxtEditorMode::callCustomEvent(void* objRes, const VcString* strParam, const void* objParam)
{
	if(strParam->equals("Test::changeTestViewMode"))
	{
		///this->getParentMode()->reserveMode(GameMode::MODE_GAME_MODELVW);	// 次のデバッグモードへ
		///this->getParentMode()->reserveNextMode();	// 次のデバッグモードへ
		Game::getGame()->getGameMode()->changeEndMode((GameMode::ModeKind)getParentMode()->getNextMode());	// 次のデバッグモードへ
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TxtEditorMode::TxtEditorMode()
	: _txtbox(0L)
	, _dispCnt(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TxtEditorMode::~TxtEditorMode()
{
	ASSERT(_txtbox == 0L);
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool TxtEditorMode::notifyBegin(void* objParamRef)
{
	// テクスチャ、フォントを得る
	GameGui* gui = Game::getGame()->getGui();

	// テスト用テキストを読み込む
	VcString strTxt;
	#if defined(_WINDOWS)
	File file(CcString("test_txt.txt"), File::MODE_READ);
	if(file.isOpened())
	{
		file.readString(&strTxt, file.getLength());
	}
	#endif

	#if TEST_DIRECT_TB
		// テキストエディタを作成
		Texture* texGui = gui->getGuiTexture();
		Font* fontJp = gui->getFontSet()->getFont(GameFontSet::JP);
		TxtBox* txteditor = new TxtBox();
		RectF32 rect(0, 0, Game::getGame()->getLogicalWidth(), Game::getGame()->getLogicalHeight());
		if(!txteditor->create(fontJp, 10, texGui, &rect, TxtBox::MODE_SRCEDIT))
		{
			delete txteditor;
			ASSERT(false);
			return false;
		}
		_txtbox = txteditor;
	#else
		// ソースエディタを表示
		SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();
		wndSrcedt->showWindow(true);
		wndSrcedt->setup(SourceEditWindow::ACTMODE_EDIT, &strTxt);
	#endif

	// 値の初期化
	_dispCnt = 0;

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void TxtEditorMode::notifyEnd(void* objParamRef)
{
	#if !TEST_DIRECT_TB
		// ソースエディタを非表示に
		SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();
		wndSrcedt->showWindow(false);
	#endif

	// テキストエディタを削除
	if(_txtbox != 0L)
	{
		_txtbox->destroy();
		delete _txtbox;
		_txtbox = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void TxtEditorMode::exec(ExecRes* res, const ExecCtx* ec)
{
	GameModeBase::exec(res, ec);

	// テキストエディタの制御
	if(_txtbox != 0L)
	{
		_txtbox->exec(res, ec);
	}

	// モードをスクリーントレース
	if(_dispCnt < 60)
	{
		VcString stmsg = ": TXTEDITOR :\n";
		STRACE(stmsg.getRaw());
	}
	_dispCnt++;
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void TxtEditorMode::render(const RenderCtx* rc)
{
	GameModeBase::render(rc);
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void TxtEditorMode::draw(const RenderCtx* rc)
{
	GameModeBase::draw(rc);

	// テキストエディタの描画
	if(_txtbox != 0L)
	{
		_txtbox->draw(rc);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
