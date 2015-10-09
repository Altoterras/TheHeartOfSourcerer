/***********************************************************************//**
 *	Party.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_BODY_PARTY_H_
#define _SRCR_BODY_PARTY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/serialize/SerializeBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

SRCR_BEGIN_NS

class Game;
class ItemManager;
class PcSpirit;
class PcUnit;
class SourceManager;
class Unit;

/*---------------------------------------------------------------------*//**
 *	プレイヤー キャラクタ パーティ
 *
**//*---------------------------------------------------------------------*/
class Party : public SerializeBase
{
	//======================================================================
	// クラス
protected:
	/*-----------------------------------------------------------------*//**
	 *	プレイヤー キャラクタ
	 *
	**//*-----------------------------------------------------------------*/
	class Pc
	{
	public:
		PcUnit* _unitRef;
		Pc() : _unitRef(0L) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// プレイヤーキャラクタの数を得る
	inline s32 getPcNum() const { return _numPc; }
	// プレイヤーキャラクタのユニットを得る
	const PcUnit* getPcUnit(s32 ipc) const;
	PcUnit* pcUnit(s32 ipc);
	// プレイヤーキャラクタのスピリットを得る
	const PcSpirit* getPcSpirit(s32 ipc) const;
	PcSpirit* pcSpirit(s32 ipc);
	// プレイヤーキャラクタが有効かどうかを得る
	virtual bool isEnablePc(s32 ipc) const = 0;
	// リーダープレイヤーキャラクタのインデックスを得る
	inline s32 getLeaderPcIndex() const { return _ipcLeader; }
	// リーダープレイヤーキャラクタのユニットを得る
	const PcUnit* getLeaderPcUnit() const;
	PcUnit* leaderPcUnit();

	// サブシステムを得る
	virtual SourceManager*	getSourceManager() const	{	return 0L;	}
	virtual ItemManager*	getItemManager() const		{	return 0L;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setEnablePcChar(s32 ipc, bool isEnable);
	virtual bool setLeader(s32 ipc);
	virtual bool isValidLeader(s32 ipc);
	s32 findPcUnit(const Unit* pcunit) const;

	void animUnitOnEvent(const Unit* unitTrg, u16 evid);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Party();
	~Party();
	virtual bool create(s32 numPc);
	virtual void destroy();
	virtual bool serialize(Serializer* ser) const = 0;
	virtual bool deserialize(Serializer* ser) = 0;
	virtual void notifyChangedMap(bool isDeserializeCase);
	virtual void notifyEventActMatterTrigger(u16 evid, bool start, const Unit* unitTrg);

protected:
	virtual bool moveMapInitLeaderPos() = 0;

private:
	void animUnit(const Unit* unitTrg);

	//======================================================================
	// 変数
protected:
	Pc* _pcarr;
	s32 _numPc;
	s32 _ipcLeader;
	u16 _evidAim;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_BODY_PARTY_H_
