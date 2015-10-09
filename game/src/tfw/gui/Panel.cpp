/***********************************************************************//**
 *  Panel.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/11.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Panel.h"

// Friends
#include "PanelText.h"
#include "../g2d/TiledRectDraw.h"
#include "../gcmn/Renderer.h"
#include "../gcmn/RendererUtils.h"
#include "../gcmn/Texture.h"
#include "../lib/Point.h"
#include "../lib/Rect.h"
#include "../lib/Color.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Panel メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	矩形設定
**//*---------------------------------------------------------------------*/
void Panel::setRectangle(const RectF32* rect)
{
	// 矩形を設定
	_rect->copy(rect);

	// テキストがある場合はテキストもリサイズ
	if(_text != 0L)
	{
		RectF32 rect(_rect->x() + _text->_xPaddingLeft, _rect->y() + _text->_yPaddingTop, _rect->w() - _text->_xPaddingLeft - _text->_xPaddingRight, _rect->h() - _text->_yPaddingTop - _text->_yPaddingBottom);
		_text->_ptxt->setRectangle(&rect);
	}
}

/*---------------------------------------------------------------------*//**
	位置設定
**//*---------------------------------------------------------------------*/
void Panel::setPosition(const PointF32* pt)
{
	// 位置を設定
	_rect->setPoint(pt);

	// テキストがある場合はテキストも移動
	if(_text != 0L)
	{
		RectF32 rect(_rect->x() + _text->_xPaddingLeft, _rect->y() + _text->_yPaddingTop, _rect->w() - _text->_xPaddingLeft - _text->_xPaddingRight, _rect->h() - _text->_yPaddingTop - _text->_yPaddingBottom);
		_text->_ptxt->setRectangle(&rect);
	}
}

/*---------------------------------------------------------------------*//**
	未使用設定
**//*---------------------------------------------------------------------*/
void Panel::setUnused(bool isUnused)
{
	TFW_SET_FLAG(_flags, F_UNUSED, isUnused);

	// テキストがある場合はテキストも未使用設定
	if(_text != 0L)
	{
		_text->_ptxt->setUnused(isUnused);
	}
}

/*---------------------------------------------------------------------*//**
	現在の状態を設定
**//*---------------------------------------------------------------------*/
bool Panel::setCurrentState(State state)
{
	ASSERT((STATE_NULL <= state) && (state <= NUM_STATE));
	if(state == STATE_NULL)
	{
		_stateCur = STATE_NULL;
		return true;
	}

	// 状態情報が設定確認
	if(_starr[state - 1] == 0L)
	{
		// 失敗
		return false;
	}

	StateInfo* st = _starr[state - 1];

	// テクスチャアニメーションの補正
	if(st->_type == SI_ANIM_TEX)
	{
		if(_idxTexAnim >= ((StateInfoAnimTex*)st)->_numUv)
		{
			_idxTexAnim = 0;
		}
	}

	_stateCur = state;
	return true;
}

/*---------------------------------------------------------------------*//**
	ヒットテスト
**//*---------------------------------------------------------------------*/
bool Panel::hitTest(const PointF32* pt)
{
	if(!isEnable())	{	return false;	}
	return _rect->isPointIn(pt, _marginHitH, _marginHitV);
}

/*---------------------------------------------------------------------*//**
	現在位置によるヒット処理
**//*---------------------------------------------------------------------*/
bool Panel::hit(const PointF32* pt, bool isActive)
{
	if(!isEnable())	{	return false;	}
	if(isUnused())	{	return false;	}	// 使用不能

	return hit(_rect->isPointIn(pt, _marginHitH, _marginHitV), isActive);
}

/*---------------------------------------------------------------------*//**
	開始位置と現在位置によるヒット処理
**//*---------------------------------------------------------------------*/
bool Panel::hit(const PointF32* ptNow, const PointF32* ptStart, bool isActive)
{
	if(!isEnable())	{	return false;	}
	if(isUnused())	{	return false;	}	// 使用不能

	return hit(_rect->isPointIn(ptNow, _marginHitH, _marginHitV) && _rect->isPointIn(ptStart, _marginHitH, _marginHitV), isActive);
}

/*---------------------------------------------------------------------*//**
	ヒット処理
**//*---------------------------------------------------------------------*/
bool Panel::hit(bool isHover, bool isActive)
{
	if(!isEnable())	{	return false;	}
	if(isUnused())	{	return false;	}	// 使用不能

	if(isHover)	// ヒットしている
	{
		if(_cntHit < 0)
		{
			// 状態をホバーに
			_cntHit = 0;
			if(!TFW_IS_FLAG(_flags, F_MANUAL_STATE))	{	setCurrentState(STATE_HOVER);	}
		}
		if(TFW_IS_FLAG(_execflag, EF_CLICK_ACTIVE) && isActive && (_cntActive < 0))
		{
			// 状態をアクティブに
			_cntActive = 0;
			if(!TFW_IS_FLAG(_flags, F_MANUAL_STATE))	{	setCurrentState(STATE_ACTIVE);	}
		}
		return true;
	}
	else						// ヒットしていない
	{
		if(_cntHit >= 0)
		{
			// 状態をノーマルに
			_cntHit = -1;
			_cntActive = -1;
			if(!TFW_IS_FLAG(_flags, F_MANUAL_STATE))	{	setCurrentState(STATE_NORMAL);	}
		}
		return false;
	}
}

/*---------------------------------------------------------------------*//**
	ヒット毎フレーム処理
**//*---------------------------------------------------------------------*/
Panel::HitExecRes Panel::hitExec(const PointF32* ptNow, const PointF32* ptStart, const PointF32* ptAction, bool isActive, bool isAction)
{
	if (!isEnable()) { return HERES_NULL; }
	if (isUnused())	{ return HERES_NULL; }	// 使用不能

	bool isHit = hit(ptNow, ptStart, isActive);

	if (isAction && hitTest(ptAction))
	{
		return HERES_HIT_AND_ACTION;
	}

	return isHit ? HERES_HIT : HERES_NOHIT;
}

/*---------------------------------------------------------------------*//**
	テキスト作成
**//*---------------------------------------------------------------------*/
bool Panel::createText(Font* fontRef, s32 fontsize, const ColorU8* color, s16 xPaddingLeft, s16 xPaddingRight, s16 yPaddingTop, s16 yPaddingBottom, bool isEnableReadingAnimation, bool isEnableVerticalScrollAnimation, bool isAutoScroll)
{
	return createText(fontRef, fontsize, 0L, color, xPaddingLeft, xPaddingRight, yPaddingTop, yPaddingBottom, isEnableReadingAnimation, isEnableVerticalScrollAnimation, isAutoScroll);
}

/*---------------------------------------------------------------------*//**
	テキスト作成
**//*---------------------------------------------------------------------*/
bool Panel::createText(Font* fontRef, s32 fontsize, const VcString* str, const ColorU8* color, s16 xPaddingLeft, s16 xPaddingRight, s16 yPaddingTop, s16 yPaddingBottom, bool isEnableReadingAnimation, bool isEnableVerticalScrollAnimation, bool isAutoScroll)
{
	if(_text != 0L)	{	return false;	}

	PanelText* ptxt = new PanelText();
	if(ptxt == 0L)
	{
		ASSERT(false);
		return false;
	}
	RectF32 rect(_rect->x() + xPaddingLeft, _rect->y() + yPaddingTop, _rect->w() - xPaddingLeft -xPaddingRight, _rect->h() - yPaddingTop - yPaddingBottom);
	if(!ptxt->create(fontRef, fontsize, &rect, color))
	{
		delete ptxt;
		ASSERT(false);
		return false;
	}
	ptxt->setReadingAnimation(isEnableReadingAnimation, 1.0f, 0.0f);
	ptxt->setVerticalScrollAnimation(isEnableVerticalScrollAnimation, 1.0f);
	ptxt->setAutoScroll(isAutoScroll);

	_text = new TextSupport();
	_text->_ptxt = ptxt;
	_text->_xPaddingLeft = xPaddingLeft;
	_text->_xPaddingRight = xPaddingRight;
	_text->_yPaddingTop = yPaddingTop;
	_text->_yPaddingBottom = yPaddingBottom;
	if(str != 0L)
	{
		ptxt->setText(str, false);
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Panel::Panel()
	: _rect(0L)
	, _color(0L)
	, _text(0L)
	, _stateCur(STATE_NULL)
	, _execflag(EF_HOLF_ACTIVE)
	, _cntHit(-1)
	, _cntActive(-1)
	, _thcntHoldHitActive(MAX_HOLD_ACTIVE_COUNT_DEFAULT)
	, _ratioDissolveColor(0.25f)
	, _idxTexAnim(0)
	, _fcntTexAnim(0.0f)
	, _flags(0)
	, _marginHitH(0)
	, _marginHitV(0)
{
	for(int i = 0; i < NUM_STATE - 1; i++)	{	_starr[i] = 0L;	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Panel::~Panel()
{
	ASSERT(_rect == 0L);
	ASSERT(_color == 0L);
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_STATE - 1; i++)	{	ASSERT(_starr[i] == 0L);	}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Panel::create()
{
	RectF32 rect(0, 0, 10, 10);
	return create(&rect);	// 仮矩形で作成
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Panel::create(const RectF32* rect)
{
	// 矩形を作成
	ASSERT(_rect == 0L);	// 未作成のはず
	_rect = new RectF32(*rect);

	setEnable(true);	// デフォルトで有効化

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Panel::destroy()
{
	setEnable(false);	// 無効化

	// テキストを削除
	if(_text != 0L)
	{
		_text->_ptxt->destroy();
		delete _text;
		_text = 0L;
	}

	// 状態情報を削除
	for(int i = 0; i < NUM_STATE - 1; i++)
	{
		deleteStateInfo(_starr[i]);
		_starr[i] = 0L;
	}

	// 色を削除
	delete _color;
	_color = 0L;

	// 矩形を削除
	delete _rect;
	_rect = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Panel::exec(ExecRes* res, const ExecCtx* ec)
{
	if(_rect == 0L)	{	return;	}	// 未作成
	if(!isEnable())	{	return;	}	// 無効中

	// ヒット状態の更新
	if(_cntHit >= 0)	// ヒットしている場合
	{
		_cntHit++;	// ヒットカウントを増加
		if(TFW_IS_FLAG(_execflag, EF_HOLF_ACTIVE))	// 長押しでアクティブに
		{
			if(_cntHit >= _thcntHoldHitActive)
			{
				if(_cntActive < 0)
				{
					// 状態をアクティブに
					if(!TFW_IS_FLAG(_flags, F_MANUAL_STATE))	{	setCurrentState(STATE_ACTIVE);	}
					_cntActive = 1;
				}
				else
				{
					_cntActive++;
				}
			}
		}
		else
		{
			if(_cntActive >= 0)
			{
				_cntActive++;
			}
		}
	}
	else				// ヒットしない場合
	{
		_cntActive = -1;	// 非アクティブ
	}

	// 現在の状態による判定
	if((STATE_NULL < _stateCur) && (_stateCur < NUM_STATE))
	{
		StateInfo* st = _starr[_stateCur - 1];

		// 色の更新
		if((_color != 0L) && (st->_color != 0L))
		{
			if(*_color != *st->_color)	// 色が異なるとき
			{
				// 色をディゾルブする
				f32 rratio = 1.0f - _ratioDissolveColor;
				_color->r() = (u8)(st->_color->r() * _ratioDissolveColor + _color->r() * rratio);
				_color->g() = (u8)(st->_color->g() * _ratioDissolveColor + _color->g() * rratio);
				_color->b() = (u8)(st->_color->b() * _ratioDissolveColor + _color->b() * rratio);
				_color->a() = (u8)(st->_color->a() * _ratioDissolveColor + _color->a() * rratio);
			}
		}

		// テクスチャアニメーションの更新
		if(st->_type == SI_ANIM_TEX)
		{
			_fcntTexAnim += ec->getDeltaFrame();
			if(_fcntTexAnim >= ((StateInfoAnimTex*)st)->_intvlfrm)
			{
				// 次の UV へ
				_fcntTexAnim = 0.0f;
				_idxTexAnim++;
				if(_idxTexAnim >= ((StateInfoAnimTex*)st)->_numUv)
				{
					_idxTexAnim = 0;
				}
			}
		}
	}

	// テキストがある場合はテキストも処理
	if(_text != 0L)
	{
		_text->_ptxt->exec(res, ec);
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void Panel::draw(const RenderCtx* rc)
{
	if(_rect == 0L)	{	return;	}	// 未作成
	if(!isEnable())	{	return;	}	// 無効中

	// パネル自身の描画
	if((STATE_NULL < _stateCur) && (_stateCur < NUM_STATE))
	{
		StateInfo* st = _starr[_stateCur - 1];
		if(st == 0L)	{	return;	}

		Renderer* rdr = rc->getRenderer();

		// 色の設定
		if(isUnused())	// 使用不能
		{
			rdr->setSolidColor(127, 127, 127, 255);
		}
		else if(_color != 0L)
		{
			rdr->setSolidColor(_color);
		}
		else
		{
			rdr->setSolidColor(255, 255, 255, 255);
		}

		// 描画
		switch(st->_type)
		{
		case SI_COLOR_ONLY:
			if(_color != 0L)
			{
				// 固定ベクトル描画
				ColorU8 color1, color2;
				// 上半分
				RendererUtils::boostColor(&color1, _color, 1.5f);
				RendererUtils::boostColor(&color2, _color, 1.1f);
				RendererUtils::draw2dColorRect(rdr, (f32)_rect->x(), (f32)_rect->y(), (f32)_rect->w(), (f32)(_rect->h() / 2), &color1, &color1, &color2, &color2);
				// 下半分
				color1 = *_color;
				RendererUtils::boostColor(&color2, _color, 0.9f);
				RendererUtils::draw2dColorRect(rdr, (f32)_rect->x(), (f32)(_rect->y() + _rect->h() / 2), (f32)_rect->w(), (f32)(_rect->h() / 2), &color1, &color1, &color2, &color2);
				// 上線
				RendererUtils::boostColor(&color1, _color, 1.7f);
				rdr->setSolidColor(&color1);
				RendererUtils::draw2dLine(rdr, (f32)_rect->x(), (f32)_rect->y(), (f32)(_rect->x() + _rect->w()), (f32)_rect->y());
				// 下線
				RendererUtils::boostColor(&color1, _color, 0.5f);
				rdr->setSolidColor(&color1);
				RendererUtils::draw2dLine(rdr, (f32)_rect->x(), (f32)(_rect->y() + _rect->h()), (f32)(_rect->x() + _rect->w()), (f32)(_rect->y() + _rect->h()));
				// 横線
				RendererUtils::boostColor(&color1, _color, 0.6f);
				rdr->setSolidColor(&color1);
				RendererUtils::draw2dLine(rdr, (f32)_rect->x(), (f32)_rect->y(), (f32)_rect->x(), (f32)(_rect->y() + _rect->h()));
				RendererUtils::draw2dLine(rdr, (f32)(_rect->x() + _rect->w()), (f32)_rect->y(), (f32)(_rect->x() + _rect->w()), (f32)(_rect->y() + _rect->h()));
				rdr->setSolidColor(255, 255, 255, 255);
			}
			break;
		case SI_ONE_TEX:
			{
				StateInfoOneTex* sto = (StateInfoOneTex*)st;
				if(sto->_texRef != 0L)
				{
					rdr->bindTexture(sto->_texRef);

					if(TFW_IS_FLAG(_flags, F_DRAW__MASK_SHADOW))	// 影付き
					{
						RectF32 rectS(_rect);
						if(TFW_IS_FLAG(_flags, F_DRAW_LEFT_SHADOW))			{	rectS.left() -= 1;	}
						else if(TFW_IS_FLAG(_flags, F_DRAW_RIGHT_SHADOW))	{	rectS.left() += 1;	}
						if(TFW_IS_FLAG(_flags, F_DRAW_TOP_SHADOW))			{	rectS.top() -= 1;	}
						else if(TFW_IS_FLAG(_flags, F_DRAW_BOTTOM_SHADOW))	{	rectS.top() += 1;	}
						ColorU8 colBk(*rdr->getSolidColor());
						rdr->setSolidColor(0, 0, 0, 255);
						RendererUtils::draw2dTextureRect(rdr, &rectS, sto->_uv);
						rdr->setSolidColor(&colBk);
					}

					RendererUtils::draw2dTextureRect(rdr, _rect, sto->_uv);
				}
			}
			break;
		case SI_ANIM_TEX:
			{
				StateInfoAnimTex* sta = (StateInfoAnimTex*)st;
				if(sta->_texRef != 0L)
				{
					rdr->bindTexture(sta->_texRef);

					ASSERT(_idxTexAnim < sta->_numUv);
					RectF32* uv = &sta->_uv[_idxTexAnim];

					if(TFW_IS_FLAG(_flags, F_DRAW__MASK_SHADOW))	// 影付き
					{
						RectF32 rectS(_rect);
						if(TFW_IS_FLAG(_flags, F_DRAW_LEFT_SHADOW))			{	rectS.left() -= 1;	}
						else if(TFW_IS_FLAG(_flags, F_DRAW_RIGHT_SHADOW))	{	rectS.left() += 1;	}
						if(TFW_IS_FLAG(_flags, F_DRAW_TOP_SHADOW))			{	rectS.top() -= 1;	}
						else if(TFW_IS_FLAG(_flags, F_DRAW_BOTTOM_SHADOW))	{	rectS.top() += 1;	}
						ColorU8 colBk(*rdr->getSolidColor());
						rdr->setSolidColor(0, 0, 0, 255);
						RendererUtils::draw2dTextureRect(rdr, &rectS, uv);
						rdr->setSolidColor(&colBk);
					}

					RendererUtils::draw2dTextureRect(rdr, _rect, uv);
				}
			}
			break;
		case SI_TILE_TEX:
			{
				StateInfoTileTex* stt = (StateInfoTileTex*)st;
				if(stt->_tile != 0L)
				{
					if(TFW_IS_FLAG(_flags, F_DRAW__MASK_SHADOW))	// 影付き
					{
						RectF32 rectS(_rect);
						if(TFW_IS_FLAG(_flags, F_DRAW_LEFT_SHADOW))			{	rectS.left() -= 1;	}
						else if(TFW_IS_FLAG(_flags, F_DRAW_RIGHT_SHADOW))	{	rectS.left() += 1;	}
						if(TFW_IS_FLAG(_flags, F_DRAW_TOP_SHADOW))			{	rectS.top() -= 1;	}
						else if(TFW_IS_FLAG(_flags, F_DRAW_BOTTOM_SHADOW))	{	rectS.top() += 1;	}
						ColorU8 colBk(*rdr->getSolidColor());
						rdr->setSolidColor(0, 0, 0, 255);
						stt->_tile->draw(rc, &rectS);
						rdr->setSolidColor(&colBk);
					}

					stt->_tile->draw(rc, _rect);
				}
			}
			break;
		}
	}

	// テキストがある場合はテキストも描画
	if(_text != 0L)
	{
		_text->_ptxt->draw(rc);
	}
}

/*---------------------------------------------------------------------*//**
	状態情報を設定する（テクスチャなし）
**//*---------------------------------------------------------------------*/
void Panel::setStateInfo(State state, const ColorU8* color)
{
	// 情報を得る
	StateInfo* st = setStateInfoCommonPre(state, SI_COLOR_ONLY);

	// 共通処理
	setStateInfoCommonPost(st, state, color);
}

/*---------------------------------------------------------------------*//**
	状態情報を設定する（アニメーションなし、色なし）
**//*---------------------------------------------------------------------*/
void Panel::setStateInfo(State state, Texture* texRef, const RectF32* uv)
{
	setStateInfo(state, texRef, uv, 0L);
}

/*---------------------------------------------------------------------*//**
	状態情報を設定する（アニメーションなし、色あり）
**//*---------------------------------------------------------------------*/
void Panel::setStateInfo(State state, Texture* texRef, const RectF32* uv, const ColorU8* color)
{
	// 情報を得る
	StateInfoAnimTex* st = (StateInfoAnimTex*)setStateInfoCommonPre(state, SI_ONE_TEX);

	// テクスチャ設定
	st->_texRef = texRef;

	// テクスチャ座標設定
	if(st->_uv == 0L)
	{
		st->_uv = new RectF32(uv);
	}
	ASSERT(st->_uv != 0L);

	// 共通処理
	setStateInfoCommonPost(st, state, color);
}

/*---------------------------------------------------------------------*//**
	状態情報を設定する（アニメーションあり、色なし）
**//*---------------------------------------------------------------------*/
void Panel::setStateInfo(State state, Texture* texRef, const RectF32* uv, s16 numUv, s16 intvlfrm)
{
	setStateInfo(state, texRef, uv, numUv, intvlfrm, 0L);
}

/*---------------------------------------------------------------------*//**
	状態情報を設定する（アニメーションあり、色あり）
**//*---------------------------------------------------------------------*/
void Panel::setStateInfo(State state, Texture* texRef, const RectF32* uv, s16 numUv, s16 intvlfrm, const ColorU8* color)
{
	// 情報を得る
	StateInfoAnimTex* st = (StateInfoAnimTex*)setStateInfoCommonPre(state, SI_ANIM_TEX);

	// テクスチャ設定
	st->_texRef = texRef;
	st->_intvlfrm = intvlfrm;

	// テクスチャ座標設定
	if(numUv > 0)
	{
		delete st->_uv;
		st->_uv = new RectF32[numUv];
		ASSERT(st->_uv != 0L);
		st->_numUv = numUv;
		for(int i = 0; i < st->_numUv; i++)
		{
			st->_uv[i].copy(&uv[i]);
		}
	}

	// 共通処理
	setStateInfoCommonPost(st, state, color);
}

/*---------------------------------------------------------------------*//**
	状態情報を設定する（タイル、色なし）
**//*---------------------------------------------------------------------*/
void Panel::setStateInfo(State state, TiledRectDraw* tileEntr)
{
	setStateInfo(state, tileEntr, 0L);
}

/*---------------------------------------------------------------------*//**
	状態情報を設定する（タイル、色あり）
**//*---------------------------------------------------------------------*/
void Panel::setStateInfo(State state, TiledRectDraw* tileEntr, ColorU8* color)
{
	// 情報を得る
	StateInfoTileTex* st = (StateInfoTileTex*)setStateInfoCommonPre(state, SI_TILE_TEX);

	// 設定
	st->_tile = tileEntr;

	// 共通処理
	setStateInfoCommonPost(st, state, color);
}

/*---------------------------------------------------------------------*//**
	状態情報作成
**//*---------------------------------------------------------------------*/
Panel::StateInfo* Panel::newStateInfo(StateInfoType sitype) const
{
	switch(sitype)
	{
	case SI_COLOR_ONLY:	return new StateInfo();
	case SI_ONE_TEX:	return new StateInfoOneTex();
	case SI_ANIM_TEX:	return new StateInfoAnimTex();
	case SI_TILE_TEX:	return new StateInfoTileTex();
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	状態情報削除
	※	仮想関数テーブルのメモリ節約のため、型を指定して削除する
**//*---------------------------------------------------------------------*/
void Panel::deleteStateInfo(Panel::StateInfo* st)
{
	if(st != 0L)
	{
		switch(st->_type)
		{
		case SI_COLOR_ONLY:	delete (StateInfo*)st;			break;
		case SI_ONE_TEX:	delete (StateInfoOneTex*)st;	break;
		case SI_ANIM_TEX:	delete (StateInfoAnimTex*)st;	break;
		case SI_TILE_TEX:	delete (StateInfoTileTex*)st;	break;
		}
	}
}

/*---------------------------------------------------------------------*//**
	状態情報設定、共通前処理
**//*---------------------------------------------------------------------*/
Panel::StateInfo* Panel::setStateInfoCommonPre(State state, StateInfoType sitype)
{
	// 情報を得る
	ASSERT((1 <= state) && (state <= (NUM_STATE - 1)));

	StateInfo* st = _starr[state - 1];

	// 型確認
	if(st != 0L)
	{
		// 型が違う場合は一度削除
		if(st->_type != sitype)
		{
			deleteStateInfo(st);
			st = 0L;
		}
	}

	// ない場合は新規に作成
	if(st == 0L)
	{
		st = newStateInfo(sitype);
		ASSERT(st != 0L);

		_starr[state - 1] = st;
	}

	return st;
}

/*---------------------------------------------------------------------*//**
	状態情報設定、共通後処理
**//*---------------------------------------------------------------------*/
void Panel::setStateInfoCommonPost(Panel::StateInfo* st, State state, const ColorU8* color)
{
	// 色の設定
	if(color != 0L)
	{
		if(st->_color != 0L)	{	st->_color->copy(color);			}
		else					{	st->_color = new ColorU8(color);	}
		ASSERT(st->_color != 0L);

		// Panel にカラーが用意されていなければ用意する
		if(_color == 0L)
		{
			_color = new ColorU8(255, 255, 255, 255);
			ASSERT(_color != 0L);
		}
	}

	// 状態を STATE_NULL から脱却
	if((state == STATE_NORMAL) && (_stateCur == STATE_NULL))
	{
		setCurrentState(STATE_NORMAL);

		// デフォルトの色を設定
		if(_color != 0L)
		{
			_color->copy(color);
		}
	}
}

//==========================================================================
// Panel::StateInfo メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Panel::StateInfo::StateInfo()
	: _color(0L)
{
	_type = SI_COLOR_ONLY;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Panel::StateInfo::~StateInfo()
{
	delete _color;
}

//==========================================================================
// Panel::StateInfoOneTex メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Panel::StateInfoOneTex::StateInfoOneTex()
	: _texRef(0L)
	, _uv(0L)
{
	_type = SI_ONE_TEX;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Panel::StateInfoOneTex::~StateInfoOneTex()
{
	delete[] _uv;
}

//==========================================================================
// Panel::StateInfoAnimTex メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Panel::StateInfoAnimTex::StateInfoAnimTex()
	: _texRef(0L)
	, _uv(0L)
	, _numUv(0)
	, _intvlfrm(0)
{
	_type = SI_ANIM_TEX;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Panel::StateInfoAnimTex::~StateInfoAnimTex()
{
	delete[] _uv;
}

//==========================================================================
// Panel::StateInfoTileTex メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Panel::StateInfoTileTex::StateInfoTileTex()
	: _tile(0L)
{
	_type = SI_TILE_TEX;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Panel::StateInfoTileTex::~StateInfoTileTex()
{
	_tile->destroy();
	delete _tile;
}

//==========================================================================
// Panel::TextSupport メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Panel::TextSupport::TextSupport()
	: _ptxt(0L)
	, _xPaddingLeft(0)
	, _xPaddingRight(0)
	, _yPaddingTop(0)
	, _yPaddingBottom(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Panel::TextSupport::~TextSupport()
{
	delete _ptxt;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
