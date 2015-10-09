/***********************************************************************//**
 *  HrAnimSetCtx.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/12/29.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_HR_ANIM_SET_CTX_H_
#define _TFW_G3D_HR_ANIM_SET_CTX_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class HrAnim;
class HrAnimCtx;
class HrAnimObj;
class HrAnimObjCtxArray;
class HrAnimObjFrame;
class HrAnimObjPosture;
class HrAnimPlayingFrame;
class Shape;
class ShapeCtx;

/*---------------------------------------------------------------------*//**
 *	階層アニメーションセット 可変値コンテキスト
 *
**//*---------------------------------------------------------------------*/
class HrAnimSetCtx
{
	//======================================================================
	// 定数
public:
	// 動作
	static const u16	F_LOOP	= 0x0001;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HrAnimSetCtx();
	~HrAnimSetCtx();
	bool create(HrAnim** animarr, u16 numAnim, HrAnimObj** aobjRoot, u16 numRootAobj, u16 numAobjTotal, u16 numShape, Shape** sarr, ShapeCtx** scarr);
	void destroy(HrAnim** animarr, u16 numAnim, HrAnimObj** aobjRoot, u16 numRootAobj, u16 numShape);

	//======================================================================
	// 変数
public:
	s32						_animidxCur;		// 現在のアニメーションインデックス
	f32						_frameAnimIntp;		// アニメ間補間用フレーム減算カウンタ値
	f32						_frameAnimIntpMax;	// アニメ間補間用フレーム最大値
	bool					_replcIntpHeadKf;	// アニメ間補間時の先頭キーフレーム置換
	s32						_frameIntpRsvFirst;	// １フレーム目切り替え予約時の補間フレーム値
	s32						_frameIntpRsvEnd;	// 最終フレーム切り替え予約時の補間フレーム値
	HrAnimPlayingFrame*		_apfState;			// アニメーション再生枠状態
	u16						_animidRsvFirst;	// １フレーム目切り替えとして予約されたアニメーション ID
	u16						_animidRsvEnd;		// 最終フレーム切り替えとして予約されたアニメーション ID
	u16						_flags;				// 動作フラグ

	HrAnimCtx**				_acarrAnimCtx;

	HrAnimObjCtxArray*		_aocaObjCtxArray;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_HR_ANIM_SET_CTX_H_