/***********************************************************************//**
 *	ListWindowBase.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/03.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_LIST_WINDOW_BASE_H_
#define _SRCR_GUI_WINDOW_LIST_WINDOW_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
#include "../../../etk/gui/window/Window.h"
#include "../../../tfw/smenu/MenuFuncTable.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class Font;
class Texture;
class StringDrawer;
namespace smenu
{
	class Menu;
	class MenuTreeNode;
}

TFW_END_NS
using namespace smenu;

SRCR_BEGIN_NS

class HeaderPanel;
class ItemManager;
class GameMenuShowParam;
class MenuWindow;

/*---------------------------------------------------------------------*//**
 *	アイテム リスト ウインドウ
 *
**//*---------------------------------------------------------------------*/
class ListWindowBase : public Window
{
	//======================================================================
	// 定数
protected:
	enum LwMode
	{
		LWMODE_NULL,
		LWMODE_SHOW,	// 見るモード
		LWMODE_USE,		// 使用モード
		LWMODE_SORT,	// 並び替えモード
		LWMODE_CLONE,	// 複製モード
		LWMODE_DELETE,	// 削除モード
		LWMODE_EDIT,	// 編集モード
		LWMODE__CUSTOM_	// 派生クラス用カスタムモード
	};

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	結果値
	 *
	**//*-----------------------------------------------------------------*/
	class Result
	{
		//==================================================================
		// 定数
	private:
		static const u32 F_YES	= 0x00000001;
		static const u32 F_NO	= 0x00000002;

		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// 選択されたインデックスを得る
		inline s32 getSelectedIndex() const { return _idxSelected; }

		// 選択されたインデックスを設定する
		inline void setSelectedIndex(s32 idxSelected) { _idxSelected = idxSelected; }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		Result() { reset(); }
		void reset() { _idxSelected = -1; }

		//==================================================================
		// 変数
	private:
		s32 _idxSelected;
	};

protected:
	/*-----------------------------------------------------------------*//**
	 *	リストアイテム
	 *	
	**//*-----------------------------------------------------------------*/
	class Litem
	{
	public:
		static const u32 LIF_NOEDIT	= 0x00000001;

	public:
		VcString* _name;		// 名称
		void* _obj;				// オブジェクトへの汎用ポインタ
		s32 _ridx;				// 実リストのインデックス
		u32 _liflags;			// フラグ

	public:
		Litem();
		~Litem();
		void copy(const Litem* src);
	};

	/*-----------------------------------------------------------------*//**
	 *	メニュー機能テーブル
	 *	
	**//*-----------------------------------------------------------------*/
	class MenuFuncTable_ListBase : public smenu::MenuFuncTable
	{
		//==================================================================
		// メソッド
	public:
		void onShow(Menu* menu, MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam);
		void onHide(Menu* menu, void* objCreateParam, void* objShowParam);
		bool onCall(Menu* menu, const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);
		void onClickOutside(Menu* menu, const ExecCtx* ec, void* objCreateParam, void* objShowParam);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 選択インデックスを得る
	inline s32 getSelectedIndex() const { return _idxSelected; }
	// フォーカスインデックスを得る
	s32 getFocusedIndex() const;
	// 動作を停止しているかどうかを得る
	inline bool isDone() const { return _isDone; }
	// ヘッダーパネルを得る
	inline const HeaderPanel* getHeaderPanel() const { return _hpnlHbar; }
	inline HeaderPanel* headerPanel() const { return _hpnlHbar; }
	// メニューを得る
	inline const Menu* getListMenu() const { return _menuList; }
	inline Menu* listMenu() const { return _menuList; }
	// 結果値を得る
	inline const Result* getResult() const { return &_res; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setDone()							{	_isDone = true;					}
	void setMode(u8 mode, const VcString* title);
	void setSelfClose()						{	_isSelfClose = true;			}
	void setSelectedIndex(s32 index);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ListWindowBase();
	virtual ~ListWindowBase();
	virtual bool create(Font* fontRef, Texture* texRef, const VcString* titleDefault);
	virtual void destroy();
	virtual void execContents(ExecRes* res, const ExecCtx* ec);
	virtual void draw(const RenderCtx* rc);
	virtual void notifyChangeLanguage();

protected:
	virtual void onSubMenuShow(Menu* menuSub, MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam) {}
	virtual bool onClickMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);
	virtual bool onClickSubMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam) { return true; }
	void onClickOutside(const ExecCtx* ec, void* objCreateParam, void* objShowParam);

private:
	bool createMenu();
	void destroyMenu();
	void setListState();
	void selectSortFirst();
	void doSort(MenuTreeNode* mtnodeParent);
	void cancelSort(s32 idxSortSelected2);
	void confirmClone();
	void doClone();
	void confirmDelete();
	void doDelete();
	void confirmEdit();
	void doEdit();

protected:
	void showYesNoMessageWindow(const VcString* msg);
	void updateLitem(s32 idxLitem);
	virtual bool takeConfirmMessage(VcString* msg, LwMode mode) const { return false; }
	virtual bool onReplaceLitem(s32 index1, s32 index2) { return true; }
	virtual bool onCloneCheck(s32 indexClone) const { return true; }
	virtual bool onCloneLitem(s32* indexShiftMax, s32 indexClone) { return true; }
	virtual bool onDeleteCheck(s32 indexDelete) const { return true; }
	virtual bool onDeleteLitem(s32 indexDelete) { return true; }
	virtual bool onEditCheck(s32 indexEdit) const { return true; }
	virtual bool onEditLitem(s32 indexEdit) { return true; }
	virtual void onUpdateLitem(Litem* litem) {}
	virtual void onShowWindow(bool isShow);

	//======================================================================
	// メンバ変数
protected:
	List<Litem*>* _listLitem;
	VcString* _strEmpty;
	u32 _fileidSubMenu;
	s32 _paramGameMenuCreate;
	Font* _fontRef;
	Texture* _texRef;
	u8 _lwmode;

private:
	HeaderPanel* _hpnlHbar;
	Menu* _menuList;
	Menu* _menuSub;
	GameMenuShowParam* _msprmList;
	GameMenuShowParam* _msprmSub;
	bool _isDone;
	bool _isDoneReserved;
	bool _isSelfClose;
	s32 _idxSelected;
	s32 _idxSortSelected;
	Result _res;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_LIST_WINDOW_BASE_H_
