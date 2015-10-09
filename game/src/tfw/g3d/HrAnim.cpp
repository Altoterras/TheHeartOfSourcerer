/***********************************************************************//**
 *  HrAnim.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/29.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HrAnim.h"

// Friends
#include "HrAnimCtx.h"
#include "HrAnimObj.h"
#include "HrAnimObjFrame.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// HrAnim メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	現在のフレーム番号を取得する
**//*---------------------------------------------------------------------*/
f32 HrAnim::getFrame(HrAnimCtx* ctx) const
{
	return ctx->_frame;
}

/*---------------------------------------------------------------------*//**
	フレーム情報を取得する
**//*---------------------------------------------------------------------*/
const HrAnimObjFrame* HrAnim::getAnimObjFrame(s32 ishape, u16 iaof) const
{
	return &_arraof[ishape][iaof];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	先頭のフレーム インデックスを得る
**//*---------------------------------------------------------------------*/
void HrAnim::getHeadFrameIndex(u16* idxarrAof) const
{
	for(int ishape = 0; ishape < (int)_numShape; ishape++)
	{
		u16 numAof = _numAof[ishape];
		idxarrAof[ishape] = (numAof >= 1) ? 0 : AOFIDX_INVALID;
	}
}

/*---------------------------------------------------------------------*//**
	指定のフレーム インデックスを得る

	@retval true 全てのシェイプが最終フレームに達している
	@retval false 全てのシェイプが最終フレームに達していない
**//*---------------------------------------------------------------------*/
bool HrAnim::getFrameIndex(u16* idxarrAof, f32 frame) const
{
	u16 cntLastFrameShape = 0;
	for(int ishape = 0; ishape < (int)_numShape; ishape++)
	{
		const HrAnimObjFrame* arraof = _arraof[ishape];
		ASSERT(_numAof[ishape] < AOFIDX_VALID_MAX);
		u16 numAof = _numAof[ishape];

		// 指定フレームにおけるキーフレームを求める
		u16 idxAof = AOFIDX_INVALID;
#if 1
		for(u16 iaof = numAof - 1; iaof >= 0; iaof--)
		{
			if(frame >= arraof[iaof].getFrame())
			{
				idxAof = iaof;
				break;
			}
		}
		if(idxAof == (numAof - 1))	// 全てのフレームを再生し終わっている
		{
			cntLastFrameShape++;
		}
		else if(idxAof == AOFIDX_INVALID)
		{
			idxAof = 0;
		}
#else
		for(u16 iaof = 0; iaof < numAof; iaof++)
		{
			if(frame == arraof[iaof].getFrame())
			{
				idxAof = iaof;
				break;
			}
			else if(frame < arraof[iaof].getFrame())
			{
				idxAof = (iaof > 0) ? iaof - 1 : AOFIDX_INVALID;
				break;
			}
		}
		if(idxAof == AOFIDX_INVALID)	// 全てのフレームを再生し終わっている
		{
			cntLastFrameShape++;
			if((numAof > 0) && (frame >= arraof[numAof - 1].getFrame()))
			{
				idxAof = numAof - 1;		// 最終フレームに設定
			}
		}
#endif
		ASSERT(idxAof != AOFIDX_INVALID);
		idxarrAof[ishape] = idxAof;
	}

	// 全てのシェイプのアニメーションフレームを再生し終わっているかを判定する
	if(cntLastFrameShape >= _numShape)
	{
		return true;
	}

	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HrAnim::HrAnim()
	: _arraof(0L)
	, _animid(0)
	, _numShape(0)
	, _numAof(0L)
	, _numAofMax(0)
	, _frameMax(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HrAnim::~HrAnim()
{
	ASSERT(_arraof == 0L);
	ASSERT(_numAof == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool HrAnim::create(u16 animid, HrAnimObjFrame** arraofEntr, u16* numAofDlg, u16 numShape)
{
	// アニメーション ID
	_animid = animid;

	// フレーム配列は委譲されたインスタンス
	_arraof = arraofEntr;
	_numAof = numAofDlg;

	// シェイプ数を保存
	_numShape = numShape;

	// 最大キーフレーム数、最大フレーム値を求める
	_numAofMax = 0;
	_frameMax = 0;
	for(u16 ishape = 0; ishape < _numShape; ishape++)
	{
		if(_numAofMax < _numAof[ishape])
		{
			_numAofMax = _numAof[ishape];
		}

		const HrAnimObjFrame* arraof = &_arraof[ishape][_numAof[ishape] - 1];
		if(_frameMax < arraof->getFrame())
		{
			_frameMax = arraof->getFrame();
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void HrAnim::destroy()
{
	// フレーム配列を削除（HrAnimObjFrame インスタンスを委譲されている）
	for(int ishape = 0; ishape < (int)_numShape; ishape++)
	{
		delete[] _arraof[ishape];
	}
	delete[] _arraof;
	delete[] _numAof;
	_arraof = 0L;
	_numAof = 0L;

	_numShape = 0;
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを作成する
**//*---------------------------------------------------------------------*/
bool HrAnim::createContext(HrAnimCtx* ctx)
{
	return ctx->create();
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを破棄する
**//*---------------------------------------------------------------------*/
void HrAnim::destroyContext(HrAnimCtx* ctx)
{
	ctx->destroy();
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS