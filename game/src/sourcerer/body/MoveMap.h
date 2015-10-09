/***********************************************************************//**
 *	MoveMap.h
 *	Enlight MoveMap Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/26.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MOVE_MAP_H_
#define _SRCR_MOVE_MAP_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/serialize/SerializeBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Game;

/*---------------------------------------------------------------------*//**
 *	マップ移動モジュール
 *		※	Game モジュールのマップ移動部分を抜き出したもの
 *
**//*---------------------------------------------------------------------*/
class MoveMap : public SerializeBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 現在のマップ ID を得る
	inline s32 getCurMapId() const { return _mapidCur; }
	// 一つ前のマップ ID を得る
	inline s32 getPrevMapId() const { return _mapidPrev; }
	// 予約中のマップ ID を得る
	inline s32 getReservedMapId() const { return _mapidReserved; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool changeMap(s32 mapid, u32 evparamMapStart);
	void reserveChangeMap(s32 mapid);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MoveMap();
	~MoveMap();
	bool create();
	void destroy();
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);
	void exec();

	bool changeMap(s32 mapid, u32 evparamMapStart, bool isDeserializeCase);

	//======================================================================
	// 変数
private:
	s32 _mapidCur;			// 現在のマップ ID
	s32 _mapidPrev;			// 一つ前のマップ ID
	s32 _mapidReserved;		// 予約された次のマップ ID
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MOVE_MAP_H_
