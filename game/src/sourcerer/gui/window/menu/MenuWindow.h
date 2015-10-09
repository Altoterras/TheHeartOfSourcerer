/***********************************************************************//**
 *	MenuWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/05.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_MENU_MENU_WINDOW_H_
#define _SRCR_GUI_WINDOW_MENU_MENU_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../etk/gui/window/Window.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{
	template<class TYPE> class Vector4;
	typedef Vector4<f32> Vector4F;
	template<class TYPE> class Rect;
	typedef Rect<f32> RectF32;
	typedef Rect<f32> RectF32;
	template<class TYPE> class Size;
	typedef Size<f32> SizeF32;
	template<class TYPE> class Point;
	typedef Point<f32> PointF32;
	template<class TYPE> class TreeNode;
	template<class TYPE> class List;
	class Font;
	class VcString;
	class Pad;
	class StringDrawer;
	class Texture;
	class TouchPanel;
	namespace smenu
	{
		class MenuExecRes;
		class MenuTreeNode;
	}
}
using namespace smenu;

SRCR_BEGIN_NS

class GameMenuCreateParam;
class ScrollBar;

/*---------------------------------------------------------------------*//**
 *	メニュー ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class MenuWindow : public Window
{
	//======================================================================
	// 定数
protected:
	// 制御フラグ
	static const u32	EF_NO_SELF_CANCEL		= 0x00000001;
	static const u32	EF_REVERSE_DRAW			= 0x00000002;
	static const u32	EF_FOR_USE				= 0x00000004;

private:
	static const u32	COL_EXTLINK				= 0x00010000;
	static const f32	X_SCRBARWD;
	static const f32	W_SCRBAR_PAD;

	//======================================================================
	// クラス
protected:
	class Block
	{
	public:
		static const u16 TF_TITLE	= 0x0100;	// タイトル
		static const u16 TF_TAIL	= 0x0200;	// 最終要素

		#define MWBLOCK_FLAG_STATE	0	// ブロックに状態フラグを保持する（古い仕様）［無効化 r-kishi 2012/09/09］
#if MWBLOCK_FLAG_STATE
		static const u16 F_UNUSED	= 0x0001;	// 使用不能
		static const u16 F_HIDE		= 0x0002;	// 隠し
		static const u16 F_DISABLE	= 0x0004;	// 無効
#endif

	public:
		bool isDisabled() const;
		bool isUnused() const;
		bool isHidden() const;

	public:
		Block();
		~Block();
		bool create(const VcString* str, u32 flags, u64 param, TreeNode<MenuTreeNode>* tnode, s32 index);
		void destroy();
		void exec(const ExecCtx* ec);

		void moveDiff(f32 xDiff, f32 yDiff);
		void enableOffsetAnim(f32 deltaOfsAnim, f32 rateOfsAnim);
		void set(const VcString* str, u32 flags, u64 param, const MenuTreeNode* mtnode);

	public:
		s32						_index;
		VcString*				_str;
		TreeNode<MenuTreeNode>*	_tnode;
		StringDrawer*			_strdraw;
		RectF32*				_rect;
		SizeF32*				_sizeTxt;
		Vector4F*				_color;
		u16						_typef;
		RectF32*				_rectOfs;
		RectF32*				_rectOfsAnim;
		f32						_deltaOfsAnim;
		f32						_rateOfsAnim;
		u64						_param;
		u16						_flags;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 有効かどうかを得る
	inline bool isAvailable() { return _isAvailable; }
	// 現在の位置を得る
	inline const PointF32* getCurPos() const { return _ptBase; }
	// 最大サイズを得る
	inline const SizeF32* getMaxSize() const { return _sizeMax; }
	// 現在フォーカスされている要素のインデックスを得る
	inline s32 getFocusedElementIndex() const { return _indexCur; }
	// 現在のスクロール位置を得る
	inline const PointF32* getCurScrollPos() const { return _ptScrollCur; }

	// 自己キャンセルを無効にする
	inline void setNoSelfCencel() { TFW_SET_FLAG(_eflags, EF_NO_SELF_CANCEL, true); }
	// 逆順描画を有効にする
	inline void setReverseDraw() {  TFW_SET_FLAG(_eflags, EF_REVERSE_DRAW, true); }
	// 選択使用モードに設定する
	inline void setForUse(bool is) {  TFW_SET_FLAG(_eflags, EF_FOR_USE, is); }
	// 現在フォーカスされている要素のインデックスを設定する
	inline void setFocusedElementIndex(s32 index) { _indexCur = index; }

protected:
	// フォントを得る
	inline Font* getFont() const { return _fontRef; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void move(PointF32* pos);

	s32 getElementNum();
	s32 getElementContentIndexByPoint(const PointF32* pt) const;
	s32 getElementContentIndexByNode(const MenuTreeNode* mtnode) const;
	bool setElementUnused(s32 index, bool isUnused);
	bool getElementRect(s32 index, RectF32* rect, RectF32* rectOfs, RectF32* rectOfsAnim);
	bool setElementRect(s32 index, const RectF32* rect, const RectF32* rectOfs, const RectF32* rectOfsAnim);
	bool setElementContent(s32 index, const VcString* str);
	bool setElementParam(s32 index, u64 param);
	bool replaceElementContent(s32 index1, s32 index2);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MenuWindow();
	virtual ~MenuWindow();
	virtual bool create(TreeNode<MenuTreeNode>* tnodeSrc, s32 depth, Font* font, s32 fontsize, Texture* tex, void* paramShow, void* paramCreate);
	virtual void destroy();

protected:
	virtual void execContents(ExecRes* res, const ExecCtx* ec);
	virtual void onTapRelease(MenuExecRes* cres, TouchPanel* ui, const PointF32* ptTouch, const PointF32* ptStart, Block* block);
	virtual void onDoubleTapRelease(MenuExecRes* cres, TouchPanel* ui, const PointF32* ptTouch, Block* block);
	virtual void onTouching(MenuExecRes* cres, TouchPanel* ui, const PointF32* ptTouch, Block* block);
	virtual void onDrag(MenuExecRes* cres, TouchPanel* ui, const PointF32* ptCur, const PointF32* ptPrev, const PointF32* ptStart);
	virtual void onFlip(MenuExecRes* cres, TouchPanel* ui, const PointF32* ptCur, const PointF32* ptStart, f32 speed);
	virtual void onPadOk(MenuExecRes* cres, Pad* ui);
	virtual void onPadCancel(MenuExecRes* cres, Pad* ui);
	virtual void onPadLeft(MenuExecRes* cres, Pad* ui);
	virtual void onPadUp(MenuExecRes* cres, Pad* ui);
	virtual void onPadRight(MenuExecRes* cres, Pad* ui);
	virtual void onPadDown(MenuExecRes* cres, Pad* ui);
	virtual void drawContents(const RenderCtx* rc, const RectF32* rectBase);
	virtual void drawBlock(const RenderCtx* rc, const Block* block, bool isFocus);
	virtual void onCreatePanel();
	virtual void onDestroyPanel();
	virtual bool make(s32 depth);
	virtual void makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth);
	virtual void remakeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, const SizeF32* sizeTxt);
	virtual bool adjustBlockString(VcString* str, RectF32* rectText, StringDrawer* strdraw, const RectF32* rectBlock);

protected:
	Block* getBlock(s32 index) const;
	Block* getBlock(const PointF32* pt) const;
	void scrollCurIndex();

	//======================================================================
	// メンバ変数
private:
	PointF32*				_ptBase;
	SizeF32*				_sizeMax;
	List<Block*>*			_listBlock;
	s32						_depth;
	bool					_isAvailable;
	s32						_indexCur;
	s32						_indexNum;
	s32						_cntAnim;
	PointF32*				_ptScrollTrg;
	PointF32*				_ptScrollCur;
	PointF32*				_ptScrollTapStart;
	bool					_isScorllBarKnobDrag;
	ScrollBar*				_scrlbar;
	f32						_velFlip;
	u32						_eflags;

protected:
	TreeNode<MenuTreeNode>*	_tnodeSrc;
	Font*					_fontRef;
	s32						_fontsize;
	Texture*				_texRef;
	GameMenuCreateParam*	_gmcpRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_MENU_MENU_WINDOW_H_
