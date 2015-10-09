/***********************************************************************//**
 *	CalcUtils.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/31.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CalcUtils.h"

// Friends
#include "../body/Game.h"
#include "../unit/Unit.h"
#include "../unit/UnitManager.h"

// External
#include "../../etk/sg/SorSgm.h"
#include "../../tfw/Tfw.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/View.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/lib/QuickSort.h"
#include "../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CalcUtils 静的メンバ変数

//==========================================================================
// CalcUtils メソッド

/*---------------------------------------------------------------------*//**
	0.0f ～ 1.0f の乱数値を得る
**//*---------------------------------------------------------------------*/
f32 CalcUtils::randF()
{
	#if defined(_DEBUG)
		f32 rf = ::rand() / (f32)RAND_MAX;
		return rf;
	#else
		return ::rand() / (f32)RAND_MAX;
	#endif
}

/*---------------------------------------------------------------------*//**
	-1.0f ～ 1.0f の乱数値を得る
**//*---------------------------------------------------------------------*/
f32 CalcUtils::randPlusMinusOneF()
{
	#if defined(_DEBUG)
		f32 rf = (((::rand() / (f32)RAND_MAX) * 2.0f) - 1.0f);
		return rf;
	#else
		return (((::rand() / (f32)RAND_MAX) * 2.0f) - 1.0f);
	#endif
}

/*---------------------------------------------------------------------*//**
	3D 座標位置をスクリーン座標位置に変換する
**//*---------------------------------------------------------------------*/
bool CalcUtils::calc3dPosToScreenPos(Vector2F* posScr, const Vector3F* pos3d)
{
	// 現在のビュー・カメラを得る
	Tfw* tfw = Game::getGame()->getTfw();			if(tfw == 0L)	{	return false;	}
	View* view = tfw->getView();					if(view == 0L)	{	return false;	}
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();		if(sgm == 0L)	{	return false;	}
	Camera* cam = sgm->getVisibleCamera();			if(cam == 0L)	{	return false;	}

	// スクリーン座標に変換
	Gcalc::conv3dPosToScreenPos(posScr, pos3d, cam, view);

	return true;
}

/*---------------------------------------------------------------------*//**
	3D 座標位置をスクリーン座標位置に変換する（配列でまとめて計算）
**//*---------------------------------------------------------------------*/
bool CalcUtils::calc3dPosToScreenPos(Vector2F* arrposScr, const Vector3F* arrpos3d, s32 numArr)
{
	// 現在のビュー・カメラを得る
	Tfw* tfw = Game::getGame()->getTfw();			if(tfw == 0L)	{	return false;	}
	View* view = tfw->getView();					if(view == 0L)	{	return false;	}
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();		if(sgm == 0L)	{	return false;	}
	Camera* cam = sgm->getVisibleCamera();			if(cam == 0L)	{	return false;	}

	// スクリーン座標に変換
	Gcalc::conv3dPosToScreenPos(arrposScr, arrpos3d, numArr, cam, view);

	return true;
}

/*---------------------------------------------------------------------*//**
	スクリーン座標位置を 3D 座標位置に変換する
**//*---------------------------------------------------------------------*/
bool CalcUtils::calcScreenPosTo3dPos(Vector3F* pos3d, const Vector2F* posScr, const f32 zScrPos)
{
	// 現在のビュー・カメラを得る
	Tfw* tfw = Game::getGame()->getTfw();			if(tfw == 0L)	{	return false;	}
	View* view = tfw->getView();					if(view == 0L)	{	return false;	}
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();		if(sgm == 0L)	{	return false;	}
	Camera* cam = sgm->getVisibleCamera();			if(cam == 0L)	{	return false;	}

	// スクリーン座標に変換
	Gcalc::convScreenPosTo3dPos(pos3d, posScr, zScrPos, cam, view);

	return true;
}

/*---------------------------------------------------------------------*//**
	スクリーン座標位置を 3D 座標位置に変換する（配列でまとめて計算）
**//*---------------------------------------------------------------------*/
bool CalcUtils::calcScreenPosTo3dPos(Vector3F* arrpos3d, const Vector2F* arrposScr, const f32* arrzScrPos, s32 numArr)
{
	// 現在のビュー・カメラを得る
	Tfw* tfw = Game::getGame()->getTfw();			if(tfw == 0L)	{	return false;	}
	View* view = tfw->getView();					if(view == 0L)	{	return false;	}
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();		if(sgm == 0L)	{	return false;	}
	Camera* cam = sgm->getVisibleCamera();			if(cam == 0L)	{	return false;	}

	// スクリーン座標に変換
	Gcalc::convScreenPosTo3dPos(arrpos3d, arrposScr, arrzScrPos, numArr, cam, view);

	return true;
}

/*---------------------------------------------------------------------*//**
	スクリーン座標位置と 3D Z 座標位置を 3D 座標位置に変換する
**//*---------------------------------------------------------------------*/
bool CalcUtils::calcScreenPosAnd3dZTo3dPos(Vector3F* pos3d, const Vector2F* posScr, const f32 z3dPos)
{
	// 現在のビュー・カメラを得る
	Tfw* tfw = Game::getGame()->getTfw();			if(tfw == 0L)	{	return false;	}
	View* view = tfw->getView();					if(view == 0L)	{	return false;	}
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();		if(sgm == 0L)	{	return false;	}
	Camera* cam = sgm->getVisibleCamera();			if(cam == 0L)	{	return false;	}

	// スクリーン座標に変換
	Gcalc::convScreenPosAnd3dZTo3dPos(pos3d, posScr, z3dPos, cam, view);

	return true;
}

/*---------------------------------------------------------------------*//**
	スクリーン座標位置と 3D Z 座標位置を 3D 座標位置に変換する
	（配列でまとめて計算）
**//*---------------------------------------------------------------------*/
bool CalcUtils::calcScreenPosAnd3dZTo3dPos(Vector3F* arrpos3d, const Vector2F* arrposScr, const f32* arrz3dPos, s32 numArr)
{
	// 現在のビュー・カメラを得る
	Tfw* tfw = Game::getGame()->getTfw();			if(tfw == 0L)	{	return false;	}
	View* view = tfw->getView();					if(view == 0L)	{	return false;	}
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();		if(sgm == 0L)	{	return false;	}
	Camera* cam = sgm->getVisibleCamera();			if(cam == 0L)	{	return false;	}

	// スクリーン座標に変換
	Gcalc::convScreenPosAnd3dZTo3dPos(arrpos3d, arrposScr, arrz3dPos, numArr, cam, view);

	return true;
}

/*---------------------------------------------------------------------*//**
	Z 方向に開いた扇状の範囲内のユニットを集める

	@retval radiusRangeSq 範囲半径の二乗値
	@retval cosDotRange dir に対しこの内積値以内のものを対象とする
		例えば、前方半円形であれば ±cos 90°なので cosDotRange = 0.0f
		前方四分円扇型であれば ±cos 45°なので cosDotRange = 0.7071...f
**//*---------------------------------------------------------------------*/
void CalcUtils::collectUnitFanRange(List<Unit*>* listUnits, const Vector3F* center, f32 angleDirY, u16 unitflagsTrg, f32 radiusRangeSq, f32 cosDotRange)
{
	Vector3F dir(0.0f, 0.0f, 1.0f);
	Matrix4F mtxRot;
	mtxRot.rotateY(angleDirY);
	mtxRot.mult(&dir, &dir);

	UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
	for(s32 i = 0; i < unitmng->getUnitNum(); i++)
	{
		Unit* unitWk = unitmng->getUnitFromIndex(i);
		if(unitWk->isEnable() && TFW_IS_FLAG(unitWk->getUnitFlags(), unitflagsTrg))
		{
			Vector3F diff = *unitWk->getPosition() - *center;
			// 距離判定
			f32 lenDiffSq = diff.lengthSq();
			if(lenDiffSq <= radiusRangeSq)
			{
				diff.normalize();
				f32 dotDirAndUnit = dir.dot(&diff);	// 内積を求める
				if(cosDotRange <= dotDirAndUnit)
				{
					listUnits->addTail(unitWk);
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	円形の範囲内のユニットを集める

	@retval radiusRangeSq 範囲半径の二乗値
**//*---------------------------------------------------------------------*/
void CalcUtils::collectUnitRoundRange(List<Unit*>* listUnits, const Vector3F* center, u16 unitflagsTrg, f32 radiusRangeSq)
{
	UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
	for(s32 i = 0; i < unitmng->getUnitNum(); i++)
	{
		Unit* unitWk = unitmng->getUnitFromIndex(i);
		if(unitWk->isEnable() && TFW_IS_FLAG(unitWk->getUnitFlags(), unitflagsTrg))
		{
			Vector3F diff = *unitWk->getPosition() - *center;
			// 距離判定
			f32 lenDiffSq = diff.lengthSq();
			if(lenDiffSq <= radiusRangeSq)
			{
				listUnits->addTail(unitWk);
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	リスト化されたユニットを距離の昇順でソートする
**//*---------------------------------------------------------------------*/
void CalcUtils::sortUnitArrayByDistanceAscending(List<Unit*>* listUnits, const Vector3F* center)
{
	class SortData
	{
	public:
		Unit* _unit;
		f32 _distSq;
		SortData() : _unit(0L), _distSq(0.0f) {}
	};

	s32 cnt = 0;
	SortData* sdarr = new SortData[listUnits->getCount()];
	for(ListIterator<Unit*> it = listUnits->iterator(); it.has(); it.next())
	{
		sdarr[cnt]._unit = it.object();
		sdarr[cnt]._distSq = (*it.object()->getPosition() - *center).lengthSq();
		cnt++;
	}
	ASSERT(cnt == listUnits->getCount());

	class QuickSortForUnitArrayByDistanceAscending : public QuickSort
	{
		s32 compare(void* arr, s32 unitsize, s32 idxA, s32 idxB)
		{
			const SortData* a = (SortData*)((addr_size)arr + (unitsize * idxA));
			const SortData* b = (SortData*)((addr_size)arr + (unitsize * idxB));
			if(b->_distSq < a->_distSq)	{	return 1;	}
			if(b->_distSq > a->_distSq)	{	return -1;	}
			return 0;
		}
		void swap(void* arr, s32 unitsize, s32 idxA, s32 idxB)
		{
			SortData* a = (SortData*)((addr_size)arr + (unitsize * idxA));
			SortData* b = (SortData*)((addr_size)arr + (unitsize * idxB));
			Unit* unitTmp = a->_unit;
			f32 distSqTmp = a->_distSq;
			a->_unit = b->_unit;
			a->_distSq = b->_distSq;
			b->_unit = unitTmp;
			b->_distSq = distSqTmp;
		}
	};

	QuickSortForUnitArrayByDistanceAscending qsort;
	qsort.run(sdarr, sizeof(SortData), cnt);

	listUnits->removeAll();

	for(int i = 0; i < cnt; i++)
	{
		listUnits->addTail(sdarr[i]._unit);
	}

	delete[] sdarr;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
