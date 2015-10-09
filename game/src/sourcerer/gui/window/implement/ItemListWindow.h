/***********************************************************************//**
 *	ItemListWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/08.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_ITEM_LIST_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_ITEM_LIST_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../ListWindowBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class StringDrawer;

TFW_END_NS

SRCR_BEGIN_NS

class Item;
class ItemManager;

/*---------------------------------------------------------------------*//**
 *	アイテム リスト ウインドウ
 *
**//*---------------------------------------------------------------------*/
class ItemListWindow : public ListWindowBase
{
	//======================================================================
	// 定数
public:
	// 使用場面
	enum IlwCase
	{
		ILWCASE_NULL,
		ILWCASE_SELECT,	// 選択モード
		ILWCASE_MENU,	// メニューモード
	};

	// 動作フラグ
	static const u32 AF_STONE_ONLY		= 0x00000001;	// 石のみ選択可能

private:
	// テキスト描画パーツ
	enum TdParts
	{
		TDP_NAME,
		NUM_TDP
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 選択されたアイテムを得る
	Item* getSelectedItem() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// 使用場面設定
	void setCase(IlwCase ilwcase, u32 aflags) {	_ilwcase = ilwcase; _aflags = aflags; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ItemListWindow();
	~ItemListWindow();
	bool create(Font* fontRef, Texture* texRef);
	void destroy();
	void execContents(ExecRes* res, const ExecCtx* ec);
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);
	void notifyChangeLanguage();

	bool onClickMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);
	bool onClickSubMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);

protected:
	bool onReplaceLitem(s32 index1, s32 index2);
	bool onDeleteCheck(s32 indexDelete) const;
	bool onDeleteLitem(s32 indexDelete);
	void onUpdateLitem(Litem* litem);
	void onShowWindow(bool isShow);

private:
	Item* getItemFromListIndex(s32 index) const;
	void makeList();
	void setupLitem(Litem* litem, s32 ridx, Item* item) const;

	//======================================================================
	// メンバ変数
private:
	ItemManager* _itmmngRef;
	IlwCase _ilwcase;
	StringDrawer* _strdraw[NUM_TDP];
	s32 _idxFocusedRem;
	u32 _aflags;		// 動作フラグ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_ITEM_LIST_WINDOW_H_
