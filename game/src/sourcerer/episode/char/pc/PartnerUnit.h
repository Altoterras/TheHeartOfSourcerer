/***********************************************************************//**
 *	PartnerUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_PC_PARTNER_UNIT_H_
#define _SRCR_EPISODE_CHAR_PC_PARTNER_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../unit/char/pc/PcUnit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

class Elene;
class Item;
class ItemEquip;
class PartnerSpirit;

/*---------------------------------------------------------------------*//**
 *	準主人公（プレイヤーキャラクタ２）ユニット
 *
**//*---------------------------------------------------------------------*/
class PartnerUnit : public PcUnit
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// アイテム装備機能を得る
	inline const ItemEquip* getItemEquip() const { return _itmeq; }
	inline ItemEquip* itemEquip() { return _itmeq; }
	// 自らの（型の）スピリットを得る
	inline PartnerSpirit* getThisSpirit() const	{ return (PartnerSpirit*)getSpirit(); }
	// フレンド関係にあるユニットの配列を得る
	UnitArray getFriends() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void changeAnim(u16 animid, bool isLoop);
	void equipBest();
	void updateEquipModelObjVisibility();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PartnerUnit();
	~PartnerUnit();
	bool create(Spirit* sprtRef, Party* party);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void execChangeMoveAnimationToStop(const ExecCtx* ec, u16 cmflags);
	void execChangeMoveAnimationToRun(const ExecCtx* ec, u16 cmflags);
	void execChangeMoveAnimationToWalk(const ExecCtx* ec, u16 cmflags);
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);
	void notifyChangedEventMode(bool isEventMode);

	void calcTargetPos();

	//======================================================================
	// 変数
protected:
	ItemEquip* _itmeq;	// アイテム装備機能.
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_CHAR_PC_PARTNER_UNIT_H_
