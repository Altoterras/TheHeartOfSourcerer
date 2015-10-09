/***********************************************************************//**
 *  HrAnimObjFrame.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/07/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_HR_ANIM_OBJ_FRAME_H_
#define _TFW_G3D_HR_ANIM_OBJ_FRAME_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "HrAnimObjPosture.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
template<class TYPE> class Vector4;
typedef Vector3<f32> Vector3F;
typedef Vector4<f32> Vector4F;
class HrAnimObj;
struct TbfI1HrAnimPosture;

/*---------------------------------------------------------------------*//**
 *	階層アニメ オブジェ キーフレーム クラス
 *
**//*---------------------------------------------------------------------*/
class HrAnimObjFrame
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// フレーム値を得る
	inline s32 getFrame() const { return _frame; }
	// 姿勢を得る
	inline const HrAnimObjPosture* getPos() const { return &_pos; }
	inline HrAnimObjPosture* pos() { return &_pos; }
	// 関連付けられているアニメオブジェを得る
	inline HrAnimObj* getObj() const { return _aobjTrgRef; }

	// フレーム値を設定する
	inline void setFrame(s32 frame) { _frame = frame; }
	// アニメオブジェを設定する
	inline void setObj(HrAnimObj* aobj) { _aobjTrgRef = aobj; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setTranslate(const Vector3F* trans);
	void setAngle(const Vector3F* angle);
	void setQuaternion(const Vector4F* qtrn);
	void setScale(const Vector3F* scale);

	void set(s32 frame, const Vector3F* trs, const Vector3F* angle, const Vector3F* scl, HrAnimObj* aobjTrgRef);
	void set(const TbfI1HrAnimPosture* tbfhapos, HrAnimObj* aobjTrgRef);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HrAnimObjFrame();
	void copy(const HrAnimObjFrame* src);

	//======================================================================
	// 変数
private:
	s32 _frame;
	HrAnimObjPosture _pos;
	HrAnimObj* _aobjTrgRef;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_HR_ANIM_OBJ_H_