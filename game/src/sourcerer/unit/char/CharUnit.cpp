/***********************************************************************//**
 *	CharUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CharUnit.h"

// Friends
#include "CharUnitSaveStructure.h"
#include "../../char/CharDefTbl.h"
#include "../../char/CharLogicData.h"
#include "../../effect/StatusChangeEffect.h"
#include "../../body/Game.h"
#include "../../body/GameExecCtx.h"
#include "../../body/GameRenderCtx.h"
#include "../../model/CircleShadow.h"
#include "../../map/Map.h"
#include "../../particle/GameParticleDef.h"
#include "../../spirit/Spirit.h"
#include "../../status/CharStat.h"
#include "../../episode/data/FixedData.h"

// External
#include "../../../etk/particle/ParticleSys.h"
#include "../../../etk/sg/SorSgm.h"
#include "../../../tfw/collision/Gcol.h"
#include "../../../tfw/common/ResourcePool.h"
#include "../../../tfw/lib/Matrix.h"
#include "../../../tfw/lib/Vector.h"
#include "../../../tfw/lib/Calc.h"
#include "../../../tfw/gcmn/Renderer.h"
#include "../../../tfw/gcmn/RendererUtils.h"
#include "../../../tfw/g3d/sg/node/implement/Model.h"
#include "../../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../../tfw/g3d/sg/node/SgNodeChain.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define VEL_MAX_DEFAULT			(460.0f / (f32)FRAMERATE)			// 標準の移動最大速さ
#define RATIO_RUN_DEFAULT		(1.0f)								// 標準の走り速さ率
#define RATIO_FRICT_ACC_DEFAULT	(0.5f)								// 標準の速さ摩擦率（加速フレーム時）
#define RATIO_FRICT_NRM_DEFAULT	(0.5f)								// 標準の速さ摩擦率（非フレーム時）
#define VEL_WALK_MIN			(6.7f / (f32)FRAMERATE)				// 最小歩き速さ
#define VEL_WALK_MIN_SQ			(VEL_WALK_MIN * VEL_WALK_MIN)
#define RATIO_TURN_RATE			(0.1f)
#define CNT_STOP_ANIM			(30)								// 停止アニメーション調整カウンタ

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CharUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	代表名を得る（無い場合は NULL が返る）
**//*---------------------------------------------------------------------*/
const VcString* CharUnit::getMainName() const
{
	if(_chrdefRef != 0L)
	{
		return _chrdefRef->getName(Env_getLangId());
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	現在のアニメーション ID を得る
**//*---------------------------------------------------------------------*/
u16 CharUnit::getCurrentAnimId() const
{
	return getShapeModel()->getCurAnimId();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	歩行
**//*---------------------------------------------------------------------*/
void CharUnit::walk(const Vector2F* accXz, bool isAllowRun)
{
	_vel->x() += accXz->x();
	_vel->z() += accXz->y();

	if(isAllowRun)	// 走りを許可する場合
	{
		if(_vel->lengthSq() > _velMaxSq)						// 最大速さを超えている場合
		{
			_vel->normalize();
			_vel->x() *= _velMax;
			_vel->z() *= _velMax;
		}
	}
	else			// 走りを許可しない場合
	{
		f32 velWalkMax = _velMax * _ratioRun;
		if(_vel->lengthSq() >= (velWalkMax * velWalkMax))	// 走り速さを超えている場合
		{
			velWalkMax *= 0.9999f;	// 少し早さを遅くして
			_vel->normalize();
			_vel->x() *= velWalkMax;
			_vel->z() *= velWalkMax;
		}
	}

	_isAccFrame = true;
	_vel->y() = 0.0f;
}

/*---------------------------------------------------------------------*//**
	アニメーション変更
**//*---------------------------------------------------------------------*/
void CharUnit::changeAnim(u16 animid, bool isLoop, bool isChangeFirstFrame, bool isChangeEndFrame)
{
	getShapeModel()->setAnimId(animid, ANIMFRAME_INTP, false, false, isLoop, isChangeFirstFrame, isChangeEndFrame);
	_mstat = MSTAT_NULL;	// 不明な状態へ
	_fdcntStopAnim = 0.0f;	// 即停止できるように
}

/*---------------------------------------------------------------------*//**
	有効・無効を設定
**//*---------------------------------------------------------------------*/
void CharUnit::setEnable(bool isEnable)
{
	Unit::setEnable(isEnable);

	if(_mdlCrcShdw != 0L)
	{
		_mdlCrcShdw->setShow(isEnable);
	}
}

/*---------------------------------------------------------------------*//**
	位置を設定
**//*---------------------------------------------------------------------*/
void CharUnit::setPosition(const Vector3F* pos)
{
	// 地面コリジョン平面を得る
	s32 idxPlane = -1;
	Gcol* gcol = Game::getGame()->getMap()->getGroundCollision();
	if(gcol != 0L)
	{
		idxPlane = gcol->getPlaneIndex(pos->x(), pos->z());
	}

	setPosition(pos, idxPlane);
}

/*---------------------------------------------------------------------*//**
	方向を設定
**//*---------------------------------------------------------------------*/
void CharUnit::setDirection(const Vector3F* dir)
{
	_dir->copy(dir);
}

/*---------------------------------------------------------------------*//**
	ターゲット位置を設定
**//*---------------------------------------------------------------------*/
void CharUnit::setTargetPosition(const Vector3F* pos)
{
	ASSERT(_posTarget != 0L);
	_posTarget->copy(pos);
}

/*---------------------------------------------------------------------*//**
	速さ関係のパラメータを設定
**//*---------------------------------------------------------------------*/
void CharUnit::setVelocityParam(f32 velMax, f32 ratioRun, f32 ratioFrictNrm, f32 ratioFrictAcc)
{
	_velMax = velMax;
	_velMaxSq = velMax * velMax;
	_ratioRun = ratioRun;
	_ratioFrictNrm = ratioFrictNrm;
	_ratioFrictAcc = ratioFrictAcc;
}

/*---------------------------------------------------------------------*//**
	歩行フレーム制御
**//*---------------------------------------------------------------------*/
void CharUnit::execWalking(const ExecCtx* ec)
{
	Gcol* gcol = Game::getGame()->getMap()->getGroundCollision();
	if(gcol == 0L)	{	return;	}
	_isHitKeepoutColl = false;

	// 移動状態の更新とアニメーション変更
	bool isMovable = execChangeMoveAnimationAndState(ec);

	// 向き更新
	if(isMovable)
	{
		f32 ydir = _dir->_v[1];
		const Unit* unitFocus = getSpirit()->getFocusUnit();
		if(unitFocus == 0L) { unitFocus = getSpirit()->getTalkUnit(); }
		if((unitFocus != 0L) && (unitFocus != this))	{	Calc::calcAngle(&ydir, unitFocus->getPosition()->z() - _pos->z(), unitFocus->getPosition()->x() - _pos->x());	}	// 注視相手があるとき
		else											{	Calc::calcAngle(&ydir, _vel->z(), _vel->x());	}
		_dir->_v[1] = Calc::closeAngle(ydir, _dir->_v[1], RATIO_TURN_RATE);
	}

	// 歩行
	if((_vel->lengthSq() > 0.00001f) && isMovable)
	{
		Vector3F vel(*_vel);

		// 倦怠時は遅く
		if(TFW_IS_FLAG(_cstat->getSpStatFlags(), CharStat::SPSTAT_HEAVY))
		{
			vel.x() *= 0.5f;
			vel.z() *= 0.5f;
		}

		for(int cntSafe = 1; cntSafe <= 10; cntSafe++)	// 進入禁止属性当たり時の滑り処理用のリトライループ
		{
			Vector3F pos = *_pos;
			pos.x() += vel.x() * ec->getDeltaFrame();
			pos.z() += vel.z() * ec->getDeltaFrame();

			// 地面コリジョン平面を得る
			s32 idxPlane = gcol->getPlaneIndex(pos.x(), pos.z());
			if(idxPlane == -1)
			{
				// コリジョン外に出た
				_isHitKeepoutColl = true;

				///TRACE("{CharUnit::execWalking} null plane col.\n");
				break;
			}

			// 進行可否判定
			u16 flagAtrb = gcol->getPlaneAtrbFlag(idxPlane);	// 地面属性フラグを得る
			if(	!Gcol::isMovablePlaneAtrbFlag(flagAtrb) && !TFW_IS_FLAG(_eflags, F_IGNORE_KEEPOUT) )	// 進入禁止
			{
				// 進入禁止コリジョンに触れた
				_isHitKeepoutColl = true;

				// 該当三角形との交点を求める
				Vector3F vip;
				Vector3F vi1;
				Vector3F vi2;
				if(/**/gcol->getIntersectionZx(&vip, &vi1, &vi2, _idxCurGcolPlane, _pos, &pos, 0.001f))//*/false)	// 前の位置との線分と三角形との交点が見つかった
				{
					// 交差する線分 vi1→vi2 と速度ベクトルの内積を求める
					Vector3F vi12 = vi2 - vi1;
					vi12.normalize();
					Vector3F veln = vel;
					veln.normalize();
					f32 dot = veln.dot(&vi12);

					#if 0
						// 直交に近い場合は移動不可
						const f32 tvang = 0.1736f;	// = cos(80°); 移動 OK の交差境界角度（直交に近い角度）
						if((-tvang <= dot) && (dot <= tvang))	// 直交に近い
						{
							TRACE("<CharUnit::exec> tyokkou.\n");
							break;	// 移動不可
						}
					#endif

					// 速度の方向に近い方の頂点を向いたベクトルを求める
					Vector3F vdir;
					if(dot > 0.0)	{	vdir = vi2 - vi1;	}	// vi2 の方が近い
					else			{	vdir = vi1 - vi2;	}	// vi1 の方が近い

					// 新しい速度ベクトル
					f32 lenVel = vel.length();	// 速度ベクトルの長さ
					vel = vdir;
					vel.normalize();
					lenVel *= TFW_FABS(dot);	// 直交に近い（ cos(90°) = 0 ）方が速度を緩める
					vel.x() *= lenVel;
					vel.z() *= lenVel;
					continue;	// ループ再処理
				}
				else	// 禁止領域に入っておきながら交差点が見つからないというのは、おそらく頂点が現在位置に非常に近い
				{
					TRACE("<CharUnit::exec> no intersection.\n");

					// 移動できなかった平面の最も近い頂点インデックスを得る
					s32 vtxidxB = gcol->getNearestVertexIndexZx(idxPlane, _pos);
					// 近接頂点を基点とするベクトル２つを求める
					Vector3F v1, v2;
					gcol->getPlaneVertexDirectionVector(&v1, &v2, idxPlane, vtxidxB);
					// ベクトルを反転させる
					v1 = - v1;
					v2 = - v2;

					// それぞれ速度ベクトルとの内積を求める
					Vector3F veln = vel;
					veln.normalize();
					v1.normalize();
					f32 dot1 = veln.dot(&v1);
					v2.normalize();
					f32 dot2 = veln.dot(&v2);

					// 角度 90 ～ 270 °は反転しているものと考える
					if(dot1 < 0.0f)
					{
						v1 = - v1;
						dot1 = - dot1;
					}
					if(dot2 < 0.0f)
					{
						v2 = - v2;
						dot2 = - dot2;
					}

					// 速度の方向に角度が近い方（角度が 0 に近い方）のベクトルを求める
					const Vector3F* vdir;
					if(dot1 > dot2)	{	vdir = &v1;					}	// v1 の方が近い（角度が 0 （ cos(0°) = 1 ）に近い）
					else			{	vdir = &v2;	dot1 = dot2;	}	// v2 の方が近い（角度が 0 （ cos(0°) = 1 ）に近い）

					// 新しい速度ベクトル
					f32 lenVel = vel.length();	// 速度ベクトルの長さ
					vel = *vdir;
					vel.normalize();
					lenVel *= TFW_FABS(dot1);	// 直交に近い（ cos(90°) = 0 ）分、速度を緩める
					vel.x() *= lenVel;
					vel.z() *= lenVel;
					continue;	// ループ再処理
				}
			}

			// Y 位置をコリジョンから得る
			f32 y = gcol->getY(idxPlane, pos.x(), pos.z());
			if(y == INVALID_FLOAT)
			{
				TRACE("<CharUnit::exec> invalid y.\n");
				break;	// 移動不可
			}
			else
			{
				pos.y() = y;
			}

			// 位置設定
			if(!checkBeforeMove(&pos, idxPlane))	{	break;	}	// 移動直前チェック
			s32 idxPlanePrev = _idxCurGcolPlane;
			setPosition(&pos, idxPlane);
			checkAfterMove(&pos, _idxCurGcolPlane, idxPlanePrev);	// 移動直後チェック

			break;
		}
	}

	// 速度減速
	if(_isAccFrame)	// 加速フレーム
	{
		_vel->x() *= _ratioFrictAcc;
		_vel->z() *= _ratioFrictAcc;
	}
	else			// 非加速フレーム
	{
		_vel->x() *= _ratioFrictNrm;
		_vel->z() *= _ratioFrictNrm;
	}

	// モデルの姿勢を設定
	if(getShapeModel() != 0L)	{	getShapeModel()->setPosition(_pos, _dir);	}

	_isAccFrame = false;	// 加速フラグのクリア
}

/*---------------------------------------------------------------------*//**
	移動時のアニメーションと状態変更フレーム制御
**//*---------------------------------------------------------------------*/
bool CharUnit::execChangeMoveAnimationAndState(const ExecCtx* ec)
{
	bool isNoMovable = (_cstat->getStat() != CharStat::STAT_ALIVE) ||		// 行動不能. 
			TFW_IS_FLAG(_cstat->getSpStatFlags(), CharStat::SPSTAT_SLEEP);	// 睡眠

	// 移動状態フラグ
	u16 cmflags = 0;
	if(isNoMovable)					{	cmflags |= CMF_MOVABLE;		}
	if(getSpirit()->isConfront())	{	cmflags |= CMF_CONFRONT;	}

	// 移動状態の更新とアニメーション変更
	f32 vl2 = _vel->lengthSq();
	if(getShapeModel() != 0L)
	{
		if(isNoMovable)	// 行動不能
		{
			if(_mstat != MSTAT_DOWN)
			{
				_vel->set(0.0f, 0.0f, 0.0f);	// 強制停止
				execChangeMoveAnimationToDown(ec, cmflags);
				_mstat = MSTAT_DOWN;
			}
		}
		else if(vl2 >= (_velMaxSq * _ratioRun * _ratioRun))
		{	
			_cstat->runningFrame();	// 走行中である
			if(_mstat != MSTAT_RUN)
			{
				TFW_SET_FLAG(cmflags, CMF_IMMEDIATELY, (_mstat == MSTAT_STOP));
				execChangeMoveAnimationToRun(ec, cmflags);
				_mstat = MSTAT_RUN;
			}
		}
		else if(vl2 > VEL_WALK_MIN_SQ)
		{
			if(_mstat != MSTAT_WALK)
			{
				TFW_SET_FLAG(cmflags, CMF_IMMEDIATELY, (_mstat == MSTAT_STOP));
				execChangeMoveAnimationToWalk(ec, cmflags);
				_mstat = MSTAT_WALK;
			}
		}
		else
		{
			if(_fdcntStopAnim <= 0.0f)
			{
				if(_mstat != MSTAT_STOP)
				{
					_vel->set(0.0f, 0.0f, 0.0f);	// 強制停止
					execChangeMoveAnimationToStop(ec, cmflags);
					_mstat = MSTAT_STOP;
					_fdcntStopAnim = CNT_STOP_ANIM;	// 激しく停止アニメーションを繰り返さないための調整カウンタ
				}
			}
			else
			{
				_fdcntStopAnim -= ec->getDeltaFrame();
			}
		}
	}

	return !isNoMovable;
}

/*---------------------------------------------------------------------*//**
	移動アニメーション変更 - ダウンへ
**//*---------------------------------------------------------------------*/
void CharUnit::execChangeMoveAnimationToDown(const ExecCtx* ec, u16 cmflags)
{
	getShapeModel()->setAnimId(ANIMID_TO_DOWN, ANIMFRAME_INTP, true, false, false, false, false);
}

/*---------------------------------------------------------------------*//**
	移動アニメーション変更 - 停止へ
**//*---------------------------------------------------------------------*/
void CharUnit::execChangeMoveAnimationToStop(const ExecCtx* ec, u16 cmflags)
{
	getShapeModel()->setAnimId(ANIMID_STOP, ANIMFRAME_INTP, false, false, true, false, false);
}

/*---------------------------------------------------------------------*//**
	移動アニメーション変更 - 走行へ
**//*---------------------------------------------------------------------*/
void CharUnit::execChangeMoveAnimationToRun(const ExecCtx* ec, u16 cmflags)
{
	// デフォルト動作では、対決中以外は歩行アニメーション
	if(TFW_IS_FLAG(cmflags, CMF_CONFRONT))
	{
		getShapeModel()->setAnimId(ANIMID_RUN, ANIMFRAME_INTP, true, false, true, !TFW_IS_FLAG(cmflags, CMF_IMMEDIATELY), false);
	}
	else
	{
		getShapeModel()->setAnimId(ANIMID_WALK, ANIMFRAME_INTP, true, false, true, !TFW_IS_FLAG(cmflags, CMF_IMMEDIATELY), false);
	}
}

/*---------------------------------------------------------------------*//**
	移動アニメーション変更 - 歩行へ
**//*---------------------------------------------------------------------*/
void CharUnit::execChangeMoveAnimationToWalk(const ExecCtx* ec, u16 cmflags)
{
	getShapeModel()->setAnimId(ANIMID_WALK, ANIMFRAME_INTP, true, false, true, !TFW_IS_FLAG(cmflags, CMF_IMMEDIATELY), false);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
CharUnit::CharUnit(UnitType unittype)
	: Unit(unittype)
	, _mdlCrcShdw(0L)
	, _posCenter(0L)
	, _vel(0L)
	, _dir(0L)
	, _posTarget(0L)
	, _heightCenter(0.0f)
	, _idxCurGcolPlane(-1)
	, _rateIsShadowCur(0.0f)
	, _rateIsShadowTrg(0.0f)
	, _isAccFrame(false)
	, _isHitKeepoutColl(false)
	, _eflags(0)
	, _mstat(MSTAT_NULL)
	, _fdcntStopAnim(0)
	, _qsflags(0)
	, _eestat(0L)
	, _cstat(0L)
	, _sprtSelfRef(0L)
	, _chrdefRef(0L)
	, _velMax(0.0f)
	, _velMaxSq(0.0f)
	, _ratioRun(0.0f)
	, _ratioFrictNrm(0.0f)
	, _ratioFrictAcc(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
CharUnit::~CharUnit()
{
	ASSERT(_mdlCrcShdw == 0L);
	ASSERT(_eestat == 0L);
	ASSERT(_cstat == 0L);
	ASSERT(_dir == 0L);
	ASSERT(_vel == 0L);
	ASSERT(_posCenter == 0L);
	ASSERT(_posTarget == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool CharUnit::create(Model* mdlRef, Spirit* sprtRef, u16 chrdid, u16 unitflags, CharClass charcls, bool isHasCircleShadow)
{
	// 速度・向き・目標位置ベクトルを作成
	_posCenter = new Vector3F();
	_vel = new Vector3F();
	_dir = new Vector3F();
	_posTarget = new Vector3F();

	// エネルギー状態を作成
	_eestat = new EleneStat();

	// キャラクタ状態を作成
	_cstat = new CharStat();
	// エネルギー状態・装備状態をキャラクタ状態にバインド
	_cstat->bind(this, _eestat, 0L);

	// 最大速度のデフォルト値（未設定の場合）
	if(_velMax == 0.0f)
	{
		setVelocityParam(VEL_MAX_DEFAULT, RATIO_RUN_DEFAULT, RATIO_FRICT_NRM_DEFAULT, RATIO_FRICT_ACC_DEFAULT);
	}

	// スピリット参照を保存する
	_sprtSelfRef = sprtRef;

	// キャラクタパラメータを取得する
	_chrdefRef = FixedData::getCharDefTable()->getDef(chrdid);							// キャラクタ定義を自ら得る
	// キャラクタパラメータ反映
	reflectParameterFromDefData(unitflags);

	// キャラクタクラス保存
	_charcls = charcls;

	// 親クラスによる作成
	if(!Unit::create(mdlRef, sprtRef, UNITFLAG_CHAR | unitflags))
	{
		return false;
	}

	// 中央位置の高さを得る
	_heightCenter = _radius;	// デフォルトは半径そのまま

	// 丸影作成
	if(isHasCircleShadow)
	{
		// 半径算出
		f32 rCircleShadow = _radius * 1.2f;
		if(_chrdefRef != 0L)
		{
			if(_chrdefRef->getShadowRadius() != 0)
			{
				rCircleShadow = (f32)_chrdefRef->getShadowRadius();
			}
		}

		// 不透明度算出
		f32 falphaCircleShadow = (_cstat->getEleneStat()->getCql()->getMaterialRatio() * 0.8f) - 0.2f;

		if((rCircleShadow > 10.0f) && (falphaCircleShadow > 0.2f))
		{
			CircleShadow* mdlCrcShdw = new CircleShadow();
			if(mdlCrcShdw->create())
			{
				_mdlCrcShdw = mdlCrcShdw;			// 参照を保存しておく

				// オフセットを設定する
				if(_chrdefRef != 0L)	{	_mdlCrcShdw->setOffsetPos(_chrdefRef->getShadowOffsetX(), _chrdefRef->getShadowOffsetZ());	}
				// 半径を設定する
				_mdlCrcShdw->setRadius(rCircleShadow);
				// 不透明度を設定する
				_mdlCrcShdw->setOpacity((u8)(falphaCircleShadow * 255.0f));

				// リソースプールに追加（リソース削除の管理を委譲）
				_mdlCrcShdw->setUsed(true);			// 使用中フラグをセット
				ResourcePool* resp = Game::getGame()->getResourcePool();
				resp->add((SgEntity**)&_mdlCrcShdw, 1);
			}
			else
			{
				delete mdlCrcShdw;
			}
		}
	}

	// スピリットにユニット作成を伝達
	if(_sprtSelfRef != 0L)
	{
		const CharLogicData* chrlgd = FixedData::geCharLogicDataTable()->getData(chrdid);	// キャラクタロジックデータを自ら得る
		if(!_sprtSelfRef->bind(this, _chrdefRef, chrlgd))
		{
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void CharUnit::destroy()
{
	// シーングラフから削除（配置されているときのみ）
	SorSgm* sgm = Game::getGame()->getSceneGraph();	ASSERT(sgm != 0L);
	for(int j = 0; j < getModelNum(); j++)
	{
		SgNode* sgnode = getModel(j);
		if(sgnode->isOnSg())
		{
			sgm->removeNode(sgnode);
		}
	}
	// 丸影も削除
	if(_mdlCrcShdw != 0L)
	{
		if(_mdlCrcShdw->isOnSg())
		{
			sgm->removeNode(_mdlCrcShdw);
		}
	}
	
	// キャラクタ状態を削除
	if(_cstat != 0L)
	{
		delete _cstat;
		_cstat = 0L;
	}

	// エネルギー状態を削除
	if(_eestat != 0L)
	{
		delete _eestat;
		_eestat = 0L;
	}

	// 速度・向き・目標位置ベクトルを削除
	delete _dir;
	_dir = 0L;
	delete _vel;
	_vel = 0L;
	delete _posCenter;
	_posCenter = 0L;
	delete _posTarget;
	_posTarget = 0L;

	// 丸影を削除
	if(_mdlCrcShdw != 0L)
	{
		// ResourcePool に管理委譲しているので、未使用に設定するのみ
		_mdlCrcShdw->setUsed(false);

		// 丸影の参照をリセット
		_mdlCrcShdw = 0L;
	}

	Unit::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void CharUnit::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!isEnable())	{	return;	}

	// モデルの姿勢を設定
	if(getShapeModel() != 0L)	{	getShapeModel()->setPosition(_pos, _dir);	}

	GameExecCtx* gec = (GameExecCtx*)ec;
	
	// キャラクタ状態を更新
	if(!gec->isLockOperating())	// 操作ロック（イベント等）以外
	{
		_cstat->exec(ec);
	}

	// キャラクタ特殊状態をパーティクル表現する
	if(!gec->isLockOperating() && !gec->isOpenedLockGui())
	{
		if((gec->getBodyExecCount() & 0xf) == 0)	// 適当に間引く
		{
			// 睡眠
			if(TFW_IS_FLAG(_cstat->getSpStatFlags(), CharStat::SPSTAT_SLEEP))
			{
				if((((GameExecCtx*)ec)->getBodyExecCount() & 0x7f) == 0)
				{
					ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
					Vector3F pos(*getCenterPos());
					pos.x() -= 80.0f;
					pos.y() += 70.0f;
					pos.z() -= 50.0;
					ptclsys->appearParticle(GameParticleDef::PTCLK_STAT_SLEEP, &pos, 0);
				}
			}
			// 倦怠（睡眠とは排他関係）
			else if(TFW_IS_FLAG(_cstat->getSpStatFlags(), CharStat::SPSTAT_HEAVY) && (_cstat->getDullnessRate() > 100))
			{
				if((::rand() & 0x7ff) < _cstat->getDullnessRate())
				{
					ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
					Vector3F pos(*getCenterPos());
					pos.x() -= 50.0f;
					pos.y() += 60.0f;
					pos.z() -= 50.0;
					ptclsys->appearParticle(GameParticleDef::PTCLK_STAT_DULL, &pos, 0);
				}
			}
			// 錯乱
			if(TFW_IS_FLAG(_cstat->getSpStatFlags(), CharStat::SPSTAT_CONFU) && (_cstat->getExcitednessRate() > 100))
			{
				if((::rand() & 0x1ff) < _cstat->getExcitednessRate())
				{
					ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
					Vector3F pos(*getCenterPos());
					pos.x() += 50.0f;
					pos.y() += 70.0f;
					pos.z() -= 50.0;
					ptclsys->appearParticle(GameParticleDef::PTCLK_STAT_CONFU, &pos, 0);
				}
			}
		}
	}

	// 影中率更新
	///STRACE("%f -> %f\n", _rateIsShadowCur, _rateIsShadowTrg);
	if(_rateIsShadowCur != _rateIsShadowTrg)
	{
		_rateIsShadowCur = (_rateIsShadowCur * 0.95f) + (_rateIsShadowTrg * 0.05f);
		// 影の中にいるときはモデルカラーで暗くする
		ShapeModel* smdl = getShapeModel();
		if(smdl != 0L)
		{
			u8 g = (u8)(255.0f * (1.0f - _rateIsShadowCur));
			ColorU8 col(g, g, g, 255);
			smdl->setColor(&col);
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void CharUnit::render(const RenderCtx* rc)
{
	#if defined(_DEBUG)
		if(getShapeModel() == 0L)
		{
			// キャラを▼で描く
			GameRenderCtx* grc = (GameRenderCtx*)rc;
			Renderer* rdr = rc->getRenderer();
			rdr->bindTexture(0L);
			rdr->setSolidColor(255, 0, 0, 127);
			rdr->setCulling(false);
			Vector3F vtx[3];
			f32 theta = grc->getBodyElapsedFrame() * 0.1f;
			f32 fsin = ::sinf(theta);
			f32 fcos = ::cosf(theta);
			vtx[0].copy(_pos);
			vtx[1].set(vtx[0].x() - 20.0f * fcos, vtx[0].y() + 50.0f, vtx[0].z() - 20.0f * fsin);
			vtx[2].set(vtx[0].x() + 20.0f * fcos, vtx[0].y() + 50.0f, vtx[0].z() + 20.0f * fsin);
			RendererUtils::render3dTriangles(rdr, vtx, 0L, 0L, 0L, 3, RendererUtils::TRIANGLES);
			rdr->setCulling(true);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	マップ変更前通知
**//*---------------------------------------------------------------------*/
void CharUnit::notifyChangeMapBefore()
{
	Unit::notifyChangeMapBefore();

	// スピリットに伝達
	if(_sprtSelfRef != 0L)
	{
		_sprtSelfRef->notifyChangeMapBefore();
	}
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void CharUnit::notifyChangedMap()
{
	Unit::notifyChangedMap();

	// スピリットに伝達
	if(_sprtSelfRef != 0L)
	{
		_sprtSelfRef->notifyChangedMap();
	}

	// シーングラフに配置（配置されてないときのみ）
	SorSgm* sgm = Game::getGame()->getSceneGraph();	ASSERT(sgm != 0L);
	for(int j = 0; j < getModelNum(); j++)
	{
		SgNode* sgnode = getModel(j);
		if(!sgnode->isOnSg())
		{
			sgm->addNode(sgnode);
		}
	}
	// 丸影も配置
	if(_mdlCrcShdw != 0L)
	{
		if(!_mdlCrcShdw->isOnSg())
		{
			sgm->addNode(_mdlCrcShdw);
		}
	}

	// 速度リセット
	_vel->set(0.0f, 0.0f, 0.0f);
	_isAccFrame = false;	// 加速フラグのクリア

	// 移動状態リセット
	_mstat = MSTAT_NULL;
}

/*---------------------------------------------------------------------*//**
	対決状態変更通知
**//*---------------------------------------------------------------------*/
void CharUnit::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition)
{
	Unit::notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);

	// スピリットに伝達
	if(_sprtSelfRef != 0L)
	{
		_sprtSelfRef->notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);
	}
}

/*---------------------------------------------------------------------*//**
	イベントゲームモード変更通知
**//*---------------------------------------------------------------------*/
void CharUnit::notifyChangedEventMode(bool isEventMode)
{
	Unit::notifyChangedEventMode(isEventMode);

	// 移動を停止へ
	if(!isEventMode)	// イベント終了時に
	{
		_mstat = MSTAT_NULL;			// 状態は不明に
		_vel->set(0.0f, 0.0f, 0.0f);	// 強制停止
		getShapeModel()->setAnimId(ANIMID_STOP, 0, false, false, true, false, false);
	}

	// スピリットに伝達
	if(_sprtSelfRef != 0L)
	{
		_sprtSelfRef->notifyChangedEventMode(isEventMode);
	}
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャに保存する
**//*---------------------------------------------------------------------*/
bool CharUnit::storeToStructure(CharUnitSaveStructure* svst) const
{
	ASSERT(sizeof(CharUnitSaveStructure) == CharUnitSaveStructure::SIZE_THIS);

	svst->_ctrlevid = getCtrlEvid();
	if(!Unit::storeToStructure(&svst->_usvst))			{	return false;	}
	if(!_cstat->storeToStructure(&svst->_cssvst))		{	return false;	}
	svst->_dir.copy(_dir);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャから読み込む
**//*---------------------------------------------------------------------*/
bool CharUnit::restoreFromStructure(const CharUnitSaveStructure* svst)
{
	ASSERT(sizeof(CharUnitSaveStructure) == CharUnitSaveStructure::SIZE_THIS);
	
	ASSERT(getCtrlEvid() == svst->_ctrlevid);
	if(!Unit::restoreFromStructure(&svst->_usvst))		{	return false;	}
	if(!_cstat->restoreFromStructure(&svst->_cssvst))	{	return false;	}
	setDirection(&svst->_dir);

	// ターゲット位置は本来保存すべき項目のようだが、
	// セーブデータ読み込み後に再検索するとして、
	// まずは動かないように自分をターゲットする
	setTargetPosition(&svst->_usvst._pos);

	return true;
}

/*---------------------------------------------------------------------*//**
	定義データからパラメータを反映させる
**//*---------------------------------------------------------------------*/
void CharUnit::reflectParameterFromDefData(u16 unitflags)
{
	if(_chrdefRef == 0L)	{	return;	}

	u8 flagsCreate = 0;
	u16 chrdid = _chrdefRef->getCharDefId();
	const CharLogicData* chrlgd = FixedData::geCharLogicDataTable()->getData(chrdid);	// キャラクタロジックデータを自ら得る
	// 五元素状態を設定
	_eestat->setup(_chrdefRef->getOqlElene(), _chrdefRef->getCurElene());
	// 敵の場合は与えられる現在値が本来の元素量で、原質値（ボディプラクリティ）は変質してモンスター化を持ってしまった、という世界設定
	if(getUnitType() == UNITTYPE_ENEMY_CHAR)	{	_eestat->oql()->copy(_chrdefRef->getCurElene());	}	// 現在量を根本原質量にコピー
	// 攻撃を受けるか判定
	flagsCreate |= ((getUnitType() != UNITTYPE_ENEMY_CHAR) && !TFW_IS_FLAG(unitflags, Unit::UNITFLAG_PC)) ? CharStat::CF_IGNORE_ATTACK : 0;
	// 反属性判定
	flagsCreate |= ((chrlgd != 0L) && (chrlgd->getParam(CharLogicData::PARAM_NEGATIVE_ATRB) != 0)) ? CharStat::CF_NEGATIVE_ATRB : 0;
	// キャラクタ状態を設定
	_cstat->setup(_chrdefRef->getDefaultCharParam(), flagsCreate, true);
}

/*---------------------------------------------------------------------*//**
	影の位置を設定する
**//*---------------------------------------------------------------------*/
void CharUnit::setShadowPos(const Vector3F* pos)
{
	if(_mdlCrcShdw == 0L)		{	return;	}

	// 影モデル位置設定
	_mdlCrcShdw->setPosition(pos, 0L);

#if ENABLE_CIRCLESHADOW_AFFINE
	// アフィン行列を設定する
	if(_idxCurGcolPlane == -1)	{	return;	}
	Gcol* gcol = Game::getGame()->getMap()->getGroundCollision();
	const Gcol::Plane* plane = gcol->getPlane(_idxCurGcolPlane);
	const Vector3F* v1 = gcol->getVertex(plane->_vtxidx1);
	const Vector3F* v2 = gcol->getVertex(plane->_vtxidx2);
	const Vector4F* coeff = gcol->getPlaneCoefficient(plane->_coeffidx);
	Matrix4F mtxAffine;
	mtxAffine.set(0, &(*v2 - *v1));				((Vector3F*)mtxAffine._v[0])->normalize();			// X 軸 （三角形の適当な２点を X 軸とする）
	mtxAffine.set(1, (const Vector3F*)coeff);	((Vector3F*)mtxAffine._v[1])->normalize();			// Y 軸 （三角形の法線を Y 軸とする）
	mtxAffine.set(2, &((const Vector3F*)mtxAffine._v[0])->cross((const Vector3F*)mtxAffine._v[1]));	// Z 軸 （X 軸 と Y 軸の外積）
	_mdlCrcShdw->setAffineMatrix(&mtxAffine);
#endif
}

/*---------------------------------------------------------------------*//**
	位置を設定
**//*---------------------------------------------------------------------*/
void CharUnit::setPosition(const Vector3F* pos, s32 idxPlane)
{
	_posCenter->copy(pos);
	_posCenter->y() += _heightCenter;
	_idxCurGcolPlane = idxPlane;

	// 影中率を求める
	if(_idxCurGcolPlane != -1)
	{
		Gcol* gcol = Game::getGame()->getMap()->getGroundCollision(); ASSERT(gcol != 0L);
		if(gcol != 0L)
		{
			u16 flagAtrb = gcol->getPlaneAtrbFlag(_idxCurGcolPlane);	// 地面属性フラグを得る
			switch(flagAtrb & Gcol::Plane::ATRBF_SHADOW_MASK)
			{
			default:							_rateIsShadowTrg = 0.0f;	break;
			case Gcol::Plane::ATRBF_SHADOW_S:	_rateIsShadowTrg = 0.25f;	break;
			case Gcol::Plane::ATRBF_SHADOW_M:	_rateIsShadowTrg = 0.5f;	break;
			case Gcol::Plane::ATRBF_SHADOW_L:	_rateIsShadowTrg = 0.75f;	break;
			}
		}
	}
	else
	{
		_rateIsShadowTrg = 0.0f;
	}

	// 丸影の位置設定
	setShadowPos(pos);

	Unit::setPosition(pos);
}


////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
