/***********************************************************************//**
 *	QuestDef.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/25.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QuestDef.h"

// Friends
#include "../../tfw/file/FileBase.h"

// External

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QuestDef メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QuestDef::QuestDef()
	: _questid(0)
	, _storyidMin(0)
	, _storyidMax(0)
	, _questidCleared(0)
	, _situflagsC01(0)
	, _isSituflagsC01Negative(0)
	, _probabilityFirst(0)
	, _probabilityRepeat(0)
	, _grpidXor(0)
	, _mapidBasePos(0)
	, _mapidClearPos(0)
	, _charidBasePos(0)
	, _charidClearPos(0)
	, _ctrlevidBasePos(0)
	, _ctrlevidClearPos(0)
	, _achvkind(0)
	, _didAchv(0)
	, _rewkind(0)
	, _rewkindPub(0)
	, _valueReward(0)
	, _difflv(0)
	, _param1(0)
	, _param2(0)
	, _param3(0)
	, _param4(0)
	, _strName(0L)
	, _strRequisite(0L)
	, _strReward(0L)
	, _strClient(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
QuestDef::~QuestDef()
{
	ASSERT(_strName == 0L);
	ASSERT(_strRequisite == 0L);
	ASSERT(_strReward == 0L);
	ASSERT(_strClient == 0L);
}

/*---------------------------------------------------------------------*//**
	データファイルからデータ部を読み込む
**//*---------------------------------------------------------------------*/
bool QuestDef::loadFromDataFile(FileBase* file)
{
	u16 wk;

	// 定義 ID
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_questid = wk;

	// ［起動条件］このストーリー ID 以上である必要がある
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_storyidMin = wk;

	// ［起動条件］このストーリー ID 以下である必要がある
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_storyidMax = wk;

	// ［起動条件］このクエストをクリアしている必要がある
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_questidCleared = wk;

	// ［起動条件］シチュエーションフラグ
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_situflagsC01 = wk;

	// ［起動条件］シチュエーションフラグ判定を反転するかどうか
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_isSituflagsC01Negative = wk;

	// ［起動条件］条件一致時の初期起動確率（1～100）
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_probabilityFirst = wk;

	// ［起動条件］条件一致時の２回目以降起動確率（1～100）
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_probabilityRepeat = wk;

	// ［起動条件］排他起動グループ ID
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_grpidXor = wk;

	// ［基点］発生マップ ID
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_mapidBasePos = wk;

	// ［基点］クリアマップ ID
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_mapidClearPos = wk;

	// ［基点］発生キャラ ID
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_charidBasePos = wk;

	// ［基点］クリアキャラ ID
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_charidClearPos = wk;

	// ［基点］発生制御イベント ID
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_ctrlevidBasePos = wk;

	// ［基点］クリア制御イベント ID
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_ctrlevidClearPos = wk;

	// ［達成］達成種別
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_achvkind = wk;

	// ［達成］達成対象 ID
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_didAchv = wk;

	// ［報酬］報酬種別
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_rewkind = wk;

	// ［報酬］公開報酬種別
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_rewkindPub = wk;

	// ［報酬］報酬値
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_valueReward = wk;

	// ［その他］公開難易度
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_difflv = wk;

	// ［汎用パラメータ］汎用パラメータ 1
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_param1 = wk;

	// ［汎用パラメータ］汎用パラメータ 2
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_param2 = wk;

	// ［汎用パラメータ］汎用パラメータ 3
	if(file->read(&wk, sizeof(u16)) != sizeof(u16))	{	return false;	}
	_param3 = wk;

	return true;
}

/*---------------------------------------------------------------------*//**
	テキスト設定
**//*---------------------------------------------------------------------*/
bool QuestDef::setText(const VcString* strName, const VcString* strCondition, const VcString* strReward, const VcString* strClient)
{
	delete _strName;
	delete _strRequisite;
	delete _strReward;
	delete _strClient;
	_strName = new VcString(strName);
	_strRequisite = new VcString(strCondition);
	_strReward = new VcString(strReward);
	_strClient = new VcString(strClient);
	return (_strName != 0L) && (_strRequisite != 0L) && (_strReward != 0L) && (_strClient != 0L);
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void QuestDef::cleanup()
{
	delete _strName;
	_strName = 0L;
	delete _strRequisite;
	_strRequisite = 0L;
	delete _strReward;
	_strReward = 0L;
	delete _strClient;
	_strClient = 0L;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
