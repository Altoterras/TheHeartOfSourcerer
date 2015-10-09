/***********************************************************************//**
 *	MoveCursor.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MoveCursor.h"

// Friends
#include "MoveCursorExtension.h"
#include "../body/GameExecCtx.h"

// External

// Library
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/lib/Point.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/lib/Calc.h"
#include "../../tfw/ui/TouchPanel.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 制御情報
#define LEN_MIN			(2.0f)							// 最小スクリーン座標差（ピクセル）
#define LEN_MIN_SQ		(LEN_MIN * LEN_MIN)				// 最小スクリーン座標差の２乗
#define LEN_MAX			(88.0f)						// 最大スクリーン座標差（ピクセル）
#define LEN_MAX_SQ		(LEN_MAX * LEN_MAX)				// 最大スクリーン座標差の２乗
#define RATIO_WALK		(0.5f)							// 歩き率
#define RATIO_RUN		(0.9f)							// 走り率
#define LEN_WALK		(LEN_MAX * RATIO_WALK)			// 歩き距離
#define LEN_WALK_SQ		(LEN_WALK * LEN_WALK)			// 歩き距離の２乗
#define LEN_RUN			(LEN_MAX * RATIO_RUN)			// 走り距離
#define LEN_RUN_SQ		(LEN_RUN * LEN_RUN)				// 走り距離の２乗

// テクスチャ情報
#define W_TEX			(1024.0f)						// テクスチャ全体の幅
#define H_TEX			(1024.0f)						// テクスチャ全体の高さ
#define W_CIRC			(32)							// 丸部分の幅
#define H_CIRC			(32)							// 丸部分の高さ
#define UV_CIRC_SLOW	608.0f, 992.0f, 32.0f, 32.0f	// 丸部分のテクスチャ座標（低速）
#define UV_CIRC_WALK	608.0f, 960.0f, 32.0f, 32.0f	// 丸部分のテクスチャ座標（中速）
#define UV_CIRC_RUN		608.0f, 928.0f, 32.0f, 32.0f	// 丸部分のテクスチャ座標（高速）
#define W_BAR			(32)							// 棒部分の幅
#define H_BAR			(32)							// 棒部分の高さ
#define UV_BAR_SLOW		576.0f, 992.0f, 32.0f, 32.0f	// 棒部分のテクスチャ座標（低速）
#define UV_BAR_WALK		576.0f, 960.0f, 32.0f, 32.0f	// 棒部分のテクスチャ座標（低速）
#define UV_BAR_RUN		576.0f, 928.0f, 32.0f, 32.0f	// 棒部分のテクスチャ座標（低速）
#define W_ARW			(40)							// 矢印部分の幅
#define H_ARW			(32)							// 矢印部分の高さ
#define UV_ARW_SLOW		536.0f, 992.0f, 40.0f, 32.0f	// 矢印部分のテクスチャ座標（低速）
#define UV_ARW_WALK		536.0f, 960.0f, 40.0f, 32.0f	// 矢印部分のテクスチャ座標（低速）
#define UV_ARW_RUN		536.0f, 928.0f, 40.0f, 32.0f	// 矢印部分のテクスチャ座標（低速）

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MoveCursor メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	X 方向タッチ位置差分の最大値を得る
**//*---------------------------------------------------------------------*/
s32 MoveCursor::getMaxTouchDiffX() const
{
	return (s32)LEN_MAX;
}

/*---------------------------------------------------------------------*//**
	Y 方向タッチ位置差分の最大値を得る
**//*---------------------------------------------------------------------*/
s32 MoveCursor::getMaxTouchDiffY() const
{
	return (s32)LEN_MAX;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	有効かどうかを設定する
**//*---------------------------------------------------------------------*/
void MoveCursor::setEnable(u16 edch, bool isEnable)
{
	if(TFW_IS_FLAG(_edchFlags, edch) != !isEnable)
	{
		TFW_SET_FLAG(_edchFlags, edch, !isEnable);

		if(_edchFlags != 0)
		{
			reset();
		}
	}
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void MoveCursor::reset()
{
	if(_isTouching)
	{
		_ext->onTapEnd();
		_isTouching = false;
	}

	_ptTouchStart->set(F32_PMAX, F32_PMAX);
	_ptTouchLast->set(F32_PMAX, F32_PMAX);
	_xDiff = 0;
	_yDiff = 0;
	_state = STATE_STOP;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MoveCursor::MoveCursor()
	: _edchFlags(0)
	, _isTouching(false)
	, _ptTouchStart(0L)
	, _ptTouchLast(0L)
	, _xDiff(0)
	, _yDiff(0)
	, _angle(0.0f)
	, _length(0.0f)
	, _rateLength(0.0f)
	, _state(STATE_STOP)
	, _texRef(0L)
	, _ext(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MoveCursor::~MoveCursor()
{
	ASSERT(_ptTouchLast == 0L);
	ASSERT(_ptTouchStart == 0L);
	ASSERT(_ext == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MoveCursor::create(Texture* texRef)
{
	// ポイントを作成する
	_ptTouchLast = new PointF32(F32_PMAX, F32_PMAX);
	_ptTouchStart = new PointF32(F32_PMAX, F32_PMAX);

	// 拡張モジュールを作成する
	_ext = new MoveCursorExtension();
	if(!_ext->create())
	{
		ASSERT(false);
		return false;
	}

	_texRef = texRef;				// テクスチャポインタを保持
	setEnable(EDCH_SELF, true);		// 自身を有効に

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MoveCursor::destroy()
{
	setEnable(EDCH_SELF, false);	// 自身に無効に
	_texRef = 0L;					// テクスチャポインタをクリア

	// 拡張モジュールを削除する
	if(_ext != 0L)
	{
		_ext->destroy();
		delete _ext;
		_ext = 0L;
	}

	// ポイントを削除する
	delete _ptTouchLast;
	_ptTouchLast = 0L;
	delete _ptTouchStart;
	_ptTouchStart = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void MoveCursor::exec(ExecRes* res, const ExecCtx* ec)
{
	if(TFW_IS_FLAG(_edchFlags, EDCH_EXT_MASK))	{	return;	}	// 外部からの無効化

	// 歩行処理
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* ui = gec->getTouchPanel(); 

	bool isEnableSelf = (!gec->isOpenedLockGui()) && (!gec->isLockOperating());
	TFW_SET_FLAG(_edchFlags, EDCH_SELF, !isEnableSelf);
	if(isEnableSelf && !ui->isReacted())
	{
		// 差分を求める
		Vector2F vDiff;
		do
		{
			// 最初にクリックしたとこからの差分で移動
			bool isTouched = _isTouching;
			_isTouching = (ui->getTouchingNum() == 1);
			// タッチ時のポイントを得る
			if(!isTouched && _isTouching)		// タッチした
			{
				ui->getTouchPosition(_ptTouchStart);	// 開始のタッチ位置を得る
				ui->setDragOperating(true);
				_ext->onTapStart(_ptTouchStart);
			}
			else if(isTouched && !_isTouching)	// 離した
			{
				// UI 反応済みフラグを立てるかどうかの判定
				Vector2F vDiff( (f32)(_ptTouchLast->x() - _ptTouchStart->x()), (f32)(_ptTouchLast->y() - _ptTouchStart->y()) );
				f32 lenDiffSq = vDiff.lengthSq();
				if(lenDiffSq >= LEN_MIN_SQ)	// 最小長さを超えている
				{
					ui->setReacted(true);	// 反応済みフラグを立てる
				}

				_ptTouchStart->set(F32_PMAX, F32_PMAX);
				_ptTouchLast->set(F32_PMAX, F32_PMAX);
				_xDiff = 0;
				_yDiff = 0;
				_ext->onTapEnd();
				break;	// 差分計算から抜ける
			}
			if(_isTouching)
			{
				ui->getTouchPosition(_ptTouchLast);		// 現在のタッチ位置を得る
			}

			// 差分を計算する
			vDiff.set( (f32)(_ptTouchLast->x() - _ptTouchStart->x()), (f32)(_ptTouchLast->y() - _ptTouchStart->y()) );
			f32 lenDiffPw = vDiff.lengthSq();
			if(lenDiffPw > LEN_MAX_SQ)	// 最大長さを超えている
			{
				// 最大長さに収める
				vDiff.normalize();
				vDiff *= LEN_MAX;
				// タッチ位置の補正
				_ptTouchLast->x() = _ptTouchStart->x() + (s32)vDiff.x(); 
				_ptTouchLast->y() = _ptTouchStart->y() + (s32)vDiff.y(); 
				ui->setReacted(true);	// 反応済みフラグを立てる
			}
			else if(lenDiffPw < LEN_MIN_SQ)	// 最小長さ未満
			{
				vDiff.set(0.0f, 0.0f);
				_xDiff = 0;
				_yDiff = 0;
				break;	// 差分計算から抜ける
			}
			else
			{
				ui->setReacted(true);	// 反応済みフラグを立てる
			}
			_xDiff = (s32)vDiff.x();
			_yDiff = (s32)vDiff.y();
		}
		while(false);

		// 描画用に角度を得ておく
		Calc::calcAngle(&_angle, vDiff.x(), vDiff.y());
	}
	else if(_isTouching)
	{
		_xDiff = 0;
		_yDiff = 0;
		_isTouching = false;
		_ext->onTapEnd();
	}

	// ２点間の距離を求める
	f32 lenPw = (f32)((_xDiff * _xDiff) + (_yDiff * _yDiff));
	_length = ::sqrtf(lenPw);
	_rateLength = _length / LEN_MAX;

	// 状態を更新する
	if(_length < LEN_WALK)
	{
		_state = STATE_STOP;
	}
	else if(_length < LEN_RUN)
	{
		_state = STATE_WALK;
	}
	else
	{
		_state = STATE_RUN;
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void MoveCursor::draw(const RenderCtx* rc)
{
	if(_edchFlags != 0)	{	return;	}	// 無効化中

	Renderer* rdr = rc->getRenderer();

#if 0	// テスト用のテクスチャ無し描画
	rdr->bindTexture(0L);						// テクスチャなし
	rdr->setSolidColor(127, 191, 255, 63);		// 頂点カラー設定

	RendererUtils::draw2dRect(rdr, _ptTouchStart->x() - 10.0f, _ptTouchStart->y() - 10.0f, 20.0f, 20.0f);

	Vector3F vtxarr[2] = { Vector3F((f32)_ptTouchStart->x(), (f32)_ptTouchStart->y(), 0.0f), Vector3F((f32)_ptTouchLast->x(), (f32)_ptTouchLast->y(), 0.0f) };
	RendererUtils::render3dLines(rdr, vtxarr, 2, RendererUtils::LINES);

	rdr->setSolidColor(255, 255, 255, 255);		// 頂点カラー白
#else	// テクスチャを用いた描画
	if((_xDiff != 0) || (_yDiff != 0))
	{
		RectF32 vtx;
		RectF32 uv;

		// 行列を保存
		::glPushMatrix();
		// テクスチャ設定
		rdr->bindTexture(_texRef);
		// 加算
		rdr->setAlphaBlendFunc(Renderer::AFUNC_ADD);
		// 2D 解像描画比
		f32 rate2dr = Env_get2drRate();

		// 状態によって色を変える
		rdr->setSolidColor(255, 255, 255, 255);
		switch(_state)
		{
		default:
			// 丸部分
			RendererUtils::draw2dTextureRect(
				rdr,
				RectF32::setout(&vtx, (f32)(_ptTouchStart->x() - (W_CIRC / 2)), (f32)(_ptTouchStart->y() - (H_CIRC / 2)), W_CIRC, H_CIRC),
				Gcalc::calcTexUv(&uv, UV_CIRC_SLOW, W_TEX, H_TEX, rate2dr)	);
			// 棒部分
			::glTranslatef((f32)_ptTouchLast->x(), (f32)_ptTouchLast->y(), 0.0f);
			::glRotatef(TFW_RAD_TO_DEG(_angle), 0.0f, 0.0f, 1.0f);
			RendererUtils::draw2dTextureRect(
				rdr,
				RectF32::setout(&vtx, - _length, (f32)(- H_BAR / 2), _length, H_BAR),
				Gcalc::calcTexUv(&uv, UV_BAR_SLOW, W_TEX, H_TEX, rate2dr)	);
			// 矢印部分
			RendererUtils::draw2dTextureRect(
				rdr,
				RectF32::setout(&vtx, (f32)(W_ARW / 2), (f32)(- H_ARW / 2), - W_ARW, H_ARW),
				Gcalc::calcTexUv(&uv, UV_ARW_SLOW, W_TEX, H_TEX, rate2dr)	);
			break;
		case STATE_WALK:
			// 丸部分
			RendererUtils::draw2dTextureRect(
				rdr,
				RectF32::setout(&vtx, (f32)(_ptTouchStart->x() - (W_CIRC / 2)), (f32)(_ptTouchStart->y() - (H_CIRC / 2)), W_CIRC, H_CIRC),
				Gcalc::calcTexUv(&uv, UV_CIRC_WALK, W_TEX, H_TEX, rate2dr)	);
			// 棒部分
			::glTranslatef((f32)_ptTouchLast->x(), (f32)_ptTouchLast->y(), 0.0f);
			::glRotatef(TFW_RAD_TO_DEG(_angle), 0.0f, 0.0f, 1.0f);
			RendererUtils::draw2dTextureRect(
				rdr,
				RectF32::setout(&vtx, - _length, (f32)(- H_BAR / 2), _length, H_BAR),
				Gcalc::calcTexUv(&uv, UV_BAR_WALK, W_TEX, H_TEX, rate2dr)	);
			// 矢印部分
			RendererUtils::draw2dTextureRect(
				rdr,
				RectF32::setout(&vtx, (f32)(W_ARW / 2), (f32)(- H_ARW / 2), - W_ARW, H_ARW),
				Gcalc::calcTexUv(&uv, UV_ARW_WALK, W_TEX, H_TEX, rate2dr)	);
			break;
		case STATE_RUN:
			// 丸部分
			RendererUtils::draw2dTextureRect(
				rdr,
				RectF32::setout(&vtx, (f32)(_ptTouchStart->x() - (W_CIRC / 2)), (f32)(_ptTouchStart->y() - (H_CIRC / 2)), W_CIRC, H_CIRC),
				Gcalc::calcTexUv(&uv, UV_CIRC_RUN, W_TEX, H_TEX, rate2dr)	);
			// 棒部分
			::glTranslatef((f32)_ptTouchLast->x(), (f32)_ptTouchLast->y(), 0.0f);
			::glRotatef(TFW_RAD_TO_DEG(_angle), 0.0f, 0.0f, 1.0f);
			RendererUtils::draw2dTextureRect(
				rdr,
				RectF32::setout(&vtx, - _length, (f32)(- H_BAR / 2), _length, H_BAR),
				Gcalc::calcTexUv(&uv, UV_BAR_RUN, W_TEX, H_TEX, rate2dr)	);
			// 矢印部分
			RendererUtils::draw2dTextureRect(
				rdr,
				RectF32::setout(&vtx, (f32)(W_ARW / 2), (f32)(- H_ARW / 2), - W_ARW, H_ARW),
				Gcalc::calcTexUv(&uv, UV_ARW_RUN, W_TEX, H_TEX, rate2dr)	);
			break;
		}

		// 行列を戻す
		::glPopMatrix();
	}
#endif
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void MoveCursor::notifyChangedMap()
{
	// 値をリセット
	reset();
}

/*---------------------------------------------------------------------*//**
	リーダー変更通知
**//*---------------------------------------------------------------------*/
void MoveCursor::notifyChangeLeader()
{
	_ext->notifyChangeLeader();
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

