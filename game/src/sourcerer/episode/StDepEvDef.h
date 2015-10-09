/***********************************************************************//**
 *	StDepEvDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/01.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_ST_DEP_EV_DEF_H_
#define _SRCR_EPISODE_ST_DEP_EV_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	タイトル（ストーリー）依存のイベント定義
 *
**//*---------------------------------------------------------------------*/
class StDepEvDef
{
	//======================================================================
	// 定数
public:
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// Epsode01 依存の固定的イベント ID

	// マップイベント割り当て
	// ・地面コリジョンマップイベント 1～ 99
	// ・XML 配置オブジェマップイベント 101～ 999
	// ・特殊マップイベント（エピソード内でユニーク） 1001～ 1999

	// シネンの森出口の地面当たり変更マップイベント ID
	static const u16	EVID_EP01_MAP_FOREST4_EXIT_PASS				= 6;

	// アサ宅での休息マップイベント ID
	static const u16	EVID_EP01_MAP_REST_AT_HOME					= 1001;
	// アサ宅で休息後に外に出たときのマップイベント ID
	static const u16	EVID_EP01_MAP_DEPARTURE_AFTER_REST_AT_HOME	= 1002;
	// コルポが出迎えるマップイベント ID （Ev1002 からの自動呼出し or Overrider 用）
	static const u16	EVID_EP01_MAP_MEETING_BY_CORPO				= 1003;
	// 村入口での対決敗北確認マップイベント ID
	static const u16	EVID_EP01_MAP_CHECK_CONFRONT_LOSE			= 1004;
	// 村入口での対決敗北台詞マップイベント ID
	static const u16	EVID_EP01_MAP_MESSAGE_CONFRONT_LOSE			= 1005;
	// コルポが出迎えるマップイベント ID（手動呼び出し用）
	static const u16	EVID_EP01_MAP_MEETING_BY_CORPO_MANUAL		= 1007;
	// アサ宅で休息直後（まだ自宅内）のマップイベント ID（Overrider 用）
	static const u16	EVID_EP01_MAP_AFTER_REST_AT_HOME			= 1008;
	// 手動呼び出しアサ宅での休息マップイベント ID（Overrider 用）
	static const u16	EVID_EP01_MAP_AFTER_REST_AT_HOME_MANUAL		= 1009;

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// Epsode01 依存の固定的イベントケース ID

	// アサ宅での休息マップイベント
	static const u32	ECID_EP01_MAP_REST_AT_HOME					= 1701;

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// Epsode01 依存の固定的制御イベント ID

	static const u16	CTRLEVID_EP01_TUTORIAL_STONE				= 51002;	// チュートリアルの石
	static const u16	CTRLEVID_EP01_CERJA_TREE					= 51006;	// セルジャ記念樹
	static const u16	CTRLEVID_EP01_REIKA							= 51007;	// レイカ
	static const u16	CTRLEVID_EP01_SARUNAHA_ENEMY				= 51009;	// 大猿敵キャラオブジェ
	static const u16	CTRLEVID_EP01_KUMA_ENEMY					= 51013;	// シネンの森出口のコグナ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_ST_DEP_EV_DEF_H_
