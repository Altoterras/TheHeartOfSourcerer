/***********************************************************************//**
 *  HrAnimSetCtx.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/12/29.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HrAnimSetCtx.h"

// Friends
#include "HrAnimSet.h"
#include "HrAnim.h"
#include "HrAnimCtx.h"
#include "HrAnimObj.h"
#include "HrAnimObjCtx.h"
#include "HrAnimObjCtxArray.h"
#include "HrAnimObjFrame.h"
#include "HrAnimPlayingFrame.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// HrAnimSetCtx メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HrAnimSetCtx::HrAnimSetCtx()
	: _animidxCur(-1)
	, _frameAnimIntp(0.0f)
	, _frameAnimIntpMax(0.0f)
	, _replcIntpHeadKf(false)
	, _frameIntpRsvFirst(0)
	, _frameIntpRsvEnd(0)
	/*
	, _aoparrAnimIntp(0L)
	, _idxarrAofCur(0L)
	, _idxarrAofNext(0L)
	*/
	, _apfState(0L)
	, _animidRsvFirst(HrAnimSet::ANIMID_INVALID)
	, _animidRsvEnd(HrAnimSet::ANIMID_INVALID)
	, _flags(0)
	, _acarrAnimCtx(0L)
	, _aocaObjCtxArray(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HrAnimSetCtx::~HrAnimSetCtx()
{
	/*
	ASSERT(_aoparrAnimIntp == 0L);
	ASSERT(_idxarrAofCur == 0L);
	ASSERT(_idxarrAofNext == 0L);
	*/
	ASSERT(_apfState == 0L);
	ASSERT(_acarrAnimCtx == 0L);
	ASSERT(_aocaObjCtxArray == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool HrAnimSetCtx::create(HrAnim** animarr, u16 numAnim, HrAnimObj** aobjRoot, u16 numRootAobj, u16 numAobjTotal, u16 numShape, Shape** sarr, ShapeCtx** scarr)
{
	bool isSucceeded = true;

	/*
	// アニメーション間補間用フレーム情報テーブルを作成する
	_aoparrAnimIntp = new HrAnimObjPosture*[numShape];
	for(int i = 0; i < numShape; i++)
	{
		_aoparrAnimIntp[i] = new HrAnimObjPosture();
	}
	*/

	// アニメーション再生枠状態を作成する
	_apfState = new HrAnimPlayingFrame(numShape);

	// アニメーションコンテキストを作成する
	_acarrAnimCtx = new HrAnimCtx*[numAobjTotal];
	for(int i = 0; i < numAnim; i++)
	{
		_acarrAnimCtx[i] = new HrAnimCtx();
		if(!animarr[i]->createContext(_acarrAnimCtx[i]))	{	isSucceeded = false;	}
	}

	// アニメオブジェコンテキストを作成する
	_aocaObjCtxArray = new HrAnimObjCtxArray();
	if(!_aocaObjCtxArray->create(aobjRoot, numRootAobj, numAobjTotal, numShape, sarr, scarr))	{	isSucceeded = false;	}

	/*
	// 補間フレーム情報インデックス配列を作成する
	_idxarrAofCur = new u16[numShape];
	_idxarrAofNext = new u16[numShape];
	*/

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void HrAnimSetCtx::destroy(HrAnim** animarr, u16 numAnim, HrAnimObj** aobjRoot, u16 numRootAobj, u16 numShape)
{
	/*
	// 補間フレーム情報インデックス配列を削除する
	delete[] _idxarrAofCur;
	_idxarrAofCur = 0L;
	delete[] _idxarrAofNext;
	_idxarrAofNext = 0L;
	*/

	// アニメオブジェコンテキストを削除する
	if(_aocaObjCtxArray != 0L)
	{
		_aocaObjCtxArray->destroy(aobjRoot, numRootAobj);
		delete _aocaObjCtxArray;
		_aocaObjCtxArray = 0L;
	}

	// アニメーションコンテキストを削除する
	for(int i = 0; i < numAnim; i++)
	{
		animarr[i]->destroyContext(_acarrAnimCtx[i]);
		delete _acarrAnimCtx[i];
	}
	delete[] _acarrAnimCtx;
	_acarrAnimCtx = 0L;

	// アニメーション再生枠状態を削除
	delete _apfState;
	_apfState = 0L;

	/*
	// アニメーション間補間用フレーム情報テーブルを削除する
	for(int i = 0; i < numShape; i++)
	{
		delete _aoparrAnimIntp[i];
	}
	delete[] _aoparrAnimIntp;
	_aoparrAnimIntp = 0L;
	*/
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS