/***********************************************************************//**
 *	IdManager.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/07/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_COMMON_ID_MANAGER_H_
#define _TFW_COMMON_ID_MANAGER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../lib/BitFlagArray.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ID マネージャ クラス
 *	
**//*---------------------------------------------------------------------*/
class IdManager : public BitFlagArray
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline u32 getIdNum() const { return getFlagNum(); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool isUsedId(u16 id) const { return getFlag(id); }
	bool setUsedId(u16 id, bool isUsed) { return setFlag(id, isUsed); }
	u16 getNewId();
	void releaseId(u16 id);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	bool create(u32 numId) { return BitFlagArray::create(numId); }
	bool recreate(u32 numIdNew) { return BitFlagArray::recreate(numIdNew); }
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_COMMON_ID_MANAGER_H_

