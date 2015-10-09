/***********************************************************************//**
 *	EventMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_EVENT_MATTER_H_
#define _SRCR_EVENT_MATTER_EVENT_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
#include "../../../tfw/evsys/EvMatter.h"

#include "../EventCause.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class StatusChangeResult;
class EventMatterCreateParam;
class EventSys;
class Game;
class MagicCluster;
class Unit;
class Souma;
class TransStat;

/*---------------------------------------------------------------------*//**
 *	イベント マター（事象）
 *
 *		※１：
 *			イベント ID は同時存在するマター間で重複してはならない
 *		※２：
 *			ケース ID はマター間で重複しても良い（重複すること前提）が、
 *			同時存在するマター間で別の意味合いのものがあってはならない
 *
**//*---------------------------------------------------------------------*/
class EventMatter : public EvMatter
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	システム依存のフラグによる起動条件
	 *
	**//*-----------------------------------------------------------------*/
	class CondFlags
	{
		//==================================================================
		// 定数
	public:
		static const u32	FLG_NULL				= 0x00000000;
		static const u32	FLG_SDF_POSITIVE		= 0x00000001;
		static const u32	FLG_PDF_POSITIVE		= 0x00000002;
		static const u32	FLG_SDF_TURN			= 0x00000004;
		static const u32	FLG_PDF_TURN			= 0x00000008;
		static const u32	FLG_AUTOBGN_MAPSTART	= 0x00000010;

		//==================================================================
		// メソッド
	public:
		// フラグを得る
		inline int		getFlags() const				{	return _flags;									}
		// セーブデータフラグ ID を得る
		inline int		getSaveDataFlagId() const		{	return _fidSaveData;							}
		// プレイデータフラグ ID を得る
		inline int		getPlayDataFlagId() const		{	return _fidPlayData;							}
		// セーブデータ数値 ID を得る
		inline int		getSaveDataNumberId() const		{	return _nidSaveData;							}
		// キー数値を得る
		inline int		getKeyNumber() const			{	return _numberKey;								}
		// セーブデータフラグを＋値で評価するかどうかを得る
		inline bool		isSaveDataFlagPositive() const	{	return (_flags & FLG_SDF_POSITIVE) != 0;		}
		// プレイデータフラグを＋値で評価するかどうかを得る
		inline bool		isPlayDataFlagPositive() const	{	return (_flags & FLG_PDF_POSITIVE) != 0;		}
		// マター起動後にセーブデータフラグを反転させるかを得る
		inline bool		isSaveDataFlagTurn() const		{	return (_flags & FLG_SDF_TURN) != 0;			}
		// マター起動後にプレイデータフラグを反転させるかを得る
		inline bool		isPlayDataFlagTurn() const		{	return (_flags & FLG_PDF_TURN) != 0;			}
		// マップ開始時にマターを起動させるかどうかを得る
		inline bool		isBeginMapStart() const			{	return (_flags & FLG_AUTOBGN_MAPSTART) != 0;	}

		CondFlags()
			: _fidSaveData(0)
			, _fidPlayData(0)
			, _nidSaveData(0)
			, _numberKey(0)
			, _flags(0)
		{}
		CondFlags(s32 fidSaveData, s32 fidPlayData, s32 nidSaveData, s32 numberKey, s32 flags)
			: _fidSaveData(fidSaveData)
			, _fidPlayData(fidPlayData)
			, _nidSaveData(nidSaveData)
			, _numberKey(numberKey)
			, _flags(flags)
		{}
		void copy(const CondFlags* src)
		{
			_fidSaveData = src->_fidSaveData;
			_fidPlayData = src->_fidPlayData;
			_nidSaveData = src->_nidSaveData;
			_numberKey = src->_numberKey;
			_flags = src->_flags;
		}

		//==================================================================
		// 変数
	public:
		s32 _fidSaveData;
		s32 _fidPlayData;
		s32 _nidSaveData;
		s32 _numberKey;
		s32 _flags;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 条件を得る
	inline const CondFlags*	getCondFlags() const { return &_condf; }

protected:
	EventSys* getOwner() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool judgeBeginFlags(u16 evid, u32 flagsCond) const;
	bool judgeBeginCustom() const;
	void markBegin();
	void setLockWithParent(bool isLock);
	void offsetEventId(const EventMatterCreateParam* emcp);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EventMatter(u16 emtype);
	virtual ~EventMatter();
	void setCondFlags(const CondFlags* condf);

	virtual void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat) {}
	virtual void notifyDischargeSouma(Souma* souma, u16 mcid) {}
	virtual void notifyDisappearMagicCluster(MagicCluster* mc, u32 dcflags) {}

	//======================================================================
	// 変数
private:
	CondFlags _condf;				// システム依存のフラグによる起動条件
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_EVENT_MATTER_H_
