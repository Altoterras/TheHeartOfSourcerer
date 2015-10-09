/***********************************************************************//**
 *	EvMatterCreateParam.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/24.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_EVENT_MATTER_CREATE_PARAM_H_
#define _SRCR_EVENT_MATTER_EVENT_MATTER_CREATE_PARAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
#include "../../../tfw/evsys/EvMatterCreateParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	イベント マター作成パラメータ
 *
**//*---------------------------------------------------------------------*/
class EventMatterCreateParam : public EvMatterCreateParam
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	inline u16 offsetEvid(u16 evid) const
	{
		if((_evidOfsRangeStart <= evid) && (evid < _evidOfsRangeEnd))
		{
			return evid + _ofsEvid;
		}
		else
		{
			return evid;
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EventMatterCreateParam()
		: _ofsMsgid(0)
		, _ofsEvid(0)
		, _evidOfsRangeStart(0)
		, _evidOfsRangeEnd(0)
		, _questid(0)
	{
	}

	//======================================================================
	// 変数
public:
	u16 _ofsMsgid;			// メッセージ ID を設定値よりずらすオフセット値
	u16 _ofsEvid;			// イベントID を設定値よりずらすオフセット値
	u16 _evidOfsRangeStart;	// オフセットイベントの対象開始イベント ID （この値を含む）
	u16 _evidOfsRangeEnd;	// オフセットイベントの対象終了ベント ID （この値を含まない）
	u16 _questid;			// クエストイベント解析時のクエスト ID
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_EVENT_MATTER_CREATE_PARAM_H_
