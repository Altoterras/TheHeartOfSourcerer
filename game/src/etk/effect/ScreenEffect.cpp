/***********************************************************************//**
 *	ScreenEffect.cpp
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/11/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ScreenEffect.h"

// Friends
#include "../body/EtkExecCtx.h"

// External
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/lib/Color.h"

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ScreenEffect 静的変数

ScreenEffect::ModeInfo s_modeinfo[] =
{
	ScreenEffect::ModeInfo(0),	// MODE_NULL
	ScreenEffect::ModeInfo(30),	// MODE_FILL
	ScreenEffect::ModeInfo(30),	// MODE_FADE_IN
	ScreenEffect::ModeInfo(30),	// MODE_FADE_OUT
	ScreenEffect::ModeInfo(24),	// MODE_FLASH_1
	ScreenEffect::ModeInfo(24),	// MODE_FLASH_2
};

//==========================================================================
// ScreenEffect メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	色１を設定する
**//*---------------------------------------------------------------------*/
void ScreenEffect::setColor1(const ColorU8* color)
{
	_color1->copy(color);
}

/*---------------------------------------------------------------------*//**
	最大有効フレームを設定する
**//*---------------------------------------------------------------------*/
void ScreenEffect::setMaxFrame(f32 frmMax)
{
	_frmMax = frmMax;
}

/*---------------------------------------------------------------------*//**
	スクリーンサイズを設定する
**//*---------------------------------------------------------------------*/
void ScreenEffect::setScreenSize(f32 wScreen, f32 hScreen)
{
	_rectScreen->w() = wScreen;
	_rectScreen->h() = hScreen;
}

/*---------------------------------------------------------------------*//**
	開始する

	@return エフェクトに対応する制御ヒント値（end 時に使用）
**//*---------------------------------------------------------------------*/
s32 ScreenEffect::start(u8 mode)
{
	_modeCur = mode;
	_frmCur	= 0.0f;
	if(_frmMax == 0)	{	_frmMax	= (f32)s_modeinfo[_modeCur]._frmMaxDefault;	}	// 標準最大フレーム数
	_hintCtrl++;
	return _hintCtrl;
}

/*---------------------------------------------------------------------*//**
	終了する

	@param hintCtrl エフェクトに対応する制御ヒント値．
					ヒント値が合致しないと end しない．
					ただし、0 以下指定時はヒント値を無視して強制的に終了する
**//*---------------------------------------------------------------------*/
void ScreenEffect::end(s32 hintCtrl)
{
	if((hintCtrl > 0) && (_hintCtrl != hintCtrl))
	{
		return;
	}

	_modeCur = MODE_NULL;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ScreenEffect::ScreenEffect()
	: _rectScreen(0L)
	, _flagsConf(0L)
	, _modeCur(MODE_NULL)
	, _frmCur(0.0f)
	, _frmMax(0.0f)
	, _color1(0L)
	, _hintCtrl(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ScreenEffect::~ScreenEffect()
{
	ASSERT(_rectScreen == 0L);
	ASSERT(_color1 == 0L);
}
	
/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ScreenEffect::create(f32 wScreen, f32 hScreen)
{
	_rectScreen = new RectF32(0, 0, wScreen, hScreen);
	_color1 = new ColorU8(255, 255, 255, 255);
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ScreenEffect::destroy()
{
	delete _color1;
	_color1 = 0L;
	delete _rectScreen;
	_rectScreen = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void ScreenEffect::exec(ExecRes* res, const ExecCtx* ec)
{
	if(TFW_IS_FLAG(_flagsConf, CF_ENABLE_EVENT_PAUSE))
	{
		if(((EtkExecCtx*)ec)->isEventPause())
		{
			return;
		}
	}

	_frmCur += ec->getDeltaFrame();
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void ScreenEffect::draw(const RenderCtx* rc)
{
	Renderer* rdr = rc->getRenderer();
	switch(_modeCur)
	{
	case MODE_FILL:			drawFill(rdr);			break;
	case MODE_FADE_IN:		drawFadeIn(rdr);		break;
	case MODE_FADE_OUT:		drawFadeOut(rdr);		break;
	case MODE_FLASH_1:		drawFlash1(rdr);		break;
	case MODE_FLASH_2:		drawFlash2(rdr);		break;
	}
}

/*---------------------------------------------------------------------*//**
	描画 - 塗りつぶし
**//*---------------------------------------------------------------------*/
void ScreenEffect::drawFill(Renderer* rdr)
{
	if(	(_frmMax == FRAME_INFINITY) ||
		(_frmCur < (_frmMax + 1.0f + 0.01f))	)	// 最終フレームまで描画する
													// 整数的に判定するため、+1.0f 未満としている
													// + 0.01f は _frmCur の誤差蓄積の可能性があると思われるので（数値にあまり根拠ない）
	{
		// 矩形描画
		RendererUtils::draw2dColorRect(rdr, _rectScreen, _color1);
	}
}

/*---------------------------------------------------------------------*//**
	描画 - フェードイン
**//*---------------------------------------------------------------------*/
void ScreenEffect::drawFadeIn(Renderer* rdr)
{
	// 初速度高減衰 alpha
	if(_frmCur < _frmMax)	// 最大フレームを含めないのは、必ずアルファが 0 になるから
	{
		f32 alpha = 1.0f - (f32)::sinf(_frmCur / _frmMax * TFW_PI * 0.5f);

		// 矩形描画
		ColorU8 col;
		RendererUtils::draw2dColorRect(rdr, _rectScreen, ColorU8::setout(&col, _color1->r(), _color1->g(), _color1->b(), (u8)(_color1->a() * alpha)));
	}

	// フェードイン後放置してもあまり負荷がかからないように
}

/*---------------------------------------------------------------------*//**
	描画 - フェードアウト
**//*---------------------------------------------------------------------*/
void ScreenEffect::drawFadeOut(Renderer* rdr)
{
	// 初速度高増加 alpha
	f32 alpha;
	if(_frmCur < _frmMax)	{	alpha = (f32)::sinf(_frmCur / _frmMax * TFW_PI * 0.5f);		}
	else					{	alpha = 1.0f;												}
	
	// 矩形描画
	ColorU8 col;
	RendererUtils::draw2dColorRect(rdr, _rectScreen, ColorU8::setout(&col, _color1->r(), _color1->g(), _color1->b(), (u8)(_color1->a() * alpha)));
}

/*---------------------------------------------------------------------*//**
	描画 - １回フラッシュ
**//*---------------------------------------------------------------------*/
void ScreenEffect::drawFlash1(Renderer* rdr)
{
	if(_frmCur < (_frmMax + 1.0f))
	{
		f32 alpha;
		/*
		// 中央立ち上がり型 alpha
		if(_frmMode < (FRAME_START_PRS / 2))
		{
			alpha = 1.0f - (f32)Math.cos(_frmMode / (double)FRAME_START_PRS * TFW_PI);
		}
		else
		{
			alpha = 1.0f + (f32)Math.cos(_frmMode / (double)FRAME_START_PRS * TFW_PI);
		}
		*/
		/*
		// 放物線（初速度低）減衰 alpha
		alpha = (f32)::cosf(_frmMode / (double)FRAME_START_PRS * TFW_PI * 0.5);
		*/
		// 初速度高減衰 alpha
		alpha = 1.0f - ::sinf(_frmCur / _frmMax * TFW_PI * 0.5f);
		
		// 矩形描画
		ColorU8 col;
		RendererUtils::draw2dColorRect(rdr, _rectScreen, ColorU8::setout(&col, _color1->r(), _color1->g(), _color1->b(), (u8)(_color1->a() * alpha)));
	}
}

/*---------------------------------------------------------------------*//**
	描画 - ２回フラッシュ
**//*---------------------------------------------------------------------*/
void ScreenEffect::drawFlash2(Renderer* rdr)
{
	const f32 frmMaxH = _frmMax / 2;

	if(_frmCur < (frmMaxH + 1.0f))
	{
		// 初速度高減衰 alpha
		f32 alpha = 1.0f - ::sinf(_frmCur / frmMaxH * TFW_PI * 0.5f);
		
		// 矩形描画
		ColorU8 col;
		RendererUtils::draw2dColorRect(rdr, _rectScreen, ColorU8::setout(&col, _color1->r(), _color1->g(), _color1->b(), (u8)(_color1->a() * alpha)));
	}
	else if(_frmCur < ((frmMaxH * 2) + 1.0f))
	{
		// 初速度高減衰 alpha
		f32 alpha = 1.0f - ::sinf((_frmCur - frmMaxH) / frmMaxH * TFW_PI * 0.5f);
		
		// 矩形描画
		ColorU8 col;
		RendererUtils::draw2dColorRect(rdr, _rectScreen, ColorU8::setout(&col, _color1->r(), _color1->g(), _color1->b(), (u8)(_color1->a() * alpha)));
	}
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS