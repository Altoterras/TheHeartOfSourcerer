/***********************************************************************//**
 *	AtArFixedData.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_BODY_AT_AR_FIXED_DATA_H_
#define _ATAR_BODY_AT_AR_FIXED_DATA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../etk/particle/ParticleDefTbl.h"
#include "../../tfw/lib/StringTable.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ATAR_BEGIN_NS

class Game;

/*---------------------------------------------------------------------*//**
 *	固定データ
 *		※固定データ集
 *		　将来的に外部ファイル化も検討
 *		※タイトル依存
 *
**//*---------------------------------------------------------------------*/
class AtArFixedData
{
	//======================================================================
	// 定数
private:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
public:
	// システム固定文字列テーブルを得る
	static inline const StringTable* getFixedSystemStringTable() { return &_strtblSys; }
	// パーティクル定義テーブルを得る
	static inline const ParticleDefTbl* getParticleDefTable() { return &_ptcldftbl; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool init();
	static void	cleanup();
	static void notifyChangeLanguage();

private:
	static void initFixedSystemStringTable();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 変数
private:
	static StringTable _strtblSys;
	static ParticleDefTbl _ptcldftbl;
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_BODY_AT_AR_FIXED_DATA_H_