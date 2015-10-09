/***********************************************************************//**
 *  HrAnim.h
 *  Terras Framework
 *		※ Hr = Hierarchy
 *
 *  Created by Ryoutarou Kishi on 2009/06/29.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_HR_ANIM_H_
#define _TFW_G3D_HR_ANIM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class HrAnimCtx;
class HrAnimObj;
class HrAnimObjFrame;

/*---------------------------------------------------------------------*//**
 *	階層アニメ
 *		一つのアニメーションに対応
 *		複数の HrAnim を束ねたものが HrAnimSet
 *
**//*---------------------------------------------------------------------*/
class HrAnim
{
	//======================================================================
	// 定数
public:
	static const u16		AOFIDX_INVALID		= U16_MAX;		// 無効な HrAnimObjFrame インデックス
	static const u16		AOFIDX_VALID_MAX	= U16_MAX - 1;	// 有効な HrAnimObjFrame インデックスの最大値

	// advNextFrame の返り値
	static const u32		ADVRES_NULL			= 0;
	static const u32		ADVRES_LOOP			= 1;
	static const u32		ADVRES_END			= 2;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// アニメーション ID を得る
	inline u16 getAnimId() const { return _animid; }
	// 現在のフレーム番号を取得する
	f32 getFrame(HrAnimCtx* ctx) const;
	// キーフレームを取得する
	const HrAnimObjFrame* getAnimObjFrame(s32 ishape, u16 iaof) const;
	// キーフレーム数を得る
	inline const u16 getAnimObjFrameNum(s32 ishape) const { return _numAof[ishape]; }
	// 最大キーフレーム数を得る
	inline const u16 getAnimObjFrameMaxNum() const { return _numAofMax; }
	// 最大フレーム値を得る
	inline s32 getMaxFrameNumber() const { return _frameMax; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void getHeadFrameIndex(u16* idxarrAof) const;
	bool getFrameIndex(u16* idxarrAof, f32 frame) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HrAnim();
	~HrAnim();
	bool create(u16 animid, HrAnimObjFrame** arraofEntr, u16* numAofDlg, u16 numShape);
	void destroy();
	bool createContext(HrAnimCtx* ctx);
	void destroyContext(HrAnimCtx* ctx);

	//======================================================================
	// メンバ変数
private:
	HrAnimObjFrame** _arraof;
	u16 _animid;
	u16 _numShape;
	u16* _numAof;
	u16 _numAofMax;
	s32 _frameMax;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_HR_ANIM_H_