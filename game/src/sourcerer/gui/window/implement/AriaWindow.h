/***********************************************************************//**
 *	AriaWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/09.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_ARIA_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_ARIA_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../etk/gui/window/Window.h"

////////////////////////////////////////////////////////////////////////////
// マクロ

#define OLD_ARIAWND_POSITION 0

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Point;
typedef Point<f32> PointF32;
class Font;
class StringDrawer;
class Texture;
class TiledRectDraw;
class TxtBox;

TFW_END_NS

SRCR_BEGIN_NS

class Souma;

/*---------------------------------------------------------------------*//**
 *	詠唱ウインドウ
 *
**//*---------------------------------------------------------------------*/
class AriaWindow : public Window
{
	//======================================================================
	// 定数
public:
	// モード
	enum ModeKind
	{
		MODE_NULL,
		MODE_RAW,
		MODE_SPELL,
	};

	// フェード値
	static const s32	FCNT_WINDOWS_FADE	= 30;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 終了済みかを得る
	inline bool isDone() const { return _isDone; }
	// 入力完了したかを得る
	inline bool isInputDone() const { return _isInputDone; }
	// エネルギー状態の表示位置を得る
	inline const PointF32* getEleneStatPos() const { return _ptGaugePos; }
	// モード取得
	inline ModeKind getMode() const { return _mode; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setMode(ModeKind mode);
	bool isEmptySourceCode() const;
	void setSourceCode(const VcString* str, bool isCursorReset);
	void setActiveCode(u32 idxBegin, u32 idxEnd);
	void setSouma(Souma* souma);

	void setRectangle(const RectF32* rect);
	void setPosition(const PointF32* pt);
	void setEnableBreakButton(bool isEnable);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	AriaWindow();
	~AriaWindow();
	bool create(Font* fontRef, Texture* texRef, ModeKind mode);
	void destroy();
	void execContents(ExecRes* res, const ExecCtx* ec);
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);

private:
	bool createRawModeObject();
	void deleteRawModeObject();
	bool createSpellModeObject();
	void deleteSpellModeObject();
	void onShowWindow(bool isShow);
	const VcString* getSourceCode() const;

	//======================================================================
	// メンバ変数
private:
	ModeKind			_mode;				// 動作モード
	Texture*			_texGuiRef;			// GUI テクスチャへの参照
	Font*				_fontRef;			// フォントへの参照
	TiledRectDraw*		_trdrawBg;			// 背景描画
	TxtBox*				_txtboxSrc;			// ソーステキストボックス
	StringDrawer*		_strdrawSpell;		// 呪文モード時の文字列描画
	Panel*				_pnlBreak;			// 中断ボタン
	VcString*			_strSpell;			// 呪文モード時の文字列
	bool				_isDone;			// 終了済み
	bool				_isInputDone;		// 入力済み
	bool				_isUserOp;			// ユーザー操作かどうか
	char				_cLastSentKey;		// 最終キー
	PointF32*			_ptTapStartPos;		// タップ開始時のウインドウ位置
	PointF32*			_ptGaugePos;		// ゲージ表示位置
	s32					_hnumChip;			// 横方向のテクスチャチップ数
	s32					_vnumChip;			// 縦方向のテクスチャチップ数

	Souma*				_soumaRef;			// ソウマへの参照
#if OLD_ARIAWND_POSITION
	bool				_isLeftPosition;	// 左位置表示
	s32					_ecntChPosition;	// 自動位置変更カウンタ
#endif
	bool				_isDefaultPosition;	// 既定位置にいる
	u8					_alphaFadeCol;		// フェードアルファ値
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_ARIA_WINDOW_H_
