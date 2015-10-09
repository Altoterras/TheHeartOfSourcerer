/***********************************************************************//**
 *  HrAnimObj.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/07/01.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_HR_ANIM_OBJ_H_
#define _TFW_G3D_HR_ANIM_OBJ_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
template<class TYPE> class Vector4;
typedef Vector3<f32> Vector3F;
typedef Vector4<f32> Vector4F;
class HrAnimObjCtxArray;
class HrAnimObjPosture;
class Matrix4F;
class Shape;
class ShapeCtx;
class RenderCtx;

/*---------------------------------------------------------------------*//**
 *	階層アニメ オブジェクト
 *		階層アニメオブジェクトは１つがシェイプに対応する
 *		親子関節構造を表すもの
 *
**//*---------------------------------------------------------------------*/
class HrAnimObj
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// インデックスを得る
	inline u16 getIndex() const { return _index; }
	// 子オブジェを得る
	inline const HrAnimObj* getChild() const { return _child; }
	inline HrAnimObj* child() const { return _child; }
	// 兄弟オブジェを得る
	inline const HrAnimObj*	getSibling() const { return _sibling; }
	inline HrAnimObj* sibling() const { return _sibling; }
	
	// インデックスを設定する
	inline void setIndex(u16 index) { _index = index; }
	// 子オブジェを設定する
	inline void setChild(HrAnimObj* child) { _child = child; }
	// 兄弟オブジェを設定する
	inline void setSibling(HrAnimObj* sibling) { _sibling = sibling; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void				getPosture(HrAnimObjCtxArray* ctxa, HrAnimObjPosture* aop);
	void				setPosture(HrAnimObjCtxArray* ctxa, const HrAnimObjPosture* aop);
	bool				setPosture(HrAnimObjCtxArray* ctxa, u16 index, const HrAnimObjPosture* aop);

	static HrAnimObj*	find(HrAnimObj* paobjRoot, const Shape* shapeTrg);

	void				calcMatrix(Matrix4F* mtx, HrAnimObjCtxArray* ctxa) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HrAnimObj();
	~HrAnimObj();
	bool				create(u16 index, Shape* shapeRef);
	void				destroy();
	bool				createContext(HrAnimObjCtxArray* ctxa, Shape** sarr, ShapeCtx** scarr, u16 numShape);
	void				destroyContext(HrAnimObjCtxArray* ctxa);
	void				render(const RenderCtx* rc, HrAnimObjCtxArray* ctxa);

	void				debug_print(int depth);

	//======================================================================
	// 変数
private:
	u16					_index;
	Shape*				_shapeRef;
	HrAnimObj*			_child;
	HrAnimObj*			_sibling;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_HR_ANIM_OBJ_H_