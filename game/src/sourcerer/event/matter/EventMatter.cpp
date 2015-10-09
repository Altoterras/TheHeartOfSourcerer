/***********************************************************************//**
 *	EventMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EventMatter.h"

// Friends
#include "EventMatterCreateParam.h"
#include "../EventData.h"
#include "../EventSys.h"
#include "../../body/Game.h"
#include "../../save/SaveSys.h"
#include "../../save/SaveData.h"

// External
#include "../../../tfw/lib/Vector.h"
#include "../../../tfw/lib/XmlParser.h"
#include "../../../tfw/string/StringUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	イベントシステムを得る
**//*---------------------------------------------------------------------*/
EventSys* EventMatter::getOwner() const
{
	return Game::getGame()->getEventSys();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	フラグによる開始判定
**//*---------------------------------------------------------------------*/
bool EventMatter::judgeBeginFlags(u16 evid, u32 flagsCond) const
{
	if((flagsCond & _condf.getFlags()) == flagsCond)
	{
		if((evid == getEvId()) || (evid == 0))		// イベント ID が 0 指定の場合はイベント ID に関わらず起動する
		{
			return true;	// 開始 OK
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	セーブデータによる開始判定
**//*---------------------------------------------------------------------*/
bool EventMatter::judgeBeginCustom() const
{
	SaveData* svdat = Game::getGame()->getSaveSys()->getSaveData();
	
	// セーブデータフラグチェック
	int fidSd = _condf.getSaveDataFlagId();
	if(fidSd != 0)
	{
		bool flagSd = svdat->getSaveF(fidSd);
		if(!_condf.isSaveDataFlagPositive())	{	if(flagSd)	{	return false;	}	}	// Flag OFF で条件合格
		else									{	if(!flagSd)	{	return false;	}	}	// Flag ON で条件合格
	}
	
	// セーブデータ数値チェック
	int nidSd = _condf.getSaveDataNumberId();
	if(nidSd != 0)
	{
		s16 svnum = svdat->getSaveN(nidSd);
		if(_condf.getKeyNumber() != svnum)		{	return false;	}
	}
	
	// プレイデータフラグチェック
	int fidPd = _condf.getPlayDataFlagId();
	if(fidPd != 0)
	{
		bool flagPd = svdat->getPlayF(fidPd);
		if(!_condf.isPlayDataFlagPositive())	{	if(flagPd)	{	return false;	}	}	// Flag OFF で条件合格
		else									{	if(!flagPd)	{	return false;	}	}	// Flag ON で条件合格
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	開始に関わるセーブデータの変更
**//*---------------------------------------------------------------------*/
void EventMatter::markBegin()
{
	SaveData* svdat = Game::getGame()->getSaveSys()->getSaveData();
	
	// セーブデータフラグ反転
	int fidSd = _condf.getSaveDataFlagId();
	if((fidSd != 0) && _condf.isSaveDataFlagTurn())
	{
		bool flagSd = svdat->getSaveF(fidSd);
		svdat->setSaveF(fidSd, !flagSd);
	}
	
	// プレイデータフラグ反転
	int fidPd = _condf.getPlayDataFlagId();
	if((fidPd != 0) && _condf.isPlayDataFlagTurn())
	{
		bool flagPd = svdat->getPlayF(fidPd);
		svdat->setPlayF(fidPd, !flagPd);
	}
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
void EventMatter::setLockWithParent(bool isLock)
{
	u16 evidParent = getParentEvId();
	if(evidParent != 0L)
	{
		EventMatter* matter = getOwner()->getEventData()->getEventMatterFromEvid(evidParent);
		if(matter != 0L)
		{
			matter->setLock(isLock);
		}
	}
	setLock(isLock);
}

/*---------------------------------------------------------------------*//**
	イベント ID のオフセット処理
**//*---------------------------------------------------------------------*/
void EventMatter::offsetEventId(const EventMatterCreateParam* emcp)
{
	if(emcp != 0L)
	{
		setEvId(emcp->offsetEvid(getEvId()));
		setNextEvId(emcp->offsetEvid(getNextEvId()));
		setConcurrentEvId(emcp->offsetEvid(getConcurrentEvId()));
		setParentEvId(emcp->offsetEvid(getParentEvId()));
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EventMatter::EventMatter(u16 emtype)
	: EvMatter(emtype)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EventMatter::~EventMatter()
{
}

/*---------------------------------------------------------------------*//**
	条件フラグをセット

	@param	condf	条件フラグ
**//*---------------------------------------------------------------------*/
void EventMatter::setCondFlags(const CondFlags* condf)
{
	_condf.copy(condf);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
