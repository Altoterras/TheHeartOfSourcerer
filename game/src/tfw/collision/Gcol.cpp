/***********************************************************************//**
 *  Gcol.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/07/14.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Gcol.h"

// Friends
#include "../lib/Vector.h"
#if defined(_DEBUG)
	#include "../framemod/RenderCtx.h"
	#include "../gcmn/Renderer.h"
	#include "../gcmn/RendererUtils.h"
#endif

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define IS_NOCROSS(vt_, v1_, v2_)	( ( (( (v2_)->_v[0] - (v1_)->_v[0] ) * ( (vt_)->_v[2] - (v1_)->_v[2] )) - (( (v2_)->_v[2] - (v1_)->_v[2] ) * ( (vt_)->_v[0] - (v1_)->_v[0] )) ) >= 0.0f )

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Gcol メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	頂点ベクトルを得る
**//*---------------------------------------------------------------------*/
const Vector3F*	Gcol::getVertex(s32 vtxindex) const
{
	return &_varrVtx[vtxindex];
}

/*---------------------------------------------------------------------*//**
	平面係数を得る
**//*---------------------------------------------------------------------*/
const Vector4F*	Gcol::getPlaneCoefficient(s32 coeffindex) const
{
	return &_varrCoeff[coeffindex];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	指定面内に座標があるかどうか判定
**//*---------------------------------------------------------------------*/
bool Gcol::isInPlane(s32 idxPlane, f32 x, f32 z) const
{
	if((idxPlane < 0) || (idxPlane >= _numPlanes) )
	{
		return false;
	}

	const Vector3F	vs(x, 0.0f, z);
	const Plane*	plane	= &_parrPlanes[idxPlane];
	const Vector3F*	v1		= &_varrVtx[ plane->_vtxidx1 ];
	const Vector3F*	v2		= &_varrVtx[ plane->_vtxidx2 ];
	const Vector3F*	v3		= &_varrVtx[ plane->_vtxidx3 ];

	// 単純な四角形（バウンティングボックス）で判定 -- if 10回分の演算
	if(v1->_v[0] < v2->_v[0])
	{
		if(v1->_v[0] < v3->_v[0])	{ if(v1->_v[0] > x) { return false; } }	// v1[0] が X 座標最小 でそれよりも小さい
		else						{ if(v3->_v[0] > x) { return false; } }	// v3[0] が X 座標最小 でそれよりも小さい
		if(v2->_v[0] > v3->_v[0])	{ if(v2->_v[0] < x) { return false; } }	// v2[0] が X 座標最大 でそれよりも大きい
		else						{ if(v3->_v[0] < x) { return false; } }	// v3[0] が X 座標最大 でそれよりも大きい
	}
	else // v1->_v[0] >= v2->_v[0]
	{
		if(v2->_v[0] < v3->_v[0])	{ if(v2->_v[0] > x) { return false; } }	// v2[0] が X 座標最小 でそれよりも小さい
		else						{ if(v3->_v[0] > x) { return false; } }	// v3[0] が X 座標最小 でそれよりも小さい
		if(v1->_v[0] > v3->_v[0])	{ if(v1->_v[0] < x) { return false; } }	// v1[0] が X 座標最大 でそれよりも大きい
		else						{ if(v3->_v[0] < x) { return false; } }	// v3[0] が X 座標最大 でそれよりも大きい
	}
	if(v1->_v[2] < v2->_v[2])
	{
		if(v1->_v[2] < v3->_v[2])	{ if(v1->_v[2] > z) { return false; } }	// v1[2] が Z 座標最小 でそれよりも小さい
		else						{ if(v3->_v[2] > z) { return false; } }	// v3[2] が Z 座標最小 でそれよりも小さい
		if(v2->_v[2] > v3->_v[2])	{ if(v2->_v[2] < z) { return false; } }	// v2[2] が Z 座標最大 でそれよりも大きい
		else						{ if(v3->_v[2] < z) { return false; } }	// v3[2] が Z 座標最大 でそれよりも大きい
	}
	else // v1->_v[2] >= v2->_v[2]
	{
		if(v2->_v[2] < v3->_v[2])	{ if(v2->_v[2] > z) { return false; } }	// v2[2] が Z 座標最小 でそれよりも小さい
		else						{ if(v3->_v[2] > z) { return false; } }	// v3[2] が Z 座標最小 でそれよりも小さい
		if(v1->_v[2] > v3->_v[2])	{ if(v1->_v[2] < z) { return false; } }	// v1[2] が Z 座標最大 でそれよりも大きい
		else						{ if(v3->_v[2] < z) { return false; } }	// v3[2] が Z 座標最大 でそれよりも大きい
	}

	// 三角形の辺と交差するか判定
	if(IS_NOCROSS(&vs, v1, v2))	{	return false;	}
	if(IS_NOCROSS(&vs, v2, v3))	{	return false;	}
	if(IS_NOCROSS(&vs, v3, v1))	{	return false;	}

	return true;
}

/*---------------------------------------------------------------------*//**
	指定座標にある面を得る
**//*---------------------------------------------------------------------*/
s32 Gcol::getPlaneIndex(f32 x, f32 z) const
{
	for(int i = 0; i < _numPlanes; i++)
	{
		if(isInPlane(i, x, z))	{	return i;	}
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	指定平面の属性フラグを得る
**//*---------------------------------------------------------------------*/
u16 Gcol::getPlaneAtrbFlag(s32 idxPlane) const
{
	ASSERT((0 <= idxPlane) && (idxPlane < _numPlanes));
	if((idxPlane < 0) || (_numPlanes <= idxPlane))	{	return 0;		}
	return _parrPlanes[idxPlane]._flagAtrb;
}

/*---------------------------------------------------------------------*//**
	指定平面の指定座標から Y 座標値を得る
**//*---------------------------------------------------------------------*/
f32 Gcol::getY(s32 idxPlane, f32 x, f32 z) const
{
	ASSERT((0 <= idxPlane) && (idxPlane < _numPlanes));
	if((idxPlane < 0) || (_numPlanes <= idxPlane))	{	return INVALID_FLOAT;	}
	const Vector4F*	vCoeff = &_varrCoeff[ _parrPlanes[idxPlane]._coeffidx ];
	return - ( (vCoeff->_v[0] * x) + (vCoeff->_v[2] * z) + vCoeff->_v[3] ) / vCoeff->_v[1];
}

/*---------------------------------------------------------------------*//**
	指定座標から Y 座標値を得る
**//*---------------------------------------------------------------------*/
f32 Gcol::getY(f32 x, f32 z) const
{
	s32 idxPlane = getPlaneIndex(x, z);
	if(idxPlane == -1)	{	return INVALID_FLOAT;	}
	return getY(idxPlane, x, z);
}

/*---------------------------------------------------------------------*//**
	指定イベント ID から平面を得る
**//*---------------------------------------------------------------------*/
const Gcol::Plane* Gcol::getPlaneFromEventId(u16 evid) const
{
	for(int iplane = 0; iplane < _numPlanes; iplane++)
	{
		const Gcol::Plane* plane = &_parrPlanes[iplane];
		u16 evidPlane = (u16)((plane->_flagAtrb & Plane::ATRBFMASK_EVENT) >> 8);
		if(evid == evidPlane)
		{
			return plane;
		}
	}
	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ユーティリティ

/*---------------------------------------------------------------------*//**
	指定平面と線分との交点を求める（XZ 値のみで判定）
	
	@param vr 計算結果の交点
	@param vi1 交差する線分の頂点１を受け取るベクトル（NULL 可）
	@param vi2 交差する線分の頂点２を受け取るベクトル（NULL 可）
	@param idxPlane 対象平面インデックス
	@param v1 判定する線分の頂点１ベクトル
	@param v2 判定する線分の頂点２ベクトル
	@param margin 線分上かの判定を緩めるためのマージン値（基本は 0.0f）
**//*---------------------------------------------------------------------*/
bool Gcol::getIntersectionZx(Vector3F* vr, Vector3F* vi1, Vector3F* vi2, s32 idxPlane, const Vector3F* v1, const Vector3F* v2, f32 margin) const
{
	const Plane*	plane	= &_parrPlanes[idxPlane];
	const Vector3F*	vp[3]	= { &_varrVtx[ plane->_vtxidx1 ], &_varrVtx[ plane->_vtxidx2 ], &_varrVtx[ plane->_vtxidx3 ] };

	for(int i = 0; i < 3; i++)
	{
		const Vector3F* v3 = vp[i];
		const Vector3F* v4 = vp[(i < 2) ? i + 1 : 0];

		f32 c = (v2->x() - v1->x()) * (v4->z() - v3->z()) - (v2->z() - v1->z()) * (v4->x() - v3->x());
		if(c == 0)	{	continue;	}	// 平行である

		f32 x31 = v3->x() - v1->x();
		f32 z31 = v3->z() - v1->z();
		f32 r = ( (v4->z() - v3->z()) * x31 - (v4->x() - v3->x()) * z31 ) / c;
		f32 s = ( (v2->z() - v1->z()) * x31 - (v2->x() - v1->x()) * z31 ) / c;
		if((r < (0.0f - margin)) || ((1.0f + margin) < r) || ((s - margin) < 0.0f) || ((1.0f + margin) < s))	{	continue;	}	// 線分内で交差しない

		vr->x() = v1->x() + (v2->x() - v1->x()) * r;
		vr->z() = v1->z() + (v2->z() - v1->z()) * r;
		if(vi1 != 0L)	{	vi1->copy(v3);	}
		if(vi2 != 0L)	{	vi2->copy(v4);	}
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	指定平面内の最も近い頂点を得る（XZ 値のみで判定）
	
	@param vs 判定する線分の頂点１ベクトル
	@return 計算結果の頂点インデックス
**//*---------------------------------------------------------------------*/
s32 Gcol::getNearestVertexIndexZx(s32 idxPlane, const Vector3F* vs) const
{
	const Plane*	plane	= &_parrPlanes[idxPlane];
	const Vector3F*	v1		= &_varrVtx[ plane->_vtxidx1 ];
	const Vector3F*	v2		= &_varrVtx[ plane->_vtxidx2 ];
	const Vector3F*	v3		= &_varrVtx[ plane->_vtxidx3 ];
	f32 wx = v1->x() - vs->x();
	f32 wz = v1->z() - vs->z();
	f32 ll1 = wx * wx + wz * wz;
	wx = v2->x() - vs->x();
	wz = v2->z() - vs->z();
	f32 ll2 = wx * wx + wz * wz;
	wx = v3->x() - vs->x();
	wz = v3->z() - vs->z();
	f32 ll3 = wx * wx + wz * wz;
	if(ll1 < ll2)
	{
		if(ll1 < ll3)
		{
			return (s32)plane->_vtxidx1;
		}
		else
		{
			return (s32)plane->_vtxidx3;
		}
	}
	else
	{
		if(ll2 < ll3)
		{
			return (s32)plane->_vtxidx2;
		}
		else
		{
			return (s32)plane->_vtxidx3;
		}
	}
}

/*---------------------------------------------------------------------*//**
	指定平面内の頂点方向ベクトルを得る
	
	@param v1 結果を受け取る方向ベクトル１
	@param v1 結果を受け取る方向ベクトル２
	@param idxPlane 平面インデックス
	@param vtxidxBase 基点となる頂点インデックス
	@return 見つけた平面インデックス（無い場合は -1）
**//*---------------------------------------------------------------------*/
void Gcol::getPlaneVertexDirectionVector(Vector3F* v1, Vector3F* v2, s32 idxPlane, s32 vtxidxBase) const
{
	const Plane*	plane	= &_parrPlanes[idxPlane];
	if(plane->_vtxidx1 == vtxidxBase)
	{
		*v1 = _varrVtx[ plane->_vtxidx2 ] - _varrVtx[ plane->_vtxidx1 ];
		*v2 = _varrVtx[ plane->_vtxidx3 ] - _varrVtx[ plane->_vtxidx1 ];
	}
	else if(plane->_vtxidx2 == vtxidxBase)
	{
		*v1 = _varrVtx[ plane->_vtxidx3 ] - _varrVtx[ plane->_vtxidx2 ];
		*v2 = _varrVtx[ plane->_vtxidx1 ] - _varrVtx[ plane->_vtxidx2 ];
	}
	else
	{
		*v1 = _varrVtx[ plane->_vtxidx1 ] - _varrVtx[ plane->_vtxidx3 ];
		*v2 = _varrVtx[ plane->_vtxidx2 ] - _varrVtx[ plane->_vtxidx3 ];
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Gcol::Gcol() :
#if defined(_DEBUG)
	_debug_idxMarkPlane(-1),
#endif
	_parrPlanes(0L), 
	_varrVtx(0L),
	_varrCoeff(0L),
	_numPlanes(0),
	_numVtx(0),
	_numCoeff(0),
	_vtxMin(0L),
	_vtxMax(0L),
	_vtxCenter(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Gcol::~Gcol()
{
	ASSERT(_parrPlanes == 0L);
	ASSERT(_varrVtx == 0L);
	ASSERT(_varrCoeff == 0L);

	ASSERT(_vtxMin == 0L);
	ASSERT(_vtxMax == 0L);
	ASSERT(_vtxCenter == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Gcol::create(Plane* parrPlanesEntr, s32 numPlanes, Vector3F* varrVtxEntr, s32 numVtx, Vector4F* varrCoeffEntr, s32 numCoeff)
{
	_parrPlanes = parrPlanesEntr;
	_numPlanes = numPlanes;
	_varrVtx = varrVtxEntr;
	_numVtx = numVtx;
	_varrCoeff = varrCoeffEntr;
	_numCoeff = numCoeff;

	// 座標値の最小・最大値を求める（バウンディング・ボックス）
	{
		ASSERT((_vtxMin == 0L) && (_vtxMax == 0L) && (_vtxCenter == 0L));
		_vtxMin = new Vector3F(F32_PMAX, F32_PMAX, F32_PMAX);
		_vtxMax = new Vector3F(F32_PMIN, F32_PMIN, F32_PMIN);
		for(int i = 0; i < _numVtx; i++)
		{
			const Vector3F* vtx = &_varrVtx[i];
			if(_vtxMin->x() > vtx->x())	{	_vtxMin->x() = vtx->x();	}
			if(_vtxMax->x() < vtx->x())	{	_vtxMax->x() = vtx->x();	}
			if(_vtxMin->y() > vtx->y())	{	_vtxMin->y() = vtx->y();	}
			if(_vtxMax->y() < vtx->y())	{	_vtxMax->y() = vtx->y();	}
			if(_vtxMin->z() > vtx->z())	{	_vtxMin->z() = vtx->z();	}
			if(_vtxMax->z() < vtx->z())	{	_vtxMax->z() = vtx->z();	}
		}
		_vtxCenter = new Vector3F((_vtxMin->x() + _vtxMax->x()) * 0.5f, (_vtxMin->y() + _vtxMax->y()) * 0.5f, (_vtxMin->z() + _vtxMax->z()) * 0.5f);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Gcol::destroy()
{
	delete[] _parrPlanes;
	_parrPlanes = 0L;
	_numPlanes = 0;
	delete[] _varrVtx;
	_varrVtx = 0L;
	_numVtx = 0;
	delete[] _varrCoeff;
	_varrCoeff = 0L;
	_numCoeff = 0;

	delete _vtxMin;
	_vtxMin = 0L;
	delete _vtxMax;
	_vtxMax = 0L;
	delete _vtxCenter;
	_vtxCenter = 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// デバッグ

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	デバッグ描画
**//*---------------------------------------------------------------------*/
void Gcol::debug_draw(const RenderCtx* rc) const
{
	Renderer* rdr = rc->getRenderer();

	rdr->setAlphaBlend(false);					// アルファブレンドなし
	rdr->bindTexture(0L);						// テクスチャなし
	rdr->setLighting(true);						// ライティングあり	
	rdr->setZTest(false);						// Z テストなし

	static const Vector4F mtrlSpecular(1.0f, 1.0f, 1.0f, 1.0f);
	static const GLfloat mtrlShininess[] = { 50.0f };
	static const Vector4F posLight0(1.0f, 1.0f, 1.0f, 0.0f);
	static const Vector4F colLightDiffuse0(0.7f, 0.7f, 0.7f, 1.0f);
	static const Vector4F colLightSpecular0(0.5f, 0.5f, 0.5f, 1.0f);
	static const Vector4F colLightAmbient0(1.0f, 1.0f, 1.0f, 1.0f);
	::glEnable(GL_LIGHT0);
	::glMaterialfv(GL_FRONT, GL_SPECULAR, mtrlSpecular.ptr());
	::glMaterialfv(GL_FRONT, GL_SHININESS, mtrlShininess);
	::glLightfv(GL_LIGHT0, GL_POSITION, posLight0.ptr());
	::glLightfv(GL_LIGHT0, GL_DIFFUSE, colLightDiffuse0.ptr());
	::glLightfv(GL_LIGHT0, GL_SPECULAR, colLightSpecular0.ptr());

	for(int iplane = 0; iplane < _numPlanes; iplane++)
	{
		const Gcol::Plane* plane = &_parrPlanes[iplane];
		const Vector4F*	coeff = &_varrCoeff[ _parrPlanes[iplane]._coeffidx ];

		// ポリゴン描画
		Vector3F vtxarr[3];
		vtxarr[0] = _varrVtx[plane->_vtxidx1];
		vtxarr[1] = _varrVtx[plane->_vtxidx2];
		vtxarr[2] = _varrVtx[plane->_vtxidx3];
		vtxarr[0].y() += 1.0f;	// Z テストで合格するようにちょっと浮かせる
		vtxarr[1].y() += 1.0f;
		vtxarr[2].y() += 1.0f;
		Vector3F nrmarr[3];
		nrmarr[0].set(coeff->x(), coeff->y(), coeff->z());
		nrmarr[1].set(coeff->x(), coeff->y(), coeff->z());
		nrmarr[2].set(coeff->x(), coeff->y(), coeff->z());
		Vector4F col(0.3f, 0.3f, 0.3f, 0.5f);
		if(TFW_IS_FLAG(plane->_flagAtrb, Plane::ATRBF_WALL))		{	col.r() += 0.4f;	}
		if(TFW_IS_FLAG(plane->_flagAtrb, Plane::ATRBF_KEEPOUT))		{	col.g() += 0.4f;	}
		u8 evid = (u8)((plane->_flagAtrb & Plane::ATRBFMASK_EVENT) >> 8);
		col.b() += evid * 10;
		if(iplane == _debug_idxMarkPlane)						{	col.r() += 0.1f; col.g() += 0.3f;	col.b() += 0.9f;	}
		if(plane->_plnidxNeib1 == _debug_idxMarkPlane)			{	col.r() += 0.3f; col.g() += 0.0f;	col.b() += 0.3f;	}
		if(plane->_plnidxNeib2 == _debug_idxMarkPlane)			{	col.r() += 0.3f; col.g() += 0.0f;	col.b() += 0.3f;	}
		if(plane->_plnidxNeib3 == _debug_idxMarkPlane)			{	col.r() += 0.3f; col.g() += 0.0f;	col.b() += 0.3f;	}
		::glLightModelfv(GL_LIGHT_MODEL_AMBIENT, col.ptr());
		RendererUtils::render3dTriangles(rdr, vtxarr, nrmarr, 0L, 0L, 3, RendererUtils::TRIANGLES);

		rdr->setLighting(false);					// ライティングなし

		// 平面境界線描画
		rdr->setSolidColor(127, 127, 127, 127);
		RendererUtils::render3dLines(rdr, vtxarr, 3, RendererUtils::LINE_STRIP);

		// 法線描画
		Vector3F vtxarrNrmLine[2];
		vtxarrNrmLine[0].set( (vtxarr[0].x() + vtxarr[1].x() + vtxarr[2].x()) * 0.3333333f, (vtxarr[0].y() + vtxarr[1].y() + vtxarr[2].y()) * 0.3333333f, (vtxarr[0].z() + vtxarr[1].z() + vtxarr[2].z()) * 0.3333333f );
		vtxarrNrmLine[1].set( vtxarrNrmLine[0].x() + coeff->x() * 100.0f, vtxarrNrmLine[0].y() + coeff->y() * 100.0f, vtxarrNrmLine[0].z() + coeff->z() * 100.0f );
		rdr->setSolidColor(255, 255, 255, 127);
		RendererUtils::render3dLines(rdr, vtxarrNrmLine, 2, RendererUtils::LINES);

		rdr->setLighting(true);						// ライティングあり	
	}

	::glLightModelfv(GL_LIGHT_MODEL_AMBIENT, colLightAmbient0.ptr());
	::glDisable(GL_LIGHT0);

	rdr->setLighting(false);				// ライティングなしに戻す
///	rdr->setSolidColor(255, 255, 255, 255);	// 色を戻す
}
#endif

////////////////////////////////////////////////////////////////////////////

TFW_END_NS