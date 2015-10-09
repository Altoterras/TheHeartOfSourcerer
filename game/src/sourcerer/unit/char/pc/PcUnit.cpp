/***********************************************************************//**
 *	PcUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/06/16.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PcUnit.h"

// Friends
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/MoveMap.h"
#include "../../../body/Party.h"
#include "../../../common/CalcUtils.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/MoveCursor.h"
#include "../../../gui/MoveCursorExtension.h"
#include "../../../item/ItemEquip.h"
#include "../../../map/Map.h"
#include "../../../spirit/Spirit.h"
#include "../../../status/CharStat.h"

// External
#include "../../../../tfw/collision/Gcol.h"
#include "../../../../tfw/lib/Calc.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/lib/Matrix.h"
#include "../../../../tfw/lib/Point.h"
#include "../../../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../../../tfw/g3d/sg/node/implement/ModelLoader.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define RATIO_TURN_RATE			(0.1f)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PcUnit 静的メンバ変数

//==========================================================================
// PcUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	リーダーをセット
**//*---------------------------------------------------------------------*/
void PcUnit::setLeader(bool isLeader)
{
	TFW_SET_FLAG(_pcaflags, PCAF_LEADER, isLeader);		// リーダーにセット
	TFW_SET_FLAG(_eflags, F_IGNORE_KEEPOUT, !isLeader);	// 非リーダー時は進入禁止を無視する

	if(!isLeader)
	{
		// 基本速さパラメータの再設定（リーダー時の移動カーソルの影響を除去）
		setVelocityParam(_velRangeMax, getRunVelocityRatio(), getFrictNormalRatio(), getFrictAccelRatio());
	}
}

/*---------------------------------------------------------------------*//**
	リーダーになれるか調べる
**//*---------------------------------------------------------------------*/
bool PcUnit::isValidLeader()
{
	// 有効性チェック
	if(!isEnable())									{	return false;	}

	// 活動チェック
	if(_cstat->getStat() != CharStat::STAT_ALIVE)	{	return false;	}

	// 移動可能な当たりにいるかチェック
	if(_isHitKeepoutColl)							{	return false;	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PcUnit::PcUnit(UnitType unittype)
	: CharUnit(unittype)
	, _partyRef(0L)
	, _pcaflags(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PcUnit::~PcUnit()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PcUnit::create(Model* mdlRef, Spirit* sprtRef, Party* party, u16 chrdid, CharClass charcls)
{
	if(!CharUnit::create(mdlRef, sprtRef, chrdid, UNITFLAG_PC | UNITFLAG_CREATION_FOCUSABLE | UNITFLAG_CREATION_HIDN_CLICK, charcls, true))
	{
		return false;
	}

	// パーティへの参照を保存
	_partyRef = party;

	// キャラクタ状態にエネルギー状態と装備状態をバインド
	if((sprtRef != 0L) && (sprtRef->charStat() != 0L))
	{
		sprtRef->charStat()->bind(this, eleneStat(), (itemEquip() != 0L) ? itemEquip()->equipStat() : 0L);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PcUnit::destroy()
{
	CharUnit::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void PcUnit::exec(ExecRes* res, const ExecCtx* ec)
{
	// 話し相手に対しての向き更新
	const Unit* unitTalk = getSpirit()->getTalkUnit();
	if(unitTalk != 0L)
	{
		f32 ydir = _dir->_v[1];
		if((unitTalk != 0L) && (unitTalk != this))
		{
			Calc::calcAngle(&ydir, unitTalk->getPosition()->z() - _pos->z(), unitTalk->getPosition()->x() - _pos->x());
			_dir->_v[1] = Calc::closeAngle(ydir, _dir->_v[1], RATIO_TURN_RATE);
		}
	}

	// キャラユニットとしての移動処理など
	CharUnit::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	歩行フレーム制御
**//*---------------------------------------------------------------------*/
void PcUnit::execWalking(const ExecCtx* ec, bool isForceAuto)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	// +++ Adjustable Parameter >>>
	#define ACC_MIN				(30.30f / (f32)FRAMERATE)	// 最低限の加速度
	#define ACC_MIN_SQ			(ACC_MIN * ACC_MIN)
	#define ACC_MAX				(250.0f / (f32)FRAMERATE)	// 最大移動加速度
	#define ACC_MAX_SQ			(ACC_MAX * ACC_MAX)
	#define DIST_TRG_MIN_SQ		(10.0f)								// 目標位置差の合格値
	#define DIST_TO_ACC			(0.2f)								// 距離から加速度への変換率
	#define DIST_RUN			(330.0f)							// 走り始める目標位置との距離境界値
	#define DIST_RUN_SQ			(DIST_RUN * DIST_RUN)
	// +++ Adjustable Parameter <<<

	// 歩行処理
	if(	(!gec->isOpenedLockGui()) &&	// 操作ロック GUI を開いていないとき
		(!gec->isLockOperating())	)	// 操作ロック中以外のとき
	{
		if(	!isForceAuto					// 強制自動移動フラグがオフでかつ･･･
			&& isOperatedForMoving() )		// または、移動操作中のとき
		{
			// 移動カーソルによる移動
			MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor(); ASSERT(mvcsr != 0L);
			if(mvcsr->isTouching())
			{
				// 加速度を得る
				Vector3F accWk;
				const PointF32* ptSt = mvcsr->getStartPt();
				const PointF32* ptNw = mvcsr->getLastPt();
				if(!ptSt->isNear(ptNw, 0, 0))
				{
					Vector2F ptScr[2] = { Vector2F((f32)ptSt->x(), (f32)ptSt->y()), Vector2F((f32)ptNw->x(), (f32)ptNw->y()) };
					Vector3F pos3d[2];
					f32 zScrPos[2] = { - (f32)ptSt->y() * 0.001f, - (f32)ptNw->y() * 0.001f };	// これは適当．正確さを求めるなら convScreenPosAnd3dZTo3dPos を使うべき
					CalcUtils::calcScreenPosTo3dPos(pos3d, ptScr, zScrPos, 2);
					Vector2F accXz(pos3d[1].x() - pos3d[0].x(), pos3d[1].z() - pos3d[0].z());

					// 最大速度を得る
					f32 velMax = mvcsr->getLengthRate() * _velRangeMax;
					if(velMax < _velRangeMin)	{	velMax = _velRangeMin;	}

					if(getSpirit()->isConfront())	// 対決時
					{
						accXz *= 0.33f;		// 移動加速度減少
					}

					if(accXz.lengthSq() > ACC_MIN_SQ)
					{
						// 歩行処理
						setVelocityParam(velMax, getRunVelocityRatio(), getFrictNormalRatio(), getFrictAccelRatio());	// 速さパラメータの設定
						walk(&accXz, mvcsr->getState() == MoveCursor::STATE_RUN);
					}
				}
			}
		}
		else	// 操作中ではない
		{
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
		}
	}

	/*
	// 足音を鳴らす（テスト）
	u16 seid = 0;
	switch(getShapeModel()->getCurAnimId())
	{
	case ANIMID_RUN:	seid = GameSoundDef::SE_RUN_SOIL;	break;
	case ANIMID_WALK:	seid = GameSoundDef::SE_WALK_SOIL;	break;
	}
	if(seid != 0)
	{
		f32 frameAnim = getShapeModel()->getCurAnimFrame();
		if(frameAnim <= 1.0f)
		{
			Game::getGame()->getSoundManager()->playSe(seid, false, 0.7f, getPosition(), getVelocity());
		}
	}
	*/

	CharUnit::execWalking(ec);
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void PcUnit::notifyChangedMap()
{
	moveMapInitPos();	// 入口フラグによる移動

	getShapeModel()->setAnimId(ANIMID_STOP, 0, false, false, true, false, false);	// 停止モーションへ変更

	CharUnit::notifyChangedMap();
}

/*---------------------------------------------------------------------*//**
	対決状態変更通知
**//*---------------------------------------------------------------------*/
void PcUnit::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition)
{
	CharUnit::notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);
}

/*---------------------------------------------------------------------*//**
	速さ範囲パラメータを設定
**//*---------------------------------------------------------------------*/
void PcUnit::setVelocityRange(f32 velRangeMin, f32 velRangeMax)
{
	_velRangeMin = velRangeMin;
	_velRangeMax = velRangeMax;
}

/*---------------------------------------------------------------------*//**
	移動操作中かどうか
**//*---------------------------------------------------------------------*/
bool PcUnit::isOperatedForMoving()
{
	MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor(); ASSERT(mvcsr != 0L);
	const MoveCursorExtension* mcex = mvcsr->getExtension(); ASSERT(mcex != 0L);
	return (isLeader() && (mcex->getHandledUnit() == 0L))	// リーダーかつ移動カーソル操作が標準ユニットのとき
			|| (mcex->getHandledUnit() == this);			// または、移動カーソル操作が自分のとき
}

/*---------------------------------------------------------------------*//**
	マップ初期位置へ移動
**//*---------------------------------------------------------------------*/
bool PcUnit::moveMapInitPos()
{
	if(isLeader())
	{
		// リーダーは Party の moveMapInitLeaderPos で先に決定されている
	}
	else
	{
		calcTargetPos();			// 目標位置の計算
		setPosition(_posTarget);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	目標位置計算
**//*---------------------------------------------------------------------*/
void PcUnit::calcTargetPosRelLeader(const Vector3F* posLeaderDiff)
{
	const CharUnit* unitLeader = _partyRef->getLeaderPcUnit(); ASSERT(unitLeader != 0L);

	MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor(); ASSERT(mvcsr != 0L);
	const MoveCursorExtension* mcex = mvcsr->getExtension(); ASSERT(mcex != 0L);
	const CharUnit* unitMoveOp = (const CharUnit*)mcex->getHandledUnit();
	if(unitMoveOp == 0L)	{	unitMoveOp = unitLeader;	}

	// 自キャラが自分をフォーカスしているときはやめる
	if(!getSpirit()->isConfront())	// 非対決時のみ
	{
		const Unit* unitLeaderFocused = unitLeader->getSpirit()->getFocusUnit();
		if(unitLeaderFocused != 0L)
		{
			if(unitLeaderFocused == this)					{	return;	}	// 自キャラがこちらをフォーカスしている
			if(unitLeaderFocused->getOwnerUnit() == this)	{	return;	}	// おそらく所持アイテムをフォーカスしている
		}
	}

	// 操作キャラとの位置差
	Matrix4F mtxRotY;
	mtxRotY.rotateY( unitMoveOp->getDirection()->y() );
	Vector3F posTrg;
	mtxRotY.mult(&posTrg, posLeaderDiff);
	posTrg += *unitMoveOp->getPosition();

	// コリジョンをとる
	Gcol* gcol = Game::getGame()->getMap()->getGroundCollision();
	if(gcol != 0L)
	{
		for(int cntSafe = 1; cntSafe <= 10; cntSafe++)	// 進入禁止属性当たり時の滑り処理用のリトライループ
		{
			// 地面コリジョン平面を得る
			s32 idxPlane = gcol->getPlaneIndex(posTrg.x(), posTrg.z());
			if(idxPlane != -1)
			{
				// 地面属性フラグを得る
				u16 flagAtrb = gcol->getPlaneAtrbFlag(idxPlane);
				if(	Gcol::isMovablePlaneAtrbFlag(flagAtrb) )	// 進入可
				{
					break;	// OK !
				}
			}

			// 進入不可の場合は徐々に操作キャラに近づける
			posTrg.set((posTrg.x() + unitMoveOp->getPosition()->x()) * 0.5f, (posTrg.y() + unitLeader->getPosition()->y()) * 0.5f, (posTrg.z() + unitLeader->getPosition()->z()) * 0.5f);
		}
	}

	setTargetPosition(&posTrg);
}

/*---------------------------------------------------------------------*//**
	移動後のチェック
**//*---------------------------------------------------------------------*/
void PcUnit::checkAfterMove(const Vector3F* pos, s32 idxPlaneCur, s32 idxPlanePrev)
{
	if(isLeader())
	{
		if(idxPlaneCur != idxPlanePrev)	// 平面が変わった場合のみ
		{
			// 地面当たりがマップ移動であったらマップ変更
			Map* map = Game::getGame()->getMap(); ASSERT(map != 0L);
			if(!map->isCloseGate())	// 逃げる許可時は戦闘時も移動できる．つまり「逃げる」はマップ移動により行う仕様
			{
				s32 mapidMove = map->getGcolPlaneMoveMapId(idxPlaneCur);
				if(mapidMove <= 0)	{	return;	}		// マップ移動ではない
				TRACE("{PcUnit::checkAfterMove} reserve move map : %d\n", mapidMove);
				Game::getGame()->getMoveMap()->reserveChangeMap( mapidMove );
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
