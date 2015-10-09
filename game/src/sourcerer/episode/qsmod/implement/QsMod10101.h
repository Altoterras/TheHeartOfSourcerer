/***********************************************************************//**
 *	QsMod10101.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/06.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_QSMOD_IMPLEMENT_QS_MOD_10101_H_
#define _SRCR_EPISODE_QSMOD_IMPLEMENT_QS_MOD_10101_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../quest/qsmod/QsMod.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	クエスト 10101 モジュール
 *
**//*---------------------------------------------------------------------*/
class QsMod10101 : public QsMod
{
	//======================================================================
	// 定数
private:
	static const s32 NUM_ENTITY = 3;
	static const u32 CTRLEVID_HERB_01 = 51010;		// ハーブ１
	static const u32 CTRLEVID_HERB_02 = 51011;		// ハーブ２
	static const u32 CTRLEVID_HERB_03 = 51012;		// ハーブ３

	//======================================================================
	// クラス
private:
	class QsEntity
	{
	public:
		u32	_ctrlevid;
		bool _ok;
		QsEntity() : _ctrlevid(0) { clear(); }
		void setup(u32 ctrlevid) { _ctrlevid = ctrlevid; }
		void clear() { _ok = false; }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	bool create(QuestArticle* qstaRef);

	void notifyChangedMap(bool isDeserializeCase);
	void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat);

	//======================================================================
	// 変数
private:
	QsEntity _ent[NUM_ENTITY];
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_QSMOD_IMPLEMENT_QS_MOD_10101_H_
