/***********************************************************************//**
 *	PlacementObj.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PlacementObj.h"

// Friends
#include "../episode/StoryManager.h"
#include "../event/EventIdInfoArray.h"
#include "../event/EventIdDef.h"
#include "../event/matter/EventMatterCreateParam.h"
#include "../body/Game.h"
#include "../body/MoveMap.h"

// External
#include "../../tfw/lib/Vector.h"
#include "../../tfw/lib/XmlParser.h"
#include "../../tfw/lib/XmlParserUtils.h"
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PlacementObj メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	XML からセットアップ
**//*---------------------------------------------------------------------*/
bool PlacementObj::setupFromXmlNode(const XmlNode* node, u8 ucase, u8 subid, const EventMatterCreateParam* emcp)
{
	_flags = 0;	// フラグクリア

	VcString strwk;
	bool bwk;
	s32 iwk;
	Vector3F vwk;

	// ［必須］タイプ
	_kind = PlacementObj::KIND_NULL;
	strwk.emptyEasy();
	if(!XmlParserUtils::analyzeAtrb(&strwk, node, "kind"))
	{
		// 失敗
		return false;
	}
	if(strwk.equalsIgnoreCase("extra"))			{	_kind = PlacementObj::KIND_EXTRA;		}
	else if(strwk.equalsIgnoreCase("enemy"))	{	_kind = PlacementObj::KIND_ENEMY;		}
	else if(strwk.equalsIgnoreCase("mapobj"))	{	_kind = PlacementObj::KIND_MAPOBJ;		}
	else if(strwk.equalsIgnoreCase("puitem"))	{	_kind = PlacementObj::KIND_PICKUPITEM;	}

	// ［オプション］無効
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "enable"))
	{
		_flags |= bwk ? 0 : F_DISABLE;
	}

	// ［オプション］隠し
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "hidden"))
	{
		_flags |= bwk ? F_HIDDEN : 0;
	}

	// ［オプション］フォーカス不可
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "enable_focus"))
	{
		_flags |= bwk ? 0 : F_DISABLE_FOCUS;
	}

	// ［オプション］クエスト受託中のみの配置
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "quest_accepting"))
	{
		_flagsMod |= bwk ? MF_QUEST_ACCEPTING : 0;
	}

	// キャラクタ ID / アイテム ID
	if((KIND_EXTRA <= _kind) && (_kind <= KIND_MAPOBJ))
	{
		// ［オプション］キャラクタ ID
		if(XmlParserUtils::analyzeAtrb(&iwk, node, "char_id"))
		{
			_did = (u16)iwk;
		}
	}
	else if((KIND_DROPITEM <= _kind) && (_kind <= KIND_PICKUPITEM))
	{
		// ［オプション］アイテム ID
		if(XmlParserUtils::analyzeAtrb(&iwk, node, "item_id"))
		{
			_did = (u16)iwk;
		}
	}

	// ［オプション］出現マップ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "map_id"))
	{
		_mapid = iwk;
	}

	// ［オプション］出現ストーリ ID 最小値（デフォルトは 0 で、最小値制限なし）
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "min_story_id"))
	{
		_storyidMin = iwk;
	}

	// ［オプション］出現ストーリ ID 最大値（デフォルトは 0 で、最大値制限なし）
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "max_story_id"))
	{
		_storyidMax = iwk;
	}

	// ［オプション］位置
	if(XmlParserUtils::analyzeAtrb(&vwk, node, "pos"))
	{
		*_pos = vwk;
	}

	// ［オプション］位置乱数半径（デフォルトは 0 で、乱数なし）
	_rRandomPos = 0.0f;
	if(XmlParserUtils::analyzeAtrb(&vwk[0], node, "rand_pos_r"))
	{
		_rRandomPos = vwk[0];
	}

	// ［オプション］Y 当たりを取らない（デフォルトは 0）
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "no_y_coll"))
	{
		TFW_SET_FLAG(_placeflags, PLACEF_NO_Y_COLL, bwk);
	}

	// ［オプション］向き
	if(XmlParserUtils::analyzeAtrb(&vwk, node, "dir"))
	{
		// Degree を Radian に変換する
		_dir->_v[0] = TFW_DEG_TO_RAD(vwk._v[0]);
		_dir->_v[1] = TFW_DEG_TO_RAD(vwk._v[1]);
		_dir->_v[2] = TFW_DEG_TO_RAD(vwk._v[2]);
	}

	// ［オプション］位置イベント ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "pos_evid"))
	{
		_evidPos = iwk;
		if(emcp != 0L) { _evidPos = emcp->offsetEvid(_evidPos); }		// イベント ID のオフセット処理
	}

	// ［オプション］制御イベント ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "ctrl_evid"))
	{
		_evidCtrl = iwk;
		if(emcp != 0L) { _evidCtrl = emcp->offsetEvid(_evidCtrl); }		// イベント ID のオフセット処理
	}

	// ［オプション］起動イベントを配置時から進めるセーブフラグ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "next_call_ev_sfid"))
	{
		_sfidNextCallEv = iwk;
	}

	// ［オプション］オブジェクトより起動するイベントのイベント ID 配列
	EventIdInfoArray* eiiarr = new EventIdInfoArray();
	CcString cstrCallEvid("call_evid");
	if(eiiarr->setupFromXmlNode(node, &cstrCallEvid, emcp))
	{
		_eiiarr = eiiarr;
	}
	else
	{
		delete eiiarr;
	}

	// ［オプション］出現確率
	iwk = 100;	// デフォルトは 100%
	XmlParserUtils::analyzeAtrb(&iwk, node, "rand");
	if(iwk < 0)		{	iwk = 0;	}
	if(iwk > 100)	{	iwk = 100;	}
	_probability = (u8)iwk;

	// ［オプション］BGM ID
	_bgmid = 0;
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "bgm_id"))
	{
		_bgmid = (u16)iwk;
	}

	// ［オプション］勝利後に実行するイベント ID
	_evidAfterWin = 0;
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "win_evid"))
	{
		_evidAfterWin = (u16)iwk;
		if(emcp != 0L) { _evidAfterWin = emcp->offsetEvid(_evidAfterWin); }		// イベント ID のオフセット処理
	}

	// ［オプション］敗北後に実行するイベント ID
	_evidAfterLose = 0;
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "lose_evid"))
	{
		_evidAfterLose = (u16)iwk;
		if(emcp != 0L) { _evidAfterLose = emcp->offsetEvid(_evidAfterLose); }	// イベント ID のオフセット処理
	}

	// ［オプション］逃げる許可（デフォルトは許可）
	TFW_SET_FLAG(_eneflags, ENEF_ALLOW_ESCAPE, true);
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "escape"))
	{
		TFW_SET_FLAG(_eneflags, ENEF_ALLOW_ESCAPE, bwk);
	}

	// ［オプション］配置されてもセーブしない（デフォルトは false）
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "no_save_placed"))
	{
		TFW_SET_FLAG(_placeflags, PLACEF_NO_SAVE, bwk);
	}

	// ［オプション］歩行速度（デフォルトは 0 で、歩かない；現状ランダム歩行のみ対応）
	_walkspeed = 0.0f;
	if(XmlParserUtils::analyzeAtrb(&vwk[0], node, "walk_speed"))
	{
		_walkspeed = vwk[0];
	}

	// 使用種別
	_ucase = ucase;
	_subid = subid;

	return true;
}

/*---------------------------------------------------------------------*//**
	セットアップ
**//*---------------------------------------------------------------------*/
bool PlacementObj::setup(Kind kind, u8 ucase, u8 subid, u16 chrdid, u16 itmdid, s32 mapid, u16 evidPos, u8 probability, EventIdInfoArray* eiiarr, const Vector3F* pos)
{
	_kind = kind;
	_ucase = ucase;
	_subid = subid;
	if((KIND_EXTRA <= _kind) && (_kind <= KIND_MAPOBJ))
	{
		_did = chrdid;
	}
	else if(_kind == PlacementObj::KIND_DROPITEM)
	{
		_did = itmdid;
	}
	_mapid = mapid;
	_evidPos = evidPos;
	_probability = probability;
	_eiiarr = eiiarr;
	if(pos != 0L)	{	_pos->copy(pos);	}
	return true;
}

/*---------------------------------------------------------------------*//**
	呼び出しイベント情報を次に送る
**//*---------------------------------------------------------------------*/
void PlacementObj::nextCallEvi(bool isAutoRewind)
{
	_idxCallEvi++;

	if(_eiiarr == 0L)
	{
		_idxCallEvi = 0;
	}
	else if(_idxCallEvi >= _eiiarr->getInfoNum())
	{
		_idxCallEvi = isAutoRewind ? 0 : _eiiarr->getInfoNum() - 1;
	}
}

/*---------------------------------------------------------------------*//**
	新規配置条件に合致するかを判定する
**//*---------------------------------------------------------------------*/
bool PlacementObj::isNewPlacedCondition() const
{
	MoveMap* mvmap = Game::getGame()->getMoveMap(); ASSERT(mvmap != 0L);
	StoryManager* strymng = Game::getGame()->getStoryManager(); ASSERT(strymng != 0L);

	// マップ ID は合っているか
	if((_mapid != 0) && (_mapid != mvmap->getCurMapId()))
	{
		return false;
	}

	// ストーリー ID 最小値以上か
	if((_storyidMin != 0) && (_storyidMin > strymng->getCurrentStoryId()))
	{
		return false;
	}

	// ストーリー ID 最大値以下か
	if((_storyidMax != 0) && (_storyidMax < strymng->getCurrentStoryId()))
	{
		return false;
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PlacementObj::PlacementObj()
	: _kind(KIND_NULL)
	, _flags(0)
	, _ucase(0)
	, _subid(0)
	, _probability(0)
	, _did(0)
	, _evidPos(0)
	, _evidCtrl(0)
	, _mapid(0)
	, _storyidMin(0)
	, _storyidMax(0)
	, _eiiarr(0L)
	, _sfidNextCallEv(0)
	, _pos(new Vector3F())
	, _dir(new Vector3F())
	, _bgmid(0)
	, _evidAfterWin(0)
	, _evidAfterLose(0)
	, _eneflags(0)
	, _placeflags(0)
	, _walkspeed(0.0f)
	, _rRandomPos(0.0f)
	, _flagsMod(0)
	, _desckey(0)
	, _idxCallEvi(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PlacementObj::~PlacementObj()
{
	delete _pos;
	delete _dir;
	delete _eiiarr;
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool PlacementObj::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		SaveStructure svst;
		if(!storeToStructure(&svst)) { return false; }
		if(!ser->store(&svst, sizeof(SaveStructure))) { return false; }
		return true;
	}
}

/*---------------------------------------------------------------------*//**
	デシリアイズ
**//*---------------------------------------------------------------------*/
bool PlacementObj::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		SaveStructure svst;
		if(!ser->restore(&svst, sizeof(SaveStructure))) { return false; }
		return restoreFromStructure(&svst);
	}
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャに保存する
**//*---------------------------------------------------------------------*/
bool PlacementObj::storeToStructure(SaveStructure* svst) const
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	svst->_ucase = _ucase;
	svst->_subid = _subid;
	svst->_kind = _kind;
	svst->_flags = _flags;
	svst->_did = _did;
	svst->_evidCtrl = _evidCtrl;
	_eestat.storeToStructure(&svst->_eestat);
	svst->_posX = _pos->x();
	svst->_posY = _pos->y();
	svst->_posZ = _pos->z();
	svst->_bgmid = _bgmid;
	svst->_evidAfterWin = _evidAfterWin;
	svst->_evidAfterLose = _evidAfterLose;
	svst->_eneflags = _eneflags;
	svst->_flagsMod = _flagsMod;

	// NoSavePlaced の場合は、配置されていても配置されてない、とする
	if(TFW_IS_FLAG(_placeflags, PLACEF_NO_SAVE) && TFW_IS_FLAG(svst->_flags, F_PLACED))
	{
		TFW_SET_FLAG(svst->_flags, F_PLACED, false);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャから読み込む
**//*---------------------------------------------------------------------*/
bool PlacementObj::restoreFromStructure(const SaveStructure* svst)
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	_ucase = svst->_ucase;
	_subid = svst->_subid;
	_kind = svst->_kind;
	_did = svst->_did;
	_evidCtrl = svst->_evidCtrl;
	_eestat.restoreFromStructure(&svst->_eestat);
	_pos->set(svst->_posX, svst->_posY, svst->_posZ);
	_bgmid = svst->_bgmid;
	_evidAfterWin = svst->_evidAfterWin;
	_evidAfterLose = svst->_evidAfterLose;
	_eneflags = svst->_eneflags;
	_flagsMod = svst->_flagsMod;

	setPlaced(TFW_IS_FLAG(svst->_flags, F_PLACED));
	setNoPlaceByProbability(TFW_IS_FLAG(svst->_flags, F_NOPLACE_PROB));

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
