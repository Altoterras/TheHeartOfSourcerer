/***********************************************************************//**
 *  CamAnim.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/25.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CamAnim.h"

// Friends
#include "CamAnimFrame.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CamAnim メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	フレーム情報を取得する
**//*---------------------------------------------------------------------*/
const CamAnimFrame*	CamAnim::getAnimFrame(u16 icaf) const
{
	return &_arrcaf[icaf];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	先頭のフレームにセットし、フレーム情報インデックスを得る
**//*---------------------------------------------------------------------*/
void CamAnim::resetHeadFrame(u16* idxCaf)
{
	_frame = 1.0f;
	*idxCaf = 0;
}

/*---------------------------------------------------------------------*//**
	フレームを進め、フレーム情報インデックスを得る

	@retval		true	ループした（フレーム１に戻った）
	@retval		false	ループしていない
**//*---------------------------------------------------------------------*/
u32 CamAnim::advNextFrame(u16* idxCafCur, u16* idxCafNext, f32 frameAdv)
{
	_frame += frameAdv;

	u16 idxCaf1 = CAFIDX_INVALID;
	u16 idxCaf2 = CAFIDX_INVALID;

	bool isLoop = false;
	bool isEnd = false;
	for(u16 icaf = 0; icaf < _numKeyFrame; icaf++)
	{
		if(_frame == _arrcaf[icaf]._frame)
		{
			idxCaf1 = icaf;
			break;
		}
		else if(_frame < _arrcaf[icaf]._frame)
		{
			idxCaf1 = (icaf > 0) ? (u16)(icaf - 1) : CAFIDX_INVALID;
			idxCaf2 = icaf;
			break;
		}
	}
	if((idxCaf1 == CAFIDX_INVALID) && (idxCaf2 == CAFIDX_INVALID))
	{
		if((_numKeyFrame > 0) && (_frame >= _arrcaf[_numKeyFrame - 1]._frame))	// 最終フレーム（を超えている）
		{
			idxCaf1 = _numKeyFrame - 1;
		}
		if(_isEnableLoop)	// ループが有効な場合
		{
			_frame = 1.0f;
			isLoop = true;
		}
		isEnd = true;
	}

	*idxCafCur = idxCaf1;
	*idxCafNext = idxCaf2;

	return isLoop ? ADVRES_LOOP : (isEnd ? ADVRES_END : ADVRES_NULL);
}

/*---------------------------------------------------------------------*//**
	最大フレーム数を得る
**//*---------------------------------------------------------------------*/
s32 CamAnim::getMaxFrame() const
{
	if(_numKeyFrame < 1)	{	return 0;	}
	return _arrcaf[_numKeyFrame - 1]._frame;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
CamAnim::CamAnim()
	: _arrcaf(0L)
	, _numKeyFrame(0)
	, _fovy(0.0f)
	, _cnear(0.0f)
	, _cfar(0.0f)
	, _isEnableLoop(false)
	, _frame(1.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
CamAnim::~CamAnim()
{
	ASSERT(_arrcaf == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool CamAnim::create(CamAnimFrame* arrcafEntr, u16 numKeyFrame, f32 fovy, f32 cnear, f32 cfar)
{
	_fovy = fovy;
	_cnear = cnear;
	_cfar = cfar;

	// フレーム配列は委譲されたインスタンス
	_arrcaf = arrcafEntr;
	_numKeyFrame = numKeyFrame;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void CamAnim::destroy()
{
	// フレーム配列を削除（HrAnimObjFrame インスタンスを委譲されている）
	delete[] _arrcaf;
	_arrcaf = 0L;
	_numKeyFrame = 0L;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS