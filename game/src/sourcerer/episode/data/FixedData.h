/***********************************************************************//**
 *	FixedData.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/27.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_DATA_FIXED_DATA_H_
#define _SRCR_EPISODE_DATA_FIXED_DATA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

#include "../../char/CharDefTbl.h"
#include "../../char/FixedCharParamTbl.h"
#include "../../char/CharLogicDataTbl.h"
#include "../../item/ItemDefTbl.h"
#include "../../quest/QuestDefTbl.h"
#include "../../source/FixedSourceDefTbl.h"

#include "../../../etk/particle/ParticleDefTbl.h"

#include "../../../tfw/lib/StringTable.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Game;

/*---------------------------------------------------------------------*//**
 *	固定データ
 *		※固定データ集
 *		　将来的に外部ファイル化も検討
 *		※タイトル依存
 *
**//*---------------------------------------------------------------------*/
class FixedData
{
	//======================================================================
	// 定数
private:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// システム固定文字列テーブルを得る
	static inline const StringTable* getFixedSystemStringTable() { return &_strtblSys; }
	// キャラクタ定義テーブルを得る
	static inline const CharDefTbl* getCharDefTable() { return &_chrdftbl; }
	// 固定キャラクタパラメータテーブルを得る
	static inline const FixedCharParamTbl* getFixedCharParamTable() { return &_fcprmtbl;  }
	// NPC ロジックデータテーブルを得る
	static inline const CharLogicDataTbl* geCharLogicDataTable() { return &_nldtbl;  }
	// アイテム定義テーブルを得る
	static inline const ItemDefTbl* getItemDefTable() { return &_itmdftbl; }
	// 固定ソース定義テーブルを得る
	static inline const FixedSourceDefTbl* getFixedSourceDefTable() { return &_fsrcdftbl; }
	// パーティクル定義テーブルを得る
	static inline const ParticleDefTbl* getParticleDefTable() { return &_ptcldftbl; }
	// クエスト定義テーブルを得る
	static inline const QuestDefTbl* getQuestDefTable() { return &_qsdftbl; }

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
	static CharDefTbl _chrdftbl;
	static FixedCharParamTbl _fcprmtbl;
	static CharLogicDataTbl _nldtbl;
	static ItemDefTbl _itmdftbl;
	static FixedSourceDefTbl _fsrcdftbl;
	static ParticleDefTbl _ptcldftbl;
	static QuestDefTbl _qsdftbl;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_DATA_FIXED_DATA_H_
