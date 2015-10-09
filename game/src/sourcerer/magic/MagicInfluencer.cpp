/***********************************************************************//**
 *	MagicInfluencer.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/07.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MagicInfluencer.h"

// Friends
#include "MagicCluster.h"
#include "MagicSys.h"
#include "../body/Game.h"
#include "../body/GameNotifier.h"
#include "../body/Conductor.h"
#include "../common/CalcUtils.h"
#include "../effect/EleneEffect.h"
#include "../effect/StatusChangeEffect.h"
#include "../episode/party/MyParty.h"
#include "../episode/save/Trophy.h"
#include "../gui/GameGui.h"
#include "../item/ItemDef.h"
#include "../item/ItemDefTbl.h"
#include "../item/ItemManager.h"
#include "../map/Map.h"
#include "../particle/GameParticleDef.h"
#include "../sound/GameSoundDef.h"
#include "../status/CharStat.h"
#include "../status/FocusArray.h"
#include "../status/StatusIfEx.h"
#include "../status/TransStat.h"
#include "../unit/char/CharUnit.h"

// External
#include "../../etk/particle/Particle.h"
#include "../../etk/particle/ParticleSys.h"
#include "../../etk/sound/SoundMngr.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/lib/Vector.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	実体化する
**//*---------------------------------------------------------------------*/
bool MagicInfluencer::materialize(u32 form, u16 mcidCreation)
{
	switch(form)
	{
	case EleneStat::FF_FORM_SOIL:
		return materializeSolidForm(mcidCreation);
	case EleneStat::FF_FORM_WATER:
		return materializeWaterForm(mcidCreation);
	case EleneStat::FF_FORM_FIRE:
		return materializeFireForm(mcidCreation);
	case EleneStat::FF_FORM_WIND:
		return materializeWindForm(mcidCreation);
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	地型クラスター実体化 - 固体 - 相手に玉を投げつける
**//*---------------------------------------------------------------------*/
bool MagicInfluencer::materializeSolidForm(u16 mcidCreation)
{
	MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
	MagicCluster* mcCrt = mgcsys->getCluster(mcidCreation);
	ASSERT(mcCrt != 0L);
	if(mcCrt == 0L)	{	return false;	}

	// パラメータ分割
	s32 numTrg = mcCrt->getFocusArray()->getCount();
	TransStat tstatOne(*mcCrt->getTransStat());
	if(numTrg >= 2)
	{
		f32 rate = 1.0f / (f32)numTrg;
		scaleClusterParameter(&tstatOne, rate);
	}

	// クラスタの有効フレーム数算出
	s32 lifeframe = 2 * mcCrt->getTransStat()->getEleneStat()->getCql()->getWater() / Elene::ELENE_VALID_FIGURE;
	s32 numParticle = 1;
	// クラスタの加速度算出
	f32 speed = 5.0f + (mcCrt->getTransStat()->getEleneStat()->getCql()->getWind() * (0.1f / (f32)Elene::ELENE_VALID_FIGURE));
	if(speed < MagicCluster::SPEED_MC_MIN)		{ speed = MagicCluster::SPEED_MC_MIN; }
	else if(speed > MagicCluster::SPEED_MC_MAX)	{ speed = MagicCluster::SPEED_MC_MAX; }
	//TRACE("{MagicInfluencer::materializeSolidForm} speed=%f\n", speed);

	// 創作中クラスターを消す
	mcCrt->setLifeFrame(8.0f);

	// 発射処理
	const Vector3F* dirOwner = mcCrt->getOwnerUnit()->getDirection();
	if(numTrg <= 0)	// ターゲットなし
	{
		// クラスタ発射
		return materializeSolidForm_serveMc(mgcsys, mcCrt, 0L, &tstatOne, lifeframe, numParticle, speed);
	}
	else	// ターゲットあり
	{
		for(int i = 0; i < numTrg; i++)
		{
			Unit* unitTrg = mcCrt->focusArray()->unit(i);
			if((unitTrg == 0L) || !unitTrg->isEnable())	{	continue;	}

			// クラスタ発射
			if(!materializeSolidForm_serveMc(mgcsys, mcCrt, unitTrg, &tstatOne, lifeframe, numParticle, speed))
			{
				return false;
			}
		}

		return true;
	}
}

/*---------------------------------------------------------------------*//**
	地型クラスター実体化 - 固体 - マジッククラスターの発射
**//*---------------------------------------------------------------------*/
bool MagicInfluencer::materializeSolidForm_serveMc(MagicSys* mgcsys, MagicCluster* mcCreation, Unit* unitTrg, TransStat* tstat, s32 lifeframe, s32 numParticle, f32 speed)
{
	// クラスター作成
	MagicClusterCreateParam mccprm(MagicClusterType::SERVED, GameParticleDef::PTCLK_MAGIC_SERVE);
	mccprm._unitOwnrRef = mcCreation->getOwnerUnit();
	mccprm._pos = mcCreation->getPosition();
	mccprm._unitMainTrgRef = unitTrg;
	mccprm._tstat.copy(tstat);
	mccprm._lifeframe = lifeframe;
	mccprm._numParticle = numParticle;
	u16 mcidNew = mgcsys->appearCluster(&mccprm);
	if(mcidNew == 0)		{ return false; }
	MagicCluster* mcNew = mgcsys->getCluster(mcidNew);
	if(mcNew == 0L)			{ return false; }

	// 発射方向
	const Vector3F* dirOwner = mcCreation->getOwnerUnit()->getDirection();
	Vector3F dir(0.0f, 0.0f, 1.0f);
	Matrix4F mtxRot;
	mtxRot.rotateY(dirOwner->y());
	mtxRot.mult(&dir, &dir);
	if(unitTrg != 0L)
	{
		Vector3F tdir = *unitTrg->getCenterPos() - *mcCreation->getPosition();
		tdir.normalize();
		// 対象方向ベクトルと術者の向きベクトルの角度が±45°以下の場合は、対象方向ベクトルを採用する
		if(dir.dot(&tdir) >= 0.7071f)	// cos(45°) = 0.7071
		{
			dir.copy(&tdir);
		}
		else
		{
			dir.y() = tdir.y();	// y 方向は無条件に対象方向をコピーする
		}
	}

	// クラスタに速度と加速度を与える
	Vector3F velMc(&dir);
	velMc *= speed;
	mcNew->setInitialVelocity(&velMc);
	mcNew->setMaxSpeed(speed);
	Vector3F accMc(&dir);
	accMc *= speed * 0.1f;
	mcNew->accelerate(&accMc);

	return true;
}

/*---------------------------------------------------------------------*//**
	水型クラスター実体化 - 液体 - 相手に玉を放射する
**//*---------------------------------------------------------------------*/
bool MagicInfluencer::materializeWaterForm(u16 mcidCreation)
{
	MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
	MagicCluster* mcCrt = mgcsys->getCluster(mcidCreation);
	ASSERT(mcCrt != 0L);
	if(mcCrt == 0L)	{	return false;	}

	// 創作中クラスターを徐々に消す
	mcCrt->setLifeFrame(30);

	// 空元素による有効距離（の二乗）
	//	パラメータ分割の前に計算するのは、
	//	空元素の総量は分割後も本質的には減らない、という理屈から
	f32 lenEffctv = mcCrt->getTransStat()->getEleneStat()->getCql()->getEffectiveRange();
	f32 lenEffctvSq = lenEffctv * lenEffctv;

	// 前方±45°の対象を検索する
	List<Unit*> listTrgUnits;
	const Vector3F* posOwner = mcCrt->getOwnerUnit()->getPosition();
	const Vector3F* dirOwner = mcCrt->getOwnerUnit()->getDirection();
	CalcUtils::collectUnitFanRange(
		&listTrgUnits,
		posOwner,
		dirOwner->y(),
		Unit::UNITFLAG_CHAR,
		lenEffctvSq,
		TFW_COS45	);	// ±45°以内
	// フォーカスしていても、±45°の対象に入ってないと除外される
	{
		ListIterator<Unit*> itCur = listTrgUnits.iterator();
		while(itCur.has())
		{
			ListIterator<Unit*> itNext = itCur;
			itNext.next();

			bool isFind = false;
			for(int i = 0; i < mcCrt->getFocusArray()->getCount(); i++)
			{
				const Unit* unitWk = mcCrt->getFocusArray()->getUnit(i);
				if(unitWk == 0L)
				{
					continue;
				}
				if(unitWk == itCur.object())
				{
					isFind = true;	// 見つかった
					break;
				}
			}
			if(!isFind)
			{
				listTrgUnits.removeNode(itCur.node());	// 除外
			}

			itCur = itNext;
		}
	}
	s32 numTrg = listTrgUnits.getCount();

	// パラメータ決定
	TransStat tstatOne(*mcCrt->getTransStat());
	#if 0
		tstatOne.scale(0.5f);	// 固定で 50% に減らされる
	#else
		if(numTrg >= 1)
		{
			f32 rate = 1.0f / (f32)numTrg;
			rate = 0.25f + (rate * 0.5f);
			scaleClusterParameter(&tstatOne, rate);
		}
		else
		{
			tstatOne.scale(0.75f);	// 固定で 75%
		}
	#endif

	// クラスタの有効フレーム数算出
	s32 lifeframe = (mcCrt->getTransStat()->getEleneStat()->getCql()->getWater() / Elene::ELENE_VALID_FIGURE) >> 4;
	s32 numParticle = lifeframe / 64;
	// クラスタの加速度算出
	f32 speed = mcCrt->getTransStat()->getEleneStat()->getCql()->getWind() * (0.2f / (f32)Elene::ELENE_VALID_FIGURE);
	//f32 rvel = acc * 0.1f;

	// 発射処理
	if(numTrg <= 0)	// ターゲットなし
	{
		/*
		// クラスター作成
		MagicClusterCreateParam mccprm(MagicClusterType::EMITTED, GameParticleDef::PTCLK_MAGIC_MAKING_1);
		mccprm._unitOwnrRef = mcCrt->getOwnerUnit();
		mccprm._pos = mcCrt->getPosition();
		mccprm._tstat.copy(&tstatOne);
		mccprm._lifeframe = lifeframe;
		mccprm._radiusVel = rvel;
		mccprm._numParticle = numParticle;
		mccprm._isNoOnlyTrgColl = true;
		u16 mcidNew = mgcsys->appearCluster(&mccprm);
		if(mcidNew == 0)		{	return false;	}
		MagicCluster* mcNew = mgcsys->getCluster(mcidNew);
		if(mcNew == 0L)			{	return false;	}

		// クラスタ発射
		Vector3F accMc(0.0, 0.0, acc);
		Matrix4F mtxRot;
		mtxRot.rotateY(dirOwner->y());
		mtxRot.mult(&accMc, &accMc);
		mcNew->accelerate(&accMc);

		return true;
		*/

		// クラスタ発射
		return materializeWaterForm_serveMc(mgcsys, mcCrt, 0L, &tstatOne, lifeframe, numParticle, speed);
	}
	else	// ターゲットあり
	{
		for(int i = 0; i < numTrg; i++)
		{
			Unit* unitTrg = mcCrt->focusArray()->unit(i);
			if((unitTrg == 0L) || !unitTrg->isEnable())	{	continue;	}

			/*
			// クラスター作成
			MagicClusterCreateParam mccprm(MagicClusterType::EMITTED, GameParticleDef::PTCLK_MAGIC_MAKING_1);
			mccprm._unitOwnrRef = mcCrt->getOwnerUnit();
			mccprm._pos = mcCrt->getPosition();
			mccprm._unitMainTrgRef = unitTrg;
			mccprm._tstat.copy(&tstatOne);
			mccprm._radiusVel = rvel;
			mccprm._lifeframe = lifeframe;
			mccprm._numParticle = numParticle;
			u16 mcidNew = mgcsys->appearCluster(&mccprm);
			if(mcidNew == 0)		{	return false;	}
			MagicCluster* mcNew = mgcsys->getCluster(mcidNew);
			if(mcNew == 0L)			{	return false;	}

			// クラスタ発射
			Vector3F accMc(0.0, 0.0, acc);
			Matrix4F mtxRot;
			mtxRot.rotateY(dirOwner->y());
			mtxRot.mult(&accMc, &accMc);
			mcNew->accelerate(&accMc);
			*/

			// クラスタ発射
			if(!materializeWaterForm_serveMc(mgcsys, mcCrt, unitTrg, &tstatOne, lifeframe, numParticle, speed))
			{
				return false;
			}
		}

		return true;
	}
}

/*---------------------------------------------------------------------*//**
	水型クラスター実体化 - 固体 - マジッククラスターの発射
**//*---------------------------------------------------------------------*/
bool MagicInfluencer::materializeWaterForm_serveMc(MagicSys* mgcsys, MagicCluster* mcCreation, Unit* unitTrg, TransStat* tstat, s32 lifeframe, s32 numParticle, f32 speed)
{
	// クラスター作成
	MagicClusterCreateParam mccprm(MagicClusterType::EMITTED, GameParticleDef::PTCLK_MAGIC_MAKING_1);
	mccprm._unitOwnrRef = mcCreation->getOwnerUnit();
	mccprm._pos = mcCreation->getPosition();
	mccprm._unitMainTrgRef = unitTrg;
	mccprm._tstat.copy(tstat);
	mccprm._radiusVel = speed * 0.1f;
	mccprm._lifeframe = lifeframe;
	mccprm._numParticle = numParticle;
	u16 mcidNew = mgcsys->appearCluster(&mccprm);
	if(mcidNew == 0)		{ return false; }
	MagicCluster* mcNew = mgcsys->getCluster(mcidNew);
	if(mcNew == 0L)			{ return false; }

	// クラスタ発射
	const Vector3F* dirOwner = mcCreation->getOwnerUnit()->getDirection();
	Vector3F accMc(0.0, 0.0, speed);
	Matrix4F mtxRot;
	mtxRot.rotateY(dirOwner->y());
	mtxRot.mult(&accMc, &accMc);
	mcNew->accelerate(&accMc);

	return true;
}

/*---------------------------------------------------------------------*//**
	火型クラスター実体化 - 範囲内で爆発・発散させる
**//*---------------------------------------------------------------------*/
bool MagicInfluencer::materializeFireForm(u16 mcidCreation)
{
	MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
	MagicCluster* mcCrt = mgcsys->getCluster(mcidCreation);
	ASSERT(mcCrt != 0L);
	if(mcCrt == 0L)	{	return false;	}

	// 創作中クラスターを徐々に消す
	mcCrt->setLifeFrame(30);

	// 空元素による有効距離（の二乗）
	//	パラメータ分割の前に計算するのは、
	//	空元素の総量は分割後も本質的には減らない、という理屈から
	f32 lenEffctv = mcCrt->getTransStat()->getEleneStat()->getCql()->getEffectiveRange();
	f32 lenEffctvSq = lenEffctv * lenEffctv;

	// パラメータ決定
	TransStat tstatOne(*mcCrt->getTransStat());
	tstatOne.scale(0.33f);	// 固定で 33% に減らされる

	// クラスタの有効フレーム数算出
	s32 lifeframe = (mcCrt->getTransStat()->getEleneStat()->getCql()->getWater() / Elene::ELENE_VALID_FIGURE) >> 4;

	// 中心を求める
	Vector3F posCenter(*mcCrt->getPosition());

	// クラスター作成
	MagicClusterCreateParam mccprm(MagicClusterType::EXPLOSION, GameParticleDef::PTCLK_SHOCK_WAVE);
	mccprm._unitOwnrRef = mcCrt->getOwnerUnit();
	mccprm._pos = &posCenter;
	mccprm._radius = lenEffctv;
	mccprm._tstat.copy(&tstatOne);
	mccprm._lifeframe = lifeframe;
	mccprm._isNewPosVel = true;
	u16 mcidNew = mgcsys->appearCluster(&mccprm);
	if(mcidNew == 0)		{	return false;	}
	MagicCluster* mcNew = mgcsys->getCluster(mcidNew);
	if(mcNew == 0L)			{	return false;	}

	// SE 再生
	u32 ffop = tstatOne.getFuncFlags() & EleneStat::FF_MASK_OP;
	u16 seid = (EleneStat::FF_OP_ATTACK <= ffop) && (ffop <= EleneStat::FF_OP_UNS_ATTACK) ? GameSoundDef::SE_EXPLOSION_2 : GameSoundDef::SE_EXPLOSION_1;
	Game::getGame()->getSoundManager()->playSe(seid, false, 1.0f, &posCenter, 0L);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	風型クラスター実体化 - 範囲内に直接実体化させる
**//*---------------------------------------------------------------------*/
bool MagicInfluencer::materializeWindForm(u16 mcidCreation)
{
	MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
	MagicCluster* mcCrt = mgcsys->getCluster(mcidCreation);
	ASSERT(mcCrt != 0L);
	if(mcCrt == 0L)	{	return false;	}
	s32 numTrg = mcCrt->getFocusArray()->getCount();

	// 創作中クラスターを徐々に消す
	mcCrt->setLifeFrame(30);

	// 空元素による有効距離（の二乗）
	//	パラメータ分割の前に計算するのは、
	//	空元素の総量は分割後も本質的には減らない、という理屈から
	f32 lenEffctv = mcCrt->getTransStat()->getEleneStat()->getCql()->getEffectiveRange();
	f32 lenEffctvSq = lenEffctv * lenEffctv;

	// パラメータ決定
	TransStat tstatOne(*mcCrt->getTransStat());
	tstatOne.scale(0.25f);	// 固定で 25% に減らされる

	// 対象毎に処理
	for(int i = 0; i < numTrg; i++)
	{
		Unit* unitTrg = mcCrt->focusArray()->unit(i);
		if((unitTrg == 0L) || !unitTrg->isEnable())	{	continue;	}

		// 対象が空元素の範囲内か判定
		Vector3F diff = *unitTrg->getPosition() - *mcCrt->getPosition();
		f32 lenDiffSq = diff.lengthSq();
		if(lenDiffSq > lenEffctvSq)
		{
			continue;
		}

		// 発動
		if(unitTrg->isCategory(Unit::UNITCAT_CHAR))
		{
#if 1
			StatusIfEx::influence(((CharUnit*)unitTrg)->charStat(), mcCrt->getTransStat(), true, true);
#else
			StatusChangeResult scres(unitTrg, mcCrt);
			StatusInfluencer::influence(&scres, ((CharUnit*)unitTrg)->charStat(), mcCrt->getTransStat());
			StatusChangeEffect::on(&scres);
#endif
		}
		EleneEffect::on(mcCrt->getTransStat(), unitTrg->getCenterPos());
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	パラメータを分割する
**//*---------------------------------------------------------------------*/
void MagicInfluencer::scaleClusterParameter(TransStat* tstat, f32 rate)
{
	tstat->scale(rate);
}

/*---------------------------------------------------------------------*//**
	汎用クラスタを発射する
**//*---------------------------------------------------------------------*/
bool MagicInfluencer::serveGeneralMc(Unit* unitOwner, Unit* unitTarget, TransStat* tstat, int ptclkind)
{
	// クラスタ作成
	MagicSys* mgcsys = Game::getGame()->getMagicSys(); ASSERT(mgcsys != 0L);
	MagicClusterCreateParam mccprm(MagicClusterType::SERVED, (GameParticleDef::ParticleKind)ptclkind);
	Vector3F pos(*unitOwner->getCenterPos());
	mccprm._unitOwnrRef = unitOwner;
	mccprm._unitMainTrgRef = unitTarget;
	mccprm._pos = &pos;
	mccprm._tstat.copy(tstat);
	mccprm._lifeframe = mccprm._tstat.getEleneStat()->getCql()->getWater() / Elene::ELENE_VALID_FIGURE;
	mccprm._isNoOnlyTrgColl = true;
	u16 mcidNew = mgcsys->appearCluster(&mccprm);
	if(mcidNew == 0)		{ return false; }
	MagicCluster* mcNew = mgcsys->getCluster(mcidNew);
	if(mcNew == 0L)			{ return false; }	// 成功ではないが終えるために次のステップへ
	TFW_SET_FLAG(*mcNew->ctrlFlags(), MagicCluster::CTRLF_DIR_TO_ROTZ, true);	// 方向を Z 軸回転に反映させる

	// 発射方向
	Vector3F dir(0.0f, 0.0f, 1.0f);
	Matrix4F mtxRot;
	mtxRot.rotateY(unitOwner->getDirection()->y());
	mtxRot.mult(&dir, &dir);

	// 発射初期位置を再設定（ユニットの半径とクラスタの半径分離す）
	f32 d = unitOwner->getRadius() + mcNew->getRadius();
	pos._v[0] += dir._v[0] * d;
	pos._v[1] += dir._v[1] * d;
	pos._v[2] += dir._v[2] * d;
	mcNew->setPosition(&pos);

	// 加速
	f32 speed = mccprm._tstat.getEleneStat()->getCql()->getWind() * (0.1f / (f32)Elene::ELENE_VALID_FIGURE);
	if(speed < 0.033f)	{ speed = 0.033f; }
	Vector3F velMc(dir);
	velMc *= speed;
	mcNew->setInitialVelocity(&velMc);
	mcNew->reflectPosture();

	return true;
}

/*---------------------------------------------------------------------*//**
	合成する
**//*---------------------------------------------------------------------*/
bool MagicInfluencer::synthesize(u16 mcidBase, u16 mcidMult, s32 numUnit)
{
	MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
	MagicCluster* mcBase = mgcsys->getCluster(mcidBase);
	MagicCluster* mcMult = mgcsys->getCluster(mcidMult);

	// 中央位置を求める
	Vector3F posCenter(*mcBase->getPosition());
	posCenter += *mcMult->getPosition();
	posCenter *= 0.5f;

	if(numUnit <= 0)
	{
		// 失敗エフェクト
		ParticleSys* ptclsys = Game::getGame()->getParticleSys(); ASSERT(ptclsys != 0L);
		ptclsys->appearParticle(GameParticleDef::PTCLK_SHOCK_WAVE, &posCenter, 0);
		return false;
	}
	f32 ivNum = 1.0f / (f32)numUnit;
	Elene eleneBase(*mcBase->getTransStat()->getEleneStat()->getVeil(EleneStat::VEIL_WP)->getElene());
	Elene eleneMult(*mcMult->getTransStat()->getEleneStat()->getVeil(EleneStat::VEIL_BR)->getElene());

	// ベースアイテムを種族特徴率で掛け合わせる
	{
		for(int i = 0; i < Elene::NUM_EL; i++)
		{
			// 特徴率と個数によるレート掛け
			f32 fr = eleneMult.get(i) * 0.01f;		// 種族特徴率を 0 ～ 100 のパーセンテージから 0.0f ～ 1.0f に変換
			f32 ef = eleneBase.get(i) * ivNum * fr;
			// 自然減衰分を繰り上げ処理をする
			ef /= Elene::ELENE_VALID_FIGURE;
			s32 ei = (s32)(ef + 0.99f) * Elene::ELENE_VALID_FIGURE;
			// 基本元素量に設定する
			eleneBase.set(i, ei);
		}
		TRACE("{MagicInfluencer::synthesize} synth=[%d,%d,%d,%d,%d]\n", eleneBase.getSoil(), eleneBase.getWater(), eleneBase.getFire(), eleneBase.getWind(), eleneBase.getAethel());
	}

	// 元素構成が近いアイテムを検索
	s32 indexItem = -1;
	ItemManager* itmmng = Game::getGame()->getMyParty()->getItemManager();
	const ItemDefTbl* itmdftbl = itmmng->getItemDefTableRef();
	for(int itry = 0; itry < 2; itry++)
	{
		s32 score, scoreAe, scoreWi, scoreFi, scoreWa, scoreSo;
		s32 scoreMin = S32_MAX;
		for(s32 i = itmdftbl->getMaxNum() - 1; i >= 1; i--)
		{
			const ItemDef* itmdf = itmdftbl->getDefFromIndex(i);	ASSERT(itmdf != 0L);
			if(itmdf->getItemDefId() == 0)	{	continue;	}
			const Elene* eleneItem = itmdf->getOqlElene();			ASSERT(eleneItem != 0L);

			// スコア計算（元素量が満たないものは除外）
			scoreAe = eleneBase.getAethel() - eleneItem->getAethel();
			if(scoreAe < 0)	{	continue;	}
			scoreWi = eleneBase.getWind() - eleneItem->getWind();
			if(scoreWi < 0)	{	continue;	}
			scoreFi = eleneBase.getFire() - eleneItem->getFire();
			if(scoreFi < 0)	{	continue;	}
			scoreWa = eleneBase.getWater() - eleneItem->getWater();
			if(scoreWa < 0)	{	continue;	}
			scoreSo = eleneBase.getSoil() - eleneItem->getSoil();
			if(scoreSo < 0)	{	continue;	}

			// 空元素量が極端に違う場合は除外
			f32 rae = eleneBase.getAethel() / (f32)eleneItem->getAethel();
			if((rae < 0.5f) || (rae >= 2.0))	{	continue;	}

			// 最小スコアのものを採用
			score = scoreAe + scoreWi + scoreFi + scoreWa + scoreSo;
			if(score >= scoreMin)	{	continue;	}
			scoreMin = score;
			indexItem = i;
		}

		// 該当が見つかった場合は終了
		if(indexItem != -1)
		{
			break;
		}

		// 該当がない場合は元に戻して再調査
		eleneBase.copy(mcBase->getTransStat()->getEleneStat()->getVeil(EleneStat::VEIL_BR)->getElene());
	}
	// 失敗処理
	if(indexItem == -1)
	{
		// 失敗エフェクト
		ParticleSys* ptclsys = Game::getGame()->getParticleSys(); ASSERT(ptclsys != 0L);
		ptclsys->appearParticle(GameParticleDef::PTCLK_SHOCK_WAVE, &posCenter, 0);
		return false;
	}
	const ItemDef* itmdf = itmdftbl->getDefFromIndex(indexItem);	ASSERT(itmdf != 0L);

	// アイテム元素状態作成
	EleneStat eestatItem(itmdf->getOqlElene());

	// ベール付加
	Elene eleneRest(*mcBase->getTransStat()->getEleneStat()->getCql());
	eleneRest.add(mcMult->getTransStat()->getEleneStat()->getCql());
	eleneRest.scale(ivNum);	// ベールも数分で割る
	///for(int i = 0; i < Elene::NUM_EL; i++)	{	eleneRest.sub(i, 100 * Elene::ELENE_VALID_FIGURE);	}	// 合成コスト
	if(eleneRest.sum() > 0)
	{
		EleneStat eestatRest(&eleneRest);
		u32 eeff = eestatRest.calcFuncFlags(false);
		switch(eeff & EleneStat::FF_MASK_OP)
		{
		case EleneStat::FF_OP_WRAP:
		case EleneStat::FF_OP_SUFFUSE:
			// ベール付加最大値クリップ
			Elene eleneVeilLimit(*eestatItem.getNql());
			eleneVeilLimit.scale(EleneStat::RATIO_WP_VEIL_MAX);
			for(int i = 0; i < Elene::NUM_EL; i++)
			{
				if(eleneRest.get(i) > eleneVeilLimit.get(i))
				{
					eleneRest.set(i, eleneVeilLimit.get(i));
				}
			}
			eestatItem.veil(EleneStat::VEIL_WP)->strengthen(&eleneRest);
			break;
		}
	}

	// ドロップアイテム配置処理
	Map* map = Game::getGame()->getMap();	ASSERT(map != 0L);
	ParticleSys* ptclsys = Game::getGame()->getParticleSys(); ASSERT(ptclsys != 0L);
	Vector3F pos;
	for(int i = 0; i < numUnit; i ++)
	{
		// 1m 以内のランダムな位置を得る
		map->calcRandomMovablePosition(&pos, &posCenter, 50.0f);
		// ドロップアイテム配置
		itmmng->dropItem(itmdf->getItemDefId(), map, &pos, &eestatItem);

		// 成功エフェクト
		pos.y() += 50.0f;
		u16 ptclid = ptclsys->appearParticle(GameParticleDef::PTCLK_ITEM_SYNTHESIZE, &pos, 0);
		Particle* ptcl = (ptclid != 0) ? ptclsys->getParticle(ptclid) : 0L;
		if(ptcl != 0L)
		{
			ptcl->setScale(2.0f);		// スケール設定
		}
	}

	// 成功 SE 再生
	Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_IRON_GUARD_2, false, 1.0f, &posCenter, 0L);

	// アイテム合成成功トロフィー
	#if defined(_ENLIGHT_V01) && ENABLE_PSNS
		Game::getGame()->getTrophy()->achievementTrophy(Trophy::TI_ITEM_ADEPT_1_SUCCEEDED_SYNTHESIS, 100);
	#endif
	return true;
}

#if 0
/*---------------------------------------------------------------------*//**
	キャラクタステータス返答値のメッセージを処理する
**//*---------------------------------------------------------------------*/
void MagicInfluencer::procResult(const StatusChangeResult* scres)
{
	Conductor* conductor = Game::getGame()->getConductor(); ASSERT(conductor != 0L);
	conductor->on(scres, 0);
}
#endif

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
