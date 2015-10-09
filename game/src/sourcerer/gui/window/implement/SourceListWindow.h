/***********************************************************************//**
 *	SourceListWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/08.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_SOURCE_LIST_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_SOURCE_LIST_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../ListWindowBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class StringDrawer;

TFW_END_NS

SRCR_BEGIN_NS

class Source;
class SourceManager;

/*---------------------------------------------------------------------*//**
 *	ソース リスト ウインドウ
 *
**//*---------------------------------------------------------------------*/
class SourceListWindow : public ListWindowBase
{
	//======================================================================
	// 定数
public:
	// 使用場面
	enum SlwCase
	{
		SLWCASE_NULL,
		SLWCASE_SELECT,	// 選択モード
		SLWCASE_MENU,	// メニューモード
	};

private:
	// テキスト描画パーツ
	enum TdParts
	{
		TDP_PROPSTR,
		NUM_TDP
	};
	// ボタン
	enum Button
	{
		BTN_EDIT,
		BTN_SHOW,
		NUM_BTN
	};

	// 内部処理モード
	enum SlwInternalMode
	{
		SLWIMODE_NULL,
		SLWIMODE_EDIT,
		SLWIMODE_CONV_TO_JS,
		SLWIMODE_CONV_TO_SS,
		SLWIMODE_EDITING,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 選択されたソースを得る
	Source* getSelectedSource() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// 使用場面設定
	void setCase(SlwCase slwcase) {	_slwcase = slwcase;	}

	bool runSourceEditor(Source* source, bool isReadOnly);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SourceListWindow();
	~SourceListWindow();
	bool create(Font* fontRef, Texture* texRef);
	void destroy();
	void execContents(ExecRes* res, const ExecCtx* ec);
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);
	void notifyChangeLanguage();

protected:
	void onSubMenuShow(Menu* menuSub, MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam);
	bool onClickSubMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);
	bool takeConfirmMessage(VcString* msg, LwMode mode) const;
	bool onReplaceLitem(s32 index1, s32 index2);
	bool onCloneLitem(s32* indexShiftMax, s32 index);
	bool onDeleteCheck(s32 indexDelete) const;
	bool onDeleteLitem(s32 indexDelete);
	bool onEditCheck(s32 indexEdit) const;
	bool onEditLitem(s32 indexEdit);
	void onUpdateLitem(Litem* litem);
	void onShowWindow(bool isShow);

private:
	Source* getSourceFromListIndex(s32 index) const;
	void makeList();
	void setupLitem(Litem* litem, s32 ridx, Source* source) const;
	void updateLitem(const Source* source);
	void updateSidePropString(const Source* source);
	void makeLitemTitleString(VcString* name, const Source* source) const; 
	void doFinishSourceEditor(bool isSave);
	bool avoidOverlapedSourceName(Source* source);

	//======================================================================
	// メンバ変数
private:
	SourceManager* _srcmngRef;
	StringDrawer* _strdraw[NUM_TDP];
	Panel* _pnlBtn[NUM_BTN];
	SlwCase _slwcase; 
	SlwInternalMode _slwimodeLast;
	s32 _idxFocusedRem;
	Source* _sourceEditing;
	VcString* _strSideProp;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_SOURCE_LIST_WINDOW_H_
