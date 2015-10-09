/***********************************************************************//**
 *	SourceManager.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOURCE_SOURCE_MANAGER_H_
#define _SRCR_SOURCE_SOURCE_MANAGER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

class FixedSourceDef;
class FixedSourceDefTbl;
class Source;

/*---------------------------------------------------------------------*//**
 *	ソウマ ソース マネージャ
 *
**//*---------------------------------------------------------------------*/
class SourceManager
{
	//======================================================================
	// 定数
public:
	enum ReservedKind
	{
		RSVK_FREE,
		RSVK_FIXED,
		RSVK_USER,
	};

protected:
	static const	s32		NUM_SOURCE_MAX	= 100;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ソースの数を得る
	inline s32 getSourceNum() const { return _num; }
	// ソースを得る
	const Source* getSource(s32 index) const;
	Source* source(s32 index);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	s32 newSource(ReservedKind rsvkind);
	bool checkUnusedSourceName(const VcString* name) const;
	s32 getEmptySourceIndex(ReservedKind rsvkind) const { return getEmptySourceIndex(rsvkind, 0, false); }
	s32 getEmptySourceIndex(ReservedKind rsvkind, s32 indexSearchStart, bool isReverse) const;
	bool isEmptySourceIndex(ReservedKind rsvkind, s32 index) const;
	const FixedSourceDef* addFixedSource(u16 fsrcid);
	void reserveSourceFirst(s32 numReserveForUser, s32 numReserveForFixed);
	s32 releaseReservedUserSource(s32 numRelease, s32 numMaxFree);
	void sortSource();
	void changeAllSourceKind(s32 srckind);

#if defined(_DEBUG)
	void debug_addFixedSourceAll();
	void debug_updateFixedSources();
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SourceManager();
	~SourceManager();
	bool create(const FixedSourceDefTbl* fsrcdftblRef);
	void destroy();

	bool serialize(Serializer* ser);
	bool deserialize(Serializer* ser);

private:

	//======================================================================
	// 変数
private:
	Source* _srcarr;
	s32	_num;

	const FixedSourceDefTbl* _fsrcdftblRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOURCE_SOURCE_MANAGER_H_
