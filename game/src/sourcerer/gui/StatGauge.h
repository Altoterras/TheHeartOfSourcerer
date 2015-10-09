/***********************************************************************//**
 *	StatGauge.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2015/02/16.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_STAT_GAUGE_H_
#define _SRCR_GUI_STAT_GAUGE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
class Texture;

TFW_END_NS

SRCR_BEGIN_NS

class CharStat;

/*---------------------------------------------------------------------*//**
 *	ステータスゲージ
 *
**//*---------------------------------------------------------------------*/
class StatGauge : public FrameExecDraw
{
	//======================================================================
	// 定数
private:
	enum GaugeAnimKind
	{
		GAK_NULL,
		GAK_LFE_DMG,
		GAK_LFE_RCV,
		GAK_CNS_DMG,
		GAK_CNS_RCV,
		GAK_CHG_DMG,
		GAK_CHG_RCV,
	};

	static const s32 NUM_SLOTS = 4;

	//======================================================================
	// クラス
private:
	class GaugeAnim
	{
	public:
		GaugeAnimKind _kind;
		RectF32* _rect;
		f32 _fcntAnim;

		GaugeAnim();
		~GaugeAnim();
		void reset();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	const CharStat* getAttachedCharStat() const { return _cstatRef; }
	const RectF32* getRectangle() const { return _rect; }
	RectF32* rectangle() { return _rect; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void attachCharStat(const CharStat* cstatRef);
	void onLifeChange(s32 cur, s32 prev);
	void onConcentrateChange(s32 cur, s32 prev);
	void onChargeChange(s32 cur, s32 prev);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StatGauge();
	virtual ~StatGauge();
	bool create(Texture* texGuiRef, const RectF32* rect);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void draw(const RenderCtx* rc);				// 描画

private:
	GaugeAnim* getEmptySlot();
	void resetAllGaugeAnim();

	//======================================================================
	// 変数
private:
	const CharStat*	_cstatRef;
	Texture* _texGuiRef;
	RectF32* _rect;
	GaugeAnim _slotGa[NUM_SLOTS];

	s32 _lvPrev;
	s32 _lfenePrev;
	s32 _depthConcPrev;
	u16 _cntChargePrev;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_STAT_GAUGE_H_
