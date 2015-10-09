/***********************************************************************//**
 *	EleneVeil.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_ELENE_BALE_H_
#define _SRCR_STATUS_ELENE_BALE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "Elene.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	元素エネルギー結界
 *
**//*---------------------------------------------------------------------*/
class EleneVeil
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 現在の元素のエネルギーを得る
	inline const Elene* getElene() const { return &_eleneCur; }
	inline Elene* elene() { return &_eleneCur; }
	// 有効かどうかを得る
	inline bool isValid() const { return _eleneCur.getAethel() > 0; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void strengthen(const Elene* eeAdd);
	void weaken(const Elene* eeSub);

	void scale(s32 ratio);
	void scale(f32 ratio);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EleneVeil()											{	clear();					}
	inline EleneVeil(const EleneVeil& src)				{	copy(&src);					}
	inline explicit EleneVeil(const Elene* eleneOql) 	{	setup(eleneOql);			}
	void copy(const EleneVeil* src);
	inline EleneVeil& operator =(const EleneVeil& src)	{	copy(&src); return *this;	}
	void clear();
	void setup(const Elene* eleneOql);
	void exec(const ExecCtx* ec);

private:
	void checkValid();

	//======================================================================
	// 変数
private:
	// [  0]
	Elene _eleneCur;		// 現在の元素エネルギー
	u16 _cntDecay;			// １秒当たりの減衰量
	u16 _cntFrame;			// フレームカウンタ
	// [ 24]
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_ELENE_BALE_H_
