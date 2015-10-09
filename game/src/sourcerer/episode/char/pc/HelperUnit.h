/***********************************************************************//**
 *	HelperUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_PC_HELPER_UNIT_H_
#define _SRCR_EPISODE_CHAR_PC_HELPER_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../unit/char/pc/PcUnit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

class Spirit;
class HelperSpirit;

/*---------------------------------------------------------------------*//**
 *	ヘルパーユニット
 *
**//*---------------------------------------------------------------------*/
class HelperUnit : public PcUnit
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 自らの（型の）スピリットを得る
	inline virtual HelperSpirit* getThisSpirit() const	{ return (HelperSpirit*)getSpirit(); }
	// フレンド関係にあるユニットの配列を得る
	UnitArray getFriends() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HelperUnit();
	~HelperUnit();
	bool create(Spirit* sprtRef, Party* party);
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);

	bool isOperatedForMoving();
	void calcTargetPos();

	//======================================================================
	// 変数
protected:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_CHAR_PC_HELPER_UNIT_H_
