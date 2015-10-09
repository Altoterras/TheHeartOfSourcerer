/***********************************************************************//**
 *	EventIdDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_EVENT_ID_DEF_H_
#define _SRCR_EVENT_EVENT_ID_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	イベント ID の定義
 *		※イベント ID は同時存在するマター間で重複してはならない
 *
**//*---------------------------------------------------------------------*/
/*

■ イベントの種別

(1) マップイベント
	[ マップ ID ]
		10001 ～ 99999
	[ イベント ID ]
		1 ～ 10000
		うち、1 ～ 100 はマップの地面コリジョンの位置指定で使用
	[ 説明 ]
		マップと同時に自動的に読み込まれ
		違うマップに移動すると破棄されるイベント
	[ ファイル名 ]
		MapXXXXX_ev.xml (XXXXX = マップ ID)

(2) ストーリーイベント
	[ ストーリー ID ]
		10001 ～ 99999
	[ イベント ID ]
		10001 ～ 20000
	[ 説明 ]
		イベントストーリーごとに読まれマップをまたぐことができる
		状態は新しいストーリーに移るまで保持される
	[ ファイル名 ]
		StEvXXXXX.xml (XXXXX = ストーリー ID)
		EvXXXXX_YY.msgd (YY = 言語識別子)

(3) クエストイベント
	[ クエスト ID ]
		10001 ～ 99999
	[ イベント ID ]
		20001 ～ 35000
		※ ＋クエストスロット× 100 をクエスト毎に増算する
		　　つまり、任意に使用できるのは下２桁分まで
	[ 説明 ]
		イベントストーリーごとに読まれマップをまたぐことができる
	[ ファイル名 ]
		QsEvXXXXX.xml (XXXXX = クエスト ID)
		EvXXXXX_YY.msgd (YY = 言語識別子)

(4) テンポラリイベント
	[ テンポラリイベント ID ]
		10001 ～ 99999
	[ イベント ID ]
		30001 ～ 40000
	[ 説明 ]
		呼び出し時に即座に読み込まれマップをまたぐことができるが
		終了後に即座破棄されるイベント
	[ ファイル名 ]
		TmpEvXXXXX.xml (XXXXX = テンポラリイベント ID)
		TmpEvXXXXX_YY.msgd (YY = 言語識別子)

(5) 自動割り当てイベント
	[ イベント ID ]
		40001 ～ 41000

(6) システム固定イベント
	[ イベント ID ]
		41001 ～ 42000
	[ 説明 ]
		システム初期化時に用意され
		（実行条件はあるが）いつでも実行できるイベント

(7) スクリプト制御イベント ID
	[ イベント ID ]
		50001 ～ 60000
	[ 説明 ]
		スクリプト制御用に使用する
		『管理表_イベント.ods』で ID を管理する
		1 作目はとりあえず 51001 ～ 51999 を使用する


■ マップ ID 規則

ABBCC
A = Episode - １作目は「1」
BB = エリア番号 (村、森など)
CC = エリア内の番号


■ エリア番号規則

01 = 寺院
02 = コメサット村
03 = エオの村
11 = グアラドの森
12 = 草原
13 = 山道
14 = エオの森
21 = 古代遺跡
22 = 闘いの洞窟
91 = デバッグ


■ ストーリー ID 規則

ABBCC
A = Episode - １作目は「1」
BB = チャプター番号
CC = チャプター内番号


■ チャプター番号

01 = オープニング
02 = アサイベント
03 = コルポイベント
04 = エオ族
05 = 古代遺跡
06 = エンディング


■ クエスト ID 規則

ABBCC
A = Episode - １作目は「1」
BB = エリア番号
CC = エリア内番号



■ テンポラリイベント ID 規則

ABBBB
A = Episode - １作目は「1」
BBBB = 作内テンポラリイベント番号

*/
class EventIdDef
{
	//======================================================================
	// 定数
public:
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// 読込イベント ID 範囲定義

	// マップイベントのイベント ID （1 ～ 10000）
	static const u16	EVID_MAP__START_			= 1;
	static const u16	EVID_MAP__END_				= 10000;
	
	// ストーリーイベントのイベント ID （10001 ～ 20000）
	static const u16	EVID_STORY__START_			= 10001;
	static const u16	EVID_STORY__END_			= 20000;
	
	// クエストイベントのイベント ID （20001 ～ 35000）
	static const u16	EVID_QUEST__START_			= 20001;
	static const u16	EVID_QUEST__END_			= 35000;
	
	// テンポラリイベントのイベント ID （35001 ～ 40000）
	static const u16	EVID_TMP__START_			= 35001;
	static const u16	EVID_TMP__END_				= 40000;
	
	// 自動割り当てイベント ID （40001 ～ 41000）
	static const u16	EVID_AUTO__START_			= 40001;
	static const u16	EVID_AUTO__END_				= 41000;
	
	// システム固定イベント ID （41001 ～ 42000）
	static const u16	EVID_FIXED__START_			= 41001;
	static const u16	EVID_FIXED__END_			= 42000;
	
	// 制御イベント ID （50001 ～ 60000）
	static const u16	EVID_CTRL__START_			= 50001;
	static const u16	EVID_CTRL__END_				= 60000;
	// PlacementObj 無指定時自動割り当て制御イベント ID
	static const u16	CTRLEVID_MAPCONF__START_	= 50001;
	static const u16	CTRLEVID_MAPCONF__END_		= 50300;
	static const u16	CTRLEVID_STORY__START_		= 50301;
	static const u16	CTRLEVID_STORY__END_		= 50600;
	static const u16	CTRLEVID_MAPTMP__START_		= 50601;
	static const u16	CTRLEVID_MAPTMP__END_		= 50650;
	static const u16	CTRLEVID_QUEST__START_		= 55001;
	static const u16	CTRLEVID_QUEST__END_		= 59000;

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// システム固定イベント ID 定義

	static const u16	EVID_FIXED_DOWN_ME					= 41001;					// 主人公ダウン
	static const u16	EVID_FIXED_QUEST_ACHVED__START_		= EVID_QUEST__START_ + 10;	// クエスト条件達成
	static const u16	EVID_FIXED_QUEST_CANCEL__START_		= EVID_QUEST__START_ + 20;	// クエスト キャンセル
	static const u16	EVID_FIXED_QUEST_FINISH__START_		= EVID_QUEST__START_ + 30;	// クエスト クリア

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// 読込ケース ID 範囲定義
	
	// クエストイベントのケース ID （20001 ～ 35000）
	static const u16	CASEID_QUEST__START_		= 20001;
	static const u16	CASEID_QUEST__END_			= 35000;

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// クエストのイベント ID とメッセージ ID のスロット追加値
	static const u16	EVID_QUEST_SLOT_MULT		= 100;
	static const u16	MSGID_QUEST_SLOT_MULT		= 100;
	static const u16	CTRLEVID_QUEST_SLOT_MULT	= 25;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_EVENT_ID_DEF_H_
