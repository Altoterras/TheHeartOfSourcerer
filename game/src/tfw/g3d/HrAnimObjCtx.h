/***********************************************************************//**
 *  HrAnimObjCtx.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/12/29.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_HR_ANIM_OBJ_CTX_H_
#define _TFW_G3D_HR_ANIM_OBJ_CTX_H_

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
class Matrix4F;
class ShapeCtx;

/*---------------------------------------------------------------------*//**
 *	階層アニメ オブジェクト 可変値コンテキスト
 *
**//*---------------------------------------------------------------------*/
class HrAnimObjCtx
{
	//======================================================================
	// 定数
public:
	static const u8		F_HAS_SCALE			= 0x01;		// スケールを持つ
	static const u8		F_OFF				= 0x02;		// 表示を消す
	static const u8		F_SAVE_MV_MTX		= 0x04;		// モデルビュー行列を保存する
	static const u8		F_QUATERNION_ROT	= 0x08;		// 回転をクォータニオンで持つ

	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	HrAnimObjCtx 拡張情報
	 *
	**//*-----------------------------------------------------------------*/
	class HrAnimObjCtxEx
	{
	public:
		f32 _scl[3];			// スケール
		f32 _mdlview[16];		// モデルビュー行列

		HrAnimObjCtxEx();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// シェイプコンテキストを得る
	inline ShapeCtx* getShapeCtx() const { return _shpctxRef;	}
	// 平行移動ベクトルを得る
	inline const Vector3F* getTranslate() const { return (Vector3F*)(_trs); }
	// 角度ベクトルを得る
	inline const Vector3F* getAngle() const { return TFW_IS_FLAG(_flags, F_QUATERNION_ROT) ? 0L : (Vector3F*)(_rot); }
	// クォータニオンを得る
	inline const Vector4F* getQuaternion() const { return TFW_IS_FLAG(_flags, F_QUATERNION_ROT) ? (Vector4F*)(_rot) : 0L; }
	// スケールベクトルを得る
	inline const Vector3F* getScale() const { return (_ex == 0L) ? 0L : (Vector3F*)(_ex->_scl); }
	// フラグを得る
	inline u8 getFlags() const { return _flags; }
	// モデルビュー行列を得る
	inline const Matrix4F* getModelViewMatrix() const { return (_ex == 0L) ? 0 : (Matrix4F*)(_ex->_mdlview); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス

	void setFlags(u8 flags, bool isValid);
	void setTranslate(const Vector3F* trans);
	void setAngle(const Vector3F* angle);
	void setQuaternion(const Vector4F* qtrn);
	void setScale(const Vector3F* scale);
	void setModelViewMatrix(const Matrix4F* mtxModelView);

	void calcAndSaveModelViewMatrix(Matrix4F* mtxMv);
	void calcAndSaveAndSupplyModelViewMatrix(Matrix4F* mtxMv);
	void calcModelViewMatrixIgnoreSaveFlag(Matrix4F* mtxMv);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HrAnimObjCtx();
	~HrAnimObjCtx();
	bool create(ShapeCtx* shpctxRef);
	void destroy();

	//======================================================================
	// 変数
private:
	ShapeCtx* _shpctxRef;
	f32 _trs[3];			// 平行移動
	f32 _rot[4];			// オイラー角度 or クォータニオン
	HrAnimObjCtxEx* _ex;
	u8 _flags;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_HR_ANIM_OBJ_CTX_H_