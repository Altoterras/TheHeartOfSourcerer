/***********************************************************************//**
 *  Model.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/27.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_NODE_IMPLEMENT_MODEL_H_
#define _TFW_G3D_SG_NODE_IMPLEMENT_MODEL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../../Base.h"

#include "../SgEntity.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Matrix4F;
class Shape;
class Texture;
class Material;
class HrAnimSet;
class Gcol;
class RenderCtx;
class CamAnim;

/*---------------------------------------------------------------------*//**
 *	3D モデル
 *
**//*---------------------------------------------------------------------*/
class Model : public SgEntity
{
	//======================================================================
	// 定数
public:
	// モデルクラス
	static const u8		MODELCLASS_NULL			= 0;		// 未設定
	static const u8		MODELCLASS_SHAPE		= 1;		// シェイプ (ShapeModel)
	static const u8		MODELCLASS_BILLBOARD	= 2;		// ビルボード (BillboardModel)
	static const u8		MODELCLASS_USER			= 100;		// ユーザーカスタム

	// モデルフラグ
	static const u16	MFLAG_NO_CLIP			= 0x0001;	// クリップ処理をしない
	static const u16	MFLAG_NO_VIEWPOS		= 0x0002;	// _viewpos に対してビュー座標計算結果を保存しない（MFLAG_NO_CLIP との併用でビュー座標計算も抑制できる）
	static const u16	MFLAG_ANIM_VIEWPOS		= 0x0004;	// _viewpos 計算時にアニメーションによる位置移動も考慮する
	static const u16	MFLAG_NO_USE			= 0x0008;	// 使用しない

	// 描画フラグ（＜注＞値をそのまま使うので TbfI1ShapeHeader と同じ数値にすること）
	static const u8		ROPTFLAG_VTXCOL			= 0x01;		// 頂点カラー有り
	static const u8		ROPTFLAG_VTXNRM			= 0x02;		// 頂点法線有り
	static const u8		ROPTFLAG_SCLANIM		= 0x04;		// スケールアニメーション有り
	static const u8		ROPTFLAG_TRANS_NML		= 0x10;		// 通常半透明
	static const u8		ROPTFLAG_TRANS_ADD		= 0x20;		// 加算半透明
	static const u8		ROPTFLAG_TRANS_SUB		= 0x40;		// 減算半透明

	// 描画結果フラグ
	static const u8		RRESFLAG_CLIPPED		= 0x01;		// クリップされた

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline u8				getModelClass() const				{	return _modelclass;			}
	inline u16				getRenderOrder() const				{	return _rendorder;			}
	inline u16				getModelFlag() const				{	return _mflags;				}
	inline u16				getExFlags() const					{	return _exflags;			}
	inline u16				getSpid() const						{	return _spid;				}
	inline u8				getRenderResFlags() const			{	return _flagsRenderRes;		}
	inline const Vector3F*	getPosition() const					{	return _posPosture;			}
	inline const Vector3F*	getDirection() const				{	return _dirPosture;			}
	inline const Vector3F*	getRotate() const					{	return _rotPosture;			}
	inline const Vector3F*	getScale() const					{	return _sclPosture;			}
	inline CamAnim*			getCamAnim() const					{	return _camanimRef;			}
	inline const Vector3F*	getViewPosition() const				{	return _viewpos;			}
	inline const ColorU8*	getColor() const					{	return _color;				}

	inline void				setModelClass(u8 modelclass)		{	_modelclass = modelclass;				}
	inline void				setRenderOrder(u16 rendorder)		{	_rendorder = rendorder;					}
	inline void				setModelFlag(u16 f, bool is)		{	TFW_SET_FLAG(_mflags, f, is);			}
	inline void				setExFlag(u16 f, bool is)			{	TFW_SET_FLAG(_exflags, f, is);			}
	inline void				setSpid(u16 spid)					{	_spid = spid;							}
	inline void				setRenderOptFlag(u16 f, bool is)	{	TFW_SET_FLAG(_flagsRenderOpt, f, is);	}
	inline void				setCamAnim(CamAnim* camanimRef)		{	_camanimRef = camanimRef;				}

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void			setPosition(const Vector3F* translate, const Vector3F* direction);
	virtual void			setPosture(const Vector3F* translate, const Vector3F* rotate, const Vector3F* scale);
	virtual void			setScale(const Vector3F* scale);
	virtual void			setColor(const ColorU8*	color);
	inline void				calcPostureMatrixByManual() { calcPostureMatrix(); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Model();
	virtual ~Model();
	bool					create();
	virtual void			destroy();
	virtual void			copy(const Model* mdlSrc);
	virtual Model*			clone() const = 0;
	virtual void			exec(ExecRes* res, const ExecCtx* ec);

protected:
	virtual bool			testRenderingTarget(const RenderCtx* rc) = 0;
	virtual void			previseImplements(const RenderCtx* rc);
	virtual void			previseContents(const RenderCtx* rc) = 0;
	virtual void			renderImplements(const RenderCtx* rc);
	virtual void			renderContents(const RenderCtx* rc) = 0;
	
	void					calcPostureMatrix();

	//======================================================================
	// 変数
private:
	u8						__pad_0010[ 1 ];
	u8						_modelclass;		// モデルクラス（MODELCLASS_*）
	u16						_mflags;			// モデルフラグ（MFLAG_*）

protected:
	u16						_rendorder;			// 描画順
	u16						_exflags;			// 拡張フラグ
	u16						_spid;				// SPID
	u8						_flagsRenderOpt;	// 描画フラグ
	u8						_flagsRenderRes;	// 描画結果フラグ

	// 姿勢
	Vector3F*				_posPosture;		// 位置
	Vector3F*				_dirPosture;		// 方向
	Vector3F*				_rotPosture;		// 回転
	Vector3F*				_sclPosture;		// スケール
	Matrix4F*				_mtxPosture;		// 姿勢行列（上４ベクトルの計算結果）
	bool					_isChangePosture;	// 姿勢変更フラグ

	Vector3F*				_viewpos;			// ビュー座標（カメラ座標）における位置
	ColorU8*				_color;				// 色
	CamAnim*				_camanimRef;		// カメラアニメーションへの参照
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SGNODEIMP_MODEL_H_