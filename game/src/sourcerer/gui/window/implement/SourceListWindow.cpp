/***********************************************************************//**
 *	SourceListWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/08.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SourceListWindow.h"

// Friends
#include "SourceEditWindow.h"
#include "../menu/ListMenuWindowBase.h"
#include "../../menu/GameMenuCreateParam.h"
#include "../../panel/HeaderPanel.h"
#include "../../window/implement/GameSysMsgWindow.h"
#include "../../window/menu/MenuWindow.h"
#include "../../../episode/save/Trophy.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/Party.h"
#include "../../../gui/GameGui.h"
#include "../../../source/FixedSourceDef.h"
#include "../../../source/Source.h"
#include "../../../source/SourceManager.h"
#include "../../../status/StatusDrawer.h"

// External
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/g2d/StringDrawer.h"
#include "../../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gui/PanelText.h"
#include "../../../../tfw/lib/TypeUtils.h"
#include "../../../../tfw/smenu/Menu.h"
#include "../../../../tfw/smenu/MenuTreeNode.h"
//#include "../../../../tfw/txt/TxtUtils.h"
//#include "../../../../tfw/txt/implement/SjisTxt.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS
	
////////////////////////////////////////////////////////////////////////////
// 定数

#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ

#define RECT_SIDE_PROP_BG			0.0f, 48.0f, 128.0f, 272.0f
#define UV_SIDE_PROP_BG				0.0f / W_TEX, 448.0f / H_TEX, 128.0f / W_TEX, 272.0f / H_TEX

#define X_SIDE_PROP_STRING			5
#define Y_SIDE_PROP_STRING			96
#define W_SIDE_PROP_STRING			108
#define H_SIDE_PROP_STRING			96

#define X_SIDE_REQ_OQL_TXT			5.0f
#define Y_SIDE_REQ_OQL_TXT			164.0f
#define X_SIDE_REQ_OQL_GRAPH		5.0f
#define Y_SIDE_REQ_OQL_GRAPH		199.0f
#define W_SIDE_REQ_OQL_GRAPH		80.0f
#define H_SIDE_REQ_OQL_GRAPH		60.0f

#define RECT_BTN_EDIT				7, 264, 96, 32
#define UV_EDIT_NRM					752, 352, 96, 32
#define UV_EDIT_HVR					752, 384, 96, 32
#define W_LOCK_ICON					24
#define H_LOCK_ICON					24
#define UV_LOCK_ICON				928, 0, 24, 24

#define RECT_SOURCE_KIND			97, 56, 24, 16
#define UV_SOURCE_KIND_SS			921, 696, 24, 16
#define UV_SOURCE_KIND_JS			896, 696, 24, 16

#define MSGWND_CTRL_HINT			675430

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SourceListWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	選択されたソースを得る
**//*---------------------------------------------------------------------*/
Source* SourceListWindow::getSelectedSource() const
{
	if(getSelectedIndex() < 0)	{	return 0L;	}
	return getSourceFromListIndex(getSelectedIndex());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ソースエディタを起動する
**//*---------------------------------------------------------------------*/
bool SourceListWindow::runSourceEditor(Source* source, bool isReadOnly)
{
	if(_slwcase == SLWCASE_SELECT)	{	return false;	}

	SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow(); ASSERT(wndSrcedt != 0L);
	if(wndSrcedt == 0L)	{	return false;	}

	// ソースエディタを設定する
	wndSrcedt->setup(isReadOnly ? SourceEditWindow::ACTMODE_LOOK : SourceEditWindow::ACTMODE_EDIT, source->getCode());

	// ソースエディットウインドウを開く
	if(!wndSrcedt->isShowWindow())
	{
		wndSrcedt->showWindow(true);
	}

	_sourceEditing = source;
	_slwimodeLast = SLWIMODE_EDITING;
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SourceListWindow::SourceListWindow()
	: _srcmngRef(0L)
	, _slwcase(SLWCASE_NULL)
	, _slwimodeLast(SLWIMODE_NULL)
	, _idxFocusedRem(-1)
	, _sourceEditing(0L)
	, _strSideProp(0L)
{
	for(int i = 0; i < NUM_BTN; i++)	{	_pnlBtn[i] = 0;		}
	for(int i = 0; i < NUM_TDP; i++)	{	_strdraw[i] = 0L;	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SourceListWindow::~SourceListWindow()
{
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_BTN; i++)	{	ASSERT(_pnlBtn[i] == 0L);	}
		for(int i = 0; i < NUM_TDP; i++)	{	ASSERT(_strdraw[i] == 0L);	}
	#endif
	ASSERT(_strSideProp == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SourceListWindow::create(Font* fontRef, Texture* texRef)
{
	// ウインドウの作成
	if(!ListWindowBase::create(fontRef, texRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SOURCEBOOK)))
	{
		return false;
	}

	RectF32 rect;
	ColorU8 col;

	// サイドプロパティ用の文字列作成
	_strSideProp = new VcString();

	// テキスト描画の作成
	for(int i = 0; i < NUM_TDP; i++)
	{
		_strdraw[i] = new StringDrawer();
		if(!_strdraw[i]->create())
		{
			return false;
		}
		_strdraw[i]->setFont(fontRef);
		_strdraw[i]->setFontSize(16);
		_strdraw[i]->setReadingAnimation(false, 1.0f, 0.0f);
		_strdraw[i]->setVerticalScrollAnimation(false, 1.0f);
	}
	_strdraw[TDP_PROPSTR]->setRectangle(RectF32::setout(&rect, X_SIDE_PROP_STRING, Y_SIDE_PROP_STRING, W_SIDE_PROP_STRING, H_SIDE_PROP_STRING));

	// 子パネル作成
	{
		RectF32 uv;
		_pnlBtn[BTN_EDIT] = new Panel();
		_pnlBtn[BTN_EDIT]->create(RectF32::setout(&rect, RECT_BTN_EDIT));
		_pnlBtn[BTN_EDIT]->setStateInfo(STATE_NORMAL, _texRef, Gcalc::calcTexUv(&uv, UV_EDIT_NRM, W_TEX, H_TEX, Env_get2drRate()));
		_pnlBtn[BTN_EDIT]->setStateInfo(STATE_HOVER, _texRef, Gcalc::calcTexUv(&uv, UV_EDIT_HVR, W_TEX, H_TEX, Env_get2drRate()));
		_pnlBtn[BTN_EDIT]->createText(fontRef, 16, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_EDIT), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 0, 0, false, false, false);
		_pnlBtn[BTN_EDIT]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
		_pnlBtn[BTN_EDIT]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
		_pnlBtn[BTN_EDIT]->setEnable(false);
		///_pnlBtn[BTN_EDIT]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
		_pnlBtn[BTN_SHOW] = new Panel();
		_pnlBtn[BTN_SHOW]->create(RectF32::setout(&rect, RECT_BTN_EDIT));
		_pnlBtn[BTN_SHOW]->setStateInfo(STATE_NORMAL, _texRef, Gcalc::calcTexUv(&uv, UV_EDIT_NRM, W_TEX, H_TEX, Env_get2drRate()));
		_pnlBtn[BTN_SHOW]->setStateInfo(STATE_HOVER, _texRef, Gcalc::calcTexUv(&uv, UV_EDIT_HVR, W_TEX, H_TEX, Env_get2drRate()));
		_pnlBtn[BTN_SHOW]->createText(fontRef, 16, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_LOOK), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 0, 0, false, false, false);
		_pnlBtn[BTN_SHOW]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
		_pnlBtn[BTN_SHOW]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
		_pnlBtn[BTN_SHOW]->setEnable(false);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SourceListWindow::destroy()
{
	// 子パネル削除
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] != 0L)
		{
			_pnlBtn[i]->destroy();
			delete _pnlBtn[i];
			_pnlBtn[i] = 0L;
		}
	}

	// テキスト描画の削除
	for(int i = 0; i < NUM_TDP; i++)
	{
		if(_strdraw[i] != 0L)
		{
			_strdraw[i]->destroy();
			delete _strdraw[i];
			_strdraw[i] = 0L;
		}
	}

	// サイドプロパティ用の文字列削除
	delete _strSideProp;
	_strSideProp = 0L;

	ListWindowBase::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SourceListWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	// 子としてのソースエディットウインドウの制御
	SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow(); ASSERT(wndSrcedt != 0L);
	if((wndSrcedt != 0L) && wndSrcedt->isShowWindow())
	{
		if(wndSrcedt->isDone())	// 完了検知
		{
			if(wndSrcedt->isChanged() && (wndSrcedt->getActMode() == SourceEditWindow::ACTMODE_EDIT))	// 変更されている
			{
				// 変更確認 YES / NO ウインドウを出す
				showYesNoMessageWindow(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_CONFIRM_REFLECT_CHANGES));
			}
			else
			{
				_slwimodeLast = SLWIMODE_NULL;	// 編集中モードを直接終了する
			}
			wndSrcedt->showWindow(false);	// ソースエディットウインドウを閉じる
		}
		return;
	}

	// YES / NO メッセージウインドウ制御
	switch(_slwimodeLast)
	{
	case SLWIMODE_EDITING:
		{
			GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
			if((wndSysMsg != 0L) && wndSysMsg->isShowWindow())
			{
				wndSysMsg->exec(res, ec);
				const GameSysMsgWindow::Result* res = wndSysMsg->getResult();
				if(res->isClickYesButton() || res->isClickNoButton())
				{
					_slwimodeLast = SLWIMODE_NULL;
					// 変更保存の確認終了
					doFinishSourceEditor(res->isClickYesButton());
					// ウインドウを消す
					wndSysMsg->showWindow(false);	
				}
				return;
			}
		}
	default:
		break;
	}

	// 子パネル制御
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* ui = gec->getTouchPanel();
	PointF32 ptTouch(S32_MIN, S32_MIN);
	PointF32 ptClick(S32_MIN, S32_MIN);
	bool isTouch = !ui->isReacted() ? ui->getTouchPosition(&ptTouch) : false;
	bool isClick = !ui->isReacted() ? ui->isClickRelease(&ptClick) : false;
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] == 0)	{	continue;	}
		if(isClick)
		{
			if(_pnlBtn[i]->hit(&ptClick, true))
			{
				switch(i)
				{
				case BTN_EDIT:
				case BTN_SHOW:
					if(_idxFocusedRem != -1)	// フォーカスされているアイテムがある
					{
						const Litem* litem = _listLitem->getObject(_idxFocusedRem);
						Source* source = (litem != 0L) ? (Source*)litem->_obj : 0L;
						runSourceEditor(source, (i == BTN_SHOW));
					}
					break;
				}
				ui->setReacted(true);
			}
		}
		else
		{
			_pnlBtn[i]->hit(&ptTouch, isTouch);
		}
		_pnlBtn[i]->exec(res, ec);
	}
	
	ListWindowBase::execContents(res, ec);

	// サイドプロパティの更新
	s32 idxFocused = getFocusedIndex();
	if(idxFocused != _idxFocusedRem)	// フォーカスが変更された
	{
		_idxFocusedRem = idxFocused;

		bool isEditEnable = false;
		bool isShowEnable = false;
		if(_idxFocusedRem != -1)	// フォーカスされているアイテムがある
		{
			const Litem* litem = _listLitem->getObject(_idxFocusedRem);
			const Source* source = (litem != 0L) ? (Source*)litem->_obj : 0L;

			// サイドプロパティの文字列を更新
			updateSidePropString(source);

			// 編集・見るボタンの有効性
			if(_slwcase != SLWCASE_SELECT)
			{
				isEditEnable = (source->getFixedSourceDef() == 0L) && !source->isReserved();
				isShowEnable = !isEditEnable;
			}
		}

		// サイドプロパティのボタンの有効・無効化
		if(_pnlBtn[BTN_EDIT]->isEnable() != isEditEnable)
		{
			_pnlBtn[BTN_EDIT]->setEnable(isEditEnable);
		}
		if(_pnlBtn[BTN_SHOW]->isEnable() != isShowEnable)
		{
			_pnlBtn[BTN_SHOW]->setEnable(isShowEnable);
		}
	}
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void SourceListWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	// 子としてのソースエディットウインドウの描画
	SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow(); ASSERT(wndSrcedt != 0L);
	if((wndSrcedt != 0L) && wndSrcedt->isShowWindow())
	{
		RectF32 rect(0, 0, (f32)Game::getGame()->getLogicalWidth(), (f32)Game::getGame()->getLogicalHeight());
		ColorU8 col(0, 0, 0, 127);
		RendererUtils::draw2dColorRect(rc->getRenderer(), &rect, &col);	// 暗転幕
		wndSrcedt->draw(rc);	// ソースエディットウインドウの描画
		return;
	}

	// サイドプロパティの描画
	if(_idxFocusedRem != -1)	// フォーカスされているアイテムがある
	{
		const Litem* litem = _listLitem->getObject(_idxFocusedRem);
		const Source* source = (litem != 0L) ? (Source*)litem->_obj : 0L;
		const FixedSourceDef* fsrcdf = source->getFixedSourceDef();
		const Elene* eeReqOql = source->getRequirementOql();

		RectF32 rect;
		RectF32 uv;
		Renderer* rdr = rc->getRenderer();
		rdr->bindTexture(_texRef);
		rdr->setSolidColor(255, 255, 255, 255);

		// 地の描画
		RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&rect, RECT_SIDE_PROP_BG), RectF32::setout(&uv, UV_SIDE_PROP_BG));

		// 言語種別
		switch(source->getSourceKind())
		{
		case SourceDef::SRCKIND_SS:	RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&rect, RECT_SOURCE_KIND), Gcalc::calcTexUv(&uv, UV_SOURCE_KIND_SS, W_TEX, H_TEX, Env_get2drRate()));	break;
		case SourceDef::SRCKIND_JS:	RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&rect, RECT_SOURCE_KIND), Gcalc::calcTexUv(&uv, UV_SOURCE_KIND_JS, W_TEX, H_TEX, Env_get2drRate()));	break;
		}

		// ソース名称・説明
		rdr->setSolidColor(95, 63, 63, 255);
		for(int i = 0; i < NUM_TDP; i++)
		{
			_strdraw[i]->draw(rc, 0, 0);
		}

		if(fsrcdf != 0L)
		{
			// 必要原質量
			if((eeReqOql != 0L) && !eeReqOql->isInitialValue())
			{
				VcString str;
				rdr->setSolidColor(0, 0, 0, 255);
				EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SOURCE_REQ_OQL), X_SIDE_REQ_OQL_TXT, Y_SIDE_REQ_OQL_TXT, 16, rc);
				f32 x = X_SIDE_REQ_OQL_GRAPH + W_SIDE_REQ_OQL_GRAPH + 3;
				f32 y = Y_SIDE_REQ_OQL_GRAPH;
				EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%d", eeReqOql->getAethel() / Elene::ELENE_VALID_FIGURE), x, y +  0.0f, 12, rc);
				EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%d", eeReqOql->getWind()   / Elene::ELENE_VALID_FIGURE), x, y + 12.0f, 12, rc);
				EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%d", eeReqOql->getFire()   / Elene::ELENE_VALID_FIGURE), x, y + 24.0f, 12, rc);
				EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%d", eeReqOql->getWater()  / Elene::ELENE_VALID_FIGURE), x, y + 36.0f, 12, rc);
				EasyStringDrawer::draw(_fontRef, VcString::format(&str, "%d", eeReqOql->getSoil()   / Elene::ELENE_VALID_FIGURE), x, y + 48.0f, 12, rc);
				StatusDrawer::drawElene(rc, _texRef, eeReqOql, X_SIDE_REQ_OQL_GRAPH, Y_SIDE_REQ_OQL_GRAPH, W_SIDE_REQ_OQL_GRAPH, H_SIDE_REQ_OQL_GRAPH);
			}

			// ロックアイコン
			if(_slwcase == SLWCASE_MENU)
			{
				rdr->bindTexture(_texRef);
				rdr->setSolidColor(0, 0, 0, 255);
				RectF32 rectLockIcon(RECT_BTN_EDIT);
				rectLockIcon.x() += (rectLockIcon.w() - W_LOCK_ICON) / 2;
				rectLockIcon.y() += (rectLockIcon.h() - H_LOCK_ICON) / 2;
				rectLockIcon.w() = W_LOCK_ICON;
				rectLockIcon.h() = H_LOCK_ICON;
				RendererUtils::draw2dTextureRect(rdr, &rectLockIcon, Gcalc::calcTexUv(&uv, UV_LOCK_ICON, W_TEX, H_TEX, Env_get2drRate()));
			}
		}
	}

	// 子パネル描画
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] != 0)	{	_pnlBtn[i]->draw(rc);	}
	}
}

/*---------------------------------------------------------------------*//**
	言語変更通知
**//*---------------------------------------------------------------------*/
void SourceListWindow::notifyChangeLanguage()
{
	ListWindowBase::notifyChangeLanguage();

	// ヘッダーパネルの言語変更
	HeaderPanel* hpnl = headerPanel();
	if(hpnl != 0L)
	{
		hpnl->changeGuiString(
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SOURCEBOOK),
			0L,
			0L,
			0L	);
	}

	// ボタンの言語変更
	if((_pnlBtn[BTN_EDIT] != 0L) && (_pnlBtn[BTN_EDIT]->getPanelText() != 0L))
	{
		_pnlBtn[BTN_EDIT]->getPanelText()->setText(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_EDIT), false);
	}
	if((_pnlBtn[BTN_SHOW] != 0L) && (_pnlBtn[BTN_SHOW]->getPanelText() != 0L))
	{
		_pnlBtn[BTN_SHOW]->getPanelText()->setText(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_LOOK), false);
	}
}

/*---------------------------------------------------------------------*//**
	サブメニュー表示通知
**//*---------------------------------------------------------------------*/
void SourceListWindow::onSubMenuShow(Menu* menuSub, MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam)
{
	if((getSelectedIndex() < 0) || (_listLitem->getCount() <= (u32)getSelectedIndex()))	{	return;	}
	Litem* litem = _listLitem->getObject(getSelectedIndex());
	if(litem == 0L)		{	return;	}
	Source* source = (Source*)litem->_obj;
	if(source == 0L)	{	return;	}
	MenuWindow* mwnd = (MenuWindow*)mtnodeShow->getPanel();
	if(mwnd == 0L)		{	return;	}

	MenuTreeNode* mtnodeDisp = menuSub->getMenuTreeNodeChild(mtnodeShow);
	while(mtnodeDisp != 0L)
	{
		MenuTreeNode* mtnodeFunc = menuSub->getMenuTreeNodeChild(mtnodeDisp);
		if(mtnodeFunc != 0L)
		{
			s32 idx = mwnd->getElementContentIndexByNode(mtnodeDisp) + 1;	// + 1 はタイトル要素分
			const VcString* name = mtnodeFunc->getName();
			if((idx > 0) && (name != 0L) && (name->getLength() > 0))
			{
				if(name->equals("Edit"))			// 編集
				{
					mwnd->setElementUnused(idx, (source->getFixedSourceDef() != 0) && !source->isReserved());
				}
				else if(name->equals("ConvToJs"))	// JS に変換
				{
					mwnd->setElementUnused(idx, source->getSourceKind() == SourceDef::SRCKIND_JS);
				}
				else if(name->equals("ConvToSs"))	// SS に変換
				{
					mwnd->setElementUnused(idx, source->getSourceKind() == SourceDef::SRCKIND_SS);
				}
				else if(name->equals("Delete"))		// 削除
				{
					mwnd->setElementUnused(idx, source->getFixedSourceDef() != 0);
				}
			}
		}

		mtnodeDisp = menuSub->getMenuTreeNodeSibling(mtnodeDisp);
	}
}

/*---------------------------------------------------------------------*//**
	サブメニューノードクリック通知
**//*---------------------------------------------------------------------*/
bool SourceListWindow::onClickSubMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	const VcString* name = mtnodeCall->getName(); ASSERT(name != 0L);
	VcString title(*GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SOURCEBOOK) + " - " + *mtnodeParent->getName());
	if(name->equals("Show"))			// 見る
	{
		// 見るモードへ
		ListWindowBase::setMode(LWMODE_SHOW, &title);
		return true;
	}
	if(name->equals("Sort"))			// 並び替える
	{
		// 並び替えモードへ
		ListWindowBase::setMode(LWMODE_SORT, &title);
		return true;
	}
	if(name->equals("Clone"))			// 複製
	{
		// 複製モードへ
		ListWindowBase::setMode(LWMODE_CLONE, &title);
		return true;
	}
	if(name->equals("Edit"))			// 編集
	{
		// 編集モードへ
		_slwimodeLast = SLWIMODE_EDIT;
		ListWindowBase::setMode(LWMODE_EDIT, &title);
		return true;
	}
	else if(name->equals("ConvToJs"))	// JS に変換
	{
		// 編集モードへ
		_slwimodeLast = SLWIMODE_CONV_TO_JS;
		ListWindowBase::setMode(LWMODE_EDIT, &title);
		return true;
	}
	else if(name->equals("ConvToSs"))	// SS に変換
	{
		// 編集モードへ
		_slwimodeLast = SLWIMODE_CONV_TO_SS;
		ListWindowBase::setMode(LWMODE_EDIT, &title);
		return true;
	}
	if(name->equals("Delete"))			// 削除
	{
		// 複製モードへ
		ListWindowBase::setMode(LWMODE_DELETE, &title);
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	確認メッセージ取得
*//*---------------------------------------------------------------------*/
bool SourceListWindow::takeConfirmMessage(VcString* msg, LwMode mode) const
{
	switch(mode)
	{
	case LWMODE_CLONE:		// 複製モード
		msg->copy(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_CONFIRM_CLONE));
		return true;
	case LWMODE_DELETE:		// 削除モード
		msg->copy(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_CONFIRM_DELETE));
		return true;
	case LWMODE_EDIT:		// 編集モード
		switch(_slwimodeLast)
		{
		case SLWIMODE_CONV_TO_JS:
			msg->copy(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_CONFIRM_CONV_TO_JS));
			return true;
		case SLWIMODE_CONV_TO_SS:
			msg->copy(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_CONFIRM_CONV_TO_SS));
			return true;
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	リストアイテム置換通知
**//*---------------------------------------------------------------------*/
bool SourceListWindow::onReplaceLitem(s32 index1, s32 index2)
{
	// ソースの置換
	Source* source1 = getSourceFromListIndex(index1);
	Source* source2 = getSourceFromListIndex(index2);
	if((source1 == 0L) || (source2 == 0L))	{	return false;	}
	Source::swap(source1, source2);

	// リストアイテムの置換処理
	Litem* litem1 = _listLitem->getObject(index1);
	Litem* litem2 = _listLitem->getObject(index2);
	makeLitemTitleString(litem1->_name, source2);
	makeLitemTitleString(litem2->_name, source1);

	return true;
}

/*---------------------------------------------------------------------*//**
	リストアイテム複製通知
**//*---------------------------------------------------------------------*/
bool SourceListWindow::onCloneLitem(s32* indexShiftMax, s32 index)
{
	const Litem* litemTrg = _listLitem->getObject(index);
	if((litemTrg == 0L) || (litemTrg->_obj == 0L))	{	return false;	}
	s32 rindexTrg = litemTrg->_ridx + 1;

	// 空きチェック
	s32 rindexEmpty = _srcmngRef->getEmptySourceIndex(SourceManager::RSVK_FREE, litemTrg->_ridx + 1, false);
	if((rindexEmpty == -1) || (rindexEmpty <= litemTrg->_ridx))
	{
		*indexShiftMax = -1;
		return false;
	}
	s32 indexEmpty = -1;
	for(u32 i = index + 1; i < _listLitem->getCount(); i++)
	{
		if(_listLitem->getObject(i)->_ridx == rindexEmpty)
		{
			indexEmpty = i;
			break;
		}
	}
	if(indexEmpty == -1)
	{
		*indexShiftMax = -1;
		return false;
	}

	const Source* sourceWk1;
	Source* sourceWk2;

	// １つずらして、対象の次のソースを空ける
	for(s32 i = rindexEmpty - 1; i >= litemTrg->_ridx + 1; i--)
	{
		sourceWk1 = _srcmngRef->getSource(i);
		sourceWk2 = _srcmngRef->source(i + 1);
		sourceWk2->copy(sourceWk1);
	}

	// 編集可能な複製を作成する
	sourceWk1 = _srcmngRef->getSource(litemTrg->_ridx);
	sourceWk2 = _srcmngRef->source(litemTrg->_ridx + 1);
	sourceWk2->clear();
	sourceWk2->reproduce(sourceWk1);

	// 名前重複回避
	if(avoidOverlapedSourceName(sourceWk2))
	{
		updateLitem(sourceWk2);
	}

	*indexShiftMax = indexEmpty;
	return true;
}

/*---------------------------------------------------------------------*//**
	リストアイテム削除チェック
**//*---------------------------------------------------------------------*/
bool SourceListWindow::onDeleteCheck(s32 indexDelete) const
{
	if((indexDelete < 0) || (_listLitem->getCount() <= (u32)indexDelete))
	{
		return false;
	}
	const Source* source = getSourceFromListIndex(indexDelete);
	if(source == 0L)
	{
		return false;
	}
	return source->getFixedSourceDef() == 0L;
}

/*---------------------------------------------------------------------*//**
	リストアイテム削除通知
**//*---------------------------------------------------------------------*/
bool SourceListWindow::onDeleteLitem(s32 indexDelete)
{
	if((indexDelete < 0) || (_listLitem->getCount() <= (u32)indexDelete))
	{
		return false;
	}

	Source* sourceWk1;
	const Source* sourceWk2;

	// ソースクリア
	const Litem* litemTrg = _listLitem->getObject(indexDelete);
	if((litemTrg == 0L) || (litemTrg->_obj == 0L))	{	return false;	}
	sourceWk1 = (Source*)litemTrg->_obj;
	sourceWk1->clear();

	// １つずらす
	s32 i = litemTrg->_ridx;
	while(i < (_srcmngRef->getSourceNum() - 1))
	{
		sourceWk1 = _srcmngRef->source(i);
		sourceWk2 = _srcmngRef->getSource(i + 1);
		sourceWk1->copy(sourceWk2);
		i++;
	}

	// 最終要素のクリア
	sourceWk1 = _srcmngRef->source(i);
	if(sourceWk1 == 0L)	{	return false;	}
	sourceWk1->clear();

	return true;
}

/*---------------------------------------------------------------------*//**
	リストアイテム編集確認
**//*---------------------------------------------------------------------*/
bool SourceListWindow::onEditCheck(s32 indexEdit) const
{
	const Litem* litem = _listLitem->getObject(indexEdit);
	if(litem == 0L)		{	return false;	}
	Source* source = (Source*)litem->_obj;
	if(source == 0L)	{	return false;	}
	switch(_slwimodeLast)
	{
	case SLWIMODE_EDIT:
		return source->getFixedSourceDef() == 0L;
		break;
	default:
		return source->getCode()->getLength() > 0;
		break;
	}
}

/*---------------------------------------------------------------------*//**
	リストアイテム編集通知
**//*---------------------------------------------------------------------*/
bool SourceListWindow::onEditLitem(s32 indexEdit)
{
	const Litem* litem = _listLitem->getObject(indexEdit);
	if(litem == 0L)		{	return false;	}
	Source* source = (Source*)litem->_obj;
	if(source == 0L)	{	return false;	}
	switch(_slwimodeLast)
	{
	case SLWIMODE_EDIT:
		return runSourceEditor(source, false);	// 編集へ
	case SLWIMODE_CONV_TO_JS:
		return source->convToJs();				// JS へ変換
	case SLWIMODE_CONV_TO_SS:
		return source->convToSs();				// SS へ変換
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	アイテム更新通知
**//*---------------------------------------------------------------------*/
void SourceListWindow::onUpdateLitem(Litem* litem)
{
	setupLitem(litem, litem->_ridx, (Source*)litem->_obj);
}

/*---------------------------------------------------------------------*//**
	表示状態の変更通知
**//*---------------------------------------------------------------------*/
void SourceListWindow::onShowWindow(bool isShow)
{
	if(isShow)
	{
		// ソースマネージャを得る
		_srcmngRef = Game::getGame()->getMyParty()->getSourceManager(); ASSERT(_srcmngRef != 0L);

		// リストを作成
		makeList();

		// 初期設定
		if(_slwcase == SLWCASE_MENU)
		{
			_fileidSubMenu = FILEID_CONV_LANG(MENU_MENU_IM_SOURCELISTSUB_EN_XML);	// サブメニューのファイル ID を設定する
			ListWindowBase::setMode(LWMODE_SHOW, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SOURCEBOOK));	// デフォルトモードの設定
		}
		else
		{
			_fileidSubMenu = 0;	// サブメニューのファイル ID を設定する
			ListWindowBase::setMode(LWMODE_USE, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SOURCEBOOK));	// デフォルトモードの設定
		}
	}
	else
	{
		// YES / NO メッセージウインドウが出ている場合は強制的に消す
		GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
		if((wndSysMsg != 0L) && wndSysMsg->isShowWindow())
		{
			wndSysMsg->showWindow(false);	
		}
		// 子としてのソースエディットウインドウが出ている場合は強制的に消す
		SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow(); ASSERT(wndSrcedt != 0L);
		if((wndSrcedt != 0L) && wndSrcedt->isShowWindow())
		{
			wndSrcedt->showWindow(false);	
		}
	}

	// 親クラスでメニューが作成される
	ListWindowBase::onShowWindow(isShow);
}

/*---------------------------------------------------------------------*//**
	リストインデックスからソースを得る
**//*---------------------------------------------------------------------*/
Source* SourceListWindow::getSourceFromListIndex(s32 index) const
{
	if((index < 0) || (_listLitem->getCount() <= (u32)index))	{	return 0L;	}
	const Litem* litem = _listLitem->getObject(index);
	if(litem == 0L)	{	return 0L;	}
	return (Source*)litem->_obj;
}

/*---------------------------------------------------------------------*//**
	リストを作成する
**//*---------------------------------------------------------------------*/
void SourceListWindow::makeList()
{
	// リストを作成
	_listLitem->removeAll();
	_paramGameMenuCreate = GameMenuCreateParam::CASE_INGAME_SRCL;
	for(s32 i = 0; i < _srcmngRef->getSourceNum(); i++)
	{
		Source* source = _srcmngRef->source(i);
		if(source->isReserved())	{	continue;	}
		Litem* litem = new Litem();
		setupLitem(litem, i, source);
		_listLitem->addTail(litem);
	}
}

/*---------------------------------------------------------------------*//**
	リストアイテムの情報を設定する
**//*---------------------------------------------------------------------*/
void SourceListWindow::setupLitem(Litem* litem, s32 ridx, Source* source) const
{
	litem->_ridx = ridx;
	litem->_obj = source;
	makeLitemTitleString(litem->_name, source);
	TFW_SET_FLAG(litem->_liflags, Litem::LIF_NOEDIT, source->isReserved());
}

/*---------------------------------------------------------------------*//**
	リストアイテムの情報を更新する
**//*---------------------------------------------------------------------*/
void SourceListWindow::updateLitem(const Source* source)
{
	s32 cntIdx = 0;
	for(ListIterator<Litem*> it = _listLitem->iterator(); it.has(); it.next())
	{
		Litem* litem = it.object();
		if(source == (Source*)litem->_obj)
		{
			makeLitemTitleString(litem->_name, (Source*)litem->_obj);
			ListWindowBase::updateLitem(cntIdx);
			return;
		}
		cntIdx++;
	}
}

/*---------------------------------------------------------------------*//**
	サイドプロパティの文字列を更新する
**//*---------------------------------------------------------------------*/
void SourceListWindow::updateSidePropString(const Source* source)
{
	const Litem* litemFocus = _listLitem->getObject(_idxFocusedRem);
	const Source* sourceFocus = (_listLitem != 0L) ? (Source*)litemFocus->_obj : 0L;
	if(sourceFocus == source)
	{
		if(source->getName()->getLength() > 0)
		{
			_strSideProp->set("\x81\x75");		// 「
			_strSideProp->add(source->getName());
			_strSideProp->add("\x81\x76\n");	// 」
		}
		else
		{
			_strSideProp->set(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SOURCE_FREE_SLOT));
			_strSideProp->add("\n");
		}
		_strSideProp->add(source->getExplain());
		_strdraw[TDP_PROPSTR]->setString(_strSideProp);
	}
}

/*---------------------------------------------------------------------*//**
	ソースタイトル文字列を作成する
**//*---------------------------------------------------------------------*/
void SourceListWindow::makeLitemTitleString(VcString* name, const Source* source) const
{
	if(source->getName()->getLength() > 0)
	{
		VcString strWk;
		// 名称
		strWk.set(source->getName());
		// 説明文付加
		if(source->getExplain()->getLength() > 0)
		{
			strWk += " / ";
			strWk += *source->getExplain();
		}

#if 0	// SourceListMenuWindow::adjustBlockString へ移行	【2015/03/02 r-kishi】
		SjisTxt txt;
		txt.setSingleString(&strWk);
		u32 limit = Game::getGame()->isWideResolution() ? 34 : 24;
		if(TxtUtils::shortenByRawCharCount(&txt, limit))	// 長さ制限
		{
			name->copy(txt.getRawString());
			name->add("...");
		}
		else
		{
			name->copy(&strWk);
		}
#else
		name->copy(&strWk);
#endif

		return;
	}

	name->copy(_strEmpty);
}

/*---------------------------------------------------------------------*//**
	ソースエディタを終了する
**//*---------------------------------------------------------------------*/
void SourceListWindow::doFinishSourceEditor(bool isSave)
{
	if(_sourceEditing == 0L)	{	return;	}

	if(isSave)
	{
		// ソースコードを保存する
		SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow(); ASSERT(wndSrcedt != 0L);
		if(wndSrcedt != 0L)
		{
			// コード設定
			VcString code;
			wndSrcedt->takeSourceText(&code);
			_sourceEditing->setCustomCode(&code);
			// 名前重複自動回避
			avoidOverlapedSourceName(_sourceEditing);
			// リストアイテム更新
			updateLitem(_sourceEditing);
			// サイドプロパティの文字列を更新
			updateSidePropString(_sourceEditing);
			// ユーザー独自ソース作成成功トロフィー
			#if defined(_ENLIGHT_V01) && ENABLE_PSNS
				if(_sourceEditing->isUserCustomized())
				{
					Game::getGame()->getTrophy()->achievementTrophy(Trophy::TI_SOUMA_ADEPT_1_CUSTOMIZED_SOURCE_OF_SOUMA, 100);
				}
			#endif
		}
	}

	_sourceEditing = 0L;
	setMode(LWMODE_SHOW, TypeUtils::getEmptyString());
}

/*---------------------------------------------------------------------*//**
	ソース名重複回避
**//*---------------------------------------------------------------------*/
bool SourceListWindow::avoidOverlapedSourceName(Source* source)
{
	VcString nameTrg(source->getName());
	if(nameTrg.getLength() <= 0)
	{
		return false;
	}

	for(int cntRetry = 1; cntRetry <= 256; cntRetry++)
	{
		int i;
		// 重複チェック
		for(i = 0; i < _srcmngRef->getSourceNum(); i++)
		{
			const Source* sourceWk = _srcmngRef->getSource(i);
			if(sourceWk != source)
			{
				if(sourceWk->getName()->equals(&nameTrg))
				{
					break;
				}
			}
		}
		if(i >= _srcmngRef->getSourceNum())	// 重複なし
		{
			if(cntRetry == 1)	{	return false;	}	// １回目で OK の場合は書き換え不要
			break;	// 終了
		}

		// 新しい名前
		nameTrg = VcString::format("%s_copied%02d", source->getName()->getRaw(), cntRetry);
	}
	// 名前書き換え
	return source->rewriteName(&nameTrg);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
