/***********************************************************************//**
 *	FieldItemUnit.cpp
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
#include "FieldItemUnit.h"

// Friends
#include "../char/CharUnitSaveStructure.h"
#include "../../common/CalcUtils.h"
#include "../../body/Game.h"
#include "../../body/GameRenderCtx.h"
#include "../../map/PlacementObj.h"
#include "../../particle/GameParticleDef.h"
#include "../../status/CharStat.h"
#include "../../status/EleneStat.h"

// External
#include "../../../etk/particle/Particle.h"
#include "../../../etk/particle/ParticleSys.h"
#include "../../../tfw/gcmn/Renderer.h"
#include "../../../tfw/gcmn/RendererUtils.h"
#include "../../../tfw/lib/Vector.h"

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
// FieldItemUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	制御用イベント ID を得る
**//*---------------------------------------------------------------------*/
u16 FieldItemUnit::getCtrlEvid() const
{
	if(_pobjRef == 0L)	{	return 0;	}
	return _pobjRef->getCtrlEvid();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FieldItemUnit::FieldItemUnit()
	: ItemUnit(UNITTYPE_FIELD_ITEM)
	, _idxCurGcolPlane(-1)
	, _fcntShow(0.0f)
	, _eestat(0L)
	, _pobjRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FieldItemUnit::~FieldItemUnit()
{
	ASSERT(_eestat == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool FieldItemUnit::create(u16 itmdid, const EleneStat* eestatDefault, bool isHasCircleShadow)
{
	// エネルギー状態を作成
	_eestat = new EleneStat();

	// 瞬きのタイミングを乱数化
	_fcntShow = (f32)FRAME_BLINK_MIN + (f32)(FRAME_BLINK_MAX - FRAME_BLINK_MIN) * CalcUtils::randF();

	// 親クラスによる作成
	return ItemUnit::create(itmdid, _eestat, UNITFLAG_FIELD_ITEM | UNITFLAG_DEPEND_MAP);	// マップ依存かつフォーカス可能である
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void FieldItemUnit::destroy()
{
	// エネルギー状態を削除
	if(_eestat != 0L)
	{
		delete _eestat;
		_eestat = 0L;
	}

	ItemUnit::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void FieldItemUnit::exec(ExecRes* res, const ExecCtx* ec)
{
	ItemUnit::exec(res, ec);

	if(isEnable())
	{
		// 瞬き処理
		if(_fcntShow <= 0.0f)
		{
			_fcntShow = (f32)FRAME_BLINK_MIN + (f32)(FRAME_BLINK_MAX - FRAME_BLINK_MIN) * CalcUtils::randF();

			// パーティクル表示
			Vector3F pos(*getPosition());
			pos.y() += 20.0f;	// 少し地面から浮かす
			ParticleSys* ptclsys = Game::getGame()->getParticleSys(); ASSERT(ptclsys != 0L);
			u16 ptclid = ptclsys->appearParticle(GameParticleDef::PTCLK_FILD_ITEM_BLINK, &pos, 0);
			if(ptclid != 0)
			{
				Particle* ptcl = ptclsys->getParticle(ptclid);
				if(ptcl != 0L)
				{
					ptcl->setScale(1.0f + CalcUtils::randF());	// 1.0f ～ 2.0f
				}
			}
		}
		else
		{
			_fcntShow -= ec->getDeltaFrame();
		}

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
void FieldItemUnit::render(const RenderCtx* rc)
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
			vtx[0].copy(_pos);
			vtx[1].set(vtx[0].x() - 20.0f * fcos, vtx[0].y() + 50.0f, vtx[0].z() - 20.0f * fsin);
			vtx[2].set(vtx[0].x() + 20.0f * fcos, vtx[0].y() + 50.0f, vtx[0].z() + 20.0f * fsin);
			RendererUtils::render3dTriangles(rdr, vtx, 0L, 0L, 0L, 3, RendererUtils::TRIANGLES);

			rdr->setCulling(true);

		#elif 0

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
	セーブストラクチャに保存する
**//*---------------------------------------------------------------------*/
bool FieldItemUnit::storeToStructure(CharUnitSaveStructure* svst) const
{
	ASSERT(sizeof(CharUnitSaveStructure) == CharUnitSaveStructure::SIZE_THIS);

	svst->_ctrlevid = getCtrlEvid();
	if(!Unit::storeToStructure(&svst->_usvst))					{	return false;	}
	if(!_eestat->storeToStructure(&svst->_cssvst._eessvst))		{	return false;	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャから読み込む
**//*---------------------------------------------------------------------*/
bool FieldItemUnit::restoreFromStructure(const CharUnitSaveStructure* svst)
{
	ASSERT(sizeof(CharUnitSaveStructure) == CharUnitSaveStructure::SIZE_THIS);
	
	ASSERT(getCtrlEvid() == svst->_ctrlevid);
	if(!Unit::restoreFromStructure(&svst->_usvst))				{	return false;	}
	if(!_eestat->restoreFromStructure(&svst->_cssvst._eessvst))	{	return false;	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
