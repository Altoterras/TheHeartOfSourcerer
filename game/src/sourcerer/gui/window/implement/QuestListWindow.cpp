/***********************************************************************//**
 *	QuestListWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/15.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QuestListWindow.h"

// Friends
#include "../../menu/GameMenuCreateParam.h"
#include "../../panel/HeaderPanel.h"
#include "../../../episode/party/MyParty.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/MoveMap.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../quest/QuestArticle.h"
#include "../../../quest/QuestDef.h"
#include "../../../quest/QuestManager.h"

// External
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/g2d/StringDrawer.h"
#include "../../../../tfw/g2d/TiledRectDraw.h"
#include "../../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gui/PanelText.h"
#include "../../../../tfw/lib/TypeUtils.h"
#include "../../../../tfw/smenu/MenuTreeNode.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ

#define RECT_SIDE_PROP_BG			0.0f, 48.0f, 208.0f, 272.0f
#define UV_SIDE_PROPBG_CM_U			0.0f
#define UV_SIDE_PROPBG_CM_V			448.0f
#define UV_SIDE_PROPBG_CM_W			64.0f
#define UV_SIDE_PROPBG_CM_H			272.0f
#define UV_SIDE_PROPBG_RM_U			64.0f
#define UV_SIDE_PROPBG_RM_V			448.0f
#define UV_SIDE_PROPBG_RM_W			64.0f
#define UV_SIDE_PROPBG_RM_H			272.0f

#define RECT_SIDE_PROP_ICON			158.0f, 50.0f, 24.0f, 24.0f
#define UV_QSTSTAT_ICON_ACCEPTED	1000.0f / W_TEX, 320.0f / H_TEX, 24.0f / W_TEX, 24.0f / H_TEX
#define UV_QSTSTAT_ICON_ACHVED		1000.0f / W_TEX, 392.0f / H_TEX, 24.0f / W_TEX, 24.0f / H_TEX
#define UV_QSTSTAT_ICON_CANCELED	1000.0f / W_TEX, 368.0f / H_TEX, 24.0f / W_TEX, 24.0f / H_TEX
#define UV_QSTSTAT_ICON_FINISHED	1000.0f / W_TEX, 344.0f / H_TEX, 24.0f / W_TEX, 24.0f / H_TEX

#define W_SIDE_PROP_CONTENTS		168
#define X_SIDE_PROP_NAME			5
#define Y_SIDE_PROP_NAME			80
#define W_SIDE_PROP_NAME			W_SIDE_PROP_CONTENTS
#define H_SIDE_PROP_NAME			32
#define X_SIDE_PROP_STAT			140
#define Y_SIDE_PROP_STAT			68
#define W_SIDE_PROP_STAT			60
#define H_SIDE_PROP_STAT			32
#define X_SIDE_PROP_REQS			5
#define Y_SIDE_PROP_REQS			144
#define W_SIDE_PROP_REQS			W_SIDE_PROP_CONTENTS
#define H_SIDE_PROP_REQS			48
#define X_SIDE_PROP_RWRD			5
#define Y_SIDE_PROP_RWRD			224
#define W_SIDE_PROP_RWRD			W_SIDE_PROP_CONTENTS / 2
#define H_SIDE_PROP_RWRD			32
#define X_SIDE_PROP_DFLV			5 + (W_SIDE_PROP_CONTENTS / 2)
#define Y_SIDE_PROP_DFLV			224
#define W_SIDE_PROP_DFLV			W_SIDE_PROP_CONTENTS / 2
#define H_SIDE_PROP_DFLV			32
#define X_SIDE_PROP_CLNT			5
#define Y_SIDE_PROP_CLNT			272
#define W_SIDE_PROP_CLNT			W_SIDE_PROP_CONTENTS
#define H_SIDE_PROP_CLNT			32

#define X_NO_DATA					0.0f
#define Y_NO_DATA					160.0f
#define W_NO_DATA					(f32)Game::getGame()->getLogicalWidth()
#define H_NO_DATA					16.0f

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QuestListWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	選択されたクエストを得る
**//*---------------------------------------------------------------------*/
QuestArticle* QuestListWindow::getSelectedQuest() const
{
	if(getSelectedIndex() < 0)	{	return 0L;	}
	const Litem* litem = _listLitem->getObject(getSelectedIndex());
	return (litem != 0L) ? (QuestArticle*)litem->_obj : 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QuestListWindow::QuestListWindow()
	: _qstmngRef(0L)
	, _qlwcase(QLWCASE_NULL)
	, _pnlStat(0L)
	, _idxFocusedRem(-1)
{
	for(int i = 0; i < NUM_TDP; i++) { _strdraw[i] = 0L; }
	for(int i = 0; i < NUM_TRDP; i++) { _trdraw[i] = 0L; }
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
QuestListWindow::~QuestListWindow()
{
#if defined(_DEBUG)
	for(int i = 0; i < NUM_TDP; i++) { ASSERT(_strdraw[i] == 0L); }
	for(int i = 0; i < NUM_TRDP; i++) { ASSERT(_trdraw[i] == 0L); }
#endif
	ASSERT(_pnlStat == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool QuestListWindow::create(Font* fontRef, Texture* texRef)
{
	// ウインドウの作成
	if(!ListWindowBase::create(fontRef, texRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_ITEM_LIST)))
	{
		return false;
	}

	RectF32 rect;
	RectF32 uv;

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
	_strdraw[TDP_NAME]->setRectangle(RectF32::setout(&rect, X_SIDE_PROP_NAME, Y_SIDE_PROP_NAME, W_SIDE_PROP_NAME, H_SIDE_PROP_NAME));
	_strdraw[TDP_REQS]->setRectangle(RectF32::setout(&rect, X_SIDE_PROP_REQS, Y_SIDE_PROP_REQS, W_SIDE_PROP_REQS, H_SIDE_PROP_REQS));
	_strdraw[TDP_RWRD]->setRectangle(RectF32::setout(&rect, X_SIDE_PROP_RWRD, Y_SIDE_PROP_RWRD, W_SIDE_PROP_RWRD, H_SIDE_PROP_RWRD));
	_strdraw[TDP_CLNT]->setRectangle(RectF32::setout(&rect, X_SIDE_PROP_CLNT, Y_SIDE_PROP_CLNT, W_SIDE_PROP_CLNT, H_SIDE_PROP_CLNT));

	// タイル描画の作成
	for(int i = 0; i < NUM_TRDP; i++)
	{
		_trdraw[i] = new TiledRectDraw();
		if(!_trdraw[i]->create(texRef))
		{
			return false;
		}
	}
	Gcalc::calcTexUv(&uv, UV_SIDE_PROPBG_CM_U, UV_SIDE_PROPBG_CM_V, UV_SIDE_PROPBG_CM_W, UV_SIDE_PROPBG_CM_H, W_TEX, H_TEX, 1.0f);
	_trdraw[TRDP_PROP]->setTile(TiledRectDraw::TP_CM, &uv, UV_SIDE_PROPBG_CM_W, UV_SIDE_PROPBG_CM_H, TFW_DISPF_RANG_0);
	Gcalc::calcTexUv(&uv, UV_SIDE_PROPBG_RM_U, UV_SIDE_PROPBG_RM_V, UV_SIDE_PROPBG_RM_W, UV_SIDE_PROPBG_RM_H, W_TEX, H_TEX, 1.0f);
	_trdraw[TRDP_PROP]->setTile(TiledRectDraw::TP_RM, &uv, UV_SIDE_PROPBG_RM_W, UV_SIDE_PROPBG_RM_H, TFW_DISPF_RANG_0);

	// 状態パネルの作成
	_pnlStat = new Panel();
	if(!_pnlStat->create(RectF32::setout(&rect, X_SIDE_PROP_STAT, Y_SIDE_PROP_STAT, W_SIDE_PROP_STAT, H_SIDE_PROP_STAT)))
	{
		ASSERT(false);
		delete _pnlStat;
		_pnlStat = 0L;
		return false;
	}
	ColorU8 col(255, 127, 63, 255);
	_pnlStat->createText(fontRef, 12, TypeUtils::getEmptyString(), &col, 0, 0, 0, 0, false, false, false);
	_pnlStat->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
	_pnlStat->getPanelText()->setFlags(PanelText::F_DRAW_LEFT_SHADOW | PanelText::F_DRAW_TOP_SHADOW, true);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void QuestListWindow::destroy()
{
	// 状態パネルの削除
	if(_pnlStat != 0L)
	{
		_pnlStat->destroy();
		delete _pnlStat;
		_pnlStat = 0L;
	}

	// タイル描画の削除
	for(int i = 0; i < NUM_TRDP; i++)
	{
		if(_trdraw[i] != 0L)
		{
			_trdraw[i]->destroy();
			delete _trdraw[i];
			_trdraw[i] = 0L;
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

	ListWindowBase::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void QuestListWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	ListWindowBase::execContents(res, ec);

	if(isShowWindow())
	{
		// サイドプロパティのテキストを更新
		s32 idxFocused = getFocusedIndex();
		if(idxFocused != _idxFocusedRem)	// フォーカスが変更された
		{
			_idxFocusedRem = idxFocused;
			if(_idxFocusedRem != -1)	// フォーカスされているアイテムがある
			{
				const Litem* litem = _listLitem->getObject(_idxFocusedRem);
				const QuestArticle* qsta = (litem != 0L) ? (QuestArticle*)litem->_obj : 0L;
				const QuestDef* qstdf = (qsta != 0L) ? qsta->getDef() : 0L;

				// テキスト設定
				_strdraw[TDP_NAME]->setString(((qstdf != 0L) && (qstdf->getNameText() != 0L)) ? qstdf->getNameText() : TypeUtils::getEmptyString());
				_strdraw[TDP_REQS]->setString(((qstdf != 0L) && (qstdf->getRequisiteText() != 0L)) ? qstdf->getRequisiteText() : TypeUtils::getEmptyString());
				_strdraw[TDP_RWRD]->setString(((qstdf != 0L) && (qstdf->getRewardText() != 0L)) ? qstdf->getRewardText() : TypeUtils::getEmptyString());
				_strdraw[TDP_CLNT]->setString(((qstdf != 0L) && (qstdf->getClientText() != 0L)) ? qstdf->getClientText() : TypeUtils::getEmptyString());

				// 状態テキストの設定
				if(qsta->getStat() == QuestArticle::STAT_ACCEPT)
				{
					_pnlStat->getPanelText()->setText(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_QUEST_ACCEPTED), false);
				}
				else if(qsta->getStat() == QuestArticle::STAT_ACHVED)
				{
					_pnlStat->getPanelText()->setText(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_QUEST_ACHIEVED), false);
				}
				else if(TFW_IS_FLAG(qsta->getFlags(), QuestArticle::F_FINISHED))
				{
					_pnlStat->getPanelText()->setText(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_QUEST_FINISHED), false);
				}
				else
				{
					_pnlStat->getPanelText()->setText(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_QUEST_CANCELED), false);
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void QuestListWindow::draw(const RenderCtx* rc)
{
	ListWindowBase::draw(rc);

	if(isShowWindow())
	{
		// No Data 表示
		if(_listLitem->getCount() <= 0)
		{
			const int FONTSIZE = 16;
			rc->getRenderer()->setSolidColor(255, 255, 255, 255);
			EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_QUEST_NO_DATA), X_NO_DATA, Y_NO_DATA, W_NO_DATA, H_NO_DATA, FONTSIZE, EasyStringDrawer::F_ALIGN_CENTER, rc);
		}

		// サイドプロパティの描画
		s32 idxFocused = getFocusedIndex();	///getSelectedIndex();
		if(idxFocused != -1)	// フォーカスされているアイテムがある
		{
			const Litem* litem = _listLitem->getObject(idxFocused);
			const QuestArticle* qsta = (litem != 0L) ? (QuestArticle*)litem->_obj : 0L;
			const QuestDef* qstdf = (qsta != 0L) ? qsta->getDef() : 0L;
			if(qstdf != 0L)
			{
				Renderer* rdr = rc->getRenderer();
				rdr->bindTexture(_texRef);

				RectF32 rect(RECT_SIDE_PROP_BG);
				RectF32 uv;

				// 地の描画
				for(int i = 0; i < NUM_TRDP; i++)
				{
					_trdraw[i]->draw(rc, &rect);
				}

				// アイコンの描画
				if(qsta->getStat() == QuestArticle::STAT_ACCEPT)
				{
					RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&rect, RECT_SIDE_PROP_ICON), RectF32::setout(&uv, UV_QSTSTAT_ICON_ACCEPTED));
				}
				else if(qsta->getStat() == QuestArticle::STAT_ACHVED)
				{
					RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&rect, RECT_SIDE_PROP_ICON), RectF32::setout(&uv, UV_QSTSTAT_ICON_ACHVED));
				}
				else if(TFW_IS_FLAG(qsta->getFlags(), QuestArticle::F_FINISHED))
				{
					RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&rect, RECT_SIDE_PROP_ICON), RectF32::setout(&uv, UV_QSTSTAT_ICON_FINISHED));
				}
				else
				{
					RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&rect, RECT_SIDE_PROP_ICON), RectF32::setout(&uv, UV_QSTSTAT_ICON_CANCELED));
				}

				// 状態テキストの描画
				_pnlStat->draw(rc);

				// 項目のタイトル
				const int FONTSIZE = 16;
				rdr->setSolidColor(127, 127, 127, 255);
				EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_QUEST_NAME), X_SIDE_PROP_NAME, Y_SIDE_PROP_NAME - FONTSIZE, FONTSIZE, rc);
				EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_QUEST_REQUISITE), X_SIDE_PROP_REQS, Y_SIDE_PROP_REQS - FONTSIZE, FONTSIZE, rc);
				EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_QUEST_REWARD), X_SIDE_PROP_RWRD, Y_SIDE_PROP_RWRD - FONTSIZE, FONTSIZE, rc);
				EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_QUEST_DIFFICULTY), X_SIDE_PROP_DFLV, Y_SIDE_PROP_DFLV - FONTSIZE, FONTSIZE, rc);
				EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_QUEST_CLIENT), X_SIDE_PROP_CLNT, Y_SIDE_PROP_CLNT - FONTSIZE, FONTSIZE, rc);

				// テキスト
				rdr->setSolidColor(95, 63, 63, 255);
				for(int i = 0; i < NUM_TDP; i++)
				{
					_strdraw[i]->draw(rc, 0, 0);
				}

				// 難易度
				VcString strWk;
				for(int i = 0; i < qstdf->getDifficultyLevel(); i++) { strWk += "\x81\x96"; }	// ＊
				EasyStringDrawer::draw(_fontRef, &strWk, X_SIDE_PROP_DFLV, Y_SIDE_PROP_DFLV, FONTSIZE, rc);
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	サブメニューノードクリック通知
**//*---------------------------------------------------------------------*/
bool QuestListWindow::onClickMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	if(!ListWindowBase::onClickMenuNode(ec, mtnodeCall, mtnodeParent, objCreateParam, objShowParam))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	サブメニューノードクリック通知
**//*---------------------------------------------------------------------*/
bool QuestListWindow::onClickSubMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
#if 0
	const VcString* nameNode = mtnodeCall->getName(); ASSERT(nameNode != 0L);
	VcString title(*GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_QUEST) + " - " + *mtnodeParent->getName());
	if(nameNode->equals("Show"))		// 見る
	{
		// 見るモードへ
		ListWindowBase::setMode(LWMODE_SHOW, &title);
		_ilwmode = ILWMODE_SHOW;
		return true;
	}
	else if(nameNode->equals("Sort"))	// 並び替える
	{
		// 並び替えモードへ
		ListWindowBase::setMode(LWMODE_SORT, &title);
		_ilwmode = ILWMODE_SORT;
		return true;
	}
	else if(nameNode->equals("Throw"))	// 捨てる
	{
		// 捨てるモードへ
		ListWindowBase::setMode(LWMODE_SHOW, &title);
		_ilwmode = ILWMODE_THROW;
		return true;
	}
#endif
	return false;
}

/*---------------------------------------------------------------------*//**
	言語変更通知
**//*---------------------------------------------------------------------*/
void QuestListWindow::notifyChangeLanguage()
{
	ListWindowBase::notifyChangeLanguage();

	HeaderPanel* hpnl = headerPanel();
	if(hpnl != 0L)
	{
		hpnl->changeGuiString(
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_ITEM_LIST),
			0L,
			0L,
			0L	);
	}
}

/*---------------------------------------------------------------------*//**
	表示状態の変更通知
**//*---------------------------------------------------------------------*/
void QuestListWindow::onShowWindow(bool isShow)
{
	s32 numElm = 0;
	if(isShow)
	{
		// クエストマネージャを得る
		_qstmngRef = Game::getGame()->getQuestManager(); ASSERT(_qstmngRef != 0L);

		// リストを作成
		numElm = makeList();

		// 初期設定
		if(_qlwcase == QLWCASE_MENU)
		{
			_fileidSubMenu = FILEID_CONV_LANG(MENU_MENU_IM_ITEMLISTSUB_EN_XML);	// サブメニューのファイル ID を設定する
			ListWindowBase::setMode(LWMODE_SHOW, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_ITEM_LIST));	// デフォルトモードの設定
		}
	}

	// 親クラスでメニューが作成される
	ListWindowBase::onShowWindow(isShow);

	// 最初の要素を選択する
	if(isShow && (numElm > 0))
	{
		setSelectedIndex(0);
	}
}

/*---------------------------------------------------------------------*//**
	リストを作成する
**//*---------------------------------------------------------------------*/
s32 QuestListWindow::makeList()
{
	s32 cnt = 0;

	_listLitem->removeAll();
	_paramGameMenuCreate = GameMenuCreateParam::CASE_INGAME_QSTL;
	for(s32 order = 1; order <= _qstmngRef->getQuestNum(); order++)
	{
		bool isFound = false;
		for(s32 iq = 0; iq < _qstmngRef->getQuestNum(); iq++)
		{
			QuestArticle* qsta = _qstmngRef->questByIndex(iq);
			if(qsta->getMenuOrder() == order)
			{
				isFound = true;
				cnt++;
				ASSERT(qsta->isValid());
				ASSERT(TFW_IS_FLAG(qsta->getFlags(), QuestArticle::F_LISTED));

				const QuestDef* qstdf = qsta->getDef();
				ASSERT((qstdf != 0L) && (qstdf->getNameText() != 0L));
				Litem* litem = new Litem();
				litem->_obj = qsta;
				litem->_ridx = iq;
				litem->_name->set(qstdf->getNameText());
				_listLitem->addTail(litem);
				break;
			}
		}
		if(!isFound)	{	break;	}	// 以降、項目無し
	}

	return cnt;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
