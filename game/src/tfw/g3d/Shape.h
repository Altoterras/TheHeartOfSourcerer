/***********************************************************************//**
 *  Shape.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/13.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SHAPE_H_
#define _TFW_G3D_SHAPE_H_

////////////////////////////////////////////////////////////////////////////
// コンパイルスイッチ

#define ENABLE_SHAPE_RENDER_CB	0	// シェイプ描画コールバックを有効化

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector2;
template<class TYPE> class Vector3;
template<class TYPE> class Vector4;
template<class TYPE> class List;
typedef Vector2<f32> Vector2F;
typedef Vector3<f32> Vector3F;
typedef Vector4<u8> Vector4U8;
class Matrix4F;
class Material;
class VcString;
class ExecCtx;
class RenderCtx;
class ShapeCtx;

/*---------------------------------------------------------------------*//**
 *	3D シェイプ クラス
 *
**//*---------------------------------------------------------------------*/
class Shape
{
	//======================================================================
	// 定数
public:
	static const s32	MAX_DEFNAME_LEN	= 8;	// 定義名の最大長

#if ENABLE_SHAPE_RENDER_CB
	typedef bool (* ShapeRenderCallback)(Shape* shape, const RenderCtx* rc, ShapeCtx* shpctx, void* param);
#endif

	//======================================================================
	// クラス
public:
	class FaceSet
	{
	public:
		GLuint		_vtxbuf;
		GLuint		_nrmbuf;
		GLuint		_colbuf;
		GLuint		_texbuf;
		GLuint		_idxbuf;
		GLuint		_ilvbuf;
		u16*		_arrVnumInFace;
		u16			_numVnumInFace;
		u32			_sizeVertex;

		FaceSet();
		virtual ~FaceSet();
		void init();
		void cleanup();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ID を得る
	inline u16 getId() const { return _id; }
	// SPID を得る
	inline u16 getSpid() const { return _spid; }
	// 定義名を得る
	inline const VcString* getDefineName() const { return _defname; }
	// マテリアルを得る
	inline Material* getMaterial() const { return _mtrlRef; }
#if defined(_DEBUG)
	// デバッグ名を得る
	inline const VcString* getDebugName() const { return _dbgname; }
#endif

	// ID を設定する
	inline void setId(u16 id) { _id = id; }
	// SPID を設定する
	inline void setSpid(u16 spid) { _spid = spid; }
	// 定義名を設定する
	void setDefineName(const CStringBase* defname);
#if defined(_DEBUG)
	// デバッグ名を設定する
	void setDebugName(const CStringBase* dbgname);
#endif
#if ENABLE_SHAPE_RENDER_CB
	// 3D 描画後コールバックを設定する
	void setRenderAfterCallback(ShapeRenderCallback rcb, void* param) { _rcbAfter = rcb; _paramAfterRcb = param; }
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void			bindMaterial(Material* mtrlRef);
	bool			getBoundingBox(Vector3F* min, Vector3F* max);
	void			setBoundingBox(const Vector3F* min, const Vector3F* max);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Shape();
	~Shape();
	bool			create(Vector3F* varrCoordEntr, Vector3F* varrNormalEntr, Vector4U8* varrColorEntr, Vector2F* varrTextureCoordEntr, u32 numVertex, u16* idxarrEntr, u16 numIndex, u16* arrVnumInFaceEntr, u16 numVnumInFace);
	void			destroy();
	bool			createContext(ShapeCtx* ctx);
	void			destroyContext(ShapeCtx* ctx);
	void			exec(const ExecCtx* ec, ShapeCtx* shpctx);
	void			render(const RenderCtx* rc, ShapeCtx* shpctx);

	//======================================================================
	// 変数
private:
	u16					_id;
	u16					_spid;
	VcString*			_defname;
	FaceSet*			_faceset;
	Material*			_mtrlRef;
	Vector3F*			_bboxMin;
	Vector3F*			_bboxMax;
#if ENABLE_SHAPE_RENDER_CB
	ShapeRenderCallback	_rcbAfter;
	void*				_paramAfterRcb;
#endif

#if defined(_DEBUG)
	VcString*			_dbgname;
#endif
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SHAPE_H_