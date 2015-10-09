/***********************************************************************//**
 *	SoftKeyboardWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SoftKeyboardWindow.h"

// Friends
#include "../../../file/GameFileIdDef.h"
#include "../../../body/GameExecCtx.h"

// External

// Library
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Size.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/g2d/StringDrawer.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/gcmn/Texture.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/ui/Keyboard.h"
#include "../../../../tfw/ui/TouchPanel.h"

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 表示位置基本グリッド
#define W_BTN_PAD				(4)
#define H_BTN_PAD				(2)
#define W_BTN_S1				(36 + W_WIDE_ADD)
#define W_BTN_S2				((W_BTN_S1 * 2) + W_BTN_PAD)
#define W_BTN_S3				((W_BTN_S1 * 3) + (W_BTN_PAD * 2))
#define W_BTN_S4				((W_BTN_S1 * 4) + (W_BTN_PAD * 3))
#define H_BTN					(28)
#define X_C01					(0)
#define X_C02					(W_BTN_S1 + W_BTN_PAD)
#define X_C03					((W_BTN_S1 + W_BTN_PAD) * 2)
#define X_C04					((W_BTN_S1 + W_BTN_PAD) * 3)
#define X_C05					((W_BTN_S1 + W_BTN_PAD) * 4)
#define X_C06					((W_BTN_S1 + W_BTN_PAD) * 5)
#define X_C07					((W_BTN_S1 + W_BTN_PAD) * 6)
#define X_C08					((W_BTN_S1 + W_BTN_PAD) * 7)
#define X_C09					((W_BTN_S1 + W_BTN_PAD) * 8)
#define X_C10					((W_BTN_S1 + W_BTN_PAD) * 9)
#define X_C11					((W_BTN_S1 + W_BTN_PAD) * 10)
#define X_C12					((W_BTN_S1 + W_BTN_PAD) * 11)
#define Y_R01					(0)
#define Y_R02					(H_BTN + H_BTN_PAD)
#define Y_R03					((H_BTN + H_BTN_PAD) * 2)
#define Y_R04					((H_BTN + H_BTN_PAD) * 3)
#define Y_R05					((H_BTN + H_BTN_PAD) * 4)

// 表示位置
#define RECT_THIS				0, 0, Game::getGame()->getLogicalWidth(), Game::getGame()->getLogicalHeight()
#define X_STD					W_WHOLE_PAD
#define Y_STD					0
#define RECT_BTN_1				(X_C01 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_2				(X_C02 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_3				(X_C03 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_4				(X_C04 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_5				(X_C05 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_6				(X_C06 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_7				(X_C07 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_8				(X_C08 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_9				(X_C09 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_0				(X_C10 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_HYP			(X_C11 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// -
#define RECT_BTN_HAT			(X_C12 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// ^
#define RECT_BTN_EXP			(X_C01 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// !
#define RECT_BTN_QUOT			(X_C02 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// "
#define RECT_BTN_SHARP			(X_C03 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// #
#define RECT_BTN_DOL			(X_C04 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// $
#define RECT_BTN_PER			(X_C05 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// %
#define RECT_BTN_AMP			(X_C06 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// &
#define RECT_BTN_APOS			(X_C07 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// '
#define RECT_BTN_PARE_B			(X_C08 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// (
#define RECT_BTN_PARE_E			(X_C09 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// )
#define RECT_BTN_EQ				(X_C11 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// =
#define RECT_BTN_TILD			(X_C12 + X_STD), (Y_R01 + Y_STD), W_BTN_S1, H_BTN	// ~
#define RECT_BTN_Q				(X_C01 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_W				(X_C02 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_E				(X_C03 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_R				(X_C04 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_T				(X_C05 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_Y				(X_C06 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_U				(X_C07 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_I				(X_C08 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_O				(X_C09 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_P				(X_C10 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_ATM			(X_C11 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN	// @
#define RECT_BTN_SBRK_B			(X_C12 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN	// [
#define RECT_BTN_GACC			(X_C11 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN	// `
#define RECT_BTN_CBRK_B			(X_C12 + X_STD), (Y_R02 + Y_STD), W_BTN_S1, H_BTN	// {
#define RECT_BTN_A				(X_C01 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_S				(X_C02 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_D				(X_C03 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_F				(X_C04 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_G				(X_C05 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_H				(X_C06 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_J				(X_C07 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_K				(X_C08 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_L				(X_C09 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_SEM			(X_C10 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN	// ;
#define RECT_BTN_COL			(X_C11 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN	// :
#define RECT_BTN_SBRK_E			(X_C12 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN	// }
#define RECT_BTN_PLUS			(X_C10 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN	// +
#define RECT_BTN_ASTA			(X_C11 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN	// *
#define RECT_BTN_CBRK_E			(X_C12 + X_STD), (Y_R03 + Y_STD), W_BTN_S1, H_BTN	// }
#define RECT_BTN_Z				(X_C01 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_X				(X_C02 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_C				(X_C03 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_V				(X_C04 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_B				(X_C05 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_N				(X_C06 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_M				(X_C07 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN
#define RECT_BTN_COM			(X_C08 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN	// ,
#define RECT_BTN_PERI			(X_C09 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN	// .
#define RECT_BTN_SLA			(X_C10 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN	// /
#define RECT_BTN_BSLA			(X_C11 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN	// バックスラッシュ
#define RECT_BTN_YEN			(X_C12 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN	// ￥マーク
#define RECT_BTN_LT				(X_C08 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN	// <
#define RECT_BTN_GT				(X_C09 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN	// >
#define RECT_BTN_QUES			(X_C10 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN	// ?
#define RECT_BTN_UDSC			(X_C11 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN	// _
#define RECT_BTN_VBAR			(X_C12 + X_STD), (Y_R04 + Y_STD), W_BTN_S1, H_BTN	// |
#define RECT_BTN_TAB			(X_C01 + X_STD), (Y_R05 + Y_STD), W_BTN_S1, H_BTN	// Tab
#define RECT_BTN_SFT			(X_C02 + X_STD), (Y_R05 + Y_STD), W_BTN_S2, H_BTN	// Shift
#define RECT_BTN_SP				(X_C04 + X_STD), (Y_R05 + Y_STD), W_BTN_S4, H_BTN	// スペース
#define RECT_BTN_RET			(X_C08 + X_STD), (Y_R05 + Y_STD), W_BTN_S2, H_BTN	// リターン
#define RECT_BTN_STD_LEFT		(X_C10 + X_STD), (Y_R05 + Y_STD), W_BTN_S1, H_BTN	// 
#define RECT_BTN_STD_RIGHT		(X_C11 + X_STD), (Y_R05 + Y_STD), W_BTN_S1, H_BTN	// 
#define X_EXT					0
#define Y_EXT					0
#define RECT_BTN_INS			(X_C01 + X_EXT), (Y_R01 + Y_EXT), W_BTN_S1, H_BTN	// Insert
#define RECT_BTN_HOM			(X_C02 + X_EXT), (Y_R01 + Y_EXT), W_BTN_S1, H_BTN	// Home
#define RECT_BTN_PGU			(X_C03 + X_EXT), (Y_R01 + Y_EXT), W_BTN_S1, H_BTN	// PageUp
#define RECT_BTN_BS				(X_C01 + X_EXT), (Y_R01 + Y_EXT), W_BTN_S1, H_BTN	// BackSpace
#define RECT_BTN_DEL			(X_C01 + X_EXT), (Y_R02 + Y_EXT), W_BTN_S1, H_BTN	// Delete
#define RECT_BTN_END			(X_C02 + X_EXT), (Y_R02 + Y_EXT), W_BTN_S1, H_BTN	// End
#define RECT_BTN_PGD			(X_C03 + X_EXT), (Y_R02 + Y_EXT), W_BTN_S1, H_BTN	// PageDown
#define RECT_BTN_EXT_UP			(X_C02 + X_EXT), (Y_R03 + Y_EXT), W_BTN_S1, H_BTN	// ↑（拡張）
#define RECT_BTN_EXT_LEFT		(X_C01 + X_EXT), (Y_R04 + Y_EXT), W_BTN_S1, H_BTN	// ←（拡張）
#define RECT_BTN_EXT_DOWN		(X_C02 + X_EXT), (Y_R04 + Y_EXT), W_BTN_S1, H_BTN	// ↓（拡張）
#define RECT_BTN_EXT_RIGHT		(X_C03 + X_EXT), (Y_R04 + Y_EXT), W_BTN_S1, H_BTN	// →（拡張）
#define X_CHB					(Game::getGame()->getLogicalWidth() - 180)
#define Y_CHB					0
#define RECT_BTN_CHANGE_FORM	X_CHB, ((   8 ) + Y_CHB), (  75), (  24 )

// テクスチャ座標値
#define W_TEX					(1024.0f)
#define H_TEX					(1024.0f)
#define UV_BTN_CHANGE_FORM_N	( 458.0f), (  92.0f), ( 72.0f), ( 24.0f)
#define UV_BTN_CHANGE_FORM_H	( 458.0f), ( 118.0f), ( 72.0f), ( 24.0f)
// 1 ～ ^
#define UV_BTN_1_N				(   2.0f), (   2.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_1_H				(   2.0f), ( 152.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_2_N				(  40.0f), (   2.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_2_H				(  40.0f), ( 152.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_3_N				(  78.0f), (   2.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_3_H				(  78.0f), ( 152.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_4_N				( 116.0f), (   2.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_4_H				( 116.0f), ( 152.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_5_N				( 154.0f), (   2.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_5_H				( 154.0f), ( 152.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_6_N				( 192.0f), (   2.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_6_H				( 192.0f), ( 152.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_7_N				( 230.0f), (   2.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_7_H				( 230.0f), ( 152.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_8_N				( 268.0f), (   2.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_8_H				( 268.0f), ( 152.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_9_N				( 306.0f), (   2.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_9_H				( 306.0f), ( 152.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_0_N				( 344.0f), (   2.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_0_H				( 344.0f), ( 152.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_HYP_N			( 382.0f), (   2.0f), ( 36.0f), ( 28.0f)	// -
#define UV_BTN_HYP_H			( 382.0f), ( 152.0f), ( 36.0f), ( 28.0f)	// -
#define UV_BTN_HAT_N			( 420.0f), (   2.0f), ( 36.0f), ( 28.0f)	// ^
#define UV_BTN_HAT_H			( 420.0f), ( 152.0f), ( 36.0f), ( 28.0f)	// ^
// q ～	[
#define UV_BTN_Q_S_N			(   2.0f), (  32.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_Q_S_H			(   2.0f), ( 182.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_W_S_N			(  40.0f), (  32.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_W_S_H			(  40.0f), ( 182.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_E_S_N			(  78.0f), (  32.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_E_S_H			(  78.0f), ( 182.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_R_S_N			( 116.0f), (  32.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_R_S_H			( 116.0f), ( 182.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_T_S_N			( 154.0f), (  32.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_T_S_H			( 154.0f), ( 182.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_Y_S_N			( 192.0f), (  32.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_Y_S_H			( 192.0f), ( 182.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_U_S_N			( 230.0f), (  32.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_U_S_H			( 230.0f), ( 182.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_I_S_N			( 268.0f), (  32.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_I_S_H			( 268.0f), ( 182.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_O_S_N			( 306.0f), (  32.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_O_S_H			( 306.0f), ( 182.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_P_S_N			( 344.0f), (  32.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_P_S_H			( 344.0f), ( 182.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_ATM_N			( 382.0f), (  32.0f), ( 36.0f), ( 28.0f)	// @
#define UV_BTN_ATM_H			( 382.0f), ( 182.0f), ( 36.0f), ( 28.0f)	// @
#define UV_BTN_SBRK_B_N			( 420.0f), (  32.0f), ( 36.0f), ( 28.0f)	// [
#define UV_BTN_SBRK_B_H			( 420.0f), ( 182.0f), ( 36.0f), ( 28.0f)	// [
// a ～ ]
#define UV_BTN_A_S_N			(   2.0f), (  62.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_A_S_H			(   2.0f), ( 212.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_S_S_N			(  40.0f), (  62.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_S_S_H			(  40.0f), ( 212.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_D_S_N			(  78.0f), (  62.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_D_S_H			(  78.0f), ( 212.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_F_S_N			( 116.0f), (  62.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_F_S_H			( 116.0f), ( 212.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_G_S_N			( 154.0f), (  62.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_G_S_H			( 154.0f), ( 212.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_H_S_N			( 192.0f), (  62.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_H_S_H			( 192.0f), ( 212.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_J_S_N			( 230.0f), (  62.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_J_S_H			( 230.0f), ( 212.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_K_S_N			( 268.0f), (  62.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_K_S_H			( 268.0f), ( 212.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_L_S_N			( 306.0f), (  62.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_L_S_H			( 306.0f), ( 212.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_SEM_N			( 344.0f), (  62.0f), ( 36.0f), ( 28.0f)	// ;
#define UV_BTN_SEM_H			( 344.0f), ( 212.0f), ( 36.0f), ( 28.0f)	// ;
#define UV_BTN_COL_N			( 382.0f), (  62.0f), ( 36.0f), ( 28.0f)	// :
#define UV_BTN_COL_H			( 382.0f), ( 212.0f), ( 36.0f), ( 28.0f)	// :
#define UV_BTN_SBRK_E_N			( 420.0f), (  62.0f), ( 36.0f), ( 28.0f)	// ]
#define UV_BTN_SBRK_E_H			( 420.0f), ( 212.0f), ( 36.0f), ( 28.0f)	// ]
// z ～ ￥マーク
#define UV_BTN_Z_S_N			(   2.0f), (  92.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_Z_S_H			(   2.0f), ( 242.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_X_S_N			(  40.0f), (  92.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_X_S_H			(  40.0f), ( 242.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_C_S_N			(  78.0f), (  92.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_C_S_H			(  78.0f), ( 242.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_V_S_N			( 116.0f), (  92.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_V_S_H			( 116.0f), ( 242.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_B_S_N			( 154.0f), (  92.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_B_S_H			( 154.0f), ( 242.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_N_S_N			( 192.0f), (  92.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_N_S_H			( 192.0f), ( 242.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_M_S_N			( 230.0f), (  92.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_M_S_H			( 230.0f), ( 242.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_COM_N			( 268.0f), (  92.0f), ( 36.0f), ( 28.0f)	// ,
#define UV_BTN_COM_H			( 268.0f), ( 242.0f), ( 36.0f), ( 28.0f)	// ,
#define UV_BTN_PERI_N			( 306.0f), (  92.0f), ( 36.0f), ( 28.0f)	// .
#define UV_BTN_PERI_H			( 306.0f), ( 242.0f), ( 36.0f), ( 28.0f)	// .
#define UV_BTN_SLA_N			( 344.0f), (  92.0f), ( 36.0f), ( 28.0f)	// /
#define UV_BTN_SLA_H			( 344.0f), ( 242.0f), ( 36.0f), ( 28.0f)	// /
#define UV_BTN_BSLA_N			( 382.0f), (  92.0f), ( 36.0f), ( 28.0f)	// バックスラッシュ
#define UV_BTN_BSLA_H			( 382.0f), ( 242.0f), ( 36.0f), ( 28.0f)	// バックスラッシュ
#define UV_BTN_YEN_N			( 420.0f), (  92.0f), ( 36.0f), ( 28.0f)	// ￥マーク
#define UV_BTN_YEN_H			( 420.0f), ( 242.0f), ( 36.0f), ( 28.0f)	// ￥マーク
// ! ～ ~
#define UV_BTN_EXP_N			(   2.0f), ( 302.0f), ( 36.0f), ( 28.0f)	// !
#define UV_BTN_EXP_H			(   2.0f), ( 422.0f), ( 36.0f), ( 28.0f)	// !
#define UV_BTN_QUOT_N			(  40.0f), ( 302.0f), ( 36.0f), ( 28.0f)	// "
#define UV_BTN_QUOT_H			(  40.0f), ( 422.0f), ( 36.0f), ( 28.0f)	// "
#define UV_BTN_SHARP_N			(  78.0f), ( 302.0f), ( 36.0f), ( 28.0f)	// #
#define UV_BTN_SHARP_H			(  78.0f), ( 422.0f), ( 36.0f), ( 28.0f)	// #
#define UV_BTN_DOL_N			( 116.0f), ( 302.0f), ( 36.0f), ( 28.0f)	// $
#define UV_BTN_DOL_H			( 116.0f), ( 422.0f), ( 36.0f), ( 28.0f)	// $
#define UV_BTN_PER_N			( 154.0f), ( 302.0f), ( 36.0f), ( 28.0f)	// %
#define UV_BTN_PER_H			( 154.0f), ( 422.0f), ( 36.0f), ( 28.0f)	// %
#define UV_BTN_AMP_N			( 192.0f), ( 302.0f), ( 36.0f), ( 28.0f)	// &
#define UV_BTN_AMP_H			( 192.0f), ( 422.0f), ( 36.0f), ( 28.0f)	// &
#define UV_BTN_APOS_N			( 230.0f), ( 302.0f), ( 36.0f), ( 28.0f)	// '
#define UV_BTN_APOS_H			( 230.0f), ( 422.0f), ( 36.0f), ( 28.0f)	// '
#define UV_BTN_PARE_B_N			( 268.0f), ( 302.0f), ( 36.0f), ( 28.0f)	// (
#define UV_BTN_PARE_B_H			( 268.0f), ( 422.0f), ( 36.0f), ( 28.0f)	// (
#define UV_BTN_PARE_E_N			( 306.0f), ( 302.0f), ( 36.0f), ( 28.0f)	// )
#define UV_BTN_PARE_E_H			( 306.0f), ( 422.0f), ( 36.0f), ( 28.0f)	// )
#define UV_BTN_EQ_N				( 382.0f), ( 302.0f), ( 36.0f), ( 28.0f)	// =
#define UV_BTN_EQ_H				( 382.0f), ( 422.0f), ( 36.0f), ( 28.0f)	// =
#define UV_BTN_TILD_N			( 420.0f), ( 302.0f), ( 36.0f), ( 28.0f)	// ~
#define UV_BTN_TILD_H			( 420.0f), ( 422.0f), ( 36.0f), ( 28.0f)	// ~
// Q ～ {
#define UV_BTN_Q_L_N			(   2.0f), ( 332.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_Q_L_H			(   2.0f), ( 452.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_W_L_N			(  40.0f), ( 332.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_W_L_H			(  40.0f), ( 452.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_E_L_N			(  78.0f), ( 332.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_E_L_H			(  78.0f), ( 452.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_R_L_N			( 116.0f), ( 332.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_R_L_H			( 116.0f), ( 452.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_T_L_N			( 154.0f), ( 332.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_T_L_H			( 154.0f), ( 452.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_Y_L_N			( 192.0f), ( 332.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_Y_L_H			( 192.0f), ( 452.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_U_L_N			( 230.0f), ( 332.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_U_L_H			( 230.0f), ( 452.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_I_L_N			( 268.0f), ( 332.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_I_L_H			( 268.0f), ( 452.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_O_L_N			( 306.0f), ( 332.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_O_L_H			( 306.0f), ( 452.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_P_L_N			( 344.0f), ( 332.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_P_L_H			( 344.0f), ( 452.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_GACC_N			( 382.0f), ( 332.0f), ( 36.0f), ( 28.0f)	// `
#define UV_BTN_GACC_H			( 382.0f), ( 452.0f), ( 36.0f), ( 28.0f)	// `
#define UV_BTN_CBRK_B_N			( 420.0f), ( 332.0f), ( 36.0f), ( 28.0f)	// {
#define UV_BTN_CBRK_B_H			( 420.0f), ( 452.0f), ( 36.0f), ( 28.0f)	// {
// A ～ }
#define UV_BTN_A_L_N			(   2.0f), ( 362.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_A_L_H			(   2.0f), ( 482.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_S_L_N			(  40.0f), ( 362.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_S_L_H			(  40.0f), ( 482.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_D_L_N			(  78.0f), ( 362.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_D_L_H			(  78.0f), ( 482.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_F_L_N			( 116.0f), ( 362.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_F_L_H			( 116.0f), ( 482.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_G_L_N			( 154.0f), ( 362.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_G_L_H			( 154.0f), ( 482.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_H_L_N			( 192.0f), ( 362.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_H_L_H			( 192.0f), ( 482.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_J_L_N			( 230.0f), ( 362.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_J_L_H			( 230.0f), ( 482.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_K_L_N			( 268.0f), ( 362.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_K_L_H			( 268.0f), ( 482.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_L_L_N			( 306.0f), ( 362.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_L_L_H			( 306.0f), ( 482.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_PLUS_N			( 344.0f), ( 362.0f), ( 36.0f), ( 28.0f)	// +
#define UV_BTN_PLUS_H			( 344.0f), ( 482.0f), ( 36.0f), ( 28.0f)	// +
#define UV_BTN_ASTA_N			( 382.0f), ( 362.0f), ( 36.0f), ( 28.0f)	// *
#define UV_BTN_ASTA_H			( 382.0f), ( 482.0f), ( 36.0f), ( 28.0f)	// *
#define UV_BTN_CBRK_E_N			( 420.0f), ( 362.0f), ( 36.0f), ( 28.0f)	// }
#define UV_BTN_CBRK_E_H			( 420.0f), ( 482.0f), ( 36.0f), ( 28.0f)	// }
// Z ～ |
#define UV_BTN_Z_L_N			(   2.0f), ( 392.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_Z_L_H			(   2.0f), ( 512.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_X_L_N			(  40.0f), ( 392.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_X_L_H			(  40.0f), ( 512.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_C_L_N			(  78.0f), ( 392.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_C_L_H			(  78.0f), ( 512.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_V_L_N			( 116.0f), ( 392.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_V_L_H			( 116.0f), ( 512.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_B_L_N			( 154.0f), ( 392.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_B_L_H			( 154.0f), ( 512.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_N_L_N			( 192.0f), ( 392.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_N_L_H			( 192.0f), ( 512.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_M_L_N			( 230.0f), ( 392.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_M_L_H			( 230.0f), ( 512.0f), ( 36.0f), ( 28.0f)
#define UV_BTN_LT_N				( 268.0f), ( 392.0f), ( 36.0f), ( 28.0f)	// <
#define UV_BTN_LT_H				( 268.0f), ( 512.0f), ( 36.0f), ( 28.0f)	// <
#define UV_BTN_GT_N				( 306.0f), ( 392.0f), ( 36.0f), ( 28.0f)	// >
#define UV_BTN_GT_H				( 306.0f), ( 512.0f), ( 36.0f), ( 28.0f)	// >
#define UV_BTN_QUES_N			( 344.0f), ( 392.0f), ( 36.0f), ( 28.0f)	// ?
#define UV_BTN_QUES_H			( 344.0f), ( 512.0f), ( 36.0f), ( 28.0f)	// ?
#define UV_BTN_UDSC_N			( 382.0f), ( 392.0f), ( 36.0f), ( 28.0f)	// _
#define UV_BTN_UDSC_H			( 382.0f), ( 512.0f), ( 36.0f), ( 28.0f)	// _
#define UV_BTN_VBAR_N			( 420.0f), ( 392.0f), ( 36.0f), ( 28.0f)	// |
#define UV_BTN_VBAR_H			( 420.0f), ( 512.0f), ( 36.0f), ( 28.0f)	// |
// その他
#define UV_BTN_TAB_N			(   2.0f), ( 122.0f), ( 36.0f), ( 28.0f)	// Tab
#define UV_BTN_TAB_H			(   2.0f), ( 272.0f), ( 36.0f), ( 28.0f)	// Tab
#define UV_BTN_SFT_N			(  40.0f), ( 122.0f), ( 78.0f), ( 28.0f)	// Shift
#define UV_BTN_SFT_H			(  40.0f), ( 272.0f), ( 78.0f), ( 28.0f)	// Shift
#define UV_BTN_SP_N				( 120.0f), ( 122.0f), (162.0f), ( 28.0f)	// スペース
#define UV_BTN_SP_H				( 120.0f), ( 272.0f), (162.0f), ( 28.0f)	// スペース
#define UV_BTN_RET_N			( 284.0f), ( 122.0f), ( 78.0f), ( 28.0f)	// リターン
#define UV_BTN_RET_H			( 284.0f), ( 272.0f), ( 78.0f), ( 28.0f)	// リターン
#define UV_BTN_STD_LEFT_N		( 364.0f), ( 122.0f), ( 36.0f), ( 28.0f)	// ←
#define UV_BTN_STD_LEFT_H		( 364.0f), ( 272.0f), ( 36.0f), ( 28.0f)	// ←
#define UV_BTN_STD_RIGHT_N		( 402.0f), ( 122.0f), ( 36.0f), ( 28.0f)	// →
#define UV_BTN_STD_RIGHT_H		( 402.0f), ( 272.0f), ( 36.0f), ( 28.0f)	// →
#define UV_BTN_BS_N				( 458.0f), (   2.0f), ( 36.0f), ( 28.0f)	// BackSpace
#define UV_BTN_BS_H				( 458.0f), ( 152.0f), ( 36.0f), ( 28.0f)	// BackSpace
#define UV_BTN_HOM_N			( 496.0f), (   2.0f), ( 36.0f), ( 28.0f)	// Home
#define UV_BTN_HOM_H			( 496.0f), ( 152.0f), ( 36.0f), ( 28.0f)	// Home
#define UV_BTN_PGU_N			( 534.0f), (   2.0f), ( 36.0f), ( 28.0f)	// PageUp
#define UV_BTN_PGU_H			( 534.0f), ( 152.0f), ( 36.0f), ( 28.0f)	// PageUp
#define UV_BTN_DEL_N			( 458.0f), (  32.0f), ( 36.0f), ( 28.0f)	// Delete
#define UV_BTN_DEL_H			( 458.0f), ( 182.0f), ( 36.0f), ( 28.0f)	// Delete
#define UV_BTN_END_N			( 496.0f), (  32.0f), ( 36.0f), ( 28.0f)	// End
#define UV_BTN_END_H			( 496.0f), ( 182.0f), ( 36.0f), ( 28.0f)	// End
#define UV_BTN_PGD_N			( 534.0f), (  32.0f), ( 36.0f), ( 28.0f)	// PageDown
#define UV_BTN_PGD_H			( 534.0f), ( 182.0f), ( 36.0f), ( 28.0f)	// PageDown
#define UV_BTN_INS_N			( 458.0f), (  62.0f), ( 36.0f), ( 28.0f)	// Ins
#define UV_BTN_INS_H			( 458.0f), ( 212.0f), ( 36.0f), ( 28.0f)	// Ins
#define UV_BTN_EXT_UP_N			( 496.0f), (  62.0f), ( 36.0f), ( 28.0f)	// ↑（拡張）
#define UV_BTN_EXT_UP_H			( 496.0f), ( 212.0f), ( 36.0f), ( 28.0f)	// ↑（拡張）
#define UV_BTN_EXT_DOWN_N		( 534.0f), (  62.0f), ( 36.0f), ( 28.0f)	// ↓（拡張）
#define UV_BTN_EXT_DOWN_H		( 534.0f), ( 212.0f), ( 36.0f), ( 28.0f)	// ↓（拡張）
#define UV_BTN_EXT_LEFT_N		( 364.0f), ( 122.0f), ( 36.0f), ( 28.0f)	// ←（拡張）
#define UV_BTN_EXT_LEFT_H		( 364.0f), ( 272.0f), ( 36.0f), ( 28.0f)	// ←（拡張）
#define UV_BTN_EXT_RIGHT_N		( 402.0f), ( 122.0f), ( 36.0f), ( 28.0f)	// →（拡張）
#define UV_BTN_EXT_RIGHT_H		( 402.0f), ( 272.0f), ( 36.0f), ( 28.0f)	// →（拡張）

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SoftKeyboardWindow 静的変数

// ボタンからキーボード文字への変換テーブル
u32 SoftKeyboardWindow::_tblBtnToChar[SoftKeyboardWindow::NUM_BTN] =
{
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	'-',
	'^',
	'q',
	'w',
	'e',
	'r',
	't',
	'y',
	'u',
	'i',
	'o',
	'p',
	'@',
	'[',
	'a',
	's',
	'd',
	'f',
	'g',
	'h',
	'j',
	'k',
	'l',
	';',
	':',
	']',
	'z',
	'x',
	'c',
	'v',
	'b',
	'n',
	'm',
	',',
	'.',
	'/',
	'\\',
	'\\',
	'!',
	'\"',
	'#',
	'$',
	'%',
	'&',
	'\'',
	'(',
	')',
	'=',
	'~',
	'Q',
	'W',
	'E',
	'R',
	'T',
	'Y',
	'U',
	'I',
	'O',
	'P',
	'`',
	'{',
	'A',
	'S',
	'D',
	'F',
	'G',
	'H',
	'J',
	'K',
	'L',
	'+',
	'*',
	'}',
	'Z',
	'X',
	'C',
	'V',
	'B',
	'N',
	'M',
	'<',
	'>',
	'?',
	'_',
	'|',
	'\t',
	Keyboard::SYSKEY_NULL,		// BTN_SFT
	' ',
	'\n',
	Keyboard::SYSKEY_LEFT,		// BTN_STD_LEFT
	Keyboard::SYSKEY_RIGHT,		// BTN_STD_RIGHT
	Keyboard::SYSKEY_INS,		// BTN_INS
	Keyboard::SYSKEY_HOME,		// BTN_HOM
	Keyboard::SYSKEY_PGUP,		// BTN_PGU
	Keyboard::SYSKEY_UP,		// BTN_EXT_UP
	Keyboard::SYSKEY_DEL,		// BTN_DEL
	Keyboard::SYSKEY_END,		// BTN_END
	Keyboard::SYSKEY_PGDOWN,	// BTN_PGD
	Keyboard::SYSKEY_DOWN,		// BTN_EXT_DOWN
	Keyboard::SYSKEY_BS,		// BTN_BS
	Keyboard::SYSKEY_LEFT,		// BTN_EXT_LEFT
	Keyboard::SYSKEY_RIGHT,		// BTN_EXT_RIGHT
	Keyboard::SYSKEY_NULL,		// BTN_CHANGE_FORM
};

//==========================================================================
// SoftKeyboardWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	使用不能設定
**//*---------------------------------------------------------------------*/
void SoftKeyboardWindow::setUnused(bool isUnused)
{
	Window::setUnused(isUnused);

	for(int i = 0; i < NUM_BTN; i++)
	{
		_pnlBtn[i]->setUnused(isUnused);
	}
}

/*---------------------------------------------------------------------*//**
	読み込み専用設定
**//*---------------------------------------------------------------------*/
void SoftKeyboardWindow::setReadOnlyMode(bool isReadOnly)
{
	_isReadOnly = isReadOnly;

	#define BTN_SET_READONLY(kkk_) \
		_pnlBtn[kkk_]->setUnused(_isReadOnly);
	#define BTN_SET_READONLY_SL(kkk_, sl_) \
		_pnlBtn[kkk_##_##sl_]->setUnused(_isReadOnly);
	BTN_SET_READONLY( BTN_1 );
	BTN_SET_READONLY( BTN_2 );
	BTN_SET_READONLY( BTN_3 );
	BTN_SET_READONLY( BTN_4 );
	BTN_SET_READONLY( BTN_5 );
	BTN_SET_READONLY( BTN_6 );
	BTN_SET_READONLY( BTN_7 );
	BTN_SET_READONLY( BTN_8 );
	BTN_SET_READONLY( BTN_9 );
	BTN_SET_READONLY( BTN_0 );
	BTN_SET_READONLY( BTN_HYP );			// -
	BTN_SET_READONLY( BTN_HAT );			// ^
	BTN_SET_READONLY_SL( BTN_Q, S );
	BTN_SET_READONLY_SL( BTN_W, S );
	BTN_SET_READONLY_SL( BTN_E, S );
	BTN_SET_READONLY_SL( BTN_R, S );
	BTN_SET_READONLY_SL( BTN_T, S );
	BTN_SET_READONLY_SL( BTN_Y, S );
	BTN_SET_READONLY_SL( BTN_U, S );
	BTN_SET_READONLY_SL( BTN_I, S );
	BTN_SET_READONLY_SL( BTN_O, S );
	BTN_SET_READONLY_SL( BTN_P, S );
	BTN_SET_READONLY( BTN_ATM );			// @
	BTN_SET_READONLY( BTN_SBRK_B );			// [
	BTN_SET_READONLY_SL( BTN_A, S );
	BTN_SET_READONLY_SL( BTN_S, S );
	BTN_SET_READONLY_SL( BTN_D, S );
	BTN_SET_READONLY_SL( BTN_F, S );
	BTN_SET_READONLY_SL( BTN_G, S );
	BTN_SET_READONLY_SL( BTN_H, S );
	BTN_SET_READONLY_SL( BTN_J, S );
	BTN_SET_READONLY_SL( BTN_K, S );
	BTN_SET_READONLY_SL( BTN_L, S );
	BTN_SET_READONLY( BTN_SEM );			// ;
	BTN_SET_READONLY( BTN_COL );			// :
	BTN_SET_READONLY( BTN_SBRK_E );			// ]
	BTN_SET_READONLY_SL( BTN_Z, S );
	BTN_SET_READONLY_SL( BTN_X, S );
	BTN_SET_READONLY_SL( BTN_C, S );
	BTN_SET_READONLY_SL( BTN_V, S );
	BTN_SET_READONLY_SL( BTN_B, S );
	BTN_SET_READONLY_SL( BTN_N, S );
	BTN_SET_READONLY_SL( BTN_M, S );
	BTN_SET_READONLY( BTN_COM );			// ,
	BTN_SET_READONLY( BTN_PERI );			// .
	BTN_SET_READONLY( BTN_SLA );			// /
	BTN_SET_READONLY( BTN_BSLA );			// バックスラッシュ
	BTN_SET_READONLY( BTN_YEN );			// ￥マーク
	BTN_SET_READONLY( BTN_EXP );			// !
	BTN_SET_READONLY( BTN_QUOT );			// "
	BTN_SET_READONLY( BTN_SHARP );			// #
	BTN_SET_READONLY( BTN_DOL );			// $
	BTN_SET_READONLY( BTN_PER );			// %
	BTN_SET_READONLY( BTN_AMP );			// &
	BTN_SET_READONLY( BTN_APOS );			// '
	BTN_SET_READONLY( BTN_PARE_B );			// (
	BTN_SET_READONLY( BTN_PARE_E );			// )
	BTN_SET_READONLY( BTN_EQ );				// =
	BTN_SET_READONLY( BTN_TILD );			// ~
	BTN_SET_READONLY_SL( BTN_Q, L );
	BTN_SET_READONLY_SL( BTN_W, L );
	BTN_SET_READONLY_SL( BTN_E, L );
	BTN_SET_READONLY_SL( BTN_R, L );
	BTN_SET_READONLY_SL( BTN_T, L );
	BTN_SET_READONLY_SL( BTN_Y, L );
	BTN_SET_READONLY_SL( BTN_U, L );
	BTN_SET_READONLY_SL( BTN_I, L );
	BTN_SET_READONLY_SL( BTN_O, L );
	BTN_SET_READONLY_SL( BTN_P, L );
	BTN_SET_READONLY( BTN_GACC );			// `
	BTN_SET_READONLY( BTN_CBRK_B );			// {
	BTN_SET_READONLY_SL( BTN_A, L );
	BTN_SET_READONLY_SL( BTN_S, L );
	BTN_SET_READONLY_SL( BTN_D, L );
	BTN_SET_READONLY_SL( BTN_F, L );
	BTN_SET_READONLY_SL( BTN_G, L );
	BTN_SET_READONLY_SL( BTN_H, L );
	BTN_SET_READONLY_SL( BTN_J, L );
	BTN_SET_READONLY_SL( BTN_K, L );
	BTN_SET_READONLY_SL( BTN_L, L );
	BTN_SET_READONLY( BTN_PLUS );			// +
	BTN_SET_READONLY( BTN_ASTA );			// *
	BTN_SET_READONLY( BTN_CBRK_E );			// }
	BTN_SET_READONLY_SL( BTN_Z, L );
	BTN_SET_READONLY_SL( BTN_X, L );
	BTN_SET_READONLY_SL( BTN_C, L );
	BTN_SET_READONLY_SL( BTN_V, L );
	BTN_SET_READONLY_SL( BTN_B, L );
	BTN_SET_READONLY_SL( BTN_N, L );
	BTN_SET_READONLY_SL( BTN_M, L );
	BTN_SET_READONLY( BTN_LT );				// <
	BTN_SET_READONLY( BTN_GT );				// >
	BTN_SET_READONLY( BTN_QUES );			// ?
	BTN_SET_READONLY( BTN_UDSC );			// _
	BTN_SET_READONLY( BTN_VBAR );			// |
	BTN_SET_READONLY( BTN_TAB );			// Tab
	//BTN_SET_READONLY( BTN_SFT );			// Shift
	BTN_SET_READONLY( BTN_SP );				// スペース
	BTN_SET_READONLY( BTN_RET );			// リターン
	//BTN_SET_READONLY( BTN_STD_LEFT );		// ←
	//BTN_SET_READONLY( BTN_STD_RIGHT );	// →
	BTN_SET_READONLY( BTN_INS );			// Insert
	//BTN_SET_READONLY( BTN_HOM );			// Home
	//BTN_SET_READONLY( BTN_PGU );			// PageUp
	BTN_SET_READONLY( BTN_DEL );			// Delete
	//BTN_SET_READONLY( BTN_END );			// End
	//BTN_SET_READONLY( BTN_PGD );			// PageDown
	//BTN_SET_READONLY( BTN_EXT_UP );		// ↑（拡張）
	//BTN_SET_READONLY( BTN_EXT_LEFT );		// ←（拡張）
	//BTN_SET_READONLY( BTN_EXT_DOWN );		// ↓（拡張）
	//BTN_SET_READONLY( BTN_EXT_RIGHT );	// →（拡張）
	BTN_SET_READONLY( BTN_BS );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SoftKeyboardWindow::SoftKeyboardWindow()
	: _texSkbd(0L)
	, _cKeyLast(0)
	, _kbd(0L)
	, _isReadOnly(false)
	, _isShift(false)
	, _fmmode(FMMODE_NORMAL)
{
	for(int i = 0; i < NUM_BTN; i++)
	{
		_pnlBtn[i] = 0L;
		_rectBtnDefault[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SoftKeyboardWindow::~SoftKeyboardWindow()
{
	ASSERT(_texSkbd == 0L);
	ASSERT(_kbd == 0L);
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_BTN; i++)
		{
			ASSERT(_pnlBtn[i] == 0L);
			ASSERT(_rectBtnDefault[i] == 0L);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SoftKeyboardWindow::create(Texture* texGuiRef)
{
	RectF32 rect;
	RectF32 uv;
	ColorU8 col;

	// ウインドウの作成
	if(!Window::create(RectF32::setout(&rect, RECT_THIS)))
	{
		return false;
	}

	// キーボード情報を作成
	_kbd = new Keyboard();

	// 自身のパネル状態の設定
	///setStateInfo(STATE_NORMAL, texRef, RectF32::setout(&uv, THIS_UV));

	// 専用テクスチャ読み込み
	Texture* texSkbd = new Texture();
	if(!texSkbd->create(FILEID_CONV_RD( SYSTEM_SKB01_SD_PNG ), Texture::FMT_PNG))
	{
		delete texSkbd;
		ASSERTM(false, "software-keyboard Texture::create failed.");
		return false;
	}
	_texSkbd = texSkbd;

	// 各パネルの作成
	const f32 W_WIDE_ADD = (Game::getGame()->getLogicalWidth() - Game::getGame()->getStandardWidth()) / 12;
	const f32 W_WHOLE_PAD = (Game::getGame()->getLogicalWidth() - (W_BTN_S1 * 12) - (W_BTN_PAD * 11)) / 2;
	#define NEW_BTN_PANEL(kkk_) \
		_pnlBtn[kkk_] = new Panel(); \
		_rectBtnDefault[kkk_] = new RectF32(RECT_##kkk_); \
		_pnlBtn[kkk_]->create(_rectBtnDefault[kkk_]); \
		_pnlBtn[kkk_]->setStateInfo(STATE_NORMAL, texSkbd, Gcalc::calcTexUv(&uv, UV_##kkk_##_N, W_TEX, H_TEX, Env_get2drRate() * 0.5f)); \
		_pnlBtn[kkk_]->setStateInfo(STATE_HOVER, texSkbd, Gcalc::calcTexUv(&uv, UV_##kkk_##_H, W_TEX, H_TEX, Env_get2drRate() * 0.5f));
	#define NEW_BTN_PANEL_SL(kkk_, sl_) \
		_pnlBtn[kkk_##_##sl_] = new Panel(); \
		_rectBtnDefault[kkk_##_##sl_] = new RectF32(RECT_##kkk_); \
		_pnlBtn[kkk_##_##sl_]->create(_rectBtnDefault[kkk_##_##sl_]); \
		_pnlBtn[kkk_##_##sl_]->setStateInfo(STATE_NORMAL, texSkbd, Gcalc::calcTexUv(&uv, UV_##kkk_##_##sl_##_N, W_TEX, H_TEX, Env_get2drRate() * 0.5f)); \
		_pnlBtn[kkk_##_##sl_]->setStateInfo(STATE_HOVER, texSkbd, Gcalc::calcTexUv(&uv, UV_##kkk_##_##sl_##_H, W_TEX, H_TEX, Env_get2drRate() * 0.5f));
	NEW_BTN_PANEL( BTN_1 );
	NEW_BTN_PANEL( BTN_2 );
	NEW_BTN_PANEL( BTN_3 );
	NEW_BTN_PANEL( BTN_4 );
	NEW_BTN_PANEL( BTN_5 );
	NEW_BTN_PANEL( BTN_6 );
	NEW_BTN_PANEL( BTN_7 );
	NEW_BTN_PANEL( BTN_8 );
	NEW_BTN_PANEL( BTN_9 );
	NEW_BTN_PANEL( BTN_0 );
	NEW_BTN_PANEL( BTN_HYP );			// -
	NEW_BTN_PANEL( BTN_HAT );			// ^
	NEW_BTN_PANEL_SL( BTN_Q, S );
	NEW_BTN_PANEL_SL( BTN_W, S );
	NEW_BTN_PANEL_SL( BTN_E, S );
	NEW_BTN_PANEL_SL( BTN_R, S );
	NEW_BTN_PANEL_SL( BTN_T, S );
	NEW_BTN_PANEL_SL( BTN_Y, S );
	NEW_BTN_PANEL_SL( BTN_U, S );
	NEW_BTN_PANEL_SL( BTN_I, S );
	NEW_BTN_PANEL_SL( BTN_O, S );
	NEW_BTN_PANEL_SL( BTN_P, S );
	NEW_BTN_PANEL( BTN_ATM );			// @
	NEW_BTN_PANEL( BTN_SBRK_B );		// [
	NEW_BTN_PANEL_SL( BTN_A, S );
	NEW_BTN_PANEL_SL( BTN_S, S );
	NEW_BTN_PANEL_SL( BTN_D, S );
	NEW_BTN_PANEL_SL( BTN_F, S );
	NEW_BTN_PANEL_SL( BTN_G, S );
	NEW_BTN_PANEL_SL( BTN_H, S );
	NEW_BTN_PANEL_SL( BTN_J, S );
	NEW_BTN_PANEL_SL( BTN_K, S );
	NEW_BTN_PANEL_SL( BTN_L, S );
	NEW_BTN_PANEL( BTN_SEM );			// ;
	NEW_BTN_PANEL( BTN_COL );			// :
	NEW_BTN_PANEL( BTN_SBRK_E );		// ]
	NEW_BTN_PANEL_SL( BTN_Z, S );
	NEW_BTN_PANEL_SL( BTN_X, S );
	NEW_BTN_PANEL_SL( BTN_C, S );
	NEW_BTN_PANEL_SL( BTN_V, S );
	NEW_BTN_PANEL_SL( BTN_B, S );
	NEW_BTN_PANEL_SL( BTN_N, S );
	NEW_BTN_PANEL_SL( BTN_M, S );
	NEW_BTN_PANEL( BTN_COM );			// ,
	NEW_BTN_PANEL( BTN_PERI );			// .
	NEW_BTN_PANEL( BTN_SLA );			// /
	NEW_BTN_PANEL( BTN_BSLA );			// バックスラッシュ
	NEW_BTN_PANEL( BTN_YEN );			// ￥マーク
	NEW_BTN_PANEL( BTN_EXP );			// !
	NEW_BTN_PANEL( BTN_QUOT );			// "
	NEW_BTN_PANEL( BTN_SHARP );			// #
	NEW_BTN_PANEL( BTN_DOL );			// $
	NEW_BTN_PANEL( BTN_PER );			// %
	NEW_BTN_PANEL( BTN_AMP );			// &
	NEW_BTN_PANEL( BTN_APOS );			// '
	NEW_BTN_PANEL( BTN_PARE_B );		// (
	NEW_BTN_PANEL( BTN_PARE_E );		// )
	NEW_BTN_PANEL( BTN_EQ );			// =
	NEW_BTN_PANEL( BTN_TILD );			// ~
	NEW_BTN_PANEL_SL( BTN_Q, L );
	NEW_BTN_PANEL_SL( BTN_W, L );
	NEW_BTN_PANEL_SL( BTN_E, L );
	NEW_BTN_PANEL_SL( BTN_R, L );
	NEW_BTN_PANEL_SL( BTN_T, L );
	NEW_BTN_PANEL_SL( BTN_Y, L );
	NEW_BTN_PANEL_SL( BTN_U, L );
	NEW_BTN_PANEL_SL( BTN_I, L );
	NEW_BTN_PANEL_SL( BTN_O, L );
	NEW_BTN_PANEL_SL( BTN_P, L );
	NEW_BTN_PANEL( BTN_GACC );			// `
	NEW_BTN_PANEL( BTN_CBRK_B );		// {
	NEW_BTN_PANEL_SL( BTN_A, L );
	NEW_BTN_PANEL_SL( BTN_S, L );
	NEW_BTN_PANEL_SL( BTN_D, L );
	NEW_BTN_PANEL_SL( BTN_F, L );
	NEW_BTN_PANEL_SL( BTN_G, L );
	NEW_BTN_PANEL_SL( BTN_H, L );
	NEW_BTN_PANEL_SL( BTN_J, L );
	NEW_BTN_PANEL_SL( BTN_K, L );
	NEW_BTN_PANEL_SL( BTN_L, L );
	NEW_BTN_PANEL( BTN_PLUS );			// +
	NEW_BTN_PANEL( BTN_ASTA );			// *
	NEW_BTN_PANEL( BTN_CBRK_E );		// }
	NEW_BTN_PANEL_SL( BTN_Z, L );
	NEW_BTN_PANEL_SL( BTN_X, L );
	NEW_BTN_PANEL_SL( BTN_C, L );
	NEW_BTN_PANEL_SL( BTN_V, L );
	NEW_BTN_PANEL_SL( BTN_B, L );
	NEW_BTN_PANEL_SL( BTN_N, L );
	NEW_BTN_PANEL_SL( BTN_M, L );
	NEW_BTN_PANEL( BTN_LT );			// <
	NEW_BTN_PANEL( BTN_GT );			// >
	NEW_BTN_PANEL( BTN_QUES );			// ?
	NEW_BTN_PANEL( BTN_UDSC );			// _
	NEW_BTN_PANEL( BTN_VBAR );			// |
	NEW_BTN_PANEL( BTN_TAB );			// Tab
	NEW_BTN_PANEL( BTN_SFT );			// Shift
	NEW_BTN_PANEL( BTN_SP );			// スペース
	NEW_BTN_PANEL( BTN_RET );			// リターン
	NEW_BTN_PANEL( BTN_STD_LEFT );		// ←
	NEW_BTN_PANEL( BTN_STD_RIGHT );		// →
	NEW_BTN_PANEL( BTN_INS );			// Insert
	NEW_BTN_PANEL( BTN_HOM );			// Home
	NEW_BTN_PANEL( BTN_PGU );			// PageUp
	NEW_BTN_PANEL( BTN_DEL );			// Delete
	NEW_BTN_PANEL( BTN_END );			// End
	NEW_BTN_PANEL( BTN_PGD );			// PageDown
	NEW_BTN_PANEL( BTN_EXT_UP );		// ↑（拡張）
	NEW_BTN_PANEL( BTN_EXT_LEFT );		// ←（拡張）
	NEW_BTN_PANEL( BTN_EXT_DOWN );		// ↓（拡張）
	NEW_BTN_PANEL( BTN_EXT_RIGHT );		// →（拡張）
	NEW_BTN_PANEL( BTN_BS );

	// BTN_CHANGE_FORM
	_pnlBtn[BTN_CHANGE_FORM] = new Panel();
	_pnlBtn[BTN_CHANGE_FORM]->create(RectF32::setout(&rect, RECT_BTN_CHANGE_FORM));
	_pnlBtn[BTN_CHANGE_FORM]->setStateInfo(STATE_NORMAL, texSkbd, Gcalc::calcTexUv(&uv, UV_BTN_CHANGE_FORM_N, W_TEX, H_TEX, Env_get2drRate()), ColorU8::setout(&col, 255, 255, 255, 191));
	_pnlBtn[BTN_CHANGE_FORM]->setStateInfo(STATE_HOVER, texSkbd, Gcalc::calcTexUv(&uv, UV_BTN_CHANGE_FORM_H, W_TEX, H_TEX, Env_get2drRate()), ColorU8::setout(&col, 255, 255, 255, 255));

	// 初期値としては拡張キーは OFF に
	updateShiftPanels();
	updateExtPanels(false);
	onChangeFormMode();

	// INS は現在未サポート
	_pnlBtn[BTN_INS]->setEnable(false);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SoftKeyboardWindow::destroy()
{
	// 各パネルの削除
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] != 0L)
		{
			_pnlBtn[i]->destroy();
			delete _pnlBtn[i];
			_pnlBtn[i] = 0L;
		}
		delete _rectBtnDefault[i];
		_rectBtnDefault[i] = 0L;
	}

	// 専用テクスチャ削除
	if(_texSkbd != 0L)
	{
		_texSkbd->destroy();
		delete _texSkbd;
		_texSkbd = 0L;
	}

	// キーボード情報の削除
	delete _kbd;
	_kbd = 0L;

	Window::destroy();
}

/*---------------------------------------------------------------------*//**
	内容のフレーム制御
**//*---------------------------------------------------------------------*/
void SoftKeyboardWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* touchp = gec->getTouchPanel(); 

	u32 cKeyLastBk = _cKeyLast;
	_cKeyLast = 0;	// 最後に押したキーのクリア

	PointF32 ptTouch(S32_MIN, S32_MIN);
	PointF32 ptClick(S32_MIN, S32_MIN);
	bool isTouch = touchp->getTouchPosition(&ptTouch);
	bool isClick = !touchp->isReacted() && touchp->isTapRepeat(&ptClick, 0L);

	// ボタンパネルのフレーム処理
	bool isHit = false;
	for(int i = 0; i < NUM_BTN; i++)
	{
		Panel* pnlBtnWk = _pnlBtn[i];
		if(pnlBtnWk == 0L)			{	continue;	}
		if(!pnlBtnWk->isEnable())	{	continue;	}

		if(isClick)
		{
			if(!isHit && pnlBtnWk->hit(&ptClick, true))
			{
				switch(i)
				{
				case BTN_SFT:
					_isShift = !_isShift;
					updateShiftPanels();
					break;
				case BTN_CHANGE_FORM:
					_fmmode++;
					#if ENABLE_OSTXTFLD
						if(_fmmode > FMMODE_OSTXTFLD)	{	_fmmode = FMMODE_NORMAL;	} 
					#else
						if(_fmmode > FMMODE_SMALL)		{	_fmmode = FMMODE_NORMAL;	} 
					#endif
					onChangeFormMode();
					break;
				default:
					_cKeyLast = _tblBtnToChar[i];
					break;
				}
				isHit = true;
				touchp->setReacted(true);
				touchp->setReactedTap(true);
			}
		}
		else
		{
			pnlBtnWk->hit(&ptTouch, isTouch);
		}

		// シフト状態によるシフトキーの表示変更
		if(i == BTN_SFT)
		{
			_pnlBtn[BTN_SFT]->setCurrentState(_isShift ? STATE_HOVER : STATE_NORMAL);
		}

		pnlBtnWk->exec(res, ec);
	}

	// キーボード情報の更新
	_kbd->setOn(Keyboard::SYSKEY_SHIFT, _isShift);
	if(cKeyLastBk != 0)	{	_kbd->setOn(cKeyLastBk, false);	}
	if(_cKeyLast != 0)	{	_kbd->setOn(_cKeyLast, true);	}
	_kbd->exec();
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void SoftKeyboardWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	// ボタンパネルの描画処理
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] == 0L)	{	continue;	}
		_pnlBtn[i]->draw(rc);
	}
}

/*---------------------------------------------------------------------*//**
	シフトキー類の状態を更新する
**//*---------------------------------------------------------------------*/
void SoftKeyboardWindow::updateShiftPanels()
{
	for(int i = BTN_SMALL__START_; i <= BTN_SMALL__END_; i++)
	{
		if(_pnlBtn[i] != 0L)	{	_pnlBtn[i]->setEnable(!_isShift);	}
	}
	for(int i = BTN_LARGE__START_; i <= BTN_LARGE__END_; i++)
	{
		if(_pnlBtn[i] != 0L)	{	_pnlBtn[i]->setEnable(_isShift);	}
	}
}

/*---------------------------------------------------------------------*//**
	拡張キー類の状態を更新する
**//*---------------------------------------------------------------------*/
void SoftKeyboardWindow::updateExtPanels(bool isEnable)
{
	for(int i = BTN_EXT__START_; i <= BTN_EXT__END_; i++)
	{
		if(i == BTN_INS)		{	continue;	}	// INS は現在未サポート
		if(_pnlBtn[i] != 0L)	{	_pnlBtn[i]->setEnable(isEnable);	}
	}
}

/*---------------------------------------------------------------------*//**
	拡張キー類の状態を更新する
**//*---------------------------------------------------------------------*/
void SoftKeyboardWindow::updateAllKeyPanels(bool isEnable)
{
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(i == BTN_INS)		{	continue;	}	// INS は現在未サポート
		if(_pnlBtn[i] != 0L)	{	_pnlBtn[i]->setEnable(isEnable);	}
	}
}

/*---------------------------------------------------------------------*//**
	表示モードを変更する
**//*---------------------------------------------------------------------*/
void SoftKeyboardWindow::onChangeFormMode()
{
#if ENABLE_OSTXTFLD
	if(_fmmode == FMMODE_OSTXTFLD)
	{
		updateAllKeyPanels(false);
		_pnlBtn[BTN_CHANGE_FORM]->setEnable(true);
	}
	else
#endif
	{
		#if ENABLE_OSTXTFLD
			updateAllKeyPanels(true);
		#endif
		updateExtPanels((_fmmode == FMMODE_FULL) || (_fmmode == FMMODE_SMALL));

		const f32 W_WIDE_ADD = (Game::getGame()->getLogicalWidth() - Game::getGame()->getStandardWidth()) / 12;
		const f32 W_WHOLE_PAD = (Game::getGame()->getLogicalWidth() - (W_BTN_S1 * 12) - (W_BTN_PAD * 11)) / 2;
		const bool isSmall = (_fmmode == FMMODE_SMALL);
		const f32 scale = isSmall ? 0.8f : 1.0f;
		const f32 xofsStd = 0;
		const f32 yofsStd = isSmall ? 202.0f : 170.0f;
//		const f32 xofsExt = isSmall ? (Game::getGame()->getLogicalWidth() * 12 / 15) - (X_C01 / 2) : (Game::getGame()->getLogicalWidth() * 9 / 12) - X_C01;
		const f32 xofsExt = isSmall ? (Game::getGame()->getLogicalWidth() * 12 / 15) - (X_C01 / 2) : W_WHOLE_PAD + X_C10;
		const f32 yofsExt = isSmall ? 226.0f :  50.0f;
		for(int i = BTN_NORMAL__START_; i <= BTN_NORMAL__END_; i++)
		{
			if(_pnlBtn[i] == 0L)	{	continue;	}
			RectF32 rect(*_rectBtnDefault[i]);
			rect._v[0] = (_rectBtnDefault[i]->_v[0] * scale) + xofsStd;
			rect._v[1] = (_rectBtnDefault[i]->_v[1] * scale) + yofsStd;
			rect._v[2] = _rectBtnDefault[i]->_v[2] * scale;
			rect._v[3] = _rectBtnDefault[i]->_v[3] * scale;
			_pnlBtn[i]->setRectangle(&rect);
		}
		for(int i = BTN_EXT__START_; i <= BTN_EXT__END_; i++)
		{
			if(_pnlBtn[i] == 0L)	{	continue;	}
			RectF32 rect(*_rectBtnDefault[i]);
			rect._v[0] = (_rectBtnDefault[i]->_v[0] * scale) + xofsExt;
			rect._v[1] = (_rectBtnDefault[i]->_v[1] * scale) + yofsExt;
			rect._v[2] = _rectBtnDefault[i]->_v[2] * scale;
			rect._v[3] = _rectBtnDefault[i]->_v[3] * scale;
			_pnlBtn[i]->setRectangle(&rect);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
