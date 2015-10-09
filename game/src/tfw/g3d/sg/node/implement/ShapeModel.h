/***********************************************************************//**
 *  ShapeModel.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/07/27.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_NODE_IMPLEMENT_SHAPE_MODEL_H_
#define _TFW_G3D_SG_NODE_IMPLEMENT_SHAPE_MODEL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "Model.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Shape;
class ShapeCtx;
class Texture;
class Material;
class HrAnimSet;
class HrAnimSetCtx;
class Gcol;
class RenderCtx;

/*---------------------------------------------------------------------*//**
 *	3D シェイプ モデル
 *
**//*---------------------------------------------------------------------*/
class ShapeModel : public Model
{
	//======================================================================
	// 定数
public:
	// 定義タイプ
	static const u8		DEFTYPE_NULL		= 0;
	static const u8		DEFTYPE_DCL			= 1;	// 宣言
	static const u8		DEFTYPE_ALT			= 2;	// 代替

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// シェイプの数を得る
	s32				getShapeNum() const			{	return _numShape;			}
	// シェイプを得る
	Shape*			getShape(int index) const	{	return _sarrShape[index];	}
	// アニメセットを得る
	HrAnimSet*		getAnimSet() const			{	return _asetRef;			}
	// アニメセットコンテキストを得る
	HrAnimSetCtx*	getAnimSetContext() const	{	return _ascAsetCtx;			}
	// 地面当たりを得る
	Gcol*			getGcol() const				{	return _gcolRef;			}
	// 定義タイプを得る
	u8				getDefType() const			{	return _deftype;			}

	// 定期タイプを設定する
	void			setDefType(u8 deftype)		{	_deftype = deftype;			}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	s32				getAnimNum() const;
	u16				getCurAnimId() const;
	u16				getMaxAnimId() const;
	void			setAnimId(u16 animid, s32 frameIntp, bool replaceHeadKeyFrameByCurPos, bool isForceReset, bool isLoop, bool isChangeFirstFrame, bool isChangeEndFrame);
	f32				getCurAnimFrame() const;
	void			resetTexAnimFrame();

	bool			isSameShape(const ShapeModel* mdlTrg) const;
	bool			isSameDefNameShape(const ShapeModel* mdlTrg) const;

	void			getAllShapesBoundingBox(Vector3F* vBbCenter, f32* rBb);

	bool			setAnimObjPosture(u16 indexObj, const Vector3F* trs, const Vector3F* rot, const Vector3F* scl);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ShapeModel();
	virtual ~ShapeModel();
	bool			create(Shape** shapearrRef, s32 numShape, HrAnimSet* asetRef, Gcol* gcolRef);
	virtual bool	copy(const ShapeModel* mdlSrc);
	virtual Model*	clone() const;
	virtual void	destroy();
	virtual void	exec(ExecRes* res, const ExecCtx* ec);

protected:
	virtual bool	testRenderingTarget(const RenderCtx* rc);
	virtual void	previseContents(const RenderCtx* rc);
	virtual void	renderContents(const RenderCtx* rc);

	//======================================================================
	// 変数
private:
	Shape**				_sarrShape;
	ShapeCtx**			_scarrShpCtx;
	s32					_numShape;
	HrAnimSet*			_asetRef;
	HrAnimSetCtx*		_ascAsetCtx;
	Gcol*				_gcolRef;
	Vector3F*			_vShapesBbCenter;
	f32					_rShapesBb;

	u8					_deftype;	// 定義タイプ
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SG_NODE_IMPLEMENT_SHAPE_MODEL_H_