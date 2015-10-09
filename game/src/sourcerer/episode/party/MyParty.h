/***********************************************************************//**
 *	MyParty.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_PARTY_MY_PARTY_H_
#define _SRCR_EPISODE_PARTY_MY_PARTY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../body/Party.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class CharUnit;
class ItemManager;
class SaveStructure;
class SourceManager;

/*---------------------------------------------------------------------*//**
 *	プレイヤー キャラクタ パーティ
 *
**//*---------------------------------------------------------------------*/
class MyParty : public Party
{
	//======================================================================
	// 定数
public:
	// プレイヤーキャラクタ定義（タイトル依存部）
	enum PC
	{
		PC_MY,		// 自キャラ
		PC_PTN,		// パートナー
		PC_HLP,		// ヘルパー
		NUM_PC
	};

private:
	static const s32	NUM_PC_MAX	= NUM_PC;	// 今作は３人が最大

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// サブシステムを得る
	virtual SourceManager*	getSourceManager() const	{	return _srcmng;	}
	virtual ItemManager*	getItemManager() const		{	return _itmmng;	}

	// プレイヤーキャラクタが有効かどうかを得る
	bool isEnablePc(int ipc) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool loadPcChars();
	bool isValidLeader(s32 ipc);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MyParty();
	virtual ~MyParty();
	virtual bool create();
	virtual void destroy();
	virtual bool serialize(Serializer* ser) const;
	virtual bool deserialize(Serializer* ser);

	void reserveDefaultSourceList();
	bool isSerializableNow();

protected:
	bool moveMapInitLeaderPos();

	//======================================================================
	// 変数
protected:
	SourceManager* _srcmng;
	ItemManager* _itmmng;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_PARTY_MY_PARTY_H_
