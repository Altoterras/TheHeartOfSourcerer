/***********************************************************************//**
 *	ScreenEffect.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/11/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_EFFECT_SCREEN_EFFECT_H_
#define _ETK_EFFECT_SCREEN_EFFECT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
template<class TYPE> class Color;
typedef Color<u8> ColorU8;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スクリーンエフェクト
 *
**//*---------------------------------------------------------------------*/
class ScreenEffect : public FrameExecDraw
{
	//======================================================================
	// 定数
public:
	// モード
	static const u8		MODE_NULL				= 0;
	static const u8		MODE_FILL				= 1;		// 塗りつぶし
	static const u8		MODE_FADE_IN			= 2;		// フェードイン
	static const u8		MODE_FADE_OUT			= 3;		// フェードアウト
	static const u8		MODE_FLASH_1			= 4;		// 1 回フラッシュ
	static const u8		MODE_FLASH_2			= 5;		// 2 回フラッシュ
	static const u8		NUM_MODE;

	// 無限フレーム
	static const s32	FRAME_INFINITY			= -1;

	// 設定フラグ
	static const u16	CF_ENABLE_EVENT_PAUSE	= 0x0001;	// イベントポーズで制御をとめる
	
	//======================================================================
	// クラス
public:
	class ModeInfo
	{
	public:
		f32		_frmMaxDefault;
		ModeInfo(f32 frmMaxDefault) : _frmMaxDefault(frmMaxDefault) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 設定フラグを得る
	inline u16 getConfFlags() const { return _flagsConf; }
	// 有効フレーム内の動作中かどうかを得る
	inline bool isWorking() { return _frmCur < (_frmMax + 1.0f); }	// 整数的に判定する（整数的に最大フレーム内は動作中とする）

	// 設定フラグを設定する
	inline void setConfFlags(u16 flag, bool isValid) { TFW_SET_FLAG(_flagsConf, flag, isValid); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setColor1(const ColorU8* color);
	void setMaxFrame(f32 frmMax);
	void setScreenSize(f32 wScreen, f32 hScreen);
	s32 start(u8 mode);
	void end(s32 hintCtrl);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ScreenEffect();
	~ScreenEffect();
	bool create(f32 wScreen, f32 hScreen);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void draw(const RenderCtx* rc);				// 描画

private:
	void drawFill(Renderer* rdr);
	void drawFadeIn(Renderer* rdr);
	void drawFadeOut(Renderer* rdr);
	void drawFlash1(Renderer* rdr);
	void drawFlash2(Renderer* rdr);
	
	//======================================================================
	// 変数
private:
	RectF32*	_rectScreen;	// スクリーン矩形
	u16			_flagsConf;		// 設定フラグ
	u8			_modeCur;		// 現在のモード
	f32			_frmCur;		// 現在のフレーム数
	f32			_frmMax;		// 最大フレーム数
	ColorU8*	_color1;		// 色１
	s32			_hintCtrl;		// 制御ヒント値
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_EFFECT_SCREEN_EFFECT_H_
