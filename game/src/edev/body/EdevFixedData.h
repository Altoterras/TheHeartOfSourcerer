/***********************************************************************//**
 *	EdevFixedData.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/27.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _EDEV_BODY_EDEV_FIXED_DATA_H_
#define _EDEV_BODY_EDEV_FIXED_DATA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../etk/particle/ParticleDefTbl.h"
#include "../../tfw/lib/StringTable.h"

////////////////////////////////////////////////////////////////////////////
// クラス

EDEV_BEGIN_NS

class Game;

/*---------------------------------------------------------------------*//**
 *	固定データ
 *		※固定データ集
 *		　将来的に外部ファイル化も検討
 *		※タイトル依存
 *
**//*---------------------------------------------------------------------*/
class EdevFixedData
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

EDEV_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _EDEV_BODY_EDEV_FIXED_DATA_H_
