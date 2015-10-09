/***********************************************************************//**
 *	WorldMap.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/08.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "WorldMap.h"

// Friends
#include "WorldMapStateUpdater.h"
#include "WorldMapStateUpdaterFactory.h"
#include "../../camera/WorldMapCam.h"
#include "../../episode/save/SaveStructure.h"
#include "../../file/GameFileIdDef.h"
#include "../../body/Game.h"
#include "../../body/GameExecCtx.h"
#include "../../body/GameFixedStrTbl.h"
#include "../../body/GameRenderCtx.h"
#include "../../body/MoveMap.h"
#include "../../gui/GameFontSet.h"
#include "../../gui/GameGui.h"
//#include "../../mode/GameMode.h"

// External
#include "../../../etk/sg/CamSgnc.h"
#include "../../../etk/sg/SorSgm.h"
#include "../../../tfw/Tfw.h"
#include "../../../tfw/collection/List.h"
#include "../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../tfw/gcmn/Texture.h"
#include "../../../tfw/gcmn/Renderer.h"
#include "../../../tfw/gcmn/RendererUtils.h"
#include "../../../tfw/gcmn/Gcalc.h"
#include "../../../tfw/lib/Point.h"
#include "../../../tfw/lib/Vector.h"
#include "../../../tfw/serialize/Serializer.h"
#include "../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define	Y_BASE		(200000.0f)	// 基本高さ
#define Y_DIFF_BG	(- 200.0f)	// 基本高さに対する背景の高さ
#define Y_DIFF_MAP	(- 200.0f)	// 基本高さに対するマップの高さ

// テクスチャ UV 値
#define W_TEX					(1024.0f)
#define H_TEX					(1024.0f)
#define UV_MAINEV_MARK			192.0f, 992.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_SUBEV_MARK			224.0f, 992.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_QUESTEV_START_MARK	256.0f, 992.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_QUESTEV_END_MARK		256.0f, 960.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_MARK_SHADOW			192.0f, 960.0f, 32.0f, 32.0f, W_TEX, H_TEX

////////////////////////////////////////////////////////////////////////////
// 静的変数

static const WorldMap::MapDef s_mapdefs[WorldMap::NUM_MAP_MAX] =
{
	WorldMap::MapDef(	10102,  802,  521,  24,  24, 0		),
	WorldMap::MapDef(	10181,  826,  529,   8,   8, 10102	),
	WorldMap::MapDef(	10241,  838,  527,  12,  12, 10181	),
	WorldMap::MapDef(	10201,  804,  569,  12,  12, 10222	),
	WorldMap::MapDef(	10221,  832,  583,  12,  12, 10222	),
	WorldMap::MapDef(	10222,  828,  561,  12,  12, 10243	),
	WorldMap::MapDef(	10223,  844,  554,  12,  12, 10222	),
	WorldMap::MapDef(	10301,  837,  602,  12,  12, 10221	),
	WorldMap::MapDef(	10302,  838,  621,  12,  12, 10301	),
	WorldMap::MapDef(	10303,  847,  651,  12,  12, 10302	),
	WorldMap::MapDef(	11101,  794,  587,  12,  12, 10221	),
	WorldMap::MapDef(	11102,  780,  587,  12,  12, 11101	),
	WorldMap::MapDef(	11103,  732,  587,  12,  12, 11102	),
	WorldMap::MapDef(	11104,  662,  589,  12,  12, 11103	),
	WorldMap::MapDef(	11111,  772,  603,  12,  12, 11102	),
	WorldMap::MapDef(	11112,  756,  563,  12,  12, 11103	),
	WorldMap::MapDef(	11113,  712,  636,  12,  12, 11103	),
	WorldMap::MapDef(	11114,  806,  657,  12,  12, 11111	),
	// 後から追加したマップは、セーブデータの整合性を保つために末尾に追加する
	WorldMap::MapDef(	10243,  832,  544,  12,  12, 10241	),
	#if defined(_DEBUG)
		WorldMap::MapDef(	19101, 16, 770, 14, 14,	10221	),
		WorldMap::MapDef(	19102, 32, 770, 14, 14, 10221	),
		WorldMap::MapDef(	19103, 48, 770, 14, 14, 10221	),
		WorldMap::MapDef(	19104, 64, 770, 14, 14, 10221	),
		WorldMap::MapDef(	19105, 80, 770, 14, 14, 10221	),
	#endif
};

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// WorldMap メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	隣接のマップ ID を取得
**//*---------------------------------------------------------------------*/
s32 WorldMap::getNeighborMapId(s32 mapidNew, s32 mapidOld)
{
	const MapDef* mapdefOld = 0L;
	for(int i = 0; i < WorldMap::NUM_MAP_MAX; i++)
	{
		if(s_mapdefs[i]._mapid == 0)
		{
			return -1;	// 定義無し
		}
		if(s_mapdefs[i]._mapid == mapidOld)
		{
			mapdefOld = &s_mapdefs[i];
			break;
		}
	}
	if(mapdefOld == 0L)	{	return -1;	}	// 見つからず

	// 上りで探す
	s32 mapidFind = getNeighborMapIdUp(mapidNew, mapidOld, mapdefOld);
	if(mapidFind != -1)
	{
		return mapidFind;
	}

	// 下りで探す
	mapidFind = getNeighborMapIdDown(mapidNew, mapidOld, mapdefOld);
	if(mapidFind != -1)
	{
		return mapidFind;
	}

	return -1;
}

/*---------------------------------------------------------------------*//**
	隣接のマップ ID を取得（上り再帰処理）
**//*---------------------------------------------------------------------*/
s32 WorldMap::getNeighborMapIdUp(s32 mapidNew, s32 mapidDiveFrom, const WorldMap::MapDef* mapdef)
{
	for(int i = 0; i < WorldMap::NUM_MAP_MAX; i++)
	{
		const WorldMap::MapDef* mapdefWk = &s_mapdefs[i];
		if(mapdefWk->_mapid == 0)	{	break;	}
		if(mapdefWk->_mapid == mapdef->_mapidUp)
		{
			if(mapdefWk->_mapid == mapidNew)
			{
				return mapdef->_mapid;
			}
			else if(mapdefWk->_mapid == mapidDiveFrom)	// 戻ってきた
			{
				continue;	// これ以上潜らない
			}

			// その一つ上の分岐点から下り方向へ探す
			s32 mapidFind = getNeighborMapIdDown(mapidNew, mapdef->_mapid, mapdefWk);
			if(mapidFind != -1)
			{
				return mapidFind;
			}

			// 更に上り方向へ探す
			mapidFind = getNeighborMapIdUp(mapidNew, mapdefWk->_mapid, mapdefWk);
			if(mapidFind != -1)
			{
				return mapidFind;
			}
		}
	}

	return -1;
}

/*---------------------------------------------------------------------*//**
	隣接のマップ ID を取得（下り再帰処理）
**//*---------------------------------------------------------------------*/
s32 WorldMap::getNeighborMapIdDown(s32 mapidNew, s32 mapidDiveFrom, const WorldMap::MapDef* mapdef)
{
	for(int i = 0; i < WorldMap::NUM_MAP_MAX; i++)
	{
		const WorldMap::MapDef* mapdefWk = &s_mapdefs[i];
		if(mapdefWk->_mapid == 0)	{	break;	}
		if(mapdefWk->_mapidUp == mapdef->_mapid)
		{
			if(mapdefWk->_mapid == mapidNew)
			{
				return mapdefWk->_mapidUp;
			}
			else if(mapdefWk->_mapid == mapidDiveFrom)	// 戻ってきた
			{
				continue;	// これ以上潜らない
			}

			// 更に下り方向へ探す
			s32 mapidFind = getNeighborMapIdDown(mapidNew, mapdef->_mapid, mapdefWk);
			if(mapidFind != -1)
			{
				return mapidFind;
			}
		}
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	新マップを有効化
**//*---------------------------------------------------------------------*/
void WorldMap::enableNewMap(s32 mapid)
{
	for(int i = 0; i < NUM_MAP_MAX; i++)
	{
		const MapDef* md = &_mdarrRef[i];
		if(md->_mapid == mapid)
		{
			if(!TFW_IS_FLAG(_msarrStat[i]._sflags, SF_VALID))
			{
				TFW_SET_FLAG(_msarrStat[i]._sflags, SF_VALID, true);
			}
			break;
		}
	}
}

/*---------------------------------------------------------------------*//**
	スポット表示を全てクリアする
**//*---------------------------------------------------------------------*/
void WorldMap::clearSpot(SpotKind spotkind)
{
	u8 sflag = 0;
	switch(spotkind)
	{
	case SPOTKIND_MAINEV:		sflag = SF_OCCUR_MAINEV;		break;
	case SPOTKIND_SUBEV:		sflag = SF_OCCUR_SUBEV;			break;
	case SPOTKIND_QSTEV_START:	sflag = SF_OCCUR_QSTEV_START;	break;
	case SPOTKIND_QSTEV_END:	sflag = SF_OCCUR_QSTEV_END;		break;
	}

	for(int i = 0; i < NUM_MAP_MAX; i++)
	{
		TFW_SET_FLAG(_msarrStat[i]._sflags, sflag, false);
	}
}

/*---------------------------------------------------------------------*//**
	スポット表示を設定する
**//*---------------------------------------------------------------------*/
void WorldMap::markSpot(s32 mapid, SpotKind spotkind, bool isOn)
{
	u8 sflag = 0;
	switch(spotkind)
	{
	case SPOTKIND_MAINEV:		sflag = SF_OCCUR_MAINEV;		break;
	case SPOTKIND_SUBEV:		sflag = SF_OCCUR_SUBEV;			break;
	case SPOTKIND_QSTEV_START:	sflag = SF_OCCUR_QSTEV_START;	break;
	case SPOTKIND_QSTEV_END:	sflag = SF_OCCUR_QSTEV_END;		break;
	}

	for(int i = 0; i < NUM_MAP_MAX; i++)
	{
		const MapDef* md = &_mdarrRef[i];
		if(md->_mapid == mapid)
		{
			TFW_SET_FLAG(_msarrStat[i]._sflags, sflag, isOn);
			break;
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
WorldMap::WorldMap()
	: _texMap(0L)
	, _posBase(0L)
	, _camWm(0L)
	, _camidBk(0)
	, _sflagsTotal(0)
	, _mdarrRef(0L)
	, _msarrStat(0L)
	, _stupdr(0L)
	, _debug_validAll(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
WorldMap::~WorldMap()
{
	ASSERT(_texMap == 0L);
	ASSERT(_posBase == 0L);
	ASSERT(_camWm == 0L);
	ASSERT(_msarrStat == 0L);
	ASSERT(_stupdr == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool WorldMap::create()
{
	// マップ定義を取得
	_mdarrRef = s_mapdefs;

	// マップ状態を作成
	_msarrStat = new MapStat[NUM_MAP_MAX];

	// 地図テクスチャの読み込み
	u32 fidTex = SYSTEM_WORLDMAP01_PNG;
	Texture::Format fmtTex = Texture::FMT_PNG;
	#if defined(_IOS) && 0
		fidTex = SYSTEM_WORLDMAP01_PVR;
		fmtTex = Texture::FMT_PVR;
	#endif
	Texture* tex = new Texture();
	if(!tex->create(fidTex, fmtTex))
	{
		delete tex;
		ASSERTM(false, "{WorldMap::create} texture-create failed.");
		return false;
	}
	_texMap = tex;

	// 世界地図の状態更新モジュールの作成
	_stupdr = WorldMapStateUpdaterFactory::makeUpdater();

	// 基本位置ベクトルを作成
	_posBase = new Vector3F();

	// カメラを作成
	WorldMapCam* cam = new WorldMapCam();
	if(!cam->create())
	{
		delete cam;
		ASSERTM(false, "{WorldMap::create} WorldMapCam-create failed.");
		return false;
	}
	_camWm = cam;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void WorldMap::destroy()
{
	// カメラを削除する
	if(_camWm != 0L)
	{
		_camWm->destroy();
		delete _camWm;
		_camWm = 0L;
	}

	// 基本位置ベクトル削除
	if(_posBase != 0L)
	{
		delete _posBase;
		_posBase = 0L;
	}

	// 世界地図の状態更新モジュールの削除
	if(_stupdr != 0L)
	{
		_stupdr->destroy();
		delete _stupdr;
		_stupdr = 0L;
	}

	// 地図テクスチャの削除
	if(_texMap != 0L)
	{
		_texMap->destroy();
		delete _texMap;
		_texMap = 0L;
	}

	// マップ状態を削除
	if(_msarrStat != 0L)
	{
		delete[] _msarrStat;
		_msarrStat = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void WorldMap::exec(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	if(_camWm->isInputOutRange())	// ユーザーがモードから抜けようとした
	{
		// 移動モードへ
		TRACE("{WorldMap::exec} change to WALKING mode.\n");
//		Game::getGame()->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME_WALKING);
		res->setDone();
		return;
	}

	TouchPanel* ui = gec->getTouchPanel(); 
	if(!ui->isReacted())
	{
		PointF32 pt;
		if(ui->isClickRelease(&pt))
		{
			View* view = Game::getGame()->getTfw()->getView();	ASSERT(view != 0L);
			Vector3F vPosMinMax[2];
			Vector2F vScrMinMax[2];
			for(int i = 0; i < NUM_MAP_MAX; i++)
			{
				// マップをスクリーン座標変換して、クリックした位置ならマップ移動
				const MapDef* md = &_mdarrRef[i];
				if((md->_mapid != 0) && (TFW_IS_FLAG(_msarrStat[i]._sflags, SF_VALID) || _debug_validAll))
				{
					vPosMinMax[0].set(md->_x,          _posBase->y() + Y_DIFF_MAP, md->_y         );
					vPosMinMax[1].set(md->_x + md->_w, _posBase->y() + Y_DIFF_MAP, md->_y + md->_h);
					Gcalc::conv3dPosToScreenPos(vScrMinMax, vPosMinMax, 2, _camWm, view);
					if(	(vScrMinMax[0].x() <= pt.x()) && (pt.x() <= vScrMinMax[1].x())
						&& (vScrMinMax[0].y() <= pt.y()) && (pt.y() <= vScrMinMax[1].y())	)
					{
						// 移動モードへとマップ移動
//						Game::getGame()->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME_WALKING);
						MoveMap* mvmap = Game::getGame()->getMoveMap(); ASSERT(mvmap != 0L);
						if((mvmap != 0L) && (mvmap->getCurMapId() != md->_mapid))	// 違うマップのときのみ
						{
							mvmap->reserveChangeMap(md->_mapid);
						}
						res->setDone();
						break;
					}
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void WorldMap::draw(const RenderCtx* rc)
{
	GameRenderCtx* grc = (GameRenderCtx*)rc;
	Renderer* rdr = rc->getRenderer();
	Font* font = grc->getFontSet()->getFont(GameFontSet::JP);
	Texture* texGui = Game::getGame()->getGui()->getGuiTexture();

	f32 y;
	if((font != 0L) && (texGui != 0L))
	{
		rdr->setSolidColor(255, 255, 255, 255);

		// アイコン
		rdr->bindTexture(texGui);
		y = 8.0f;
		RectF32 vtx;
		RectF32 uv;
		if(TFW_IS_FLAG(_sflagsTotal, SF_OCCUR_MAINEV))
		{
			RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&vtx, 10.0f, y, 16.0f, 16.0f), Gcalc::calcTexUv(&uv, UV_MAINEV_MARK, 1.0f));
			y += 18.0f;
		}
		if(TFW_IS_FLAG(_sflagsTotal, SF_OCCUR_SUBEV))
		{
			RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&vtx, 10.0f, y, 16.0f, 16.0f), Gcalc::calcTexUv(&uv, UV_SUBEV_MARK, 1.0f));
			y += 18.0f;
		}
		if(TFW_IS_FLAG(_sflagsTotal, SF_OCCUR_QSTEV_START))
		{
			RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&vtx, 10.0f, y, 16.0f, 16.0f), Gcalc::calcTexUv(&uv, UV_QUESTEV_START_MARK, 1.0f));
			y += 18.0f;
		}
		if(TFW_IS_FLAG(_sflagsTotal, SF_OCCUR_QSTEV_END))
		{
			RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&vtx, 10.0f, y, 16.0f, 16.0f), Gcalc::calcTexUv(&uv, UV_QUESTEV_END_MARK, 1.0f));
			y += 18.0f;
		}

		// テキスト
		y = 10.0f;
		if(TFW_IS_FLAG(_sflagsTotal, SF_OCCUR_MAINEV))
		{
			EasyStringDrawer::draw(
				font,
				GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_NEXT_STORY_EVENT),
				30.0f,
				y,
				12,
				rc	);
			y += 18.0f;
		}
		if(TFW_IS_FLAG(_sflagsTotal, SF_OCCUR_SUBEV))
		{
			EasyStringDrawer::draw(
				font,
				GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_NEXT_SUB_EVENT),
				30.0f,
				y,
				12,
				rc	);
			y += 18.0f;
		}
		if(TFW_IS_FLAG(_sflagsTotal, SF_OCCUR_QSTEV_START))
		{
			EasyStringDrawer::draw(
				font,
				GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_NEXT_QUEST_START_EVENT),
				30.0f,
				y,
				12,
				rc	);
			y += 18.0f;
		}
		if(TFW_IS_FLAG(_sflagsTotal, SF_OCCUR_QSTEV_END))
		{
			EasyStringDrawer::draw(
				font,
				GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_NEXT_QUEST_END_EVENT),
				30.0f,
				y,
				12,
				rc	);
			y += 18.0f;
		}
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void WorldMap::render(const RenderCtx* rc)
{
	GameRenderCtx* grc = (GameRenderCtx*)rc;
	Renderer* rdr = rc->getRenderer();

	rdr->bindTexture(_texMap);
	rdr->setSolidColor(255, 255, 255, 255);
	rdr->setCulling(false);							// 背面カリングなし
	rdr->setAlphaBlend(true);						// アルファブレンドあり
	rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);	// 通常合成

	// 背景を描画
	Vector3F vtx[4];
	vtx[0].set(_posBase->x(),           _posBase->y() + Y_DIFF_BG, _posBase->z()          );
	vtx[1].set(_posBase->x(),           _posBase->y() + Y_DIFF_BG, _posBase->z() + 1024.0f);
	vtx[2].set(_posBase->x() + 1024.0f, _posBase->y() + Y_DIFF_BG, _posBase->z()          );
	vtx[3].set(_posBase->x() + 1024.0f, _posBase->y() + Y_DIFF_BG, _posBase->z() + 1024.0f);
	Vector2F tex[4];
	tex[0].set(0.0f, 0.0f);
	tex[1].set(0.0f, -1.0f);
	tex[2].set(1.0f, 0.0f);
	tex[3].set(1.0f, -1.0f);
	RendererUtils::render3dTriangles(rdr, vtx, 0L, tex, 0L, 4, RendererUtils::TRIANGLE_STRIP);

	// 移動可能マップの矩形を描画
	rdr->bindTexture(0L);
	rdr->setAlphaBlendFunc(Renderer::AFUNC_ADD);		// 加算合成
	for(int i = 0; i < NUM_MAP_MAX; i++)
	{
		const MapDef* md = &_mdarrRef[i];
		if(md->_mapid == 0)	{	break;	}

		if(TFW_IS_FLAG(_msarrStat[i]._sflags, SF_VALID) || _debug_validAll)
		{
			// 中の塗りつぶし
			{
				rdr->setSolidColor(74, 15, 45, 255);
				Vector3F vtx[4];
				vtx[0].set(md->_x,          _posBase->y() + Y_DIFF_MAP, md->_y         );
				vtx[1].set(md->_x,          _posBase->y() + Y_DIFF_MAP, md->_y + md->_h);
				vtx[2].set(md->_x + md->_w, _posBase->y() + Y_DIFF_MAP, md->_y         );
				vtx[3].set(md->_x + md->_w, _posBase->y() + Y_DIFF_MAP, md->_y + md->_h);
				RendererUtils::render3dTriangles(rdr, vtx, 0L, tex, 0L, 4, RendererUtils::TRIANGLE_STRIP);
			}

			// 外枠
			{
				rdr->setSolidColor(25, 117, 104, 255);
				Vector3F vtx[5];
				f32 d = 0.0f;
				vtx[0].set(md->_x + d,                 _posBase->y() + Y_DIFF_MAP, md->_y + d                );
				vtx[1].set(md->_x + d,                 _posBase->y() + Y_DIFF_MAP, md->_y + md->_h - d * 2.0f);
				vtx[2].set(md->_x + md->_w - d * 2.0f, _posBase->y() + Y_DIFF_MAP, md->_y + md->_h - d * 2.0f);
				vtx[3].set(md->_x + md->_w - d * 2.0f, _posBase->y() + Y_DIFF_MAP, md->_y + d                );
				vtx[4].set(md->_x + d,                 _posBase->y() + Y_DIFF_MAP, md->_y + d                );
				RendererUtils::render3dLines(rdr, vtx, 5, RendererUtils::LINE_STRIP);
			}

			#if defined(_DEBUG)
				if( (grc->getBodyExecCount() & 0x20) == 0 )
				{
					// マップ番号を表示
					rdr->setSolidColor(255, 255, 255, 255);
					Vector3F wChar(3.5f, 0.0f, 0.0f);
					Vector3F hChar(0.0f, 0.0f, 3.5f);
					VcString str;
					EasyStringDrawer::render(grc->getFontSet()->getFont(GameFontSet::EN), VcString::format(&str, "%d", md->_mapid), &vtx[0], &wChar, &hChar, rc);
					// 描画設定を戻す
					rdr->bindTexture(0L);
					rdr->setSolidColor(255, 0, 0, 127);
				}
			#endif
		}
	}

	// 次のイベントマップのマーク（旗）を描画
	///_msarrStat[4]._sflags = SF_OCCUR_MAINEV /*| SF_OCCUR_SUBEV*/ | SF_OCCUR_QUESTEV;	// マーク test
	enum MarkRenderPhase
	{
		MRPHASE_SHADOW,
		MRPHASE_FLAG,
		NUM_MRPHASE
	};
	RectF32 uvShadow;
	Gcalc::calcTexUv(&uvShadow, UV_MARK_SHADOW, 1.0f);
	Texture* texGui = Game::getGame()->getGui()->getGuiTexture();
	rdr->bindTexture(texGui);
	rdr->setSolidColor(255, 255, 255, 255);
	rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);	// 通常合成
	for(int iphase = 0; iphase < NUM_MRPHASE; iphase++)
	{
		bool isShadow = (iphase == MRPHASE_SHADOW);

		for(int i = 0; i < NUM_MAP_MAX; i++)
		{
			const MapDef* md = &_mdarrRef[i];
			if(md->_mapid == 0)	{	break;	}

			int num = 0;
			if(TFW_IS_FLAG(_msarrStat[i]._sflags, SF_OCCUR_MAINEV))			{	num++;	}
			if(TFW_IS_FLAG(_msarrStat[i]._sflags, SF_OCCUR_SUBEV))			{	num++;	}
			if(TFW_IS_FLAG(_msarrStat[i]._sflags, SF_OCCUR_QSTEV_START))	{	num++;	}
			if(TFW_IS_FLAG(_msarrStat[i]._sflags, SF_OCCUR_QSTEV_END))		{	num++;	}

			if(num > 0)
			{
				f32 ofs = 0;
				if(num >= 3)		{	ofs += 1.0f;	}
				else if(num >= 2)	{	ofs += 0.5f;	}

				RectF32 uv;
				if(TFW_IS_FLAG(_msarrStat[i]._sflags, SF_OCCUR_QSTEV_START))
				{
					Gcalc::calcTexUv(&uv, UV_QUESTEV_START_MARK, 1.0f);
					renderEvMark(rc, md, isShadow ? &uvShadow : &uv, ofs, isShadow);
					ofs -= 1.0f;
				}
				if(TFW_IS_FLAG(_msarrStat[i]._sflags, SF_OCCUR_QSTEV_END))
				{
					Gcalc::calcTexUv(&uv, UV_QUESTEV_END_MARK, 1.0f);
					renderEvMark(rc, md, isShadow ? &uvShadow : &uv, ofs, isShadow);
					ofs -= 1.0f;
				}
				if(TFW_IS_FLAG(_msarrStat[i]._sflags, SF_OCCUR_SUBEV))
				{
					Gcalc::calcTexUv(&uv, UV_SUBEV_MARK, 1.0f);
					renderEvMark(rc, md, isShadow ? &uvShadow : &uv, ofs, isShadow);
					ofs -= 1.0f;
				}
				if(TFW_IS_FLAG(_msarrStat[i]._sflags, SF_OCCUR_MAINEV))
				{
					Gcalc::calcTexUv(&uv, UV_MAINEV_MARK, 1.0f);
					renderEvMark(rc, md, isShadow ? &uvShadow : &uv, ofs, isShadow);
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	イベントマーク（旗）の描画
**//*---------------------------------------------------------------------*/
void WorldMap::renderEvMark(const RenderCtx* rc, const MapDef* md, const RectF32* uvwh, f32 ofs, bool isShadow)
{
	Vector3F vtx[4];
	Vector2F tex[4];

	f32 xOfs = ofs * 3.0f;
	f32 zOfs = - ofs * 2.0f;
	f32 xCenter = md->_x + (md->_w * 0.5f);
	f32 zCenter = md->_y + (md->_h * 0.5f);

	if(isShadow)
	{
		vtx[0].set(xCenter - 1.0f + xOfs - 15.0f, _posBase->y() + Y_DIFF_MAP, zCenter + zOfs - 10.0f);
		vtx[1].set(xCenter - 1.0f + xOfs,         _posBase->y() + Y_DIFF_MAP, zCenter + zOfs + 3.0f);
		vtx[2].set(xCenter + 10.0f + xOfs - 2.0f, _posBase->y() + Y_DIFF_MAP, zCenter + zOfs - 9.0f);
		vtx[3].set(xCenter + 10.0f + xOfs,        _posBase->y() + Y_DIFF_MAP, zCenter + zOfs + 3.0f);
		tex[0].set(uvwh->_v[0], 1.0f - uvwh->_v[1]);
		tex[1].set(uvwh->_v[0], 1.0f - (uvwh->_v[1] + uvwh->_v[3]));
		tex[2].set(uvwh->_v[0] + uvwh->_v[2], tex[0]._v[1]);
		tex[3].set(tex[2]._v[0], tex[1]._v[1]);
	}
	else
	{
		vtx[0].set(xCenter - 1.0f + xOfs - 5.0f,  _posBase->y() + Y_DIFF_MAP + 10.0f, zCenter + zOfs - 7.0f);
		vtx[1].set(xCenter - 1.0f + xOfs,         _posBase->y() + Y_DIFF_MAP,         zCenter + zOfs + 3.0f);
		vtx[2].set(xCenter + 10.0f + xOfs + 5.0f, _posBase->y() + Y_DIFF_MAP + 10.0f, zCenter + zOfs - 7.0f);
		vtx[3].set(xCenter + 10.0f + xOfs,        _posBase->y() + Y_DIFF_MAP,         zCenter + zOfs + 3.0f);
		tex[0].set(uvwh->_v[0], 1.0f - uvwh->_v[1]);
		tex[1].set(uvwh->_v[0], 1.0f - (uvwh->_v[1] + uvwh->_v[3]));
		tex[2].set(uvwh->_v[0] + uvwh->_v[2], tex[0]._v[1]);
		tex[3].set(tex[2]._v[0], tex[1]._v[1]);
	}

	///rc->getRenderer()->bindTexture(0L);	// ポリゴンテスト
	RendererUtils::render3dTriangles(rc->getRenderer(), vtx, 0L, tex, 0L, 4, RendererUtils::TRIANGLE_STRIP);
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool WorldMap::notifyBegin()
{
	TRACE("{WorldMap::notifyBegin} called.\n");

	// 状態アップデーターの開始処理
	if(_stupdr != 0L)
	{
		_stupdr->notifyBegin(this);
	}

	// 現在のカメラ ID を得る
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	Camera* cam = sgm->getVisibleCamera();
	_camidBk = cam->getId();			// 現在のカメラの ID を得ておく

	// 基本位置
	_posBase->set(0.0f, Y_BASE, 0.0f);
	Vector3F posCam(0.0f, Y_BASE, 0.0f);
	s32 mapid = Game::getGame()->getMoveMap()->getCurMapId();
	for(int i = 0; i < NUM_MAP_MAX; i++)
	{
		const MapDef* md = &_mdarrRef[i];
		if(md->_mapid == mapid)
		{
			posCam.x() = md->_x + md->_w * 0.5f;
			posCam.z() = md->_y + md->_h * 0.5f;
			break;
		}
	}
	_camWm->notifyBegin(&posCam);	// カメラの基本位置をセットする

	// 全体フラグを調べる
	_sflagsTotal = 0;
	for(int i = 0; i < NUM_MAP_MAX; i++)
	{
		_sflagsTotal |= _msarrStat[i]._sflags;
	}

	// カメラを変更する
	if(sgm->addNode(_camWm, false))
	{
		CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(SorSgm::EK_CAM);
		nodecCam->switchVisible(_camWm->getId());	// カメラを有効化
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void WorldMap::notifyEnd()
{
	TRACE("{WorldMap::notifyEnd} called.\n");

	_camWm->notifyEnd();

	// カメラを戻す
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	if(	(sgm->getNodeFromId(_camidBk) != 0L) &&		// カメラがまだ存在するときのみ（destroy 中など、無い場合もある）
		(sgm->getVisibleCamera() == _camWm)	)		// かつ、削除しようとしているカメラが有効のとき
	{
		CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(SorSgm::EK_CAM);
		nodecCam->switchVisible(_camidBk);	// カメラを戻す
	}
	sgm->removeNode(_camWm);
}

/*---------------------------------------------------------------------*//**
	マップ開始通知
**//*---------------------------------------------------------------------*/
void WorldMap::notifyChangedMap()
{
	// マップ表示を有効化
	s32 mapid = Game::getGame()->getMoveMap()->getCurMapId();
	enableNewMap(mapid);
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool WorldMap::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();

		// マップ状態を書き込み
		ASSERT(SaveStructure::NUM_MAP_MAX >= NUM_MAP_MAX);
		for(int i = 0; i < NUM_MAP_MAX; i++)
		{
			svst->_wmapflags[i] = _msarrStat[i]._sflags;
		}
	}
	else						// ダイレクトモード
	{
		// マップ状態を書き込み
		for(int i = 0; i < NUM_MAP_MAX; i++)
		{
			ser->store(&_msarrStat[i]._sflags);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool WorldMap::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		const SaveStructure* svst = (SaveStructure*)ser->getStructure();

		// マップ状態を読み込み
		ASSERT(SaveStructure::NUM_MAP_MAX >= NUM_MAP_MAX);
		for(int i = 0; i < NUM_MAP_MAX; i++)
		{
			_msarrStat[i]._sflags = svst->_wmapflags[i];
		}
	}
	else						// ダイレクトモード
	{
		// マップ状態を読み込み
		for(int i = 0; i < NUM_MAP_MAX; i++)
		{
			ser->restore(&_msarrStat[i]._sflags);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デバッグモード設定
**//*---------------------------------------------------------------------*/
void WorldMap::debug_onDebugLevel(s32 lv)
{
#if defined(_DEBUG)
	if(lv >= 1)
	{
		_debug_validAll = true;
	}
#endif
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
