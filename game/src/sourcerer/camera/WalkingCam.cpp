/***********************************************************************//**
 *	WalkingCam.cpp
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
#include "WalkingCam.h"

// Friends
#include "GameCustomCamClass.h"
#include "../common/VelocityInterpolation.h"
#include "../event/EventSys.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../body/Party.h"
#include "../gui/GameGui.h"
#include "../gui/MoveCursor.h"
#include "../gui/MoveCursorExtension.h"
#include "../map/Map.h"
#include "../unit/UnitManager.h"
#include "../unit/char/pc/PcUnit.h"

// External
#include "../../etk/sound/SoundMngr.h"
#include "../../tfw/ui/TouchPanel.h"
#include "../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define	Y_ZOOM_INVALID				(0.0f)		// 無効なズーム高さ
#define	Y_ZOOM_MIN_FIELD			(100.0f)	// 屋外時のズーム高さの最小値
#define	Y_ZOOM_MAX_FIELD			(10000.0f)	// 屋外時のズーム高さの最大値
#define	Y_ZOOM_ZOOM_FIELD			(600.0f)	// 屋外時の拡大ズーム高さのしきい値
#define	Y_ZOOM_MIN_INDOOR			(400.0f)	// 屋内時のズーム高さの最小値
#define	Y_ZOOM_MAX_INDOOR			(2700.0f)	// 屋内時のズーム高さの最大値
#define	Y_ZOOM_DEFAULT_FIELD		(4000.0f)	// 屋外時のデフォルトのズーム高さ
#define	Y_ZOOM_DEFAULT_INDOOR		(2000.0f)//(700.0f)	// 屋内時のデフォルトのズーム高さ
#define Y_ZOOM_MAX_STABLE			(5000.0f)	// 定常状態の最大ズーム高さ
#define Z_CHAR_CAM_OFS_FIELD		(5200.0f)	// 屋外時のキャラ位置に対するカメラの Z 位置差
#define Z_CHAR_CAM_OFS_MIN_FIELD	(1800.0f)	// 屋外時のキャラ位置に対するカメラの Z 位置差
#define Z_CHAR_CAM_OFS_INDOOR		(1800.0f)	// 屋内時のキャラ位置に対するカメラの Z 位置差
#define Y_AIM_CHAR_DIFF_INDOOR		(50.0f)		// 屋内時のキャラ位置に対するカメラの注視 Y 位置差
#define RATIO_ZOOM					(1.5f)		// タッチ座標に対するズーム率

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// WalkingCam メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ズーム距離設定
**//*---------------------------------------------------------------------*/
void WalkingCam::setZoom(f32 zoom)
{
	_yZoomCur = zoom;
	_vyZoom = 0.0f;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
WalkingCam::WalkingCam()
	: _locTrg(0L)
	, _aimTrg(0L)
	, _velLoc(0L)
	, _velAim(0L)
	, _vintpLoc(0L)
	, _vintpAim(0L)
	, _yZoomCur(Y_ZOOM_INVALID)
	, _yZoomMin(0.0f)
	, _yZoomMax(0.0f)
	, _yZoomDefault(0.0f)
	, _vyZoom(0.0f)
	, _isOutRangeY(false)
	, _isOutCtrl(false)
	, _isReverseWhenReturnState(false)
	, _state(STATE_STABLE)
{
	_camclass = GameCustomCamClass::CAMCLS_WALKING;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
WalkingCam::~WalkingCam()
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
bool WalkingCam::create()
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

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void WalkingCam::destroy()
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
void WalkingCam::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!isVisible())	{	return;	}

	if(_isOutCtrl)
	{
		LookAtCamera::exec(res, ec);
		// 設定クリア．毎フレーム設定する必要がある
		_isOutCtrl = false;
		// 制御が戻った時に急にジャンプしないように補間値をコピーし続ける
		_loc->copy(_locTrg);
		_aim->copy(_aimTrg);
		return;
	}

	Map* map = Game::getGame()->getMap();
	const MapConfig* mconf = map->getMapConfig();

	// ズーム処理
	f32 yZoomBk = _yZoomCur;
	bool isZoomAct = false;
	bool isMaxZoomY = (_yZoomCur == _yZoomMax);
	TouchPanel* ui = ((GameExecCtx*)ec)->getTouchPanel();
	switch(ui->getLastAction())
	{
	case TouchPanel::ACT_ZOOM:
		{
			f32 rCur, rPrev;
			ui->getZoomParam(0L, 0L, &rCur, &rPrev);
			f32 rateH = (_yZoomCur - _yZoomMin) / (_yZoomMax - _yZoomMin);
			if(rateH < 0.1f)	{	rateH = 0.1f;	}
			_yZoomCur -= (rCur - rPrev) * RATIO_ZOOM * rateH;	// 高い方が変化量が多いように調節
			_vyZoom = _yZoomCur - yZoomBk;
			isZoomAct = true;
			if(!_isReverseWhenReturnState && (rCur < rPrev))
			{
				_isReverseWhenReturnState = true;
			}
			ui->setReacted(true);	// 反応済みフラグを立てる
		}
		break;
	default:
		if(_state == STATE_STABLE)
		{
			// 速度更新
			_vyZoom *= 0.75f;
			_yZoomCur += _vyZoom;
		}
		else
		{
			_vyZoom = 0.0f;
		}
		break;
	}

	// 状態処理
	switch(_state)
	{
	case STATE_STABLE:	// 定常
		if(_yZoomCur > Y_ZOOM_MAX_STABLE)	// 定常最大高さを超えた
		{
			if((_yZoomCur - Y_ZOOM_MAX_STABLE) >= ((_yZoomMax - Y_ZOOM_MAX_STABLE) * 0.5f))
			{
				_state = STATE_LEAVING;	// 離脱へ
			}
			else
			{
				_state = STATE_RETURN;	// 復帰へ
			}
		}
		break;
	case STATE_LEAVING:	// 離脱中
		if(_yZoomCur < yZoomBk)			// 離脱中に戻ろうとした
		{
			_state = STATE_RETURN;	// 復帰へ
		}
		else if(ui->getTouchingNum() <= 1)
		{
			// 自動離脱
///			_yZoomCur = (_yZoomCur * 0.8f) + (_yZoomMax * 0.2f) + 5.0f;
			_yZoomCur = (_yZoomCur * 0.9f) + (_yZoomMax * 0.1f) + 2.0f;
		}
		break;
#if 0
	case STATE_RETURN:	// 復帰中
		if(_yZoomCur < _yZoomDefault)	// デフォルト位置まで自動処理
		{
			_state = STATE_STABLE;	// 定常へ
		}
		else
		{
			if((_yZoomCur != yZoomBk) && (_yZoomCur < Y_ZOOM_MAX_STABLE))	// 定常最大高さ内でユーザーが操作した
			{
				_state = STATE_STABLE;	// 定常へ
			}
			else if(_yZoomCur > yZoomBk)	// 復帰中に離れようとした
			{
				_state = STATE_LEAVING;	// 離脱へ
			}
			else if(ui->getTouchingNum() <= 1)
			{
				// 自動復帰
				_yZoomCur = (_yZoomCur * 0.9f) + (_yZoomDefault * 0.1f);
			}
		}
		break;
#else
	case STATE_RETURN:	// 復帰中
		if(_yZoomCur < _yZoomDefault)	// デフォルト位置まで自動処理
		{
			_state = STATE_STABLE;	// 定常へ
		}
		else if(isZoomAct && (_yZoomCur < Y_ZOOM_MAX_STABLE))	// 定常最大高さ内でユーザーが操作した
		{
			_state = STATE_STABLE;	// 定常へ
		}
		else if(_yZoomCur > yZoomBk)	// 復帰中に離れようとした
		{
			_state = STATE_LEAVING;	// 離脱へ
		}
		else if(isZoomAct && _isReverseWhenReturnState)	// 位置を調整しようとした
		{
			_state = STATE_SLOW_RETURN;	// 低速復帰へ
		}
		else if(ui->getTouchingNum() <= 1)
		{
			///_yZoomCur = (_yZoomCur * 0.8f) + (_yZoomDefault * 0.2f);	// 自動復帰
			///_yZoomCur = (_yZoomCur * 0.975f) + (_yZoomDefault * 0.025f);	// 自動復帰
			_yZoomCur = (_yZoomCur * 0.95f) + (_yZoomDefault * 0.05f);	// 自動復帰
		}
		break;
	case STATE_SLOW_RETURN:	// 低速復帰中
		if(_yZoomCur < _yZoomDefault)	// デフォルト位置まで自動処理
		{
			_state = STATE_STABLE;	// 定常へ
		}
		else if(isZoomAct && (_yZoomCur < Y_ZOOM_MAX_STABLE))	// 定常最大高さ内でユーザーが操作した
		{
			_state = STATE_STABLE;	// 定常へ
		}
		else if(ui->getTouchingNum() <= 1)
		{
			_yZoomCur = (_yZoomCur * 0.99f) + (_yZoomDefault * 0.01f);	// 自動復帰（低速）
		}
		break;
#endif
	}

	// ズーム範囲クリップと離脱
	if(_yZoomCur < _yZoomMin)
	{
		_yZoomCur = _yZoomMin;
	}
	else if(_yZoomCur > _yZoomMax)
	{
		_yZoomCur = _yZoomMax;

		if(isMaxZoomY)	// 最大高さの位置から更に上に上がろうとした
		{
			EventSys* evsys = Game::getGame()->getEventSys();
			if(	(mconf->_mapkind != MapConfig::MAPKIND_INDOOR) &&	// ただし屋内はマップに移れない
			   	!evsys->isNoWorldMap() &&	// ワールドマップ禁止時は移れない
			   	!evsys->isBegunEvent()	)	// イベント起動時は移れない　あ
			{
				_isOutRangeY = true;
			}
		}
	}

	// カメラの目標位置を計算する
	calcTargetPos(mconf);

#if 1	// 補完
	if(getVisibleCounter() <= 2)	// モード変更時、または有効化直後
	{
		// 補間しない
		_loc->copy(_locTrg);
		_aim->copy(_aimTrg);
	}
	else
	{
		// 補間する
		_vintpLoc->interpolation(_loc, _velLoc, _locTrg, ec->getDeltaFrame());
		_vintpAim->interpolation(_aim, _velAim, _aimTrg, ec->getDeltaFrame());
	}
#else
	// ターゲット座標へ近づける速度ベクトルと位置を求める
//	const f32 RATE_PREV_VEL		= 0.3f;
//	const f32 RATE_DIFF_TO_VEL	= 0.3f;
//	const f32 RATE_PREV_VEL = 0.9f;
	const f32 RATE_DIFF_TO_VEL = 0.001f;
	for(int i = 0; i < 3; i++)
	{
		f32 diff = _loc->_v[i] - _locTrg->_v[i];
		if(TFW_IS_NEAR(diff, 0.0f, 0.001f))
		{
			_loc->copy(_locTrg);
		}
		else
		{
			// 速度更新
//			_velLoc->_v[i] = (_velLoc->_v[i] * RATE_PREV_VEL) - (diff * RATE_DIFF_TO_VEL);
	//		_velLoc->_v[i] += - (diff * RATE_DIFF_TO_VEL);
			diff = diff + (ec->getDeltaFrame() * _velLoc->_v[i]);
			// 位置更新
			if(_loc->_v[i] > _locTrg->_v[i])
			{
				if(diff < 0.0f)
				{
					_loc->_v[i] = _locTrg->_v[i];
					_velLoc->_v[i] = 0.0f;
				}
				else
				{	
					_loc->_v[i] = _locTrg->_v[i] + diff;
				}
			}
			else
			{
				if(diff > 0.0f)
				{
					_loc->_v[i] = _locTrg->_v[i];
					_velLoc->_v[i] = 0.0f;
				}
				else
				{
					_loc->_v[i] = _locTrg->_v[i] + diff;
				}
			}
		}
	}
#endif

	// サウンドリスナの設定
	Vector3F dirLisn = *_aim - *_loc;
	dirLisn.normalize();
	Game::getGame()->getSoundManager()->setListener(_loc, &dirLisn);

	LookAtCamera::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	有効化・無効化通知
**//*---------------------------------------------------------------------*/
void WalkingCam::notifyVisibleChange(bool isVisibleNew)
{
	if(isVisibleNew)
	{
		_loc->_v[0] = _locTrg->_v[0];
		_loc->_v[1] = _locTrg->_v[1];
		_loc->_v[2] = _locTrg->_v[2];
	}
	
	_isOutRangeY = false;	// 範囲外移動フラグのクリア
	toPreferablyStable();	// 定常状態へ
	calcValidPos();			// 有効な位置を計算する
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void WalkingCam::notifyChangedMap()
{
	toPreferablyStable();	// 定常状態へ
	calcValidPos();			// 有効な位置を計算する
}

/*---------------------------------------------------------------------*//**
	目標位置を計算する
**//*---------------------------------------------------------------------*/
void WalkingCam::calcTargetPos(const MapConfig* mconf)
{
	f32 zDiff;
	if(mconf->_mapkind == MapConfig::MAPKIND_INDOOR)
	{
		zDiff = Z_CHAR_CAM_OFS_INDOOR;
	}
	else
	{
		if(_yZoomCur < Y_ZOOM_ZOOM_FIELD)
		{
			/*
			f32 r = (_yZoomCur - Y_ZOOM_MIN_FIELD) / (Y_ZOOM_ZOOM_FIELD - Y_ZOOM_MIN_FIELD);
			zDiff = Z_CHAR_CAM_OFS_MIN_FIELD + ::sinf(r) * (Z_CHAR_CAM_OFS_FIELD - Z_CHAR_CAM_OFS_MIN_FIELD);
			*/
			zDiff = Z_CHAR_CAM_OFS_MIN_FIELD + (_yZoomCur - Y_ZOOM_MIN_FIELD) / (Y_ZOOM_ZOOM_FIELD - Y_ZOOM_MIN_FIELD) * (Z_CHAR_CAM_OFS_FIELD - Z_CHAR_CAM_OFS_MIN_FIELD);
		}
		else
		{
			zDiff = Z_CHAR_CAM_OFS_FIELD;
		}
	}

	// 操作キャラを得る
	/*
	MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor(); ASSERT(mvcsr != 0L);
	const MoveCursorExtension* mcex = mvcsr->getExtension(); ASSERT(mcex != 0L);
	const Unit* unitTarget = mcex->getHandledUnit();
	if(unitTarget == 0L)
	{
		unitTarget = Game::getGame()->getGameMyParty()->leaderPcUnit();
	}
	*/
	const Unit* unitTarget = Game::getGame()->getGameMyParty()->leaderPcUnit();

	// ターゲット座標を求める
	if(unitTarget != 0L)
	{
		// 注視点を設定する
		_aimTrg->copy(unitTarget->getCenterPos());
		if(mconf->_mapkind == MapConfig::MAPKIND_INDOOR)	{ _aimTrg->y() += Y_AIM_CHAR_DIFF_INDOOR; }

		// カメラの目標位置を求める
		Vector3F posCam(_aimTrg);
		posCam.y() += _yZoomCur;
		posCam.z() += zDiff;
		_locTrg->copy(&posCam);
	}

	// マップによるカメラ移動最大最小値にクリップする
	/*
	if(_locTrg->x() < mconf->_xCamMoveMin)				{	_locTrg->x() = mconf->_xCamMoveMin;			}
	if(_locTrg->x() > mconf->_xCamMoveMax)				{	_locTrg->x() = mconf->_xCamMoveMax;			}
	if(_locTrg->z() < (mconf->_zCamMoveMin + zDiff))	{	_locTrg->z() = mconf->_zCamMoveMin + zDiff;	}
	if(_locTrg->z() > (mconf->_zCamMoveMax + zDiff))	{	_locTrg->z() = mconf->_zCamMoveMax + zDiff;	}
	*/
}

/*---------------------------------------------------------------------*//**
	有効な位置を計算する
**//*---------------------------------------------------------------------*/
void WalkingCam::calcValidPos()
{
	Map* map = Game::getGame()->getMap();
	const MapConfig* mconf = map->getMapConfig();

	// ズーム高さを最大・最小を求める
	switch(mconf->_mapkind)
	{
	default:
		_yZoomMin = Y_ZOOM_MIN_FIELD;
		_yZoomMax = Y_ZOOM_MAX_FIELD;
		_yZoomDefault = Y_ZOOM_DEFAULT_FIELD;
		// 現ズーム値を範囲内に収める
		if(_yZoomCur == Y_ZOOM_INVALID)	{ _yZoomCur = _yZoomDefault; }
		else if(_yZoomCur > _yZoomMax)	{ _yZoomCur = _yZoomMax; }
		else if(_yZoomCur < _yZoomMin)	{ _yZoomCur = _yZoomMin; }
		break;
	case MapConfig::MAPKIND_INDOOR:
		_yZoomMin = Y_ZOOM_MIN_INDOOR;
		_yZoomMax = Y_ZOOM_MAX_INDOOR;
		_yZoomDefault = Y_ZOOM_DEFAULT_INDOOR;
		// 現ズーム値をデフォルト値に設定する
		_yZoomCur = _yZoomDefault;
		break;
	}

	// カメラの目標位置を計算し、そこにぴったり合わせる
	calcTargetPos(mconf);
	*_loc = *_locTrg;
	_velLoc->set(0.0f, 0.0f, 0.0f);
}

/*---------------------------------------------------------------------*//**
	定常状態へ
**//*---------------------------------------------------------------------*/
void WalkingCam::toPreferablyStable()
{
	if(_yZoomCur <= Y_ZOOM_MAX_STABLE)
	{
		_state = STATE_STABLE;
	}
	else
	{
		_state = STATE_RETURN;
		_isReverseWhenReturnState = false;
	}
	_vyZoom = 0.0f;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
