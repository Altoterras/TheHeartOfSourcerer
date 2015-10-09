/***********************************************************************//**
 *  HrAnimObjPosture.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2012/11/16.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_HR_ANIM_OBJ_POSTURE_H_
#define _TFW_G3D_HR_ANIM_OBJ_POSTURE_H_

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

/*---------------------------------------------------------------------*//**
 *	階層アニメ オブジェクト姿勢
 *
**//*---------------------------------------------------------------------*/
class HrAnimObjPosture
{
	//======================================================================
	// 定数
public:
	static const u8		F_HAS_SCALE			= 0x01;		// スケールを持つ
	static const u8		F_HAS_QUATERNION	= 0x02;		// 回転をクォータニオンで持つ

	#define PRE_CALC_QUATERNION				(0)			// クォータニオンを設定時に計算する
	#define VALIDATION_SCALE_DEFAULT		(1)			// スケールをデフォルトで有効化

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 平行移動ベクトルを得る
	inline const Vector3F* getTranslate() const { return (Vector3F*)(_trs); }
	inline Vector3F* translate() { return (Vector3F*)(_trs); }
	// 角度ベクトルを得る
	inline const Vector3F* getAngle() const { return TFW_IS_FLAG(_flags, F_HAS_QUATERNION) ? 0L : (Vector3F*)(_rot); }
	inline Vector3F* angle() { return (Vector3F*)(_rot); }
	// クォータニオンを得る
	inline const Vector4F* getQuaternion() const { return TFW_IS_FLAG(_flags, F_HAS_QUATERNION) ? (Vector4F*)(_rot) : 0L; }
	inline Vector4F* quaternion() { return (Vector4F*)(_rot); }
	// スケールベクトルを得る
	inline const Vector3F* getScale() const { return TFW_IS_FLAG(_flags, F_HAS_SCALE) ? (Vector3F*)(_scl) : 0L; }
	inline Vector3F* scale() { return (Vector3F*)(_scl); }
	// クォータニオンを持つかを得る
	inline bool hasQuaternion() const { return TFW_IS_FLAG(_flags, F_HAS_QUATERNION); }
	// スケールを持つかを得る
	inline bool hasScale() const { return TFW_IS_FLAG(_flags, F_HAS_SCALE); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス

	void calcQuaternion(Vector4F* qtrn) const;

	void setTranslate(const Vector3F* trans);
	void setAngle(const Vector3F* angle);
	void setQuaternion(const Vector4F* qtrn);
	void setScale(const Vector3F* scale);

	void set(const Vector3F* trans, const Vector3F* angle, const Vector3F* scale);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HrAnimObjPosture();
	HrAnimObjPosture(const Vector3F* trans, const Vector3F* angle, const Vector4F* quaternion, const Vector3F* scale, bool isQuaternion);
	void copy(const HrAnimObjPosture* src);

	//======================================================================
	// 変数
private:
	f32 _trs[3];
	f32 _rot[4];
	f32 _scl[3];
	u8 _flags;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_HR_ANIM_OBJ_POSTURE_H_