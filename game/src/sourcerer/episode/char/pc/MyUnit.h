/***********************************************************************//**
 *	MyUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_PC_MY_UNIT_H_
#define _SRCR_EPISODE_CHAR_PC_MY_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../unit/char/pc/PcUnit.h"

////////////////////////////////////////////////////////////////////////////
// マクロ

#define DEBUG_SHOW_MVCURSOR_POS		0	// MoveCursor の 3D 座標表示デバッグ

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Model;
class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

class CharStat;
class FriendCharUnit;
class HelperUnit;
class Item;
class ItemEquip;
class MagicCluster;
class MySpirit;
class PartnerUnit;
class MyParty;
class Souma;
class Spirit;

/*---------------------------------------------------------------------*//**
 *	主人公（プレイヤーキャラクタ１）ユニット
 *
**//*---------------------------------------------------------------------*/
class MyUnit : public PcUnit
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 自らの（型の）スピリットを得る
	inline MySpirit* getThisSpirit() const	{ return (MySpirit*)getSpirit(); }
	// フレンド関係にあるユニットの配列を得る
	UnitArray getFriends() const;

	// アイテム装備機能を得る
	inline const ItemEquip* getItemEquip() const { return _itmeq; }
	inline ItemEquip* itemEquip() { return _itmeq; }
	// ソウマモジュールを得る
	Souma* getSouma() const;

	// 一度に呼び出せる元素エネルギー量を得る（ソウマを使えるユニットのみの実装）
	bool getEvokableElene(Elene* elene, EvokableElneCategory evecat) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MyUnit();
	~MyUnit();
	bool create(Spirit* sprtRef, Party* party);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
#if DEBUG_SHOW_MVCURSOR_POS
	void render(const RenderCtx* rc);			// 描画
#endif
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);
	void notifyChangedMap();
	void notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition);
	void notifyChangedEventMode(bool isEventMode);

	void calcTargetPos();

	//======================================================================
	// 変数
protected:
	ItemEquip* _itmeq;		// アイテム装備機能.
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_CHAR_PC_MY_UNIT_H_
