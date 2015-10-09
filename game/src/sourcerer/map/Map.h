/***********************************************************************//**
 *	Map.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAP_MAP_H_
#define _SRCR_MAP_MAP_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExecDraw.h"
#include "../../tfw/serialize/SerializeBase.h"

#include "MapConfig.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Gcol;
class Model;
class ShapeModel;
class Texture;

TFW_END_NS

SRCR_BEGIN_NS

class Game;
class ModelSwitch;
class PlacementObj;
class Unit;

/*---------------------------------------------------------------------*//**
 *	マップ
 *
**//*---------------------------------------------------------------------*/
class Map : public FrameExecDraw, SerializeBase
{
	//======================================================================
	// 定数
private:
	static const s32	NUM_GATE_MAX		= 4;
	static const s32	NUM_TEMPPOBJ_MAX	= 10;

	// 動作制御フラグ
	static const u32	EF_CLOSE_GATE		= 0x00000001;	// マップ変更不可
	static const u32	EF_LOCK_OP			= 0x00000002;	// 操作不可中

	//======================================================================
	// クラス
public:
	class Gate
	{
	public:
		Vector3F*	_posCenter;

		Gate();
		~Gate();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 現在のマップ ID を得る
	inline s32 getCurrentMapId() const { return _mapidCur; }
	// 地面コリジョンを得る
	inline Gcol* getGroundCollision() { return _gcolMapRef; }
	// マップ設定を得る
	inline const MapConfig* getMapConfig() const { return &_conf; }
	inline MapConfig* mapConfig() { return &_conf; }
	// マップ変更不可かどうかを得る
	inline bool isCloseGate() const { return TFW_IS_FLAG(_eflags, EF_CLOSE_GATE); }
	// 自動割当制御イベント ID カウンタ値の取得
	inline u16 getAutoCountCtrlEvid() const { return _ctrlevidAutoCntTmp; }

	// マップ変更不可を設定する
	void setCloseGate(bool isCloseGate) { TFW_SET_FLAG(_eflags, EF_CLOSE_GATE, isCloseGate); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool				loadMap(s32 mapid);
	void				releaseMap();

	void				playMapBgm();

	s32					getGcolPlaneMoveMapId(s32 idxPlane) const;
	void				changeMoveMapId(s32 mapidOld, s32 mapidNew);

	bool				placePlacementObj(PlacementObj* pobj, bool isForcePlacement);
	bool				unplacePlacementObj(PlacementObj* pobj);
	bool				isPlacePlacementObj(const PlacementObj* pobj);

	bool				addTemporaryPlacementObjSetting(PlacementObj* pobjEntr, bool isPlace);
	bool				removeTemporaryPlacementObjSetting(const PlacementObj* pobj);
	s32					getTemporaryPlacementObjCount() const;

	bool				calcRandomMovablePosition(Vector3F* posOut, const Vector3F* posCenter, f32 radiusRange);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Map();
	virtual ~Map();
	bool				create(Texture* texSign);
	void				destroy();
	virtual void		exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	virtual void		render(const RenderCtx* rc);			// 描画
	bool				serialize(Serializer* ser) const;
	bool				deserialize(Serializer* ser);

	void				beginMapStartEvent(u32 evparam);

private:
	bool				loadMapAfter(ShapeModel** mdlarr, s32 numModel);
	bool				prepareNcc();
	bool				placeUnitFromPlacementObj(Unit* unit, PlacementObj* pobj);
	bool				adjustCollY(Vector3F* pos);

	//======================================================================
	// 変数

private:
	s32					_mapidCur;						// 現在のマップ ID

	Unit*				_unitMapRef;					// マップユニット
	Gcol*				_gcolMapRef;					// 地面コリジョン
	Texture*			_texMarkRef;					// 記号テクスチャ

	ModelSwitch**		_msarrNcc;						// ニアクリップスイッチ配列
	s32					_numNccMsArr;					// ニアクリップスイッチ配列数

	Gate				_garrShowGate[NUM_GATE_MAX];	// マップ移動ゲート
	MapConfig			_conf;							// マップ設定

	List<PlacementObj*>* _listPlaceObjTmp;				// 一時的配置オブジェリスト
	u16					_ctrlevidAutoCntTmp;			// 一時的配置オブジェ用自動割当制御イベント ID カウンタ
	u32					_eflags;						// 動作制御フラグ
	f32					_thetaGateMark;					// ゲート記号の回転角（ラジアン）
	f32					_uvarrGateMark[8];				// ゲーム記号のテクスチャ座標
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAP_MAP_H_
