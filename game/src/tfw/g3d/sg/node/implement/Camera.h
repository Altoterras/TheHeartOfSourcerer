/***********************************************************************//**
 *  Camera.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/13.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_NODE_IMPLEMENT_CAMERA_H_
#define _TFW_G3D_SG_NODE_IMPLEMENT_CAMERA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../../Base.h"

#include "../SgEntity.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
template<class TYPE> class Vector4;
typedef Vector4<f32> Vector4F;
class Matrix4F;
class View;

/*---------------------------------------------------------------------*//**
	カメラクラス

**//*---------------------------------------------------------------------*/
class Camera : public SgEntity
{
	//======================================================================
	// 定数
public:
	// カメラクラス
	static const u16	CAMCLS_NULL			= 0;
	static const u16	CAMCLS_LOCANG		= 1;
	static const u16	CAMCLS_LOOKAT		= 2;
	static const u16	CAMCLS_ANIM			= 3;
	static const u16	CAMCLS__CUSTOM_		= 10000;

	// フラグ
	static const u16	F_NO_CALC_INVMTX	= 0x00000001;	// 逆行列を計算しない

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// カメラクラスを得る
	inline u16 getCameraClass() const					{	return _camclass;			}
	// 有効かどうか得る
	inline bool isVisible() const						{	return _cntVisible != 0;	}
	// フラグを得る
	inline u16 getFlags() const							{	return _flags;				}
	// 有効かどうかを設定する
	inline u32 getVisibleCounter() const				{	return _cntVisible;			}
	// ニアクリップ値を得る
	inline f32 getNear() const							{	return _cnear;				}
	// ファークリップ値を得る
	inline f32 getFar() const							{	return _cfar;				}
	// カメラ座標行列（ビュー行列）を得る
	inline const Matrix4F* getViewMatrix() const		{	return _mtxView;			}
	// カメラ座標逆行列（ビュー逆行列）を得る（無い場合は NULL を返す）
	inline const Matrix4F* getViewInverseMatrix() const	{	return _mtxViewInv;			}
	// ビルボード行列を得る
	inline const Matrix4F* getBillboardMatrix() const	{	return _mtxBillboard;		}
	// 左クリップ面を得る
	inline const Vector4F* getClipPlaneLeft() const		{	return _planeFovL;			}
	// 右クリップ面を得る
	inline const Vector4F* getClipPlaneRight() const	{	return _planeFovR;			}
	// 上クリップ面を得る
	inline const Vector4F* getClipPlaneTop() const		{	return _planeFovT;			}
	// 下クリップ面を得る
	inline const Vector4F* getClipPlaneBottom() const	{	return _planeFovB;			}
	// 位置を得る（取得できない場合や未対応の場合は NULL を返す）
	virtual const Vector3F* getLoc() const				{	return 0L;					}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setVisible(bool isVisible);
	virtual void resetVisibleCounter() { _cntVisible = 1; }
	virtual bool isClipWithWorldPos(const Vector3F* posWorld, f32 r);
	virtual bool isClipWithViewPos(const Vector3F* posView, f32 r);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Camera();
	virtual ~Camera();
	virtual bool		create(f32 fovy, f32 cnear, f32 cfar, u16 flags);
	virtual void		destroy();
	virtual void		exec(ExecRes* res, const ExecCtx* ec);

protected:
	virtual void		previseImplements(const RenderCtx* rc);
	virtual void		renderImplements(const RenderCtx* rc);
	virtual void		notifyVisibleChange(bool isVisibleNew) {}
	virtual void		calcViewMatrix() = 0;
	virtual void		calcViewInverseMatrix();
	virtual void		calcBillboardMatrix();
	virtual void		calcFovClipPlanes();

	//======================================================================
	// メンバ変数

protected:
	u16					_camclass;				// 任意で設定するカメラクラス（CAMCLS_*）
	u16					_flags;
	u32					_cntVisible;
	f32					_fovy;
	f32					_cnear;
	f32					_cfar;
	f32					_aspect;

	Matrix4F*			_mtxView;
	Matrix4F*			_mtxViewInv;
	Matrix4F*			_mtxBillboard;
	Vector4F*			_planeFovL;				// クリップ左平面
	Vector4F*			_planeFovR;				// クリップ右平面
	Vector4F*			_planeFovT;				// クリップ上平面
	Vector4F*			_planeFovB;				// クリップ下平面
};

TFW_END_NS
	
////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SGNODEIMP_CAMERA_H_
