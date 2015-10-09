/***********************************************************************//**
 *	SourceEditWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SourceEditWindow.h"

// Friends
#include "SoftKeyboardWindow.h"
#include "SourceListWindow.h"
#include "../../menu/GameMenuCreateParam.h"
#include "../../menu/GameMenuShowParam.h"
#include "../../menu/GameMenuPanelFactory.h"
#include "../../panel/HeaderPanel.h"
#include "../../panel/ScrollBar.h"
#include "../../window/menu/MenuWindow.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../source/Source.h"

// External
#include "../../../../etk/body/OsDepMod.h"
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/g2d/StringDrawer.h"
#include "../../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../../tfw/gcmn/Texture.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gui/PanelText.h"
#include "../../../../tfw/gui/txtbox/TxtBox.h"
#include "../../../../tfw/gui/txtbox/TxtBoxExecRes.h"
#include "../../../../tfw/smenu/Menu.h"
#include "../../../../tfw/smenu/MenuTreeNode.h"
#include "../../../../tfw/string/StringUtils.h"
#include "../../../../tfw/txt/TxtUtils.h"
#if TEST_UTF8	// UTF-8 test
#include "../../../../tfw/txt/implement/Utf8Txt.h"
#else
#include "../../../../tfw/txt/implement/SjisTxt.h"
#endif
#include "../../../../tfw/ui/Keyboard.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 表示位置基本グリッド
#define W_BTN_PAD					(4)
#define W_WIDE_ADD					((Game::getGame()->getLogicalWidth() - Game::getGame()->getStandardWidth()) / 12)
#define W_BTN_S1					(36 + W_WIDE_ADD)
#define W_WHOLE_PAD					((Game::getGame()->getLogicalWidth() - (W_BTN_S1 * 12) - (W_BTN_PAD * 11)) / 2)

// 表示位置
#define H_HEADER_BAR				48
#define W_SB						32
#define W_EXT						(W_BTN_PAD + (W_BTN_S1 * 3) + (W_BTN_PAD * 2) + W_WHOLE_PAD)
#define H_TXTE						120
#define RECT_TXTE_NORMAL_SKBD		0, H_HEADER_BAR, Game::getGame()->getLogicalWidth() - W_SB - 2,         H_TXTE
#define RECT_TXTE_FULL_SKBD			0, H_HEADER_BAR, Game::getGame()->getLogicalWidth() - W_EXT - W_SB - 2, H_TXTE
#define RECT_TXTE_SMALL_SKBD		0, H_HEADER_BAR, Game::getGame()->getLogicalWidth() - W_SB - 2,         H_TXTE + 32
#define RECT_TXTE_OS				0, H_HEADER_BAR, Game::getGame()->getLogicalWidth(), Game::getGame()->getLogicalHeight() - H_HEADER_BAR
#define RECT_SB_NORMAL_SKBD			Game::getGame()->getLogicalWidth() - W_SB,         H_HEADER_BAR, W_SB, H_TXTE
#define RECT_SB_FULL_SKBD			Game::getGame()->getLogicalWidth() - W_EXT - W_SB, H_HEADER_BAR, W_SB, H_TXTE
#define RECT_SB_SMALL_SKBD			Game::getGame()->getLogicalWidth() - W_SB,         H_HEADER_BAR, W_SB, H_TXTE + 32
#define X_CSPOS						100
#define Y_CSPOS						13
#define X_CNUM						100
#define Y_CNUM						25

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SourceEditWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	ソースを変更したかどうかを得る
**//*---------------------------------------------------------------------*/
bool SourceEditWindow::isChanged() const
{
	if(_txtbox == 0L)	{	return 0L;	}
	return _txtbox->isChanged();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ソーステキストを得る
**//*---------------------------------------------------------------------*/
void SourceEditWindow::takeSourceText(VcString* src) const
{
	if(_txtbox == 0L)	{	return;	}
	_txtbox->getText()->takeRawString(src);
}

/*---------------------------------------------------------------------*//**
	セットアップ
**//*---------------------------------------------------------------------*/
bool SourceEditWindow::setup(ActMode actmode, const VcString* src)
{
	// テキストボックスにテキスト設定
	if(_txtbox == 0L)	{	return false;	}
	Txt* txt = _txtbox->getText()->newSameType();
	txt->setSingleString(src);
	_txtbox->setText(txt, false);
	_txtbox->resetChangedFlag();
	// カーソルを行末へ
	TxtBoxExecRes res;
	_txtbox->execKeyboardKey(&res, Keyboard::SYSKEY_END, false, false, false);
	delete txt;

	// 動作モード設定
	_actmode = actmode;

	return true;
}

/*---------------------------------------------------------------------*//**
	ソーステキストから不要なスペースや改行を削除する
**//*---------------------------------------------------------------------*/
void SourceEditWindow::trimSourceText()
{
	if(!_txtbox->trim())	{	return;	}

	// カーソルを行末へ
	TxtBoxExecRes res;
	_txtbox->execKeyboardKey(&res, Keyboard::SYSKEY_END, false, false, false);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SourceEditWindow::SourceEditWindow()
	: _texRef(0L)
	, _hpnlHbar(0L)
	, _wndSoftkb(0L)
	, _txtbox(0L)
	, _scrlbar(0L)
	, _menuSub(0L)
	, _msprmSub(0L)
	, _isDone(false)
	, _isInputDone(false)
	, _skfmmodeLast(SoftKeyboardWindow::FMMODE_NORMAL)
	, _isListClickDone(false)
	, _actmode(ACTMODE_NULL)
	, _isLastSentReturnKey(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SourceEditWindow::~SourceEditWindow()
{
	ASSERT(_hpnlHbar == 0L);
	ASSERT(_wndSoftkb == 0L);
	ASSERT(_txtbox == 0L);
	ASSERT(_scrlbar == 0L);
	ASSERT(_menuSub == 0L);
	ASSERT(_msprmSub == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SourceEditWindow::create(Font* fontRef, Texture* texRef)
{
	// ウインドウの作成
	RectF32 rect(0, 0, Game::getGame()->getLogicalWidth(), Game::getGame()->getLogicalHeight());
	if(!Window::create(&rect))
	{
		return false;
	}

	HeaderPanel* hpnlHbar = 0L;
	SoftKeyboardWindow* wndSoftkb = 0L;
	Txt* txt = 0L;
	TxtBox* txtedit = 0L;
	ScrollBar* scrlbar = 0L;
	const RectF32* rectTb = 0L;

	// ヘッダーパネル作成
	hpnlHbar = new HeaderPanel();
	if(!hpnlHbar->create(
		fontRef,
		texRef,
		GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SOURCE_EDIT),
		GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_BACK),
		GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SOURCEBOOK),
		GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU),
		HeaderPanel::HPF_UCASE_SOURCEEDIT	))
	{
		goto FAILED;
	}
	_hpnlHbar = hpnlHbar;

	// ソフトキーボードウインドウの作成
	wndSoftkb = new SoftKeyboardWindow();
	if(!wndSoftkb->create(texRef))
	{
		goto FAILED;
	}
	_wndSoftkb = wndSoftkb;

	// テキスト作成
	#if TEST_UTF8	// UTF-8 test
		txt = new Utf8Txt();
	#else
		txt = new SjisTxt();
	#endif
	///Txt* txt = new AsciiTxt();
	if(txt == 0L)
	{
		goto FAILED;
	}

	// テキストエディタの作成
	txtedit = new TxtBox();
	if(!txtedit->create(txt, fontRef, 16, texRef, RectF32::setout(&rect, RECT_TXTE_NORMAL_SKBD), TxtBox::MODE_SRCEDIT, TxtBox::CF_FIXED_WIDTH_FONT))
	{
		goto FAILED;
	}
	_txtbox = txtedit;
	_txtbox->setTracking(-1, 0);
	_txtbox->setPadding(8, 8);

	// スクロールバー作成
	scrlbar = new ScrollBar();
	if(!scrlbar->create(RectF32::setout(&rect, RECT_SB_NORMAL_SKBD), texRef))
	{
		goto FAILED;
	}
	_scrlbar = scrlbar;
	_scrlbar->setIgnoreHide(true);		// 不要時も隠さない

	// スクロールバーのページサイズを設定
	rectTb = _txtbox->getAvailableRectangle();
	_scrlbar->setScrollPage(rectTb->h());

	_texRef = texRef;			// テクスチャ参照を保存
	_isDone = false;			// 終了フラグをクリア
	_isInputDone = false;		// 入力フラグをクリア
	_isListClickDone = false;	// リストクリックフラグをクリア
	return true;

FAILED:
	ASSERT(false);
	delete hpnlHbar;
	delete wndSoftkb;
	delete txt;
	delete txtedit;
	delete scrlbar;
	return false;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SourceEditWindow::destroy()
{
	// サブメニュー削除
	destroySubMenu();

	// スクロールバーの削除
	if(_scrlbar != 0L)
	{
		_scrlbar->destroy();
		delete _scrlbar;
		_scrlbar = 0L;
	}

	// テキストエディタの削除
	if(_txtbox != 0L)
	{
		_txtbox->destroy();
		delete _txtbox;
		_txtbox = 0L;
	}

	// ソフトキーボードウインドウの削除
	if(_wndSoftkb != 0L)
	{
		_wndSoftkb->destroy();
		delete _wndSoftkb;
		_wndSoftkb = 0L;
	}

	// ヘッダーパネル削除
	if(_hpnlHbar != 0L)
	{
		_hpnlHbar->destroy();
		delete _hpnlHbar;
		_hpnlHbar = 0L;
	}

	Window::destroy();
}

/*---------------------------------------------------------------------*//**
	内容のフレーム制御
**//*---------------------------------------------------------------------*/
void SourceEditWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* ui = gec->getTouchPanel(); 

	// サブメニュー制御
	if((_menuSub != 0L) && _menuSub->isShow())
	{
		ExecRes resMenu;
		_menuSub->exec(&resMenu, ec);
		return;
	}

	// ヘッダーパネル制御
	if(_hpnlHbar != 0L)
	{
		_hpnlHbar->exec(res, ec);
		const HeaderPanel::Res* cres = _hpnlHbar->getResult();
		if(cres->isClickLeftButton())
		{
			// キャンセルで終了
			if(res != 0L)	{	res->setDone();	}
			_isDone = true;
		}
		else if(cres->isClickRightButton())
		{
			// リストクリックで終了
			_isListClickDone = true;
			if(res != 0L)	{	res->setDone();	}
			_isDone = true;
		}
		else if(cres->isClickFuncButton())
		{
			// サブメニューがある場合は表示
			if((_menuSub != 0L) && !_menuSub->isShow())	{	_menuSub->showMenu(_msprmSub);	}
			ui->setReacted(true);	// 反応済み
		}
		_hpnlHbar->resetResult();
	}

	// スクロールバーの制御
	if((_scrlbar != 0L) && !ui->isReacted())
	{
		ScrollBar::ScrollBarExecRes sbres;
		_scrlbar->exec(&sbres, ui);
		if(sbres.getAction() != ScrollBar::ScrollBarExecRes::ACT_NULL)
		{
			if((sbres.getAction() == ScrollBar::ScrollBarExecRes::ACT_FLIP_UP) || (sbres.getAction() == ScrollBar::ScrollBarExecRes::ACT_FLIP_DOWN))
			{
				//_velFlip += sbres._value * 100;	// フリップ速度を足す
			}
			else
			{
				f32 yPos = _txtbox->getScrollPosition()->y();
				switch(sbres.getAction())
				{
				case ScrollBar::ScrollBarExecRes::ACT_CLICK_UP_ARROW:		yPos -= 10;							break;
				case ScrollBar::ScrollBarExecRes::ACT_CLICK_DOWN_ARROW:		yPos += 10;							break;
				case ScrollBar::ScrollBarExecRes::ACT_CLICK_PAGE_UP:		yPos -= _scrlbar->getScrollPage();	break;
				case ScrollBar::ScrollBarExecRes::ACT_CLICK_PAGE_DOWN:		yPos += _scrlbar->getScrollPage();	break;
				case ScrollBar::ScrollBarExecRes::ACT_DRAG_KNOB:			yPos = sbres.getValue();			break;
				}
				if(yPos < _scrlbar->getScrollMin())	{	yPos = _scrlbar->getScrollMin();	}
				if(yPos > _scrlbar->getScrollMax())	{	yPos = _scrlbar->getScrollMax();	}
				if(yPos != _txtbox->getScrollPosition()->y())
				{
					PointF32 pt(_txtbox->getScrollPosition()->x(), yPos);
					_txtbox->setScrollPosition(&pt, false);
				}
				///_velFlip /= 4;	// フリップ速度の急減速
			}
		}
	}
	{
		// テキストエディタのスクロール位置を反映
		const PointF32*	posScrollMax = _txtbox->getMaxScrollPosition();
		if(posScrollMax->y() != _scrlbar->getScrollMax())
		{
			_scrlbar->setScrollMinMax(0, posScrollMax->y());
		}
		const PointF32*	posScrollCur = _txtbox->getScrollPosition();
		if(posScrollCur->y() != _scrlbar->getScrollPos())
		{
			_scrlbar->setScrollPos(posScrollCur->y());
		}
	}

	// ソフトウェアキーボードの操作
	if((_wndSoftkb != 0L) && !ui->isReacted())
	{
		ExecRes resChild;
		_wndSoftkb->exec(&resChild, ec);
		if(resChild.isDone())	// DONE
		{
			// 終了フラグを立てる
			if(res != 0L)	{	res->setDone();	}
			_isDone = true;
			ui->setReacted(true);
		}
		else
		{
			if(execKeyboard(res, _wndSoftkb->getKeyboard()))	// テキストボックスのキーボードによる制御
			{
				ui->setReacted(true);
			}
		}

		// 拡張パネル対応
		SoftKeyboardWindow::FormMode skfmmode = _wndSoftkb->getFormMode();
		if(_skfmmodeLast != skfmmode)
		{
			// 形態に変更があった
			RectF32 rect;
			switch(skfmmode)
			{
			case SoftKeyboardWindow::FMMODE_NORMAL:
				_txtbox->setRectangle(RectF32::setout(&rect, RECT_TXTE_NORMAL_SKBD));
				_scrlbar->setRectangle(RectF32::setout(&rect, RECT_SB_NORMAL_SKBD));
				break;
			case SoftKeyboardWindow::FMMODE_FULL:
				_txtbox->setRectangle(RectF32::setout(&rect, RECT_TXTE_FULL_SKBD));
				_scrlbar->setRectangle(RectF32::setout(&rect, RECT_SB_FULL_SKBD));
				break;
			case SoftKeyboardWindow::FMMODE_SMALL:
				_txtbox->setRectangle(RectF32::setout(&rect, RECT_TXTE_SMALL_SKBD));
				_scrlbar->setRectangle(RectF32::setout(&rect, RECT_SB_SMALL_SKBD));
				break;
#if ENABLE_OSTXTFLD
			case SoftKeyboardWindow::FMMODE_OSTXTFLD:
				showOsTextField();
				break;
#endif
			}
#if ENABLE_OSTXTFLD
			if(_skfmmodeLast == SoftKeyboardWindow::FMMODE_OSTXTFLD)
			{
				hideOsTextField();
			}
#endif
			_skfmmodeLast = skfmmode;
		}
	}

	// ハードウェアキーボードの操作
	execKeyboard(res, ((GameExecCtx*)ec)->getKeyboard());

	// タッチパネルの操作
	if(!ui->isReacted())
	{
		TxtBoxExecRes res;
		if(_txtbox->execTouchPanelEvent(&res, ((GameExecCtx*)ec)->getTouchPanel()))
		{
			ui->setReacted(true);
		}
	}

	// テキストエディタの制御
	if(_txtbox != 0L)
	{
		_txtbox->exec(res, ec);
	}
}

/*---------------------------------------------------------------------*//**
	キーボード フレーム制御
**//*---------------------------------------------------------------------*/
bool SourceEditWindow::execKeyboard(ExecRes* res, const Keyboard* kbd)
{
	TxtBoxExecRes tbres;
	if(_txtbox->execKeyboardEvent(&tbres, kbd))
	{
		// コマンドの実行
		switch(tbres.getCommand())
		{
		case TxtBox::CMD_COPY:
			copyText();
			break;
		case TxtBox::CMD_PASTE:
			pasteText();
			break;
		}

		///廃止予定［2011/12/22 r-kishi］
		// ライブ実行時は２回改行で入力完了
		bool isSentReturnKey = kbd->isRepeatTrigger('\n');
		if(_actmode == ACTMODE_LIVE)
		{
			if(_isLastSentReturnKey && isSentReturnKey)
			{
				if(res != 0L)	{	res->setDone();	}
				_isDone = true;
				_isInputDone = true;	// 入力完了
			}
		}
		_isLastSentReturnKey = isSentReturnKey;
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void SourceEditWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	ColorU8 col;
	VcString str;

	Renderer* rdr = rc->getRenderer();

	// ヘッダーパネル描画
	if(_hpnlHbar != 0L)	{	_hpnlHbar->draw(rc);	}

	// テキストの地
	if((_txtbox != 0L) && _txtbox->isEnable())
	{
		const RectF32* rectTxtbox = _txtbox->getRectangle();
		RendererUtils::draw2dColorRect(rdr, rectTxtbox, ColorU8::setout(&col, 255, 255, 255, 255));
		rdr->setSolidColor(63, 63, 63, 255);
		RendererUtils::draw2dRectLine(rdr, rectTxtbox);
	}

	// カーソル位置を描画
	rdr->setSolidColor(255, 255, 255, 255);
	EasyStringDrawer::draw(_txtbox->getFont(), VcString::format(&str, "%d", _txtbox->getCursorLine() + 1), X_CSPOS, Y_CSPOS, 12, rc);
	EasyStringDrawer::draw(_txtbox->getFont(), VcString::format(&str, ", %d", _txtbox->getCursorColumn() + 1), X_CSPOS + 20, Y_CSPOS, 12, rc);

	// 文字数を描画
	s32 lenTxt = _txtbox->getText()->getRawCount();
	EasyStringDrawer::draw(_txtbox->getFont(), VcString::format(&str, "%d", lenTxt), X_CNUM, Y_CNUM, 12, rc);
	EasyStringDrawer::draw(_txtbox->getFont(), VcString::format(&str, "/%d", Source::MAX_SOURCE_SIZE_REGULATION), X_CNUM + 20, Y_CNUM + 2, 10, rc);

	// テキストの描画
	if(_txtbox != 0L)		{	_txtbox->draw(rc);		}

	// ソフトウェアキーボードの描画
	if(_wndSoftkb != 0L)	{	_wndSoftkb->draw(rc);	}

	// スクロールバーの描画
	_scrlbar->draw(rc);

	// サブメニュー描画
	if(_menuSub != 0L)		{	_menuSub->draw(rc);		}
}

/*---------------------------------------------------------------------*//**
	言語変更通知
**//*---------------------------------------------------------------------*/
void SourceEditWindow::notifyChangeLanguage()
{
	Window::notifyChangeLanguage();

	if(_hpnlHbar != 0L)
	{
		_hpnlHbar->changeGuiString(
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SOURCE_EDIT),
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_BACK),
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SOURCEBOOK),
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU));
	}
}

/*---------------------------------------------------------------------*//**
	テキスト コピー
**//*---------------------------------------------------------------------*/
void SourceEditWindow::copyText()
{
	Txt* txt = 0L;
	if(!_txtbox->copyText(&txt))
	{
		ASSERT(false);
		return;
	}
	Game::getGame()->getOsDepMod()->setCopyTextToClipboard(txt);
	delete txt;
}

/*---------------------------------------------------------------------*//**
	ペースト コピー
**//*---------------------------------------------------------------------*/
void SourceEditWindow::pasteText()
{
	Txt* txt = 0L;
	if(Game::getGame()->getOsDepMod()->getPasteTextFromClipboard(&txt))
	{
		#if TEST_UTF8	// UTF-8 test
			Utf8Txt utxt;
			TxtUtils::convSjisToUtf8Text(&utxt, (SjisTxt*)txt);
			_txtbox->pasteText(&utxt);
		#else
			_txtbox->pasteText(txt);
		#endif
	}
	delete txt;
}

/*---------------------------------------------------------------------*//**
	ペースト コピー
**//*---------------------------------------------------------------------*/
void SourceEditWindow::showOsTextField()
{
	_txtbox->setEnable(false);
	_scrlbar->setEnable(false);
	
	Txt* txt = 0L;
	if(!_txtbox->copyText(&txt))
	{
		ASSERT(false);
		return;
	}

	RectF32 rect(RECT_TXTE_OS);
	Game::getGame()->getOsDepMod()->createTextField(txt, &rect, Game::getGame()->getTfw()->getView(), 0);

	delete txt;
}

/*---------------------------------------------------------------------*//**
	ペースト コピー
**//*---------------------------------------------------------------------*/
void SourceEditWindow::hideOsTextField()
{
	Txt* txt = 0L;
	if(Game::getGame()->getOsDepMod()->getTextFieldString(OSCMPID_SOURCE_TEXT_FIELD, &txt))
	{
		_txtbox->pasteText(txt);
	}
	delete txt;

	Game::getGame()->getOsDepMod()->destroyTextField(OSCMPID_SOURCE_TEXT_FIELD);

	_txtbox->setEnable(true);
	_scrlbar->setEnable(true);
}

/*---------------------------------------------------------------------*//**
	サブメニュー作成
**//*---------------------------------------------------------------------*/
bool SourceEditWindow::createSubMenu()
{
	GameMenuCreateParam* mcprmSub = 0L;
	GameMenuShowParam* msprmSub = 0L;
	MenuFuncTable* mftbl = 0L;
	MenuPanelFactory* pfact = 0L;
	Menu* menuSub = 0L;

	do
	{
		// 作成パラメータ
		mcprmSub = new GameMenuCreateParam();
		if(mcprmSub == 0L)								{ break; }
		mcprmSub->_case = GameMenuCreateParam::CASE_INGAME_SUB;
		mcprmSub->_owner = this;

		// 表示パラメータ
		msprmSub = new GameMenuShowParam();
		if(msprmSub == 0L)								{ break; }
		msprmSub->_xBase = Game::getGame()->getLogicalWidth() - 144;
		msprmSub->_yBase = 4 + 16;
		msprmSub->_wMax = Game::getGame()->getLogicalWidth();
		msprmSub->_hMax = Game::getGame()->getLogicalHeight() - _hpnlHbar->getRectangle()->width();
		msprmSub->_ownerRef = this;

		// 機能テーブル
		mftbl = new MenuFuncTable_SourceEdit();
		if(mftbl == 0L)									{ break; }

		// メニューパネルファクトリ
		pfact = new GameMenuPanelFactory();
		if(pfact == 0L)									{ break; }

		// メニュー作成
		menuSub = new Menu();
		u32 fileidSubMenu = FILEID_CONV_LANG(MENU_MENU_IM_SOURCEEDITSUB_EN_XML);	// サブメニューのファイル ID
		if(!menuSub->create(fileidSubMenu, mftbl, pfact, mcprmSub))
		{
			break;
		}
		_menuSub = menuSub;
		_msprmSub = msprmSub;

		#if !defined(_DEBUG)
		{
			const s32 MTNODEID_CONV_TO_JS = 2201;
			const s32 MTNODEID_CONV_TO_SS = 2202;
			MenuTreeNode* mtnode = 0L;
			// デバッグ用途の項目を無効化
			mtnode = _menuSub->findMenuTreeNode(MTNODEID_CONV_TO_JS);
			if(mtnode != 0L)	{ mtnode->setHidden(true); }
			mtnode = _menuSub->findMenuTreeNode(MTNODEID_CONV_TO_SS);
			if(mtnode != 0L)	{ mtnode->setHidden(true); }
		}
		#endif

		return true;
	}
	while(false);

	delete mcprmSub;
	delete msprmSub;
	delete mftbl;
	delete pfact;
	delete menuSub;
	return false;
}

/*---------------------------------------------------------------------*//**
	サブメニュー破棄
**//*---------------------------------------------------------------------*/
void SourceEditWindow::destroySubMenu()
{
	if(_menuSub != 0L)
	{
		MenuFuncTable* mftbl = _menuSub->getFuncTable();
		MenuPanelFactory* pfact = _menuSub->getPanelFactory();
		GameMenuCreateParam* mcprm = (GameMenuCreateParam*)_menuSub->getCreateParam();

		_menuSub->destroy();
		delete _menuSub;
		_menuSub = 0L;
		delete _msprmSub;
		_msprmSub = 0L;
		delete mcprm;
		delete pfact;
		delete mftbl;
	}
}

/*---------------------------------------------------------------------*//**
	表示状態の変更通知
**//*---------------------------------------------------------------------*/
void SourceEditWindow::onShowWindow(bool isShow)
{
	Window::onShowWindow(isShow);

	if(isShow)
	{
		// サブメニュー作成
		createSubMenu();
	}
	else
	{
		// サブメニューが出ている場合は強制的に消す
		if((_menuSub != 0L) && _menuSub->isShow())
		{
			_menuSub->closeMenu();
		}

		// サブメニュー破棄
		destroySubMenu();
	}

	// ソフトウェアキーボードの表示
	if(_wndSoftkb != 0L)
	{
		_wndSoftkb->showWindow(isShow);
		_wndSoftkb->setReadOnlyMode(_actmode == ACTMODE_LOOK);
	}

	// モードによるソースブックボタンの有効性更新
	_hpnlHbar->rightButtonPanel()->setEnable(_actmode == ACTMODE_LIVE);

	// テキストボックスの変更フラグをリセット
	_txtbox->resetChangedFlag();
	_txtbox->setReadOnly(_actmode == ACTMODE_LOOK);

	// 値をクリア
	_isDone = false;				// 終了フラグをクリア
	_isInputDone = false;			// 入力フラグをクリア
	_isListClickDone = false;		// リストクリックフラグをクリア
	_isLastSentReturnKey = false;
	if(_hpnlHbar != 0L)	{	_hpnlHbar->resetResult();	}
}

/*---------------------------------------------------------------------*//**
	サブ メニュー ノード クリック通知
**//*---------------------------------------------------------------------*/
bool SourceEditWindow::onClickSubMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	const VcString* nameNode = mtnodeCall->getName(); ASSERT(nameNode != 0L);
	if(nameNode->equals("Copy"))			// コピー
	{
		copyText();
		return true;
	}
	if(nameNode->equals("Paste"))			// ペースト
	{
		pasteText();
		return true;
	}
	if(nameNode->equals("ConvToJs"))		// JS に変換
	{
		VcString code;
		_txtbox->getText()->takeRawString(&code);
		Source::convToJsCode(&code);
		_txtbox->text()->setSingleString(&code);
		_txtbox->setText(_txtbox->text(), true);
		return true;
	}
	if(nameNode->equals("ConvToSs"))		// SS に変換
	{
		VcString code;
		_txtbox->getText()->takeRawString(&code);
		Source::convToSsCode(&code);
		_txtbox->text()->setSingleString(&code);
		_txtbox->setText(_txtbox->text(), true);
		return true;
	}
	return false;
}


//==========================================================================
// SourceEditWindow::MenuFuncTable_SourceEdit メソッド

/*---------------------------------------------------------------------*//**
	表示通知
**//*---------------------------------------------------------------------*/
void SourceEditWindow::MenuFuncTable_SourceEdit::onShow(Menu* menu, MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam)
{
	GameMenuShowParam* sprm = (GameMenuShowParam*)objShowParam;	ASSERT(sprm != 0L);
	SourceEditWindow* own = (SourceEditWindow*)sprm->_ownerRef;	ASSERT(own != 0L);
	MenuWindow* mwnd = (MenuWindow*)mtnodeShow->getPanel();
	if(mwnd == 0L)		{	return;	}
	MenuTreeNode* mtnodeDisp = menu->getMenuTreeNodeChild(mtnodeShow);
	while(mtnodeDisp != 0L)
	{
		MenuTreeNode* mtnodeFunc = menu->getMenuTreeNodeChild(mtnodeDisp);
		if(mtnodeFunc != 0L)
		{
			s32 idx = mwnd->getElementContentIndexByNode(mtnodeDisp) + 1;	// + 1 はタイトル要素分
			const VcString* name = mtnodeFunc->getName();
			if((idx > 0) && (name != 0L) && (name->getLength() > 0))
			{
				if(name->equals("Paste"))			// 貼り付け
				{
					mwnd->setElementUnused(idx, (own->getActMode() != ACTMODE_EDIT));
				}
				else if(name->equals("ConvToJs"))	// JS に変換
				{
					mwnd->setElementUnused(idx, (own->getActMode() != ACTMODE_EDIT));
				}
				else if(name->equals("ConvToSs"))	// SS に変換
				{
					mwnd->setElementUnused(idx, (own->getActMode() != ACTMODE_EDIT));
				}
			}
		}

		mtnodeDisp = menu->getMenuTreeNodeSibling(mtnodeDisp);
	}
}

/*---------------------------------------------------------------------*//**
	非表示通知
**//*---------------------------------------------------------------------*/
void SourceEditWindow::MenuFuncTable_SourceEdit::onHide(Menu* menu, void* objCreateParam, void* objShowParam)
{
}

/*---------------------------------------------------------------------*//**
	呼び出し通知
**//*---------------------------------------------------------------------*/
bool SourceEditWindow::MenuFuncTable_SourceEdit::onCall(Menu* menu, const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	GameMenuShowParam* sprm = (GameMenuShowParam*)objShowParam;	ASSERT(sprm != 0L);
	SourceEditWindow* own = (SourceEditWindow*)sprm->_ownerRef;	ASSERT(own != 0L);
	return own->onClickSubMenuNode(ec, mtnodeCall, mtnodeParent, objCreateParam, objShowParam);
}

#if 0
/*---------------------------------------------------------------------*//**
	範囲外のクリック通知
**//*---------------------------------------------------------------------*/
void SourceEditWindow::MenuFuncTable_SourceEdit::onClickOutside(Menu* menu, const ExecCtx* ec, void* objCreateParam, void* objShowParam)
{
	GameMenuShowParam* sprm = (GameMenuShowParam*)objShowParam;	ASSERT(sprm != 0L);
	SourceEditWindow* own = (SourceEditWindow*)sprm->_ownerRef;	ASSERT(own != 0L);
///	return own->onClickOutside(ec, objCreateParam, objShowParam);
}
#endif

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
