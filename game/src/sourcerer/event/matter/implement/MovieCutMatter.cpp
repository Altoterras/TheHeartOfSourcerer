/***********************************************************************//**
 *	MovieCutMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/08.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MovieCutMatter.h"

// Friends
#include "ActMatter.h"
#include "RealtimeMovieMatter.h"
#include "act/Act.h"
#include "../EventMatter.h"
#include "../EventMatterType.h"
#include "../../EventData.h"
#include "../../EventSys.h"
#include "../../../body/Game.h"

// External
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/g3d/Shape.h"
#include "../../../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MovieCutMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	動作設定
**//*---------------------------------------------------------------------*/
void MovieCutMatter::setting(const Setting* s)
{
	_setting.copy(s);
}

/*---------------------------------------------------------------------*//**
	設定を XML から解析する
**//*---------------------------------------------------------------------*/
bool MovieCutMatter::analyzeSettingFromXml(Setting* s, const XmlNode* node)
{
	// ［必須］フレーム数
	s32 wk;
	if(!XmlParserUtils::analyzeAtrb(&wk, node, "durf"))
	{
		return false;
	}
	s->_nframe = (f32)wk;

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MovieCutMatter::MovieCutMatter()
	: EventMatter(EMTYPE_MOVIECUT)
	, _frmcnt(0.0f)
	, _evidRtmv(0)
{
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void MovieCutMatter::begin(u32 param)
{
	EventMatter::begin(param);

	// 値の初期化
	_frmcnt = 0.0f;

	// 値が実質的に反映される対応リアルタイムムービーを求める
	f32 frameRtmvMin = F32_PMAX;	// 経過時間が一番小さいリアルタイムムービーを割り出すためのフレーム値
	_evidRtmv = 0;
	EventData* evdat = getOwner()->getEventData();	ASSERT(evdat != 0L);
	for(s32 i = 0; i < evdat->getEventMatterNum(); i++)
	{
		EventMatter* matter = evdat->getEventMatter(i);
		if(matter->getMatterType() == EMTYPE_REALTIMEMOVIE)	// RealtimeMovieMatter 判定
		{
			if(matter->isBegin())
			{
				if(matter->getElapsedFrame() < frameRtmvMin)
				{
					_evidRtmv = matter->getEvId();
					frameRtmvMin = matter->getElapsedFrame();
				}
			}
		}
	}

	// アニメーション補間禁止を解除
	RealtimeMovieMatter* matterRtmv = getRtmvMatter();
	if(matterRtmv != 0L)
	{
		Game::getGame()->endNoAnimInterpolation(*matterRtmv->noAnimIntpHint());
	}
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void MovieCutMatter::end()
{
	// アニメーション補間禁止を再開
	RealtimeMovieMatter* matterRtmv = getRtmvMatter();
	if(matterRtmv != 0L)
	{
		*matterRtmv->noAnimIntpHint() = Game::getGame()->startNoAnimInterpolation();
	}

	EventMatter::end();
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool MovieCutMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
{
	if(!EventMatter::setupFromXmlNode(node, cparam))	{	return false;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// 動作設定解析

	analyzeSettingFromXml(&_setting, node);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理実装
**//*---------------------------------------------------------------------*/
void MovieCutMatter::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	// 値を更新
	_frmcnt += ec->getDeltaFrame();
	if(_frmcnt >= _setting._nframe)
	{
		end();	// 終了
	}
}

/*---------------------------------------------------------------------*//**
	リアルタイムムービーマターの取得
**//*---------------------------------------------------------------------*/
RealtimeMovieMatter* MovieCutMatter::getRtmvMatter()
{
	if(_evidRtmv != 0)
	{
		EventData* evdat = getOwner()->getEventData(); ASSERT(evdat != 0L);
		EventMatter* matter = evdat->getEventMatterFromEvid(_evidRtmv);
		if(matter != 0L)
		{
			if(matter->getMatterType() == EMTYPE_REALTIMEMOVIE)	// RealtimeMovieMatter 確認（念のため）
			{
				return (RealtimeMovieMatter*)matter;
			}
		}
	}

	return 0L;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
