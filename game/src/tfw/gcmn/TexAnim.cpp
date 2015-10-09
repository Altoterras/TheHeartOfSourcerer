/***********************************************************************//**
 *  TexAnim.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/27.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TexAnim.h"

// Friends
#include "TexAnimCtx.h"
#include "../file/File.h"
#include "../file/IdFile.h"
#include "../file/MemoryFile.h"
#include "../lib/Vector.h"
#include "../string/CStringBase.h"
#include "../string/StringUtils.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TexAnim メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	現在の平行移動値を得る
**//*---------------------------------------------------------------------*/
void TexAnim::getCurrentTranslate(TexAnimCtx* ctx, Vector2F* trs) const
{
	trs->_v[0] = _kfarrKeyFrame[ctx->_idxKeyFrame]._trs[0];
	trs->_v[1] = _kfarrKeyFrame[ctx->_idxKeyFrame]._trs[1];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	フレームを進める
**//*---------------------------------------------------------------------*/
void TexAnim::advNextFrame(TexAnimCtx* ctx, f32 frameAdv)
{
#if 1
	// フレーム数更新
	ctx->_frameCur += frameAdv;
	if(ctx->_frameCur > _numFrame)
	{
		ctx->_frameCur = 1.0f + ctx->_frameCur - _numFrame;
		ctx->_idxKeyFrame = 0;
	}

	// 一つ先のフレームを見てそのフレームに到達していたらインデックスを更新する
	for(u32 ikf = ctx->_idxKeyFrame + 1; ikf < _numKeyFrame; ikf++)
	{
		if(ctx->_frameCur < _kfarrKeyFrame[ikf]._frame)
		{
			break;
		}
		ctx->_idxKeyFrame = ikf;
	}

#else
	// フレーム数更新
	ctx->_frameCur += frameAdv;
	if(ctx->_frameCur > _numFrame)
	{
		ctx->_frameCur = 1.0f;
	}

	// 一つ先のフレームを見てそのフレームに到達していたらインデックスを更新する
	for(s32 ikf = ctx->_idxKeyFrame + 1; 
	for((ctx->_idxKeyFrame + 1) < _numKeyFrame)
	{
		if(ctx->_frameCur >= _kfarrKeyFrame[ctx->_idxKeyFrame + 1]._frame)
		{
			ctx->_idxKeyFrame = ctx->_idxKeyFrame + 1;
		}
	}

	// 現在のフレームに達していなかったら最初から探す
	if(ctx->_frameCur < _kfarrKeyFrame[ctx->_idxKeyFrame]._frame)
	{
		for(u32 ikf = 0; ikf < _numKeyFrame; ikf++)
		{
			ctx->_idxKeyFrame = ikf;
			if(ctx->_frameCur <= _kfarrKeyFrame[ikf]._frame)
			{
				break;
			}
		}
	}
#endif
}

/*---------------------------------------------------------------------*//**
	フレームをリセットする
**//*---------------------------------------------------------------------*/
void TexAnim::resetTexAnimFrame(TexAnimCtx* ctx)
{
	ctx->_frameCur = 1.0f;
	ctx->_idxKeyFrame = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TexAnim::TexAnim()
	: _numFrame(0)
	, _numKeyFrame(0)
	, _kfarrKeyFrame(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TexAnim::~TexAnim()
{
	ASSERT(_kfarrKeyFrame == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool TexAnim::create(u32 numFrame, u32 numKeyFrame)
{
	_numFrame = numFrame;
	_numKeyFrame = numKeyFrame;
	_kfarrKeyFrame = new KeyFrame[_numKeyFrame];

	return true;
}

/*---------------------------------------------------------------------*//**
	キーフレーム作成
**//*---------------------------------------------------------------------*/
bool TexAnim::createKeyFrame(s32 idxKeyFrame, u32 frame, f32 s, f32 t)
{
	if((idxKeyFrame < 0) || (_numKeyFrame <= (u32)idxKeyFrame))	{	return false;	}
	_kfarrKeyFrame[idxKeyFrame]._frame = frame;
	_kfarrKeyFrame[idxKeyFrame]._trs[0] = s;
	_kfarrKeyFrame[idxKeyFrame]._trs[1] = t;
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void TexAnim::destroy()
{
	delete[] _kfarrKeyFrame;
	_kfarrKeyFrame = 0L;
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを作成する
**//*---------------------------------------------------------------------*/
bool TexAnim::createContext(TexAnimCtx* ctx)
{
	return ctx->create();
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを破棄する
**//*---------------------------------------------------------------------*/
void TexAnim::destroyContext(TexAnimCtx* ctx)
{
	ctx->destroy();
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS