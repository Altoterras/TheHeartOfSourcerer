/***********************************************************************//**
 *	GaugeAnimEffect.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/04/14.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EFFECT_GAUGE_ANIM_EFFECT_H_
#define _SRCR_EFFECT_GAUGE_ANIM_EFFECT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class ExecCtx;
class RenderCtx;

TFW_END_NS

SRCR_BEGIN_NS

class StatusChangeResult;
class CharUnit;

/*---------------------------------------------------------------------*//**
 *	ゲージ アニメーション エフェクト
 *
**//*---------------------------------------------------------------------*/
class GaugeAnimEffect : public FrameExecDraw
{
	//======================================================================
	// 定数
private:
	static const s32 NUM_EFSLOTS = 2;
	static const u8	F_ENABLE_MAKING = 0x01;

	//======================================================================
	// クラス
private:
	class Effect
	{
	public:
		u8 _kind;
		RectF32* _rect;
		const CharUnit* _unitTrg;
		f32 _wPreset;
		f32 _fcntAnim;

		static const u8	KIND_NULL = 0;
		static const u8	KIND_MAKING = 1;
#if 0	// StatGauge に任せるように変更した	【2015/02/24 r-kishi】
		static const u8	KIND_DAMAGE = 2;
#endif

		Effect();
		~Effect();
		void reset();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void startSoumaMaking(const CharUnit* unitTrg);
	void updateSoumaMaking(const CharUnit* unitTrg, f32 x, f32 y, s32 cur, s32 max, s32 preset);
	void endSoumaMaking(const CharUnit* unitTrg);
	void onDamage(const CharUnit* unitTrg, s32 cur, s32 max);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	GaugeAnimEffect();
	~GaugeAnimEffect();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);
	void draw(const RenderCtx* rc);

private:
#if 0	// StatGauge に任せるように変更した	【2015/02/24 r-kishi】
	void updateChangeGaugePos(Effect* ef);
#endif
	Effect* getEmptySlot();
	Effect* getTargetSlot(const CharUnit* unitTrg, u8 step);

	//======================================================================
	// 変数
private:
	Effect* _slotsEf;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EFFECT_GAUGE_ANIM_EFFECT_H_
