/***********************************************************************//**
 *	SoftKeyboardWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_SOFT_KEYBOARD_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_SOFT_KEYBOARD_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../etk/gui/window/Window.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Font;
class Keyboard;
class Texture;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	メッセージウインドウ
 *
**//*---------------------------------------------------------------------*/
class SoftKeyboardWindow : public Window
{
	//======================================================================
	// 定数
public:
	// ボタン
	enum Button
	{
		BTN_NORMAL__START_,
		BTN_SMALL__START_ = BTN_NORMAL__START_,
		BTN_1 = BTN_NORMAL__START_,
		BTN_2,
		BTN_3,
		BTN_4,
		BTN_5,
		BTN_6,
		BTN_7,
		BTN_8,
		BTN_9,
		BTN_0,
		BTN_HYP,						// -
		BTN_HAT,						// ^
		BTN_Q_S,
		BTN_W_S,
		BTN_E_S,
		BTN_R_S,
		BTN_T_S,
		BTN_Y_S,
		BTN_U_S,
		BTN_I_S,
		BTN_O_S,
		BTN_P_S,
		BTN_ATM,						// @
		BTN_SBRK_B,						// [
		BTN_A_S,
		BTN_S_S,
		BTN_D_S,
		BTN_F_S,
		BTN_G_S,
		BTN_H_S,
		BTN_J_S,
		BTN_K_S,
		BTN_L_S,
		BTN_SEM,						// ;
		BTN_COL,						// :
		BTN_SBRK_E,						// ]
		BTN_Z_S,
		BTN_X_S,
		BTN_C_S,
		BTN_V_S,
		BTN_B_S,
		BTN_N_S,
		BTN_M_S,
		BTN_COM,						// ,
		BTN_PERI,						// .
		BTN_SLA,						// /
		BTN_BSLA,						// バックスラッシュ
		BTN_YEN,						// ￥マーク
		BTN_SMALL__END_ = BTN_YEN,

		BTN_LARGE__START_,
		BTN_EXP = BTN_LARGE__START_,	// !
		BTN_QUOT,						// "
		BTN_SHARP,						// #
		BTN_DOL,						// $
		BTN_PER,						// %
		BTN_AMP,						// &
		BTN_APOS,						// '
		BTN_PARE_B,						// (
		BTN_PARE_E,						// )
		BTN_EQ,							// =
		BTN_TILD,						// ~
		BTN_Q_L,
		BTN_W_L,
		BTN_E_L,
		BTN_R_L,
		BTN_T_L,
		BTN_Y_L,
		BTN_U_L,
		BTN_I_L,
		BTN_O_L,
		BTN_P_L,
		BTN_GACC,						// `
		BTN_CBRK_B,						// {
		BTN_A_L,
		BTN_S_L,
		BTN_D_L,
		BTN_F_L,
		BTN_G_L,
		BTN_H_L,
		BTN_J_L,
		BTN_K_L,
		BTN_L_L,
		BTN_PLUS,						// +
		BTN_ASTA,						// *
		BTN_CBRK_E,						// }
		BTN_Z_L,
		BTN_X_L,
		BTN_C_L,
		BTN_V_L,
		BTN_B_L,
		BTN_N_L,
		BTN_M_L,
		BTN_LT,							// <
		BTN_GT,							// >
		BTN_QUES,						// ?
		BTN_UDSC,						// _
		BTN_VBAR,						// |
		BTN_LARGE__END_ = BTN_VBAR,

		BTN_TAB,						// Tab
		BTN_SFT,						// Shift
		BTN_SP,							// スペース
		BTN_RET,						// リターン
		BTN_STD_LEFT,					// ←
		BTN_STD_RIGHT,					// →
		BTN_NORMAL__END_ = BTN_STD_RIGHT,

		BTN_EXT__START_,
		BTN_INS = BTN_EXT__START_,
		BTN_HOM,
		BTN_PGU,
		BTN_EXT_UP,						// ↑（拡張）
		BTN_DEL,
		BTN_END,
		BTN_PGD,
		BTN_EXT_DOWN,					// ↓（拡張）
		BTN_BS,
		BTN_EXT_LEFT,					// ←（拡張）
		BTN_EXT_RIGHT,					// →（拡張）
		BTN_EXT__END_ = BTN_EXT_RIGHT,

		BTN_CHANGE_FORM,				// フォーム変更キー
		NUM_BTN
	};

	// 配置モード
	enum FormMode
	{
		FMMODE_NORMAL,		// 通常のアルファベットキーのみ
		FMMODE_FULL,		// テンキー追加をしてフル表示のもの
		FMMODE_SMALL,		// 縮小表示したもの
#if ENABLE_OSTXTFLD
		FMMODE_OSTXTFLD,	// OS テキストフィールド
#endif
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// キーボード情報を得る
	inline const Keyboard* getKeyboard() const { return _kbd; }
	// 配置モードを得る
	inline FormMode getFormMode() const { return (FormMode)_fmmode; }
	// ボタンパネルを得る
	inline Panel* buttonPanel(Button btn) { return _pnlBtn[btn]; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setUnused(bool isUnused);
	void setReadOnlyMode(bool isReadOnly);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SoftKeyboardWindow();
	~SoftKeyboardWindow();
	bool create(Texture* texGuiRef);
	void destroy();
	void execContents(ExecRes* res, const ExecCtx* ec);
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);

private:
	void updateShiftPanels();
	void updateExtPanels(bool isEnable);
	void updateAllKeyPanels(bool isEnable);
	void onChangeFormMode();

	//======================================================================
	// メンバ変数
private:
	Panel*		_pnlBtn[NUM_BTN];
	RectF32*	_rectBtnDefault[NUM_BTN];
	Texture*	_texSkbd;
	u32			_cKeyLast;
	Keyboard*	_kbd;
	bool		_isReadOnly;
	bool		_isShift;
	u8			_fmmode;
	static u32	_tblBtnToChar[NUM_BTN];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_SOFT_KEYBOARD_WINDOW_H_
