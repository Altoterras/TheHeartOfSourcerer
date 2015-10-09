/***********************************************************************//**
 *  BillboardModel.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/19.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_NODE_IMPLEMENT_BILLBOARD_MODEL_H_
#define _TFW_G3D_SG_NODE_IMPLEMENT_BILLBOARD_MODEL_H_

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
class Gcol;
class RenderCtx;

/*---------------------------------------------------------------------*//**
 *	3D ビルボード モデル
 *
**//*---------------------------------------------------------------------*/
class BillboardModel : public Model
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	Shape*			getShape() const			{	return _shapeRef;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool			isSameShape(const BillboardModel* mdlTrg) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	BillboardModel();
	virtual ~BillboardModel();

	bool			create(Shape* shapeRef);
	virtual void	destroy();
	virtual bool	copy(const BillboardModel* mdlSrc);
	virtual Model*	clone() const;
	virtual void	exec(ExecRes* res, const ExecCtx* ec);

protected:
	virtual bool	testRenderingTarget(const RenderCtx* rc);
	virtual void	previseContents(const RenderCtx* rc);
	virtual void	renderContents(const RenderCtx* rc);

	//======================================================================
	// 変数
private:
	Shape*			_shapeRef;
	ShapeCtx*		_shpctx;
	f32				_rShape;	// 半径（描画範囲判定用）
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_SG_NODE_IMPLEMENT_G3D_BILLBOARD_MODEL_H_