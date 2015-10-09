/***********************************************************************//**
 *	CharFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/29.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_CHAR_FACTORY_H_
#define _SRCR_EPISODE_CHAR_CHAR_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class EnemyUnit;
class EnemySpirit;

/*---------------------------------------------------------------------*//**
 *	ストーリー用イベント マター プログラム ファクトリー
 *
**//*---------------------------------------------------------------------*/
class CharFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool newEnemy(EnemyUnit** unit, EnemySpirit** sprt, u16 charid);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_CHAR_CHAR_FACTORY_H_

