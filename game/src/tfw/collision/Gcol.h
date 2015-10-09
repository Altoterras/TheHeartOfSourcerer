/***********************************************************************//**
 *	Gcol.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/07/14.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_COLLISION_GCOL_H_
#define _TFW_COLLISION_GCOL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// マクロ

#define INVALID_FLOAT		-1.0e+38F

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
template<class TYPE> class Vector4;
typedef Vector3<f32> Vector3F;
typedef Vector4<f32> Vector4F;
#if defined(_DEBUG)
	class RenderCtx;
#endif

/*---------------------------------------------------------------------*//**
 *	地面コリジョン
 *	
**//*---------------------------------------------------------------------*/
class Gcol
{
	//======================================================================
	// クラス

public:
	class Plane
	{
		//==================================================================
		// 定数

	public:
		static const u16	ATRBF_NULL						= 0x0000;
		static const u16	ATRBF_WALL						= 0x0001;	// 壁当たり
		static const u16	ATRBF_KEEPOUT					= 0x0002;	// 進入禁止
		static const u16	ATRBF_ENTRANCE					= 0x0004;	// 入口（マップ開始位置）
		static const u16	ATRBFMASK_COL					= 0x00ff;	// コリジョンフラグのマスク
		static const u16	ATRBF_MOVE_MAP_01				= 0x0010;	// マップ移動１
		static const u16	ATRBF_MOVE_MAP_02				= 0x0020;	// マップ移動２
		static const u16	ATRBF_MOVE_MAP_03				= 0x0040;	// マップ移動３
		static const u16	ATRBF_MOVE_MAP_04				= 0x0080;	// マップ移動４
		static const u16	ATRBFMASK_MOVEMAP				= 0x00f0;	// マップ移動フラグのマスク
		static const u16	ATRBFMASK_EVENT					= 0x0f00;	// イベント ID のマスク
		static const u16	ATRBF_SHADOW_S					= 0x1000;	// 影量 - 小
		static const u16	ATRBF_SHADOW_M					= 0x2000;	// 影量 - 中
		static const u16	ATRBF_SHADOW_L					= 0x3000;	// 影量 - 大
		static const u16	ATRBF_SHADOW_MASK				= 0x3000;	// 影量マスク

		//==================================================================
		// メンバ変数

	public:
		u16			_vtxidx1;		// 三角形の頂点インデックス１
		u16			_vtxidx2;		// 三角形の頂点インデックス２
		u16			_vtxidx3;		// 三角形の頂点インデックス３
		u16			_coeffidx;		// 係数インデックス
		u16			_plnidxNeib1;	// 隣接平面の平面インデックス１
		u16			_plnidxNeib2;	// 隣接平面の平面インデックス２
		u16			_plnidxNeib3;	// 隣接平面の平面インデックス３
		u16			_flagAtrb;		// 属性フラグ

		//==================================================================
		// メソッド

	public:
		Plane() : _vtxidx1(0), _vtxidx2(0), _vtxidx3(0), _coeffidx(0), _plnidxNeib1(0xff), _plnidxNeib2(0xff), _plnidxNeib3(0xff), _flagAtrb(0) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline s32				getPlaneNum() const							{	return _numPlanes;					}
	inline s32				getVertexNum() const						{	return _numVtx;						}
	inline s32				getPlaneCoefficientNum() const				{	return _numCoeff;					}
	inline const Plane*		getPlane(s32 planeindex) const				{	return &_parrPlanes[planeindex];	}
	const Vector3F*			getVertex(s32 vtxindex) const;
	const Vector4F*			getPlaneCoefficient(s32 coeffindex) const;
	inline const Vector3F*	getVertexMin() const						{	return _vtxMin;						}
	inline const Vector3F*	getVertexMax() const						{	return _vtxMax;						}
	inline const Vector3F*	getVertexCenter() const						{	return _vtxCenter;					}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool			isInPlane(s32 idxPlane, f32 x, f32 z) const;
	s32				getPlaneIndex(f32 x, f32 z) const;
	u16				getPlaneAtrbFlag(s32 idxPlane) const;
	f32				getY(s32 idxPlane, f32 x, f32 z) const;
	f32				getY(f32 x, f32 z) const;
	const Plane*	getPlaneFromEventId(u16 evid) const;
	static bool		isMovablePlaneAtrbFlag(u16 flagAtrb)	{	return !TFW_IS_FLAG(flagAtrb, Plane::ATRBF_WALL) && !TFW_IS_FLAG(flagAtrb, Plane::ATRBF_KEEPOUT);	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// ユーティリティ
public:
	bool			getIntersectionZx(Vector3F* vr, Vector3F* vi1, Vector3F* vi2, s32 idxPlane, const Vector3F* v1, const Vector3F* v2, f32 margin) const;
	s32				getNearestVertexIndexZx(s32 idxPlane, const Vector3F* vs) const;
	void			getPlaneVertexDirectionVector(Vector3F* v1, Vector3F* v2, s32 idxPlane, s32 vtxidxBase) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Gcol();
	~Gcol();
	bool			create(Plane* parrPlanesEntr, s32 numPlanes, Vector3F* varrVtxEntr, s32 numVtx, Vector4F* varrCoeffEntr, s32 numCoeff);
	void			destroy();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// デバッグ
#if defined(_DEBUG)
public:
	void			debug_draw(const RenderCtx* rc) const;
#endif

	//======================================================================
	// メンバ変数

private:
	Plane*			_parrPlanes;
	Vector3F*		_varrVtx;
	Vector4F*		_varrCoeff;
	s32				_numPlanes;
	s32				_numVtx;
	s32				_numCoeff;

	Vector3F*		_vtxMin;
	Vector3F*		_vtxMax;
	Vector3F*		_vtxCenter;

#if defined(_DEBUG)
public:
	s32				_debug_idxMarkPlane;
#endif
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_COLLISION_GCOL_H_