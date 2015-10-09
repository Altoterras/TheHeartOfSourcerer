/***********************************************************************//**
 *	PutItemUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PutItemUnit.h"

// Friends
#include "../../item/ItemDefTbl.h"
#include "../../body/Game.h"
#include "../../body/GameRenderCtx.h"
#include "../../gui/GameGui.h"
#include "../../model/CircleShadow.h"
#include "../../map/PlacementObj.h"
#include "../../status/CharStat.h"
#include "../../status/EleneStat.h"

// External
#include "../../../etk/sg/SorSgm.h"
#include "../../../tfw/common/ResourcePool.h"
#include "../../../tfw/gcmn/Gcalc.h"
#include "../../../tfw/gcmn/Renderer.h"
#include "../../../tfw/gcmn/RendererUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// テクスチャ情報
#define W_TEX				1024.0f
#define H_TEX				1024.0f
#define HW_MAP_MARK			20.0f
#define H_MAP_MARK			80.0f
#define UV_U_MAP_MARK		480
#define UV_V_MAP_MARK		960
#define UV_W_MAP_MARK		32
#define UV_H_MAP_MARK		64

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PutItemUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PutItemUnit::PutItemUnit()
	: ItemUnit(UNITTYPE_PUT_ITEM)
	, _mdlCrcShdw(0L)
	, _idxCurGcolPlane(-1)
	, _eestat(0L)
	, _pobjRef(0L)
	, _thetaMapMark(0.0f)
{
	TFW_ZERO_MEMORY(_uvarrMapMark, sizeof(f32) * 8);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PutItemUnit::~PutItemUnit()
{
	ASSERT(_mdlCrcShdw == 0L);
	ASSERT(_eestat == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PutItemUnit::create(u16 itmdid, const EleneStat* eestatDefault, bool isHasCircleShadow)
{
	// エネルギー状態を作成
	_eestat = new EleneStat();
	// 既定の五元素状態を設定
	if(eestatDefault != 0L)
	{
		_eestat->copy(eestatDefault);
	}

	// 丸影作成
	if(isHasCircleShadow)
	{
		CircleShadow* mdlCrcShdw = new CircleShadow();
		if(mdlCrcShdw->create())
		{
			mdlCrcShdw->setUsed(true);	// 使用中フラグをセット

			// リソースプールに追加（リソース削除の管理を委譲）
			ResourcePool* resp = Game::getGame()->getResourcePool();
			resp->add((SgEntity**)&mdlCrcShdw, 1);

			_mdlCrcShdw = mdlCrcShdw;	// 参照を保存しておく
		}
		else
		{
			delete mdlCrcShdw;
		}
	}

	// マップ記号テクスチャ座標
	RectF32 uv;
	Gcalc::calcTexUv(&uv, UV_U_MAP_MARK, UV_V_MAP_MARK, UV_W_MAP_MARK, UV_H_MAP_MARK, W_TEX, H_TEX, Env_get2drRate());
	_uvarrMapMark[0] = uv._v[0];			_uvarrMapMark[1] = - uv._v[1];
	_uvarrMapMark[2] = uv._v[0] + uv._v[2];	_uvarrMapMark[3] = - uv._v[1];
	_uvarrMapMark[4] = uv._v[0];			_uvarrMapMark[5] = - uv._v[1] - uv._v[3];
	_uvarrMapMark[6] = uv._v[0] + uv._v[2];	_uvarrMapMark[7] = - uv._v[1] - uv._v[3];

	// 半径補正
	_radius = 50.0f;	// 50cm

	// 親クラスによる作成
	if(!ItemUnit::create(itmdid, _eestat, UNITFLAG_PUT_ITEM | UNITFLAG_CREATION_FOCUSABLE | UNITFLAG_DEPEND_MAP))	// マップ依存かつフォーカス可能である
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PutItemUnit::destroy()
{
	// シーングラフから配置解除（配置されているときのみ）
	SorSgm* sgm = Game::getGame()->getSceneGraph();	ASSERT(sgm != 0L);
	#if 0
		for(int j = 0; j < getModelNum(); j++)
		{
			SgNode* sgnode = getModel(j);
			if(sgnode->isOnSg())
			{
				sgm->removeNode(sgnode);
			}
		}
	#endif

	// 丸影も配置解除
	if(_mdlCrcShdw != 0L)
	{
		if(!_mdlCrcShdw->isOnSg())
		{
			sgm->removeNode(_mdlCrcShdw);
		}
	}

	// エネルギー状態を削除
	if(_eestat != 0L)
	{
		delete _eestat;
		_eestat = 0L;
	}

	// 丸影を削除
	if(_mdlCrcShdw != 0L)
	{
		// ResourcePool に管理委譲しているので、未使用に設定するのみ
		_mdlCrcShdw->setUsed(false);

		// 丸影の参照をリセット
		_mdlCrcShdw = 0L;
	}

	ItemUnit::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void PutItemUnit::exec(ExecRes* res, const ExecCtx* ec)
{
	ItemUnit::exec(res, ec);

	if(isEnable())
	{
		// マップ記号の回転角を更新
		_thetaMapMark += 0.05f * ec->getDeltaFrame();
		if(_thetaMapMark > TFW_PI)	{	_thetaMapMark -= TFW_DPI;	}

		// エネルギー状態を配置情報に伝達
		if(_pobjRef != 0L)
		{
			_pobjRef->eleneStat()->copy(_cstat->getEleneStat());
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void PutItemUnit::render(const RenderCtx* rc)
{
	if(isEnable())
	{
		GameRenderCtx* grc = (GameRenderCtx*)rc;
		Renderer* rdr = rc->getRenderer();

		#if 0	// 黄色いソリッドな三角▼描画
		
			rdr->bindTexture(0L);
			rdr->setSolidColor(0, 255, 0, 255);
			rdr->setCulling(false);

			Vector3F vtx[3];
			f32 theta = grc->getFrameCount() * 0.1f;
			f32 fsin = ::sinf(theta);
			f32 fcos = ::cosf(theta);
			vtx[0].set(_pos);
			vtx[1].set(vtx[0].x() - 20.0f * fcos, vtx[0].y() + 50.0f, vtx[0].z() - 20.0f * fsin);
			vtx[2].set(vtx[0].x() + 20.0f * fcos, vtx[0].y() + 50.0f, vtx[0].z() + 20.0f * fsin);
			RendererUtils::render3dTriangles(rdr, vtx, 0L, 0L, 0L, 3, RendererUtils::TRIANGLES);

			rdr->setCulling(true);

		#else

			// テクスチャ設定
			rdr->bindTexture(Game::getGame()->getGui()->getGuiTexture());
			// 加算アルファ合成
			rdr->setAlphaBlendFunc(Renderer::AFUNC_ADD);
			// 背面カリングなし
			rdr->setCulling(false);

			// 回転する板ポリゴンを描画
			f32 fsin = ::sinf(_thetaMapMark);
			f32 fcos = ::cosf(_thetaMapMark);
			f32 xr = HW_MAP_MARK * fcos;
			f32 zr = HW_MAP_MARK * fsin;
			const Vector3F* center = _pos;
			Vector3F vtx[4];
			f32 x1 = center->x() - xr;
			f32 x2 = center->x() + xr;
			f32 z1 = center->z() - zr;
			f32 z2 = center->z() + zr;
			f32 y1 = center->y() + H_MAP_MARK;
			f32 y2 = center->y();
			vtx[0].set(x1, y1, z1);
			vtx[1].set(x2, y1, z2);
			vtx[2].set(x1, y2, z1);
			vtx[3].set(x2, y2, z2);
			RendererUtils::render3dTriangles(rdr, vtx, 0L, (const Vector2F*)_uvarrMapMark, 0L, 4, RendererUtils::TRIANGLE_STRIP);

			// 背面カリングあり
			rdr->setCulling(false);

		#endif
	}
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void PutItemUnit::notifyChangedMap()
{
	Unit::notifyChangedMap();

	// シーングラフに配置（配置されてないときのみ）
	SorSgm* sgm = Game::getGame()->getSceneGraph();	ASSERT(sgm != 0L);
	#if 0
		for(int j = 0; j < getModelNum(); j++)
		{
			SgNode* sgnode = getModel(j);
			if(!sgnode->isOnSg())
			{
				sgm->addNode(sgnode);
			}
		}
	#endif

	// 丸影も配置
	if(_mdlCrcShdw != 0L)
	{
		if(!_mdlCrcShdw->isOnSg())
		{
			sgm->addNode(_mdlCrcShdw);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
