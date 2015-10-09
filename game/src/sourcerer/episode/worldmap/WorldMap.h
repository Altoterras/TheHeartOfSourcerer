/***********************************************************************//**
 *	WorldMap.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/08.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_WORLDMAP_WORLD_MAP_H_
#define _SRCR_EPISODE_WORLDMAP_WORLD_MAP_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

#include "../../../tfw/framemod/FrameExecDraw.h"
#include "../../../tfw/serialize/SerializeBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
class Texture;

TFW_END_NS

SRCR_BEGIN_NS

class Game;
class WorldMapCam;
class WorldMapStateUpdater;

/*---------------------------------------------------------------------*//**
 *	世界地図
 *		※タイトル依存
 *
**//*---------------------------------------------------------------------*/
class WorldMap : public FrameExecDraw, SerializeBase
{
	//======================================================================
	// 定数
public:
	static const s32	NUM_MAP_MAX				= 64;

	enum SpotKind
	{
		SPOTKIND_NULL,			// （未定義）
		SPOTKIND_MAINEV,		// メインイベント発生
		SPOTKIND_SUBEV,			// サブイベント発生
		SPOTKIND_QSTEV_START,	// クエスト開始イベント発生
		SPOTKIND_QSTEV_END,		// クエスト報酬イベント発生
	};

private:
	// 状態フラグ
	static const u8		SF_VALID				= 0x01;
	static const u8		SF_OCCUR_MAINEV			= 0x02;
	static const u8		SF_OCCUR_SUBEV			= 0x04;
	static const u8		SF_OCCUR_QSTEV_START	= 0x08;
	static const u8		SF_OCCUR_QSTEV_END		= 0x10;

	//======================================================================
	// クラス
public:
	class MapDef
	{
	public:
		s32 _mapid;
		s32 _mapidUp;	// ノバイ寺院を中心と見た、上り方面のマップ
		f32 _x;
		f32 _y;
		f32 _w;
		f32 _h;
		MapDef() : _mapid(0), _mapidUp(0), _x(0.0f), _y(0.0f), _w(0.0f), _h(0.0f) {}
		MapDef(s32 mapid, f32 x, f32 y, f32 w, f32 h, s32 mapidUp) : _mapid(mapid), _mapidUp(mapidUp), _x(x), _y(y), _w(w), _h(h) {}
	};

private:
	class MapStat
	{
	public:
		u8 _sflags;
		MapStat() : _sflags(0) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	s32 getNeighborMapId(s32 mapidNew, s32 mapidOld);
	void enableNewMap(s32 mapid);

	void clearSpot(SpotKind spotkind);
	void markSpot(s32 mapid, SpotKind spotkind, bool isOn);

private:
	s32 getNeighborMapIdUp(s32 mapidNew, s32 mapidDiveFrom, const MapDef* mapdef);
	s32 getNeighborMapIdDown(s32 mapidNew, s32 mapidDiveFrom, const MapDef* mapdef);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	WorldMap();
	~WorldMap();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);
	void draw(const RenderCtx* rc);
	void render(const RenderCtx* rc);

	bool notifyBegin();
	void notifyEnd();
	void notifyChangedMap();
	virtual bool serialize(Serializer* ser) const;
	virtual bool deserialize(Serializer* ser);
	void debug_onDebugLevel(s32 lv);

private:
	void renderEvMark(const RenderCtx* rc, const MapDef* md, const RectF32* uvwh, f32 ofs, bool isShadow);

	//======================================================================
	// 変数
private:
	Texture* _texMap;
	Vector3F* _posBase;
	WorldMapCam* _camWm;
	u16 _camidBk;
	u8 _sflagsTotal;
	MapStat* _msarrStat;
	WorldMapStateUpdater* _stupdr;
	const MapDef* _mdarrRef;

	bool _debug_validAll;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_WORLDMAP_WORLD_MAP_H_
