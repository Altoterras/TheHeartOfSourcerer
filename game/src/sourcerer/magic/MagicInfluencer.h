/***********************************************************************//**
 *	MagicInfluencer.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/07.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAGIC_MAGIC_INFLUENCER_H_
#define _SRCR_MAGIC_MAGIC_INFLUENCER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/FrameExec.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

SRCR_BEGIN_NS

class MagicCluster;
class MagicSys;
class StatusChangeResult;
class TransStat;
class Unit;

/*---------------------------------------------------------------------*//**
 *	マジック クラスター影響処理
 *
**//*---------------------------------------------------------------------*/
class MagicInfluencer : public FrameExec
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool materialize(u32 form, u16 mcidCreation);
private:
	static bool materializeSolidForm(u16 mcidCreation);
	static bool materializeSolidForm_serveMc(MagicSys* mgcsys, MagicCluster* mcCreation, Unit* unitTrg, TransStat* tstat, s32 lifeframe, s32 numParticle, f32 speed);
	static bool materializeWaterForm(u16 mcidCreation);
	static bool materializeWaterForm_serveMc(MagicSys* mgcsys, MagicCluster* mcCreation, Unit* unitTrg, TransStat* tstat, s32 lifeframe, s32 numParticle, f32 speed);
	static bool materializeFireForm(u16 mcidCreation);
	static bool materializeWindForm(u16 mcidCreation);
	static void scaleClusterParameter(TransStat* tstat, f32 rate);

public:
	static bool serveGeneralMc(Unit* unitOwner, Unit* unitTarget, TransStat* tstat, int ptclkind);
	static bool synthesize(u16 mcidBase, u16 mcidMult, s32 numUnit);

#if 0
	static void procResult(const StatusChangeResult* scres);
#endif
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAGIC_MAGIC_INFLUENCER_H_
