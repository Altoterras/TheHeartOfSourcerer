/***********************************************************************//**
 *  Shape.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/18.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

//#define _INTEL_GPU

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Shape.h"

// Friends
#include "ShapeCtx.h"
#include "Material.h"
#include "../string/VcString.h"
#include "../collection/List.h"
#include "../lib/Vector.h"
#include "../lib/Matrix.h"
#include "../framemod/ExecCtx.h"
#include "../framemod/RenderCtx.h"
#if defined(_INTEL_GPU)
#include "../gcmn/Renderer.h"
#endif

// External

// Library
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////
// マクロ

#define FORCE_VBUF		0	// 強制バーテックスバッファ描画
#define FORCE_IVBUF		0	// 強制インターリーブバーテックスバッファ描画
#define FORCE_PSPR		0	// 強制ポイントスプライト描画
#define FORCE_SPRT		0	// 強制スプライト描画

#if FORCE_SPRT
#include "./sgnodeimp/Camera.h"
#include "../gcmn/Renderer.h"
#endif

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Shape メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	定義名を設定する
**//*---------------------------------------------------------------------*/
void Shape::setDefineName(const CStringBase*	defname)
{
	char wk[MAX_DEFNAME_LEN];
	#if defined(_WINDOWS)
		::strncpy_s(wk, MAX_DEFNAME_LEN, defname->getRaw(), MAX_DEFNAME_LEN);
	#else
		::strncpy(wk, defname->getRaw(), MAX_DEFNAME_LEN);
	#endif
	wk[MAX_DEFNAME_LEN - 1] = '\0';
	_defname = new VcString(wk);
}

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	デバッグ名を設定する
**//*---------------------------------------------------------------------*/
void Shape::setDebugName(const CStringBase* dbgname)
{
	_dbgname = new VcString(dbgname);
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	マテリアルを関連づける
**//*---------------------------------------------------------------------*/
void Shape::bindMaterial(Material* mtrlRef)
{
	_mtrlRef = mtrlRef;
}

/*---------------------------------------------------------------------*//**
	バウンディングボックスを最大・最小値を取得する
**//*---------------------------------------------------------------------*/
bool Shape::getBoundingBox(Vector3F* min, Vector3F* max)
{
	if(_bboxMin == 0L)	{	return false;	}
	min->copy(_bboxMin);
	max->copy(_bboxMax);
	return true;
}

/*---------------------------------------------------------------------*//**
	バウンディングボックスを設定する
**//*---------------------------------------------------------------------*/
void Shape::setBoundingBox(const Vector3F* min, const Vector3F* max)
{
	if(_bboxMin == 0L)
	{
		_bboxMin = new Vector3F();
		_bboxMax = new Vector3F();
	}
	_bboxMin->copy(min);
	_bboxMax->copy(max);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Shape::Shape()
	: _id(0)
	, _spid(0)
	, _defname(0L)
#if defined(_DEBUG)
	, _dbgname(0L)
#endif
	, _faceset(0L)
	, _mtrlRef(0L)
	, _bboxMin(0L)
	, _bboxMax(0L)
#if ENABLE_SHAPE_RENDER_CB
	, _rcbAfter(0L)
	, _paramAfterRcb(0L)
#endif
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Shape::~Shape()
{
	ASSERT(_defname == 0L);
	ASSERT(_faceset == 0L);
	ASSERT(_bboxMin == 0L);
	ASSERT(_bboxMax == 0L);
}

#if !FORCE_VBUF && !FORCE_IVBUF && !FORCE_PSPR && !FORCE_SPRT
/*---------------------------------------------------------------------*//**
	作成
		※各配列メモリはコピーせずそのまま内部で削除するので
		　呼び出し元で削除しないこと
		　パラメータ名 ～Entr はオブジェクト内部で値を管理・保持し、
		　そのオブジェクトの責任で解放するものとする（Entrust）
	
	@param	strName 名前
	@param	varrCoordEntr 頂点座標配列
	@param	varrColorEntr 頂点カラー配列
	@param	varrTextureCoordEntr 頂点テクスチャ座標配列
	@param	numVertex 頂点数
	@param	idxarrEntr インデックス配列
	@param	numIndex インデックス数
	@param	arrVnumInFaceEntr 各頂点グループ（STRIP/FAN）内頂点数配列
	@param	numVnumInFace 各頂点グループ（STRIP/FAN）内頂点数配列要素数
**//*---------------------------------------------------------------------*/
bool Shape::create(Vector3F* varrCoordEntr, Vector3F* varrNormalEntr, Vector4U8* varrColorEntr, Vector2F* varrTextureCoordEntr, u32 numVertex, u16* idxarrEntr, u16 numIndex, u16* arrVnumInFaceEntr, u16 numVnumInFace)
{
	FaceSet* faceset = new FaceSet();

	// 頂点バッファの作成
	{
		::glGenBuffers(1, &faceset->_vtxbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_vtxbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3F) * numVertex, varrCoordEntr, GL_STATIC_DRAW);
	}
	// 法線バッファの作成
	if(varrNormalEntr != 0L)
	{
		::glGenBuffers(1, &faceset->_nrmbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_nrmbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3F) * numVertex, varrNormalEntr, GL_STATIC_DRAW);
	}
	// カラーバッファの作成
	if(varrColorEntr != 0L)
	{
		::glGenBuffers(1, &faceset->_colbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_colbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4U8) * numVertex, varrColorEntr, GL_STATIC_DRAW);
	}
	// テクスチャ UV 座標バッファの作成
	if(varrTextureCoordEntr != 0L)
	{
		::glGenBuffers(1, &faceset->_texbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_texbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2F) * numVertex, varrTextureCoordEntr, GL_STATIC_DRAW);
	}
	// インデックスバッファの作成
	if(idxarrEntr != 0L)
	{
		::glGenBuffers(1, &faceset->_idxbuf);
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceset->_idxbuf);
		::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numIndex, idxarrEntr, GL_STATIC_DRAW);
	}
	// バインドをもどす
	::glBindBuffer(GL_ARRAY_BUFFER, 0);
	::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// 面毎の頂点数を保存
	faceset->_arrVnumInFace	= arrVnumInFaceEntr;
	faceset->_numVnumInFace	= numVnumInFace;

	_faceset = faceset;

	delete[] varrCoordEntr;
	delete[] varrNormalEntr;
	delete[] varrColorEntr;
	delete[] varrTextureCoordEntr;
	delete[] idxarrEntr;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Shape::destroy()
{
	delete _defname;
	_defname = 0L;

	delete _faceset;
	_faceset = 0L;

	_mtrlRef = 0L;

	delete _bboxMin;
	_bboxMin = 0L;
	delete _bboxMax;
	_bboxMax = 0L;

	#if defined(_DEBUG)
		delete _dbgname;
		_dbgname = 0L;
	#endif
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを作成する
**//*---------------------------------------------------------------------*/
bool Shape::createContext(ShapeCtx* ctx)
{
	return ctx->create(_mtrlRef);
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを破棄する
**//*---------------------------------------------------------------------*/
void Shape::destroyContext(ShapeCtx* ctx)
{
	ctx->destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Shape::exec(const ExecCtx* ec, ShapeCtx* shpctx)
{
	// マテリアルのフレーム制御
	if(_mtrlRef != 0L)
	{
		_mtrlRef->exec(ec, shpctx->_mtrlctx);
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void Shape::render(const RenderCtx* rc, ShapeCtx* shpctx)
{
	if(_faceset == 0L)	{	return;	}

	// マテリアル設定
	if(_mtrlRef != 0L)
	{
		_mtrlRef->render(rc, shpctx->_mtrlctx);
	}

	// 頂点
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_vtxbuf);
		::glVertexPointer(3, GL_FLOAT, 0, 0);
	}
	// 法線
	if(_faceset->_nrmbuf != 0)
	{
		::glEnableClientState(GL_NORMAL_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_nrmbuf);
		::glNormalPointer(GL_FLOAT, 0, 0);
	}
	// 頂点カラー
	if(_faceset->_colbuf != 0)
	{
		///::glDisable(GL_LIGHTING);
		::glEnableClientState(GL_COLOR_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_colbuf);
		::glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
	}
	// テクスチャ
	if(_faceset->_texbuf != 0)
	{
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_texbuf);
		::glTexCoordPointer(2, GL_FLOAT, 0, 0);
	}

	// 描画
	::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _faceset->_idxbuf);
	u32 ofsVif = 0;
	for(int ivif = 0; ivif < _faceset->_numVnumInFace; ivif++)
	{
		::glDrawElements(GL_TRIANGLE_FAN, _faceset->_arrVnumInFace[ivif], GL_UNSIGNED_SHORT, (GLvoid*)((u16*)0L + ofsVif));
		ofsVif += _faceset->_arrVnumInFace[ivif];
	}

	// 設定を戻す
 	{
		::glDisableClientState(GL_VERTEX_ARRAY);
	}
	if(_faceset->_nrmbuf != 0)
	{
		::glDisableClientState(GL_NORMAL_ARRAY);
	}
	if(_faceset->_colbuf != 0)
	{
		///::glEnable(GL_LIGHTING);
		::glDisableClientState(GL_COLOR_ARRAY);
	}
	if(_faceset->_texbuf != 0)
	{
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	::glBindBuffer(GL_ARRAY_BUFFER, 0L);
	::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0L);

#if defined(_INTEL_GPU)
	GLfloat fcol[4];
	::glGetFloatv(GL_CURRENT_COLOR, fcol);
	rc->getRenderer()->setSolidColorForce((u8)(fcol[0] * 255.0f), (u8)(fcol[1] * 255.0f), (u8)(fcol[2] * 255.0f), (u8)(fcol[3] * 255.0f));
#endif

#if ENABLE_SHAPE_RENDER_CB
	// 描画後コールバック
	if(_rcbAfter != 0L)
	{
		_rcbAfter(this, rc, shpctx, _paramAfterRcb);
	}
#endif
}
#endif

#if FORCE_VBUF
/*---------------------------------------------------------------------*//**
	作成（強制バーテックスバッファ描画バージョン）
**//*---------------------------------------------------------------------*/
bool Shape::create(Vector3F* varrCoordEntr, Vector3F* varrNormalEntr, Vector4U8* varrColorEntr, Vector2F* varrTextureCoordEntr, u32 numVertex, u16* idxarrEntr, u16 numIndex, u16* arrVnumInFaceEntr, u16 numVnumInFace)
{
	u32 ofsVif = 0;
	for(int ivif = 0; ivif < numVnumInFace; ivif++)
	{
		ofsVif += arrVnumInFaceEntr[ivif];
	}
	u32 numTotalVertex = ofsVif;

	Vector3F* varrCrdV = (varrCoordEntr != 0L) ? new Vector3F[numTotalVertex] : 0L;
	Vector3F* varrNrmV = (varrNormalEntr != 0L) ? new Vector3F[numTotalVertex] : 0L;
	Vector4U8* varrColV = (varrColorEntr != 0L) ? new Vector4U8[numTotalVertex] : 0L;
	Vector2F* varrTexV = (varrTextureCoordEntr != 0L) ? new Vector2F[numTotalVertex] : 0L;

	ofsVif = 0;
	u32 idx = 0;
	for(int ivif = 0; ivif < numVnumInFace; ivif++)
	{
		for(int iv = 0; iv < arrVnumInFaceEntr[ivif]; iv++)
		{
			if(varrCrdV != 0L)	{	varrCrdV[idx] = varrCoordEntr[			idxarrEntr[ofsVif + iv]	];	}
			if(varrNrmV != 0L)	{	varrNrmV[idx] = varrNormalEntr[			idxarrEntr[ofsVif + iv]	];	}
			if(varrColV != 0L)	{	varrColV[idx] = varrColorEntr[			idxarrEntr[ofsVif + iv]	];	}
			if(varrTexV != 0L)	{	varrTexV[idx] = varrTextureCoordEntr[	idxarrEntr[ofsVif + iv]	];	}
			idx++;
		}
		ofsVif += arrVnumInFaceEntr[ivif];
	}

	FaceSet* faceset = new FaceSet();

	// 頂点バッファの作成
	{
		::glGenBuffers(1, &faceset->_vtxbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_vtxbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3F) * numTotalVertex, varrCrdV, GL_STATIC_DRAW);
	}
	// 法線バッファの作成
	if(varrNrmV != 0L)
	{
		::glGenBuffers(1, &faceset->_nrmbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_nrmbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3F) * numTotalVertex, varrNrmV, GL_STATIC_DRAW);
	}
	// カラーバッファの作成
	if(varrColV != 0L)
	{
		::glGenBuffers(1, &faceset->_colbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_colbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4U8) * numTotalVertex, varrColV, GL_STATIC_DRAW);
	}
	// テクスチャ UV 座標バッファの作成
	if(varrTexV != 0L)
	{
		::glGenBuffers(1, &faceset->_texbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_texbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2F) * numTotalVertex, varrTexV, GL_STATIC_DRAW);
	}
	// バインドをもどす
	::glBindBuffer(GL_ARRAY_BUFFER, 0);

	// 面毎の頂点数を保存
	faceset->_arrVnumInFace	= arrVnumInFaceEntr;
	faceset->_numVnumInFace	= numVnumInFace;

	_faceset = faceset;

	delete[] varrCoordEntr;
	delete[] varrNormalEntr;
	delete[] varrColorEntr;
	delete[] varrTextureCoordEntr;
	delete[] idxarrEntr;

	delete[] varrCrdV;
	delete[] varrNrmV;
	delete[] varrColV;
	delete[] varrTexV;

	return true;
}

/*---------------------------------------------------------------------*//**
	描画（強制バーテックスバッファ描画バージョン）
**//*---------------------------------------------------------------------*/
void Shape::render(const RenderCtx* rc)
{
	if(_faceset == 0L)	{	return;	}

	// マテリアル設定
	if(_mtrlRef != 0L)
	{
		_mtrlRef->render(rc);
	}

	// 頂点
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_vtxbuf);
		::glVertexPointer(3, GL_FLOAT, 0, 0);
	}
	// 法線
	if(_faceset->_nrmbuf != 0)
	{
		::glEnableClientState(GL_NORMAL_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_nrmbuf);
		::glNormalPointer(GL_FLOAT, 0, 0);
	}
	// 頂点カラー
	if(_faceset->_colbuf != 0)
	{
		///::glDisable(GL_LIGHTING);
		::glEnableClientState(GL_COLOR_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_colbuf);
		::glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
	}
	// テクスチャ
	if(_faceset->_texbuf != 0)
	{
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_texbuf);
		::glTexCoordPointer(2, GL_FLOAT, 0, 0);
	}

	// 描画
	u32 ofsVif = 0;
	for(int ivif = 0; ivif < _faceset->_numVnumInFace; ivif++)
	{
		::glDrawArrays(GL_TRIANGLE_FAN, ofsVif, _faceset->_arrVnumInFace[ivif]);
		ofsVif += _faceset->_arrVnumInFace[ivif];
	}

	// 設定を戻す
	{
		::glDisableClientState(GL_VERTEX_ARRAY);
	}
	if(_faceset->_nrmbuf != 0)
	{
		::glDisableClientState(GL_NORMAL_ARRAY);
	}
	if(_faceset->_colbuf != 0)
	{
		///::glEnable(GL_LIGHTING);
		::glDisableClientState(GL_COLOR_ARRAY);
	}
	if(_faceset->_texbuf != 0)
	{
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	::glBindBuffer(GL_ARRAY_BUFFER, 0L);
}
#endif

#if FORCE_IVBUF
/*---------------------------------------------------------------------*//**
	作成（強制インターリーブバーテックスバッファ描画バージョン）
**//*---------------------------------------------------------------------*/
bool Shape::create(Vector3F* varrCoordEntr, Vector3F* varrNormalEntr, Vector4U8* varrColorEntr, Vector2F* varrTextureCoordEntr, u32 numVertex, u16* idxarrEntr, u16 numIndex, u16* arrVnumInFaceEntr, u16 numVnumInFace)
{
	FaceSet* faceset = new FaceSet();

	u32 ofsVif = 0;
	for(int ivif = 0; ivif < numVnumInFace; ivif++)
	{
		ofsVif += arrVnumInFaceEntr[ivif];
	}
	u32 numTotalVertex = ofsVif;

	u32 sizeVertex = 0;
	if(varrCoordEntr != 0L)
	{
		sizeVertex += sizeof(Vector3F);
		faceset->_vtxbuf = 1;
	}
	if(varrNormalEntr != 0L)
	{
		sizeVertex += sizeof(Vector3F);
		faceset->_nrmbuf = 1;
	}
	if(varrColorEntr != 0L)
	{
		sizeVertex += sizeof(Vector4F);
		faceset->_colbuf = 1;
	}
	if(varrTextureCoordEntr != 0L)
	{
		sizeVertex += sizeof(Vector2F);
		faceset->_texbuf = 1;
	}

	u8* bufV = new u8[numTotalVertex * sizeVertex];

	ofsVif = 0;
	u32 idx = 0;
	u8* p = bufV;
	for(int ivif = 0; ivif < numVnumInFace; ivif++)
	{
		for(int iv = 0; iv < arrVnumInFaceEntr[ivif]; iv++)
		{
			if(varrCoordEntr != 0L)
			{
				(*((Vector3F*)(p))) = varrCoordEntr[ idxarrEntr[ofsVif + iv] ];
				p += sizeof(Vector3F);
			}
			if(varrNormalEntr != 0L)
			{
				(*((Vector3F*)(p))) = varrNormalEntr[ idxarrEntr[ofsVif + iv] ];
				p += sizeof(Vector3F);
			}
			if(varrColorEntr != 0L)
			{
				(*((Vector4F*)(p)))._v[0] = varrColorEntr[ idxarrEntr[ofsVif + iv] ]._v[0] / 255.0f;
				(*((Vector4F*)(p)))._v[1] = varrColorEntr[ idxarrEntr[ofsVif + iv] ]._v[1] / 255.0f;
				(*((Vector4F*)(p)))._v[2] = varrColorEntr[ idxarrEntr[ofsVif + iv] ]._v[2] / 255.0f;
				(*((Vector4F*)(p)))._v[3] = varrColorEntr[ idxarrEntr[ofsVif + iv] ]._v[3] / 255.0f;
				p += sizeof(Vector4F);
			}
			if(varrTextureCoordEntr != 0L)
			{
				(*((Vector2F*)(p))) = varrTextureCoordEntr[	idxarrEntr[ofsVif + iv] ];
				p += sizeof(Vector2F);
			}
		}
		ofsVif += arrVnumInFaceEntr[ivif];
	}

	// インターリーブバッファの作成
	::glGenBuffers(1, &faceset->_ilvbuf);
	::glBindBuffer(GL_ARRAY_BUFFER, faceset->_ilvbuf);
	::glBufferData(GL_ARRAY_BUFFER, numTotalVertex * sizeVertex, bufV, GL_STATIC_DRAW);

	// バインドをもどす
	::glBindBuffer(GL_ARRAY_BUFFER, 0);

	// 面毎の頂点数を保存
	faceset->_arrVnumInFace	= arrVnumInFaceEntr;
	faceset->_numVnumInFace	= numVnumInFace;

	// １頂点のサイズを得る
	faceset->_sizeVertex = sizeVertex;

	_faceset = faceset;

	delete[] varrCoordEntr;
	delete[] varrNormalEntr;
	delete[] varrColorEntr;
	delete[] varrTextureCoordEntr;
	delete[] idxarrEntr;

	delete[] bufV;

	return true;
}

/*---------------------------------------------------------------------*//**
	描画（強制インターリーブバーテックスバッファ描画バージョン）
**//*---------------------------------------------------------------------*/
void Shape::render(const RenderCtx* rc)
{
	if(_faceset == 0L)	{	return;	}

	// マテリアル設定
	if(_mtrlRef != 0L)
	{
		_mtrlRef->render(rc);
	}

	// バッファをセット
	::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_ilvbuf);

	// 頂点
	u32 ofsItv = 0;
	if(_faceset->_vtxbuf != 0)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, _faceset->_sizeVertex, (void*)ofsItv);
		ofsItv += sizeof(Vector3F);
	}
	// 法線
	if(_faceset->_nrmbuf != 0)
	{
		::glEnableClientState(GL_NORMAL_ARRAY);
		::glNormalPointer(GL_FLOAT, _faceset->_sizeVertex, (void*)ofsItv);
		ofsItv += sizeof(Vector3F);
	}
	// 頂点カラー
	if(_faceset->_colbuf != 0)
	{
		::glEnableClientState(GL_COLOR_ARRAY);
		::glColorPointer(4, GL_FLOAT, _faceset->_sizeVertex, (void*)ofsItv);
		ofsItv += sizeof(Vector4F);
	}
	// テクスチャ
	if(_faceset->_texbuf != 0)
	{
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glTexCoordPointer(2, GL_FLOAT, _faceset->_sizeVertex, (void*)ofsItv);
		ofsItv += sizeof(Vector2F);
	}

	// 描画
	u32 ofsVif = 0;
	for(int ivif = 0; ivif < _faceset->_numVnumInFace; ivif++)
	{
		::glDrawArrays(GL_TRIANGLE_FAN, ofsVif, _faceset->_arrVnumInFace[ivif]);
		ofsVif += _faceset->_arrVnumInFace[ivif];
	}

	// 設定を戻す
	if(_faceset->_vtxbuf != 0)
	{
		::glDisableClientState(GL_VERTEX_ARRAY);
	}
	if(_faceset->_nrmbuf != 0)
	{
		::glDisableClientState(GL_NORMAL_ARRAY);
	}
	if(_faceset->_colbuf != 0)
	{
		::glDisableClientState(GL_COLOR_ARRAY);
	}
	if(_faceset->_texbuf != 0)
	{
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	::glBindBuffer(GL_ARRAY_BUFFER, 0L);
}
#endif

#if FORCE_PSPR
/*---------------------------------------------------------------------*//**
	作成（強制ポイントスプライト描画バージョン）
**//*---------------------------------------------------------------------*/
bool Shape::create(Vector3F* varrCoordEntr, Vector3F* varrNormalEntr, Vector4U8* varrColorEntr, Vector2F* varrTextureCoordEntr, u32 numVertex, u16* idxarrEntr, u16 numIndex, u16* arrVnumInFaceEntr, u16 numVnumInFace)
{
	u32 numTotalVertex = 1;
	Vector2F* varrCrdV = (varrCoordEntr != 0L) ? new Vector2F(0.0f, 0.0f) : 0L;
	Vector4U8* varrColV = (varrColorEntr != 0L) ? new Vector4U8(varrColorEntr[0]) : 0L;
	Vector2F* varrTexV = 0L;

	FaceSet* faceset = new FaceSet();

	// 頂点バッファの作成
	{
		::glGenBuffers(1, &faceset->_vtxbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_vtxbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2F) * numTotalVertex, varrCrdV, GL_STATIC_DRAW);
	}
	// カラーバッファの作成
	if(varrColV != 0L)
	{
		::glGenBuffers(1, &faceset->_colbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_colbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4U8) * numTotalVertex, varrColV, GL_STATIC_DRAW);
	}
	// テクスチャ UV 座標バッファの作成
	if(varrTexV != 0L)
	{
		::glGenBuffers(1, &faceset->_texbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_texbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2F) * numTotalVertex, varrTexV, GL_STATIC_DRAW);
	}
	// バインドをもどす
	::glBindBuffer(GL_ARRAY_BUFFER, 0);

	_faceset = faceset;

	// ポイントスプライトを有効化
	#if defined(_WINDOWS)
		::glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
		::glEnable(GL_POINT_SPRITE);
	#else
		::glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE);
		::glEnable(GL_POINT_SPRITE_OES);
	#endif

	delete[] varrCoordEntr;
	delete[] varrNormalEntr;
	delete[] varrColorEntr;
	delete[] varrTextureCoordEntr;
	delete[] idxarrEntr;

	delete[] varrCrdV;
	delete[] varrColV;
	delete[] varrTexV;

	return true;
}

/*---------------------------------------------------------------------*//**
	描画（強制ポイントスプライト描画バージョン）
**//*---------------------------------------------------------------------*/
void Shape::render(const RenderCtx* rc)
{
	if(_faceset == 0L)	{	return;	}

	// マテリアル設定
	if(_mtrlRef != 0L)
	{
		_mtrlRef->render(rc);
	}

	// 頂点
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_vtxbuf);
		::glVertexPointer(2, GL_FLOAT, 0, 0);
	}
	// 頂点カラー
	if(_faceset->_colbuf != 0)
	{
		::glEnableClientState(GL_COLOR_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_colbuf);
		::glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
	}
	// テクスチャ
	if(_faceset->_texbuf != 0)
	{
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_texbuf);
		::glTexCoordPointer(2, GL_FLOAT, 0, 0);
	}

	// 描画
	::glPointSize(64);   
	::glDrawArrays(GL_POINTS, 0, 1);

	// 設定を戻す
	{
		::glDisableClientState(GL_VERTEX_ARRAY);
	}
	if(_faceset->_colbuf != 0)
	{
		::glDisableClientState(GL_COLOR_ARRAY);
	}
	if(_faceset->_texbuf != 0)
	{
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	::glBindBuffer(GL_ARRAY_BUFFER, 0L);
}
#endif

#if FORCE_SPRT
/*---------------------------------------------------------------------*//**
	作成（強制スプライト描画バージョン）
**//*---------------------------------------------------------------------*/
bool Shape::create(Vector3F* varrCoordEntr, Vector3F* varrNormalEntr, Vector4U8* varrColorEntr, Vector2F* varrTextureCoordEntr, u32 numVertex, u16* idxarrEntr, u16 numIndex, u16* arrVnumInFaceEntr, u16 numVnumInFace)
{
	u32 numTotalVertex = 4;

	Vector3F* varrCrdV = (varrCoordEntr != 0L) ? new Vector3F[numTotalVertex] : 0L;
	Vector4U8* varrColV = (varrColorEntr != 0L) ? new Vector4U8[numTotalVertex] : 0L;
	Vector2F* varrTexV = (varrTextureCoordEntr != 0L) ? new Vector2F[numTotalVertex] : 0L;

	for(u32 i = 0; i < numTotalVertex; i++)
	{
		if(varrCrdV != 0L)
		{
			varrCrdV[i] = varrCoordEntr[ idxarrEntr[i] ];
		}
		if(varrColV != 0L)
		{
			varrColV[i] = varrColorEntr[ idxarrEntr[i] ];
		}
		if(varrTexV != 0L)
		{
			varrTexV[i] = varrTextureCoordEntr[	idxarrEntr[i] ];
		}
	}

	FaceSet* faceset = new FaceSet();

	// 頂点バッファの作成
	{
		::glGenBuffers(1, &faceset->_vtxbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_vtxbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3F) * numTotalVertex, varrCrdV, GL_STATIC_DRAW);
	}
	// カラーバッファの作成
	if(varrColV != 0L)
	{
		::glGenBuffers(1, &faceset->_colbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_colbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4U8) * numTotalVertex, varrColV, GL_STATIC_DRAW);
	}
	// テクスチャ UV 座標バッファの作成
	if(varrTexV != 0L)
	{
		::glGenBuffers(1, &faceset->_texbuf);
		::glBindBuffer(GL_ARRAY_BUFFER, faceset->_texbuf);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2F) * numTotalVertex, varrTexV, GL_STATIC_DRAW);
	}
	// バインドをもどす
	::glBindBuffer(GL_ARRAY_BUFFER, 0);

	_faceset = faceset;

	delete[] varrCoordEntr;
	delete[] varrNormalEntr;
	delete[] varrColorEntr;
	delete[] varrTextureCoordEntr;
	delete[] idxarrEntr;

	delete[] varrCrdV;
	delete[] varrColV;
	delete[] varrTexV;

	return true;
}

/*---------------------------------------------------------------------*//**
	描画（強制スプライト描画バージョン）
**//*---------------------------------------------------------------------*/
void Shape::render(const RenderCtx* rc)
{
	if(_faceset == 0L)	{	return;	}

	// マテリアル設定
	if(_mtrlRef != 0L)
	{
		_mtrlRef->render(rc);
	}

	// ビルボードマトリックスの設定
	const Matrix4F* mtxBb = rc->getRenderer()->getCurrentCamera()->getBillboardMatrix();
	::glMultMatrixf(mtxBb->ptr());

	// 頂点
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_vtxbuf);
		::glVertexPointer(3, GL_FLOAT, 0, 0);
	}
	// 頂点カラー
	if(_faceset->_colbuf != 0)
	{
		::glEnableClientState(GL_COLOR_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_colbuf);
		::glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
	}
	// テクスチャ
	if(_faceset->_texbuf != 0)
	{
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glBindBuffer(GL_ARRAY_BUFFER, _faceset->_texbuf);
		::glTexCoordPointer(2, GL_FLOAT, 0, 0);
	}

	// 描画
	::glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// 設定を戻す
	{
		::glDisableClientState(GL_VERTEX_ARRAY);
	}
	if(_faceset->_nrmbuf != 0)
	{
		::glDisableClientState(GL_NORMAL_ARRAY);
	}
	if(_faceset->_colbuf != 0)
	{
		///::glEnable(GL_LIGHTING);
		::glDisableClientState(GL_COLOR_ARRAY);
	}
	if(_faceset->_texbuf != 0)
	{
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	::glBindBuffer(GL_ARRAY_BUFFER, 0L);
}
#endif

//==========================================================================
// Shape::FaceSet メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Shape::FaceSet::FaceSet()
{
	init();
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Shape::FaceSet::~FaceSet()
{
	cleanup();
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
void Shape::FaceSet::init()
{
	_vtxbuf = 0;
	_nrmbuf = 0;
	_colbuf = 0;
	_texbuf = 0;
	_idxbuf = 0;
	_ilvbuf = 0;
	_arrVnumInFace = 0L;
	_numVnumInFace = 0;
	_sizeVertex = 0;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void Shape::FaceSet::cleanup()
{
	delete _arrVnumInFace;
	if(_vtxbuf != 0)	{	::glDeleteBuffers(1, &_vtxbuf);	}
	if(_nrmbuf != 0)	{	::glDeleteBuffers(1, &_nrmbuf);	}
	if(_colbuf != 0)	{	::glDeleteBuffers(1, &_colbuf);	}
	if(_texbuf != 0)	{	::glDeleteBuffers(1, &_texbuf);	}
	if(_idxbuf != 0)	{	::glDeleteBuffers(1, &_idxbuf);	}
	if(_ilvbuf != 0)	{	::glDeleteBuffers(1, &_ilvbuf);	}
	init();
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS