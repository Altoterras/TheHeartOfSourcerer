/***********************************************************************//**
 *	EnemyUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EnemyUnit.h"

// Friends
#include "../../../char/CharLogicData.h"
#include "../../../common/CalcUtils.h"
#include "../../../confront/Confront.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../map/Map.h"
#include "../../../particle/GameParticleDef.h"
#include "../../../spirit/char/npc/EnemySpirit.h"
#include "../../../status/CharStat.h"
#include "../../../status/StatusChangeResult.h"

// External
#include "../../../../etk/particle/Particle.h"
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../etk/sg/TbfDefineEx.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/lib/Matrix.h"
#include "../../../../tfw/lib/Point.h"
#include "../../../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../../../tfw/g3d/sg/node/implement/ModelLoader.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define CNT_DETECT_INVALID			(-1)		// 対象検出無効値
#define CNT_DETECT_START			(15)		// 対象検出開始カウンタ値
#define CNT_DETECT_MOVE				(0)			// 検出時移動カウンタ値

#define FRAMECNT_CHAROVAVOID		(3 * 30)	// 3 秒
#define FRAMECNT_CHAROVAVOID_RPLUS	(3 * 30)	// 乱数追加値 3 秒

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EnemyUnit 静的メンバ変数

//==========================================================================
// EnemyUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	探知対象を設定する
**//*---------------------------------------------------------------------*/
void EnemyUnit::setDetectTarget(Unit* unitTrg)
{
	if(getThisSpirit()->getFocusUnit() == unitTrg)	{	return;	}	// 設定済み

	getThisSpirit()->setFocusUnit(unitTrg, false);	// 注視ユニットの設定

	_cntDetect = CNT_DETECT_START;	// 検出カウンタのセット
	
	// とりあえず静止しておくためにターゲット位置は現在地に設定
	setTargetPosition(getPosition());

	// エフェクト表示
	ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
	Vector3F pos(_pos->x(), _pos->y() + 100.0f, _pos->z());
	ptclsys->appearParticle(GameParticleDef::PTCLK_DETECT, &pos, 0);
}

/*---------------------------------------------------------------------*//**
	目標位置を計算する
**//*---------------------------------------------------------------------*/
void EnemyUnit::calcTargetPosition()
{
	// 目標位置設定
	const Vector3F* posThis = getPosition();
	const CharUnit* cunitTrg = (CharUnit*)getThisSpirit()->getFocusUnit();
	if(cunitTrg == 0L)
	{
		setTargetPosition(getPosition());	// 静止
		return;
	}

	const Vector3F* posTrg = cunitTrg->getPosition();
	Vector3F vTrgBase = *posThis - *posTrg;
	f32 len = vTrgBase.lengthSq();
	f32 angleBase = 0.0f;

#if 0
	if(len > (getThisSpirit()->getReachDistance() * getThisSpirit()->getReachDistance()))	// 十分離れている場合
	{
		// 対象からリーチ距離をおいた地点へ
		vTrgBase.normalize();
	}
	else
#endif
	{
		// 対象の正面からリーチ距離をおいた地点へ
		vTrgBase.set(0.0f, 0.0f, 1.0f);
		angleBase = cunitTrg->getDirection()->y();
	}
	len = getThisSpirit()->getReachDistance() * 0.9f;

	Confront* confront = Game::getGame()->getConfront();
	if(confront->isConfrontNow())	// 戦闘中
	{
		if(_fcntCharOverlapAvoid > 0)
		{
			_fcntCharOverlapAvoid--;	// カウンタ更新

			// 現在の角度のまま設定
			f32 angle = Gcalc::adjustAngle(angleBase + _angleCharOverlapAvoid);
			Vector3F vTrg;
			Matrix4F mtxRotY;
			mtxRotY.rotateY(angle);
			mtxRotY.mult(&vTrg, &vTrgBase);
			vTrg *= len;
			vTrg += *posTrg;
			setTargetPosition(&vTrg);
		}
		else
		{
			// 新たな角度を求めて設定
			Vector3F vTrg;
			f32 lenNear = getRadius() * 0.9f;
			f32 angleAvoid = _angleCharOverlapAvoid;
			bool isSetAvoid = false;
			for(int cntSafe = 0; cntSafe < 10; cntSafe++)
			{
				f32 angle = Gcalc::adjustAngle(angleBase + angleAvoid);
				Matrix4F mtxRotY;
				mtxRotY.rotateY(angle);
				mtxRotY.mult(&vTrg, &vTrgBase);
				vTrg *= len;
				vTrg += *posTrg;

				// 他の戦闘中キャラとの位置重なりチェック
				bool isRetry = false;
				for(int iunit = 0; iunit < confront->getTargets()->getCount(); iunit++)
				{
					const Unit* unitWk = confront->getTargets()->getUnit(iunit);
					if(unitWk == this)
					{
						continue;
					}
					if(TFW_IS_NEAR(unitWk->getPosition()->x(), vTrg.x(), lenNear) && TFW_IS_NEAR(unitWk->getPosition()->z(), vTrg.z(), lenNear))
					{
						// キャラ重なり回避角度の再設定
						angleAvoid = CalcUtils::randPlusMinusOneF() * TFW_HPI * 0.5f;	// ±45°で調整
						isSetAvoid = true;
						isRetry = true;
						break;	// 失格
					}
				}
				if(!isRetry)
				{
					break;
				}
			}

			_fcntCharOverlapAvoid = FRAMECNT_CHAROVAVOID + (s32)(FRAMECNT_CHAROVAVOID_RPLUS * CalcUtils::randF());
			if(isSetAvoid)
			{
				_angleCharOverlapAvoid = angleAvoid;	// 新しい角度を設定
			}
			else
			{
				_angleCharOverlapAvoid *= 0.5f;			// 徐々に角度を縮める
			}

			setTargetPosition(&vTrg);
		}
	}
	else	// 戦闘中以外
	{
		// 工夫せずそのまま設定
		Vector3F vTrg;
		if(angleBase != 0.0f)
		{
			Matrix4F mtxRotY;
			mtxRotY.rotateY(angleBase);
			mtxRotY.mult(&vTrg, &vTrgBase);
		}
		vTrg *= len;
		vTrg += *posTrg;
		setTargetPosition(&vTrg);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EnemyUnit::EnemyUnit()
	: NpcCharUnit(UNITTYPE_ENEMY_CHAR)
	, _cntDetect(CNT_DETECT_INVALID)
	, _fcntDown(0.0f)
	, _fcntCharOverlapAvoid(0)
	, _angleCharOverlapAvoid(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EnemyUnit::~EnemyUnit()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EnemyUnit::create(Spirit* sprtRef, u16 chrdid)
{
	// モデルを読み込み、リソースプールとシーングラフに追加
	u32 fileid = FILEID_FROM_CHAR_MODEL(chrdid);
	#if defined(_DEBUG)
		if(!IdFile::isIdFileExist(fileid))	{	fileid = 311031;	}	// 存在しない場合は適当なキャラをとりあえず表示
	#endif
	//SorSgm* sgm = Game::getGame()->getSceneGraph();
	ShapeModel* mdl = 0L;
	if(!ModelLoader::loadModelFromTbf(Game::getGame()->getTfw(), fileid, TbfDefineEx::MEXF_CHAR, ModelLoader::LFLAG_DISPOSE_SCENE_GRAPH, (Model**)&mdl))
	{
		return false;
	}

	_eflags |= F_IGNORE_KEEPOUT;	// 進入禁止を無視する

	// 親クラスによる作成
	if(!NpcCharUnit::create(mdl, sprtRef, chrdid, UNITFLAG_DEPEND_MAP | UNITFLAG_CREATION_FOCUSABLE, CHARCLS_NULL))	// フォーカス可能に設定する
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void EnemyUnit::exec(ExecRes* res, const ExecCtx* ec)
{
	// ダウンカウント増算
	if(_cstat->getStat() == CharStat::STAT_DOWN)
	{
		_fcntDown += ec->getDeltaFrame();
	}

	if(isEnable())
	{
		// 対象検出
		if(_cntDetect >= 0)
		{
			if(_cntDetect == CNT_DETECT_MOVE)
			{
				if(getThisSpirit()->getFocusUnit() != 0L)
				{
					calcTargetPosition();
				}
			}

			if(_cntDetect > 0)	{ _cntDetect--; }
		}
	}

	// ダウンカウント

	// キャラユニットとしての移動処理など
	NpcCharUnit::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	歩行フレーム処理
**//*---------------------------------------------------------------------*/
void EnemyUnit::execWalking(const ExecCtx* ec)
{
	// +++ Adjustable Parameter >>>
	#define DIST_TO_ACC			(0.1f)//(0.011f)					// 距離から加速度への変換率
	#define ACC_MIN				(15.0f/*3.50f*/ / (f32)FRAMERATE)	// 最低限の加速度
	#define ACC_MIN_SQ			(ACC_MIN * ACC_MIN)
	#define ACC_MAX				(250.0f / (f32)FRAMERATE)			// 最大移動加速度
	#define ACC_MAX_SQ			(ACC_MAX * ACC_MAX)
	#define DIST_RUN			(300.0f)							// 走り始める目標位置との距離境界値
	#define DIST_RUN_SQ			(DIST_RUN * DIST_RUN)
	#define DIST_TRG_MIN_SQ		(10.0f)								// 目標位置差の合格値
	// +++ Adjustable Parameter <<<

	// 歩行
	Vector3F accWk1 = *_posTarget - *getPosition();
	f32 len2ToTrg = accWk1.lengthSq();
	if(len2ToTrg > DIST_TRG_MIN_SQ)	// 距離が離れている
	{
		// 目標に距離が近い場合は速度を緩める
		accWk1 *= DIST_TO_ACC;
		if(len2ToTrg < DIST_RUN_SQ)	// デフォルトの距離
		{
			accWk1 *= len2ToTrg / DIST_RUN_SQ;	// 近いほどゆっくり
		}

		// 最低限の速度は維持する
		len2ToTrg = accWk1.lengthSq();
		if(len2ToTrg < ACC_MIN_SQ)
		{
			accWk1.normalize();
			accWk1 *= ACC_MIN;
		}
		else if(len2ToTrg > ACC_MAX_SQ)
		{
			accWk1.normalize();
			accWk1 *= ACC_MAX;
		}

		// 歩行処理
		Vector2F accXz(accWk1.x(), accWk1.z());	
		walk(&accXz, true);
	}

	NpcCharUnit::execWalking(ec);
}

/*---------------------------------------------------------------------*//**
	対決状態変更通知
**//*---------------------------------------------------------------------*/
void EnemyUnit::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition)
{
	getThisSpirit()->setFocusUnit(unitTrg, false);										// フォーカスユニットの設定
	if(unitTrg != 0L) { setTargetPosition(unitTrg->getPosition()); }					// 移動ターゲット位置を注視ユニットに
	_cntDetect = CNT_DETECT_INVALID;													// 検出カウンタを無効化
	_fcntCharOverlapAvoid = (s32)(FRAMECNT_CHAROVAVOID_RPLUS * CalcUtils::randF());		// キャラ重なり回避カウンタの初期化
	_angleCharOverlapAvoid = 0.0f;														// キャラ重なり回避角度の初期化

	NpcCharUnit::notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);
}

/*---------------------------------------------------------------------*//**
	ステータス影響発生通知
**//*---------------------------------------------------------------------*/
void EnemyUnit::notifyInfluenceStat(const StatusChangeResult* scres, const TransStat* tstat)
{
	if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_IN_DOWN))		// 活動停止した
	{
		const CharLogicData* cldat = _sprtSelfRef->getCharLogicData(); ASSERT(cldat != 0L); 
		u16 ecflags = (cldat != 0L) ? cldat->getParam(CharLogicData::PARAM_EC_FLAGS) : 0;
		if(!TFW_IS_FLAG(ecflags, CharLogicData::ECF_NO_DOWN_VANISH))	// 無効化しない場合以外
		{
			// エフェクト表示
			ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
			Vector3F pos(_pos->x(), _pos->y() + 100.0f, _pos->z());
			ptclsys->appearParticle(GameParticleDef::PTCLK_VANISH, &pos, 0);
		}
	}

	NpcCharUnit::notifyInfluenceStat(scres, tstat);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
