/***********************************************************************//**
 *	ConfrontCam.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ConfrontCam.h"

// Friends
#include "GameCustomCamClass.h"
#include "../common/VelocityInterpolation.h"
#include "../confront/Confront.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../body/Party.h"
#include "../map/Map.h"
#include "../spirit/Spirit.h"
#include "../unit/UnitManager.h"
#include "../unit/char/npc/EnemyUnit.h"
#include "../unit/char/pc/PcUnit.h"

// External
#include "../../tfw/ui/TouchPanel.h"
#include "../../tfw/lib/Calc.h"
#include "../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define	Y_ZOOM_WHOLE				(1500.0f)	// 全体俯瞰時のズーム高さ
#define	Y_ZOOM_CHARS				(600.0f)	// キャラ注視時のズーム高さ
#define	Y_ZOOM_TRG					(600.0f)	// ターゲット注視時のズーム高さ
#define	Y_ZOOM_GAZEUNIT				(300.0f)	// 一定時間指定ユニット注視時のズーム高さ
#define Z_CHAR_TI_CAM_DIFF_WHOLE	(2400.0f)	// 全体俯瞰時のキャラ位置に対するカメラの Z 位置差
#define Z_CHAR_TI_CAM_DIFF_CHARS	(2400.0f)	// キャラ注視時のキャラ位置に対するカメラの Z 位置差
#define Z_CHAR_TI_CAM_DIFF_TRG		(2400.0f)	// ターゲット注視時のキャラ位置に対するカメラの Z 位置差
#define	Z_CHAR_TI_CAM_DIFF_GAZEUNIT	(1200.0f)	// 一定時間指定ユニット注視時のキャラ位置に対するカメラの Z 位置差
#define RATIO_LEN_TO_Y_ZOOM			(0.002f)	// ２点間の距離に対するズーム率
#define RATIO_Z_ZOOM				(1.5f)		// タッチ座標に対するズーム率
#define Z_ZOOM_OFS_MIN				(-1800.0f)	// ユーザーズームの最小値
#define Z_ZOOM_OFS_MAX				(9000.0f)	// ユーザーズームの最大値

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ConfrontCam 静的メンバ変数

//==========================================================================
// ConfrontCam メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	モード設定
**//*---------------------------------------------------------------------*/
void ConfrontCam::setMode(Mode mode, const Unit* unit1, const Unit* unit2, const Unit* unit3, f32 framecnt)
{
	_modeCur = mode;
	_perfflags = 0;	// パフォーマンスフラグをリセット

	if((unit1 != 0L) && !unit1->isEnable())	{	unit1 = 0L;	}
	if((unit2 != 0L) && !unit2->isEnable())	{	unit2 = 0L;	}
	if((unit3 != 0L) && !unit3->isEnable())	{	unit3 = 0L;	}
	_unitModeUnit1 = unit1;
	_unitModeUnit2 = unit2;
	_unitModeUnit3 = unit3;
	_fcntMode = framecnt;
}

/*---------------------------------------------------------------------*//**
	演出開始
**//*---------------------------------------------------------------------*/
void ConfrontCam::startPerformance(u32 perfflags, f32 angle, f32 speed)
{
	_perfflags = perfflags;

	_zCamPosLeave = 0.0f;
	_thetaCamPosRot = angle;
	_speedPerf = speed;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ConfrontCam::ConfrontCam()
	: _locTrg(0L)
	, _aimTrg(0L)
	, _velLoc(0L)
	, _velAim(0L)
	, _vintpLoc(0L)
	, _vintpAim(0L)
	, _zZoomOfsCur(0.0f)
	, _zZoomOfsTrg(0.0f)
	, _modeCur(MODE_NULL)
	, _modePrev(MODE_NULL)
	, _perfflags(0)
	, _thetaCamPosRot(0.0f)
	, _zCamPosLeave(0.0f)
	, _speedPerf(0.0f)
	, _fcntMode(0.0f)
	, _unitModeUnit1(0L)
	, _unitModeUnit2(0L)
	, _unitModeUnit3(0L)
{
	_camclass = GameCustomCamClass::CAMCLS_CONFRONT;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ConfrontCam::~ConfrontCam()
{
	ASSERT(_locTrg == 0L);
	ASSERT(_aimTrg == 0L);
	ASSERT(_velLoc == 0L);
	ASSERT(_velAim == 0L);
	ASSERT(_vintpLoc == 0L);
	ASSERT(_vintpAim == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ConfrontCam::create()
{
	if(!LookAtCamera::create(10.0f, 100.0f, 100000.0f, 0))
	{
		return false;
	}

	_locTrg = new Vector3F();
	_aimTrg = new Vector3F();
	_velLoc = new Vector3F();
	_velAim = new Vector3F();
	_vintpLoc = new VelocityInterpolation(100.0f, 1000.0f);
	_vintpAim = new VelocityInterpolation(80.0f, 1000.0f);

	_modeCur = _modePrev = MODE_WHOLE;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ConfrontCam::destroy()
{
	delete _vintpLoc;
	_vintpLoc = 0L;
	delete _vintpAim;
	_vintpAim = 0L;
	delete _velAim;
	_velAim = 0L;
	delete _velLoc;
	_velLoc = 0L;
	delete _aimTrg;
	_aimTrg = 0L;
	delete _locTrg;
	_locTrg = 0L;

	LookAtCamera::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void ConfrontCam::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!isVisible())	{	return;	}

	GameExecCtx* gec = (GameExecCtx*)ec;

	// ズーム処理
	TouchPanel* ui = gec->getTouchPanel();
	switch(ui->getLastAction())
	{
	case TouchPanel::ACT_ZOOM:
		{
			f32 rCur, rPrev;
			ui->getZoomParam(0L, 0L, &rCur, &rPrev);
			_zZoomOfsTrg -= (rCur - rPrev) * RATIO_Z_ZOOM;
			if(_zZoomOfsTrg < Z_ZOOM_OFS_MIN)		{	_zZoomOfsTrg = Z_ZOOM_OFS_MIN;	}
			else if(_zZoomOfsTrg > Z_ZOOM_OFS_MAX)	{	_zZoomOfsTrg = Z_ZOOM_OFS_MAX;	}
	
			ui->setReacted(true);	// 反応済みフラグを立てる
		}
		break;
	}
	if(_zZoomOfsTrg != _zZoomOfsCur)
	{
		_zZoomOfsCur = _zZoomOfsCur * 0.9f + _zZoomOfsTrg * 0.1f;
		if(TFW_ABS(_zZoomOfsCur - _zZoomOfsTrg) < 0.1f)
		{
			_zZoomOfsCur = _zZoomOfsTrg;
		}
	}

	// 注視点とカメラ位置の決定
	do
	{
		Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
		const PcUnit* unitLeader = party->getLeaderPcUnit();
		if(unitLeader == 0L)	{	break;	}
		s32 ipcLeader = party->findPcUnit(unitLeader);
		const Spirit* sprtLeader = unitLeader->getSpirit();	ASSERT(sprtLeader != 0L);
		if(sprtLeader == 0L)	{	break;	}

		const Unit* unit1 = 0L;
		const Unit* unit2 = 0L;
		const Unit* unit3 = 0L;
		f32 yCam = 0.0f;
		f32 xCamAimDiff = 0.0f;
		f32 zCamAimDiff = 0.0f;

		// モードによる基本値
		switch(_modeCur)
		{
		case MODE_WHOLE:	// 全体俯瞰
			{
				// 最も遠い対決対象を得る
				unit2 = sprtLeader->getConfrontUnit();
				Confront* confront = Game::getGame()->getConfront();
				if(confront != 0L)
				{
					f32 distMax = 0.0f;
					const UnitArray* ua = confront->getTargets();
					for(int i = 0; i < ua->getCount(); i++)
					{
						const Unit* unitWk = ua->getUnit(i);
						if(unitWk == 0L) { continue; }

						//if((unitWk != 0L) && unitWk->isEnable() && (unitWk != unitLeader))
						bool target = false;
						if(	((unitWk->getUnitType() == UNITTYPE_ENEMY_CHAR) && (((EnemyUnit*)unitWk)->getDownFrameCounter() < (3.0f * FRAMERATE)))
							|| (unitWk->isEnable() && (unitWk != unitLeader)) )
						{
							/*
							f32 xDist = unitWk->getPosition()->x() - unitLeader->getPosition()->x();
							xDist = TFW_ABS(xDist);
							if(xDist < distMin)
							{
								distMin = xDist;
								unit2 = unitWk;
							}
							*/
							f32 dd = (*unitWk->getPosition() - *unitLeader->getPosition()).lengthSq();
							if(dd > distMax)
							{
								distMax = dd;
								unit2 = unitWk;
							}
						}
					}
				}

				unit1 = (_unitModeUnit1 != 0L) ? _unitModeUnit1 : unitLeader;
				yCam = Y_ZOOM_WHOLE;
				zCamAimDiff = Z_CHAR_TI_CAM_DIFF_WHOLE + _zZoomOfsCur;
			}
			break;
		case MODE_PARTY:	// プレイヤーキャラクタ（達）注視
			{
				unit1 = unitLeader;
				unit2 = party->getPcUnit((ipcLeader == 0) ? 1 : 0);
				unit3 = party->getPcUnit(2);
				if(!unit3->isEnable())
				{
					unit3 = 0L;
				}
				if(unit2->isEnable())
				{
					yCam = Y_ZOOM_CHARS;
					zCamAimDiff = Z_CHAR_TI_CAM_DIFF_CHARS;
				}
				else
				{
					unit2 = 0L;
					yCam = Y_ZOOM_TRG;
					zCamAimDiff = Z_CHAR_TI_CAM_DIFF_TRG;
				}
			}
			break;
		case MODE_TRG:		// ターゲット注視
			{
				unit1 = sprtLeader->getConfrontUnit();
				yCam = Y_ZOOM_TRG;
				zCamAimDiff = Z_CHAR_TI_CAM_DIFF_TRG;
			}
			break;
		case MODE_GAZE:		// 一定時間指定ユニット注視
			{
				unit1 = (_unitModeUnit1 != 0L) ? _unitModeUnit1 : unitLeader;
				yCam = Y_ZOOM_GAZEUNIT;
				zCamAimDiff = Z_CHAR_TI_CAM_DIFF_GAZEUNIT;
			}
			break;
		case MODE_TWO:
			{
				unit1 = (_unitModeUnit1 != 0L) ? _unitModeUnit1 : unitLeader;
				unit2 = (_unitModeUnit2 != 0L) ? _unitModeUnit2 : sprtLeader->getConfrontUnit();
				yCam = Y_ZOOM_CHARS;
				zCamAimDiff = Z_CHAR_TI_CAM_DIFF_CHARS;
			}
			break;
		case MODE_THREE:
			{
				unit1 = (_unitModeUnit1 != 0L) ? _unitModeUnit1 : unitLeader;
				unit2 = (_unitModeUnit2 != 0L) ? _unitModeUnit2 : sprtLeader->getConfrontUnit();
				unit3 = _unitModeUnit3;
				yCam = Y_ZOOM_CHARS;
				zCamAimDiff = Z_CHAR_TI_CAM_DIFF_CHARS;
			}
			break;
		}

		// 各モードは設定時間で終了する
		if(_fcntMode > 0.0f)
		{
			_fcntMode -= ec->getDeltaFrame();
			if(_fcntMode <= 0.0f)
			{
				_fcntMode = 0.0f;
				setMode(MODE_WHOLE);	// 全体俯瞰へ
			}
		}

		// 演出
		if(TFW_IS_FLAG(_perfflags, PERFF_LEAVE))	// 徐々に離れるカメラ
		{
			zCamAimDiff += _zCamPosLeave;
			_zCamPosLeave += _speedPerf * ec->getDeltaFrame();
		}
		if(TFW_IS_FLAG(_perfflags, PERFF_ROTATE))	// 回転するカメラ
		{
			xCamAimDiff = ::sinf(_thetaCamPosRot) * zCamAimDiff;
			zCamAimDiff = ::cosf(_thetaCamPosRot) * zCamAimDiff;
			_thetaCamPosRot += _speedPerf * 0.0005f * ec->getDeltaFrame();
			if(_thetaCamPosRot > TFW_PI)	{	_thetaCamPosRot -= TFW_DPI;	}
		}

		// ユニット注視
		aimUnits(unit1, unit2, unit3, yCam, xCamAimDiff, zCamAimDiff);
	}
	while(false);

	// マップによるカメラ移動最大最小値にクリップする
#if 0
	Map* map = Game::getGame()->getMap();
	const Map::MapConfig* mconf = map->getMapConfig();
	if(_locTrg->x() < mconf->_xCamMoveMin)							{	_locTrg->x() = mconf->_xCamMoveMin;							}
	if(_locTrg->x() > mconf->_xCamMoveMax)							{	_locTrg->x() = mconf->_xCamMoveMax;							}
	if(_locTrg->z() < (mconf->_zCamMoveMin + Z_CHAR_TI_CAM_DIFF))	{	_locTrg->z() = mconf->_zCamMoveMin + Z_CHAR_TI_CAM_DIFF;	}
	if(_locTrg->z() > (mconf->_zCamMoveMax + Z_CHAR_TI_CAM_DIFF))	{	_locTrg->z() = mconf->_zCamMoveMax + Z_CHAR_TI_CAM_DIFF;	}
#endif

	// ターゲット座標への補間
	if((_modeCur != _modePrev) || (getVisibleCounter() <= 2))	// モード変更時、または有効化直後
	{
		// 補間しない
		_loc->copy(_locTrg);
		_aim->copy(_aimTrg);
	}
	else
	{
		// 補間する
#if 1
		_vintpLoc->interpolation(_loc, _velLoc, _locTrg, ec->getDeltaFrame());
		_vintpAim->interpolation(_aim, _velAim, _aimTrg, ec->getDeltaFrame());
#else
		f32 rateLoc = 0.5f * ec->getDeltaFrame();
		if(rateLoc > 1.0f)	{	rateLoc = 1.0f;	}
		f32 rrateLoc = 1.0f - rateLoc;
		Vector3F locNew((_loc->_v[0] * rrateLoc) + (_locTrg->_v[0] * rateLoc), (_loc->_v[1] * rrateLoc) + (_locTrg->_v[1] * rateLoc), (_loc->_v[2] * rrateLoc) + (_locTrg->_v[2] * rateLoc));
		_loc->copy(&locNew);
		f32 rateAim = 0.5f * ec->getDeltaFrame();
		if(rateAim > 1.0f)	{	rateAim = 1.0f;	}
		f32 rrateAim = 1.0f - rateAim;
		Vector3F aimNew((_aim->_v[0] * rrateAim) + (_aimTrg->_v[0] * rateAim), (_aim->_v[1] * rrateAim) + (_aimTrg->_v[1] * rateAim), (_aim->_v[2] * rrateAim) + (_aimTrg->_v[2] * rateAim));
		_aim->copy(&aimNew);
		///STRACE("NCY: %f, %f\n", locNew.y(), aimNew.y());
#endif
	}

	// モード変更検知用
	_modePrev = _modeCur;

	LookAtCamera::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	ユニットを注視する
**//*---------------------------------------------------------------------*/
void ConfrontCam::aimUnits(const Unit* unit1, const Unit* unit2, const Unit* unit3, f32 yCam, f32 xCamAimDiff, f32 zCamAimDiff)
{
	// ３者がなるべく映るように
	const Unit* uarr[] = { unit1, unit2, unit3 };
	Vector3F posAim;
	f32 radiusAvr = 0.0f;
	const Vector3F* posFirst = 0L;
	f32 lenDistBt = 0.0f;
	s32 cnt = 0;
	for(int i = 0; i < 3; i++)
	{
		if(uarr[i] == 0L)	{	continue;	}
		cnt++;
		const Vector3F* pos = uarr[i]->getCenterPos();
		if(posFirst == 0L)
		{
			// 最初のターゲットはそのものを注視
			posAim.copy(pos);
			posFirst = pos;
			radiusAvr = uarr[i]->getRadius();
		}
		else
		{
			// 中間地点を注視
			/*
			posAim._v[0] = (posAim._v[0] * 0.5f) + (pos->_v[0] * 0.5f);
			posAim._v[1] = (posAim._v[1] * 0.5f) + (pos->_v[1] * 0.5f);
			posAim._v[2] = (posAim._v[2] * 0.5f) + (pos->_v[2] * 0.5f);
			*/
			posAim.add(pos);
			radiusAvr += uarr[i]->getRadius();

			// 距離に応じてカメラの高さを上げる
			f32 lenlen = (*pos - *posFirst).lengthSq();
			f32 len = ::sqrtf(lenlen);
			len += lenlen * 0.125f;	// 遠いほど大きく離れるため、二乗の値を少し影響させる
			if(lenDistBt < len)	{	lenDistBt = len;	}
		}
	}

	if(cnt > 0)
	{
		f32 s = 1 / (f32)cnt;
		posAim *= s;
		radiusAvr *= s;
	}
	else	// 注視物がなかったら適当にプレイヤーキャラクタを注視する
	{
		Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
		for(int ipc = 0; ipc < party->getPcNum(); ipc++)
		{
			const CharUnit* unitPc = party->getPcUnit(ipc);	ASSERT(unitPc != 0L);
			if((unitPc != 0L) && unitPc->isEnable())
			{
				posAim.copy(unitPc->getCenterPos());
				radiusAvr = unitPc->getRadius();
				break;
			}
		}
		while(false);
	}

	// 上部に UI があるため全体的に絵を下にさげる．つまり、注視点を上げる
	posAim.y() += radiusAvr;
	posAim.z() -= radiusAvr;

	// カメラ位置
	Vector3F posCam(posAim);
	posCam.x() += xCamAimDiff;
	posCam.y() += yCam + (lenDistBt * RATIO_LEN_TO_Y_ZOOM);
	///STRACE("TCY: %f <- %f +\n %f (%f)\n", posCam.y(), yCam, (lenDistBt * RATIO_LEN_TO_Y_ZOOM), posAim.y());
	posCam.z() += zCamAimDiff;
	_locTrg->copy(&posCam);

	// カメラ注視位置
	_aimTrg->copy(&posAim);
}

/*---------------------------------------------------------------------*//**
	有効化・無効化通知
**//*---------------------------------------------------------------------*/
void ConfrontCam::notifyVisibleChange(bool isVisibleNew)
{
	// ユーザーズームのリセット
	if(isVisibleNew)
	{
		_zZoomOfsCur = 0.0f;
		_zZoomOfsTrg = 0.0f;
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
