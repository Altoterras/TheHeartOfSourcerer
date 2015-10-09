/***********************************************************************//**
 *	EleneVeil.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/29.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EleneVeil.h"

// Friends
#include "Nature.h"

// External
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EleneVeil メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	結界強化
**//*---------------------------------------------------------------------*/
void EleneVeil::strengthen(const Elene* eeAdd)
{
	_eleneCur.add(eeAdd);

	// 継続時間から１秒当たり減衰量を算出する
	s32 duration = (_eleneCur.getSoil() + (_eleneCur.getWater() / 2)) * 4;
	s32 sum = _eleneCur.sumExceptAethel();
	f32 decayFrame = (f32)(sum / 4) / (f32)duration;
	_cntDecay = (s32)(decayFrame * 30.0f);		// 秒換算
	TRACE("{EleneVeil::strengthe} Veil Param: durf=%d, decayf=%f, dc=%d, sum=%d\n", duration, decayFrame, _cntDecay, sum);
	if(_cntDecay <= 0)	{	_cntDecay = 1;	}

	// 有効判定
	checkValid();
}

/*---------------------------------------------------------------------*//**
	結界弱体
**//*---------------------------------------------------------------------*/
void EleneVeil::weaken(const Elene* eeSub)
{
	_eleneCur.sub(eeSub);

	// 有効判定
	checkValid();
}

/*---------------------------------------------------------------------*//**
	スケーリング - 100 を等倍とした整数演算
**//*---------------------------------------------------------------------*/
void EleneVeil::scale(s32 ratio)
{
	_eleneCur.scale(ratio);
	_cntDecay = (u16)((s32)_cntDecay * 100 / ratio);
}

/*---------------------------------------------------------------------*//**
	スケーリング - 1.0f を等倍とした浮動小数点演算
**//*---------------------------------------------------------------------*/
void EleneVeil::scale(f32 ratio)
{
	_eleneCur.scale(ratio);
	_cntDecay = (u16)(_cntDecay * ratio);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void EleneVeil::copy(const EleneVeil* src)
{
	_eleneCur.copy(&src->_eleneCur);
	_cntDecay = src->_cntDecay;
	_cntFrame = src->_cntFrame;
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void EleneVeil::clear()
{
	ASSERT(sizeof(Elene) == 20);
	ASSERT(sizeof(EleneVeil) == 24);

	_eleneCur.clear();
	_cntDecay = 0;
	_cntFrame = 0;
}

/*---------------------------------------------------------------------*//**
	初期設定
**//*---------------------------------------------------------------------*/
void EleneVeil::setup(const Elene* eleneOql)
{
	clear();
	_eleneCur = *eleneOql;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void EleneVeil::exec(const ExecCtx* ec)
{
	if(!isValid())	{	return;	}

	// フレーム更新
	_cntFrame = (u16)(_cntFrame + ec->getDeltaFrame() * 100.0f);	// 整数なので 100 倍解像度でカウント
	if(_cntFrame >= 3000)	// 秒間 30 フレーム × 解像度 100
	{
		///TRACE("{EleneVeil::exec} _cntFrame=%d, _cntDecay=%d\n", _cntFrame, _cntDecay);
		_cntFrame -= 3000;

		// 減衰
		for(int i = 0; i < Elene::NUM_EL; i++)
		{
			_eleneCur.sub(i, _cntDecay);
		}

		// 有効判定
		checkValid();
	}
}

/*---------------------------------------------------------------------*//**
	有効判定
**//*---------------------------------------------------------------------*/
void EleneVeil::checkValid()
{
	s32 sum = _eleneCur.sumExceptAethel();
	if(sum <= 0)
	{
		clear();				// 無効化
	}
	else if(_eleneCur.getAethel() <= 0)
	{
		_eleneCur.setAethel(1);	// 有効継続
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
