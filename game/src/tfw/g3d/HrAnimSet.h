/***********************************************************************//**
 *  HrAnimSet.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/29.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_HR_ANIM_SET_H_
#define _TFW_G3D_HR_ANIM_SET_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../framemod/FrameExec.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class HrAnimSetCtx;
class HrAnim;
class HrAnimObj;
class HrAnimObjFrame;
class Matrix4F;
class Shape;
class ShapeCtx;
class RenderCtx;

/*---------------------------------------------------------------------*//**
 *	階層アニメーションセット
 *
**//*---------------------------------------------------------------------*/
class HrAnimSet
{
	//======================================================================
	// 定数
public:
	static const u16		ANIMID_INVALID		= U16_MAX;		// 無効なアニメーション ID

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// アニメーションの数を得る
	inline u16 getAnimNum() const					{	return _numAnim;			}
	// 現在のアニメーション ID を得る
	u16 getCurAnimId(HrAnimSetCtx* asctx) const;
	// 最大のアニメーション ID を得る
	u16 getMaxAnimId(HrAnimSetCtx* asctx) const;
	// 現在のアニメーションフレームを得る
	f32 getCurAnimFrame(HrAnimSetCtx* asctx) const;

	// ルートオブジェクトを得る
	inline HrAnimObj* getObjRoot(s32 idxRoot) const	{	return _aobjRoot[idxRoot];	}
	// ルートオブジェクトの数を得る
	inline s32 getObjRootNum() const				{	return _numRootAobj;		}

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool			setAnimObjPosture(HrAnimSetCtx* asctx, u16 indexObj, const Vector3F* trs, const Vector3F* rot, const Vector3F* scl);
	void			setAnimId(HrAnimSetCtx* asctx, u16 animid, s32 frameIntp, bool replaceHeadKeyFrameByCurPos, bool isLoop, bool isChangeFirstFrame, bool isChangeEndFrame); 
	void			calcObjRootMatrix(s32 idxRoot, Matrix4F* mtx, HrAnimSetCtx* asctx) const;
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HrAnimSet();
	~HrAnimSet();
	bool			create(HrAnim** animarrDlg, u16 numAnim, HrAnimObj** aobjRootEntr, u16 numRootAobj, u16 numAobjTotal, u16 numShape);
	void			destroy();
	bool			createContext(HrAnimSetCtx* asctx, Shape** sarr, ShapeCtx** scarr);
	void			destroyContext(HrAnimSetCtx* asctx);
	void			exec(const ExecCtx* ec, HrAnimSetCtx* asctx);
	void			render(const RenderCtx* rc, HrAnimSetCtx* asctx);

private:
	s32				findAnimIndex(u16 animid);
	void			setAnimIdImmediately(HrAnimSetCtx* asctx, u16 animid, s32 frameIntp, bool replaceHeadKeyFrameByCurPos);
	void			setShapesAnimPlayingFrame(HrAnimSetCtx* asctx, const u16* idxarrAof, s32 idxAofOffset, s32 kpStart);
	s32				normalizeIndex(s32 index, s32 num, bool isLoop) const;

	//======================================================================
	// 変数
private:
	HrAnim** _aarrAnim;
	u16 _numAnim;

	u16 _numShape;

	HrAnimObj** _aobjRoot;
	u16 _numRootAobj;
	u16 _numAobjTotal;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_HR_ANIM_SET_H_