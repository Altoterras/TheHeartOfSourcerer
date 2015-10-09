/***********************************************************************//**
 *	ItemListWindow.cpp
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
#include "ItemListWindow.h"

// Friends
#include "../../menu/GameMenuCreateParam.h"
#include "../../panel/HeaderPanel.h"
#include "../../../episode/party/MyParty.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../item/Item.h"
#include "../../../item/ItemManager.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/MoveMap.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../unit/char/pc/PcUnit.h"
#include "../../../status/StatusDrawer.h"

// External
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/g2d/StringDrawer.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/lib/TypeUtils.h"
#include "../../../../tfw/smenu/MenuTreeNode.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ

#define RECT_SIDE_PROP_BG			0.0f, 48.0f, 128.0f, 272.0f
#define UV_SIDE_PROP_BG				0.0f / W_TEX, 448.0f / H_TEX, 128.0f / W_TEX, 272.0f / H_TEX

#define RECT_SIDE_PROP_ICON			5.0f, 60.0f, 40.0f, 40.0f
#define UV_ITEM_ICON_GRASS_L		 928.0f / W_TEX, 48.0f / H_TEX, 40.0f / W_TEX, 40.0f / H_TEX
#define UV_ITEM_ICON_STONE_L		 968.0f / W_TEX, 48.0f / H_TEX, 40.0f / W_TEX, 40.0f / H_TEX
#define UV_ITEM_ICON_WEAPON_L		 928.0f / W_TEX, 88.0f / H_TEX, 40.0f / W_TEX, 40.0f / H_TEX
#define UV_ITEM_ICON_SHIELD_L		 968.0f / W_TEX, 88.0f / H_TEX, 40.0f / W_TEX, 40.0f / H_TEX

#define X_SIDE_PROP_NAME			5
#define Y_SIDE_PROP_NAME			100
#define W_SIDE_PROP_NAME			108
#define H_SIDE_PROP_NAME			48
#define X_SIDE_PROP_ELENE			5.0f
#define Y_SIDE_PROP_ELENE			150.0f

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ItemListWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	選択されたアイテムを得る
**//*---------------------------------------------------------------------*/
Item* ItemListWindow::getSelectedItem() const
{
	if(getSelectedIndex() < 0)	{	return 0L;	}
	return getItemFromListIndex(getSelectedIndex());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ItemListWindow::ItemListWindow()
	: _itmmngRef(0L)
	, _ilwcase(ILWCASE_NULL)
	, _idxFocusedRem(-1)
	, _aflags(0)
{
	for(int i = 0; i < NUM_TDP; i++) { _strdraw[i] = 0L; }
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ItemListWindow::~ItemListWindow()
{
#if defined(_DEBUG)
	for(int i = 0; i < NUM_TDP; i++) { ASSERT(_strdraw[i] == 0L); }
#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ItemListWindow::create(Font* fontRef, Texture* texRef)
{
	// ウインドウの作成
	if(!ListWindowBase::create(fontRef, texRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_ITEM_LIST)))
	{
		return false;
	}

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
	RectF32 rect(X_SIDE_PROP_NAME, Y_SIDE_PROP_NAME, W_SIDE_PROP_NAME, H_SIDE_PROP_NAME);
	_strdraw[TDP_NAME]->setRectangle(&rect);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ItemListWindow::destroy()
{
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
void ItemListWindow::execContents(ExecRes* res, const ExecCtx* ec)
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
				const Item* item = getItemFromListIndex(_idxFocusedRem);
				const ItemDef* itmdf = (item != 0L) ? item->getItemDef() : 0L;
				_strdraw[TDP_NAME]->setString(((itmdf != 0L) && (itmdf->getName(Env_getLangId()) != 0L)) ? itmdf->getName(Env_getLangId()) : TypeUtils::getEmptyString());
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void ItemListWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	// サイドプロパティの描画
	s32 idxFocused = getFocusedIndex();	///getSelectedIndex();
	if(idxFocused != -1)	// フォーカスされているアイテムがある
	{
		const Item* item = getItemFromListIndex(idxFocused);
		const ItemDef* itmdf = (item != 0L) ? item->getItemDef() : 0L;
		if(itmdf != 0L)
		{
			Renderer* rdr = rc->getRenderer();
			rdr->bindTexture(_texRef);

			// 地の描画
			RectF32 rect;
			RectF32 uv;
			RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&rect, RECT_SIDE_PROP_BG), RectF32::setout(&uv, UV_SIDE_PROP_BG));

			// アイテムアイコンの描画
			if((ItemDef::CAT_STONE <= itmdf->getCategory()) && (itmdf->getCategory() <= ItemDef::CAT_SHIELD))
			{
				const RectF32 uvItemIcon[4] =
				{
					RectF32( UV_ITEM_ICON_STONE_L ),
					RectF32( UV_ITEM_ICON_GRASS_L ),
					RectF32( UV_ITEM_ICON_WEAPON_L ),
					RectF32( UV_ITEM_ICON_SHIELD_L ),
				};
				RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&rect, RECT_SIDE_PROP_ICON), &uvItemIcon[itmdf->getCategory() - 2]);
			}

			// 元素バランス
			const EleneStat* eestat = item->getEleneStat();
			StatusDrawer::drawEleneStat(rc, _texRef, eestat, X_SIDE_PROP_ELENE, Y_SIDE_PROP_ELENE, 90, 45, false);

			// アイテム名
			rdr->setSolidColor(0, 0, 0, 255);
			for(int i = 0; i < NUM_TDP; i++)
			{
				_strdraw[i]->draw(rc, 0, 0);
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	サブメニューノードクリック通知
**//*---------------------------------------------------------------------*/
bool ItemListWindow::onClickMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
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
bool ItemListWindow::onClickSubMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	const VcString* nameNode = mtnodeCall->getName(); ASSERT(nameNode != 0L);
	VcString title(*GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_ITEM_LIST) + " - " + *mtnodeParent->getName());
	if(nameNode->equals("Show"))			// 見る
	{
		// 見るモードへ
		ListWindowBase::setMode(LWMODE_SHOW, &title);
		return true;
	}
	if(nameNode->equals("Sort"))			// 並び替える
	{
		// 並び替えモードへ
		ListWindowBase::setMode(LWMODE_SORT, &title);
		return true;
	}
	if(nameNode->equals("Throw"))			// 捨てる
	{
		// 削除モードへ
		ListWindowBase::setMode(LWMODE_DELETE, &title);
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	リストアイテム削除チェック
**//*---------------------------------------------------------------------*/
bool ItemListWindow::onDeleteCheck(s32 indexDelete) const
{
	if((indexDelete < 0) || (_listLitem->getCount() <= (u32)indexDelete))
	{
		return false;
	}
	const Item* item = getItemFromListIndex(indexDelete);
	if(item == 0L)
	{
		return false;
	}
	return item->getItemDef() != 0L;
}

/*---------------------------------------------------------------------*//**
	リストアイテム削除通知
**//*---------------------------------------------------------------------*/
bool ItemListWindow::onDeleteLitem(s32 indexDelete)
{
	if((indexDelete < 0) || (_listLitem->getCount() <= (u32)indexDelete))
	{
		return false;
	}

	// アイテムを捨てる
	const Litem* litem = _listLitem->getObject(indexDelete);
	if(litem == 0L)	{	return false;	}
	const Item* item = (Item*)litem->_obj;
	Map* map = Game::getGame()->getMap(); ASSERT(map != 0L);
	const Unit* unit = Game::getGame()->getMyParty()->getLeaderPcUnit(); ASSERT(unit != 0L);
	if(_itmmngRef->throwItem(item->getItemGenId(), map, unit))
	{
		// 間をつめる
		for(s32 i = litem->_ridx; i < _itmmngRef->getItemNum() - 1; i++)
		{
			Item* item1 = _itmmngRef->item(i);
			if(!item1->isNoEdit() && !item1->isEquiped())
			{
				for(s32 j = i + 1; j < _itmmngRef->getItemNum(); j++)
				{
					Item* item2 = _itmmngRef->item(j);
					if(!item2->isNoEdit() && !item2->isEquiped())
					{
						if(!item1->isEmpty() || !item2->isEmpty())
						{
							item1->copy(item2);
							item2->clear();
						}
						break;
					}
				}
			}
		}
		return true;
	}
	else
	{
		// エラー
		GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
		VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_ERROR_ERROR_ITEM_THROW)->getRaw(), ""));
		wndSysMsg->showStdMsg(&msg);	// メッセージ表示
		return false;
	}
}

/*---------------------------------------------------------------------*//**
	リストアイテム置換通知
**//*---------------------------------------------------------------------*/
bool ItemListWindow::onReplaceLitem(s32 index1, s32 index2)
{
	// アイテムの置換
	Item* item1 = getItemFromListIndex(index1);
	Item* item2 = getItemFromListIndex(index2);
	if((item1 == 0L) || (item2 == 0L))	{	return false;	}
	Item itemTmp;
	itemTmp.copy(item1);
	item1->copy(item2);
	item2->copy(&itemTmp);

	// リストアイテムの置換処理
	Litem* litem1 = _listLitem->getObject(index1);
	Litem* litem2 = _listLitem->getObject(index2);
	litem1->_name->set((item2->getItemDef() != 0L) ? item2->getItemDef()->getName(Env_getLangId()) : TypeUtils::getEmptyString());
	litem2->_name->set((item1->getItemDef() != 0L) ? item1->getItemDef()->getName(Env_getLangId()) : TypeUtils::getEmptyString());

	return true;
}

/*---------------------------------------------------------------------*//**
	言語変更通知
**//*---------------------------------------------------------------------*/
void ItemListWindow::notifyChangeLanguage()
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
	アイテム更新通知
**//*---------------------------------------------------------------------*/
void ItemListWindow::onUpdateLitem(Litem* litem)
{
	setupLitem(litem, litem->_ridx, (Item*)litem->_obj);
}

/*---------------------------------------------------------------------*//**
	表示状態の変更通知
**//*---------------------------------------------------------------------*/
void ItemListWindow::onShowWindow(bool isShow)
{
	if(isShow)
	{
		// アイテムマネージャを得る
		_itmmngRef = Game::getGame()->getMyParty()->getItemManager(); ASSERT(_itmmngRef != 0L);

		// リストを作成
		makeList();

		// 初期設定
		if(_ilwcase == ILWCASE_MENU)
		{
			_fileidSubMenu = FILEID_CONV_LANG(MENU_MENU_IM_ITEMLISTSUB_EN_XML);	// サブメニューのファイル ID を設定する
			ListWindowBase::setMode(LWMODE_SHOW, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_ITEM_LIST));	// デフォルトモードの設定
		}
		else
		{
			_fileidSubMenu = 0;	// サブメニューのファイル ID を設定する
			ListWindowBase::setMode(LWMODE_USE, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_ITEM_LIST));	// デフォルトモードの設定
		}
	}

	// 親クラスでメニューが作成される
	ListWindowBase::onShowWindow(isShow);
}

/*---------------------------------------------------------------------*//**
	リストインデックスからアイテムを得る
**//*---------------------------------------------------------------------*/
Item* ItemListWindow::getItemFromListIndex(s32 index) const
{
	if((index < 0) || (_listLitem->getCount() <= (u32)index))	{	return 0L;	}
	const Litem* litem = _listLitem->getObject(index);
	if(litem == 0L)	{	return 0L;	}
	return (Item*)litem->_obj;
}

/*---------------------------------------------------------------------*//**
	リストを作成する
**//*---------------------------------------------------------------------*/
void ItemListWindow::makeList()
{
	_listLitem->removeAll();
	_paramGameMenuCreate = GameMenuCreateParam::CASE_INGAME_ITEML;
	for(s32 i = 0; i < _itmmngRef->getItemNum(); i++)
	{
		Item* item = _itmmngRef->item(i);
		Litem* litem = new Litem();
		setupLitem(litem, i, item);
		_listLitem->addTail(litem);
	}
}

/*---------------------------------------------------------------------*//**
	リストアイテムの情報を設定する
**//*---------------------------------------------------------------------*/
void ItemListWindow::setupLitem(Litem* litem, s32 ridx, Item* item) const
{
	const ItemDef* itmdf = item->getItemDef();

	litem->_ridx = ridx;
	litem->_obj = item;
	TFW_SET_FLAG(litem->_liflags, Litem::LIF_NOEDIT, item->isNoEdit() || item->isEquiped());
	if(itmdf != 0L)	// アイテムが割り当てられている場合
	{
		ASSERT(itmdf->getName(Env_getLangId()) != 0L);
		litem->_name->set(itmdf->getName(Env_getLangId()));

		if(TFW_IS_FLAG(_aflags, AF_STONE_ONLY) && (itmdf->getCategory() != ItemDef::CAT_STONE))
		{
			TFW_SET_FLAG(litem->_liflags, Litem::LIF_NOEDIT, true);
		}
	}
	else
	{
		litem->_name->set(_strEmpty);

		if(TFW_IS_FLAG(_aflags, AF_STONE_ONLY))
		{
			TFW_SET_FLAG(litem->_liflags, Litem::LIF_NOEDIT, true);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
