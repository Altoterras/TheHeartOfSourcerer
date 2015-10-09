/***********************************************************************//**
 *	PcEquipWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PcEquipWindow.h"

// Friends
#include "../../../char/CharDef.h"
#include "../../../episode/party/MyParty.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/ItemListWindow.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../item/Item.h"
#include "../../../item/ItemDef.h"
#include "../../../item/ItemEquip.h"
#include "../../../item/ItemManager.h"
#include "../../../status/CharStat.h"
#include "../../../status/CharParam.h"
#include "../../../status/EquipStat.h"
#include "../../../status/StatusDrawer.h"
#include "../../../unit/char/pc/PcUnit.h"

// External
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Size.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gui/PanelText.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#if 1
#define X_BASE						((Game::getGame()->getLogicalWidth() - 400) / 2)
#define RECT_BTN_ITEM				X_BASE + 300, 108, 60, 24
#define X_ITEM						(X_BASE + 180.0f)
#define Y_ITEM						(70.0f)
#else
#define RECT_BTN_ITEM				340, 108, 60, 24
#endif

#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ
#define UV_ITEMBTN_NRM				928.0f / W_TEX, 264.0f / H_TEX, 60.0f / W_TEX, 24.0f / H_TEX
#define UV_ITEMBTN_HVR				928.0f / W_TEX, 288.0f / H_TEX, 60.0f / W_TEX, 24.0f / H_TEX

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PcEquipWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PcEquipWindow::PcEquipWindow()
{
	for(int i = 0; i < NUM_BTN; i++)
	{
		_pnlBtn[i] = 0;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PcEquipWindow::~PcEquipWindow()
{
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_BTN; i++)	{	ASSERT(_pnlBtn[i] == 0L);	}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PcEquipWindow::create(Font* fontRef, Texture* texRef)
{
	// ウインドウの作成
	if(!PcStatusWindowBase::create(fontRef, texRef))
	{
		return false;
	}

	RectF32 rect;
	RectF32 uv;
	ColorU8 col;

	// 子パネル作成
	{
		_pnlBtn[BTN_ITEM_ACS] = new Panel();
		_pnlBtn[BTN_ITEM_ACS]->create(RectF32::setout(&rect, RECT_BTN_ITEM));
		_pnlBtn[BTN_ITEM_ACS]->setStateInfo(STATE_NORMAL, _texRef, RectF32::setout(&uv, UV_ITEMBTN_NRM));
		_pnlBtn[BTN_ITEM_ACS]->setStateInfo(STATE_HOVER, _texRef, RectF32::setout(&uv, UV_ITEMBTN_HVR));
		_pnlBtn[BTN_ITEM_ACS]->createText(fontRef, 12, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_EQUIP_CHANGE), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
		_pnlBtn[BTN_ITEM_ACS]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
		_pnlBtn[BTN_ITEM_ACS]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
		_pnlBtn[BTN_ITEM_ACS]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
	}

	// キャラクタの無効化
	Party* party = Game::getGame()->getMyParty();
	setEnablePc(MyParty::PC_MY, party->isEnablePc(MyParty::PC_MY));
	setEnablePc(MyParty::PC_PTN, party->isEnablePc(MyParty::PC_PTN));
	setEnablePc(MyParty::PC_HLP, false);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PcEquipWindow::destroy()
{
	// 子パネル削除
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] != 0)
		{
			_pnlBtn[i]->destroy();
			delete _pnlBtn[i];
			_pnlBtn[i] = 0L;
		}
	}

	PcStatusWindowBase::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void PcEquipWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	// アイテム選択ウインドウのコントロール
	GameGui* gui = Game::getGame()->getGui(); ASSERT(gui != 0L);
	ItemListWindow* wndItemList = gui->getItemListWindow();
	if(wndItemList->isShowWindow())
	{
		if(wndItemList->isDone())
		{
			Item* item = wndItemList->getSelectedItem();
			if(item != 0L)
			{
				// アイテムを選択した
				MyParty* party = (MyParty*)Game::getGame()->getMyParty();
				if(item->isEmpty())
				{
					// 現状は PC1 のアクセサリーのみ変更可能である
					PcUnit* pcunit = (PcUnit*)party->pcUnit(MyParty::PC_MY);
					ItemEquip* itmeq = pcunit->itemEquip();
					itmeq->equip(EquipPlace::EQP_ACCSR, 0L, 0);		// 装備を外す
				}
				else
				{
					// 現状は PC1 のアクセサリーのみ変更可能である 
					PcUnit* pcunit = (PcUnit*)party->pcUnit(MyParty::PC_MY);
					ItemEquip* itmeq = pcunit->itemEquip();
					itmeq->equip(EquipPlace::EQP_ACCSR, item, 0);	// 装備変更
				}
				#if 0	// 将来的には石かどうかなどを判断して不可能な場合はエラーを出す
				else
				{
					// エラー
					GameSysMsgWindow* wndSysMsg = gui->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
					VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_ERROR_ITEM_FULL)->getRaw(), ""));
					wndSysMsg->showStdMsg(&msg);	// メッセージ表示
				}
				#endif
			}

			wndItemList->showWindow(false);
		}
		return;
	}

	// 親の制御
	PcStatusWindowBase::execContents(res, ec);

	// 子パネル制御
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
				case BTN_ITEM_ACS:
					// アイテムウインドウを表示する
					wndItemList->setCase(ItemListWindow::ILWCASE_SELECT, ItemListWindow::AF_STONE_ONLY);
					wndItemList->showWindow(true);
					wndItemList->setSelfClose();
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

	// 装備変更ボタンの有効／無効化
	_pnlBtn[BTN_ITEM_ACS]->setEnable(_ipc == MyParty::PC_MY);	// アクセサリ変更は PC1 のみ
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void PcEquipWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	PcStatusWindowBase::drawContents(rc, rectBase);

	Renderer* rdr = rc->getRenderer();
	const RectF32* rect = getRectangle();
	const Party* party = Game::getGame()->getMyParty();
	const PcUnit* pcunit = party->getPcUnit(_ipc);

	const ItemEquip* itmeq = pcunit->getItemEquip();
	const EquipStat* eqstat = (itmeq != 0L) ? itmeq->getEquipStat() : 0L;
	bool bitm[EquipPlace::NUM_EQP];
	const Item* itm[EquipPlace::NUM_EQP];
	for(int i = 0; i < EquipPlace::NUM_EQP; i++) 
	{
		bitm[i] = false;
		itm[i] = 0L;
	}
	switch(_ipc)
	{
	case MyParty::PC_MY:
		bitm[EquipPlace::EQP_ACCSR] = true;
		itm[EquipPlace::EQP_ACCSR] = eqstat->getEquipItem(EquipPlace::EQP_ACCSR);
		break;
	case MyParty::PC_PTN:
		bitm[EquipPlace::EQP_LHAND] = true;
		itm[EquipPlace::EQP_LHAND] = eqstat->getEquipItem(EquipPlace::EQP_LHAND);
		bitm[EquipPlace::EQP_RHAND] = true;
		itm[EquipPlace::EQP_RHAND] = eqstat->getEquipItem(EquipPlace::EQP_RHAND);
		break;
	}

	f32 x = X_ITEM;
	f32 y = Y_ITEM;
	for(int ieq = 0; ieq < EquipPlace::NUM_EQP; ieq++)
	{
		if(!bitm[ieq])	{	continue;	}
		rdr->setSolidColor(0, 0, 0, 255);
		EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_EQUIPPLACE_LHAND + ieq), x, y, 16, rc);
		RendererUtils::draw2dLine(rdr, x, y + 17, x + 180, y + 17);
		if(itm[ieq] != 0L)
		{
			const ItemDef* itmdef = itm[ieq]->getItemDef();
			if(itmdef != 0L)
			{
				const EleneStat* eestatItm = itm[ieq]->getEleneStat(); ASSERT(eestatItm != 0L);
				EasyStringDrawer::draw(_fontRef, itmdef->getName(Env_getLangId()), x + 16.0f, y + 20.0f, 16, rc);
				StatusDrawer::drawEleneStat(rc, _texRef, eestatItm, x + 16.0f, y + 38.0f, StatusDrawer::W_GAUGE_DEFAULT, StatusDrawer::H_ENEGAUGE_DEFAULT, false);
			}
		}
		else
		{
			EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_EQUIP_NOTHING), x + 16.0f, y + 20.0f, 16, rc);
		}
		y += 100.0f;
	}

	// 子パネル描画
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] != 0)	{	_pnlBtn[i]->draw(rc);	}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
