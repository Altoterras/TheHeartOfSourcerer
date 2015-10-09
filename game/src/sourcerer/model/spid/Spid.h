/***********************************************************************//**
 *	Spid.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_SPID_SPID_H_
#define _SRCR_EPISODE_SPID_SPID_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ETK_BEGIN_NS

class SorSgm;

ETK_END_NS

TFW_BEGIN_NS

template<class TYPE> class List;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class HrAnimObjCtx;
class Model;
class SgNode;
class Shape;
class ShapeModel;

TFW_END_NS

SRCR_BEGIN_NS

class Unit;

/*---------------------------------------------------------------------*//**
 *	SPID （指定制御 ID）
 *
**//*---------------------------------------------------------------------*/
class Spid
{
	//======================================================================
	// 定数
public:
	// SPID 大カテゴリ
	const static u16	SPID_LC_CHAR_HEAD__START_	= 20000;	// キャラクタの頭部の開始
	const static u16	SPID_LC_CHAR_HEAD__END_		= 29999;	// キャラクタの頭部の終了

	//======================================================================
	// クラス
public:
	class EnumRes
	{
	public:
		Shape* _shape;
		HrAnimObjCtx* _haobjctx;
		u16 _spid;

		EnumRes() : _shape(0L), _haobjctx(0L), _spid(0) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool find(HrAnimObjCtx** haobjctx, u16 spid, SorSgm* sgm);
	static bool find(HrAnimObjCtx** haobjctx, u16 spid, Unit* unit);
	static bool find(HrAnimObjCtx** haobjctx, u16 spid, Model* mdl);
	static bool find(Model** mdl, u16 spid, SorSgm* sgm);
	static void enumerateHrAnimObjCtx(List<EnumRes*>* listRes, Model* mdl);
	static bool findRoot(HrAnimObjCtx** haobjctx, Model* mdl);

private:
	static bool findDive(SgNode** sgnFound, Shape** shapeFound, u16 spid, SgNode* sgnCur);
	static bool findShape(Shape** shapeFound, u16 spid, Model* mdl);
	static bool findHrAnimObjCtxFromModel(HrAnimObjCtx** haobjctx, Shape* shapeSpid, ShapeModel* smdl);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_SPID_SPID_H_
