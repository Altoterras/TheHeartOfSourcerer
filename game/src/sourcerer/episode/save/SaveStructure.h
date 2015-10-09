/***********************************************************************//**
 *	SaveStructure.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_SAVE_SAVE_STRUCTURE_H_
#define _SRCR_EPISODE_SAVE_SAVE_STRUCTURE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

#include "Score.h"
#include "Trophy.h"
#include "../../item/Item.h"
#include "../../item/ItemEquip.h"
#include "../../map/PlacementObj.h"
#include "../../quest/QuestArticle.h"
#include "../../source/Source.h"
#include "../../status/ActStat.h"
#include "../../status/CharStat.h"
#include "../../status/EleneStat.h"
#include "../../status/EquipPlace.h"
#include "../../status/EquipStat.h"
#include "../../unit/char/CharUnitSaveStructure.h"

#include "../../../tfw/lib/Vector.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

SRCR_BEGIN_NS

class PlacementObj;

/*---------------------------------------------------------------------*//**
 *	セーブ データ定義
 *
**//*---------------------------------------------------------------------*/
class SaveStructure
{
	//======================================================================
	// 定数
public:
	static const s32 NUM_PC_MAX		= 3;
	static const s32 NUM_POBJ_MAX	= 64;
	static const s32 NUM_ITEM_MAX	= 128;
	static const s32 NUM_SOURCE_MAX	= 128;
	static const u32 NUM_QUEST_MAX	= 128;
	static const s32 NUM_MAP_MAX	= 64;
	static const s32 NUM_NPC_MAX	= 32;
	static const s32 NUM_FLAGS_MAX	= 256;

	static const s32 SIZE_THIS		= 37376;

	static const char CHAR_SPRIT	= 0x1f;

	// バージョン番号
	static const u16 VER_MAJOR_SAVE_STRUCTURE			= 90;	// メジャーバージョン番号
	static const u8  VER_MINOR_SAVE_STRUCTURE			= 7;	// マイナーバージョン番号
	static const u8  VER_MINOR_SAVE_STRUCTURE_VER131	= 6;	// マイナーバージョン番号

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	概略のセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class SummarySaveStructure
	{
	public:
		// シーン
		static const u8 SCENE_TITLE		= 1;		// タイトル画面
		static const u8 SCENE_INGAME	= 2;		// インゲーム
		static const u8 SCENE_CLEAR		= 3;		// クリア直後
		// フラグ
		static const u16 F_CLEARED		= 0x0001;	// クリアしている
		static const u16 F_CLEARING		= 0x0002;	// クリア時のセーブデータ
		static const u16 F_EVINTVL		= 0x0004;	// イベント途中のセーブデータ
	public:
		// [  0]
		u32 _framePlay;							// [  4] プレイ時間
		u8 _scene;								// [  1] シーン
		u8 _cntClear;							// [  1] クリア回数
		u16 _flags;								// [  2] フラグ
		u32 _score;								// [  4] スコア（表示用）
		u8 __pad_0010[4];						// [  4]
		// [ 16]
		static const s32 SIZE_THIS = 16;
		SummarySaveStructure() { TFW_ZERO_MEMORY(this, sizeof(SummarySaveStructure)); }
	};

	/*-----------------------------------------------------------------*//**
	 *	ゲームのセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class GameSaveStructure
	{
	public:
		s32	_storyid;							// [    4] ストーリー ID
		bool _isPlayedStoryStartupEvent;		// [    1] ストーリーの最初のイベントを実行しているか
		u8 _optflags;							// [    1] オプションフラグ
		u16 _evidActing;						// [    2] 実行中の Act イベント ID
		s32 _mapidCur;							// [    4] 現在のマップ ID
		s32 _mapidPrev;							// [    4] 一つ前のマップ ID
		u32 _framePlay;							// [    4] プレイフレーム数
		u16 _ctrlevidAutoCntMapTmp;				// [    2] マップ一時配置用自動割当制御イベント ID カウンタ
		u8 __pad_0020[10];						// [   10]
		// [ 32]
		static const s32 SIZE_THIS = 32;
		GameSaveStructure() { TFW_ZERO_MEMORY(this, sizeof(GameSaveStructure)); }
	};

	/*-----------------------------------------------------------------*//**
	 *	プレイヤーキャラクタのセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class PcSaveStructure
	{
	public:
		// [  0]
		CharUnitSaveStructure _cusvst;			// [176] キャラクタユニットセーブストラクチャ
		// [176]
		ActStat _astat;							// [  8] アクションステータス
		// [　8]
		ItemEquip::SaveStructure _itmeq;		// [ 16] 装備 
		// [200]
		static const s32 SIZE_THIS = 200;
		PcSaveStructure() { TFW_ZERO_MEMORY(this, sizeof(PcSaveStructure)); }
	};

	/*-----------------------------------------------------------------*//**
	 *	プレイヤーキャラクタパーティのセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class PcPartySaveStructure
	{
	public:
		// [  0]
		u8 _ipcLeader;							// [  1] リーダー PC
		u8 __pad_0010[ 7];						// [  7]
		// [  8]
		static const s32 SIZE_THIS = 8;
		PcPartySaveStructure() { TFW_ZERO_MEMORY(this, sizeof(PcPartySaveStructure)); }
	};

	/*-----------------------------------------------------------------*//**
	 *	セーブストラクチャの作成用ストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class MakingSaveStructure
	{
	public:
		// [  0]
		u8 __pad_0010[4];						// [    4]
		u32 _checksumF;							// [    4] 前方計算チェックサム
		u32 _checksumR;							// [    4] 後方計算チェックサム
		u8 __pad_0030[4];						// [    4]
		// [ 16]
		static const s32 SIZE_THIS = 16;
		MakingSaveStructure() { TFW_ZERO_MEMORY(this, sizeof(MakingSaveStructure)); }
	};

	//======================================================================
	// 変数
public:
	// [    0]
	SummarySaveStructure _sumsvst;							// [   16] 概略
	// [   16]
	u8 __pad_sysconf_reserved[16];							// [   16] システムコンフィグ予約域
	// [   32]
	GameSaveStructure _gamesvst;							// [   32] ゲーム
	// [   64]
	u8 _wmapflags[NUM_MAP_MAX];								// [   64] ワールドマップ
	// [  128]
	u8 _svflags[NUM_FLAGS_MAX];								// [  256] セーブフラグ
	// [  384]
	PlacementObj::SaveStructure _pobjsvst[NUM_POBJ_MAX];	// [ 8192] 配置オブジェクト
	// [ 8576]
	Source::SaveStructure _srcsvst[NUM_SOURCE_MAX];			// [ 4608] ソース
	// [13184]
	Item::SaveStructure _itmsvst[NUM_ITEM_MAX];				// [13312] アイテム
	// [26496]
	QuestArticle::SaveStructure _qssvst[NUM_QUEST_MAX];		// [ 4096] クエスト
	u8 __pad_quest_reserved[64];							// [   64] クエスト予約域
	u8 __pad_0010[208];										// [  208]
	// [30864]
	PcSaveStructure _pcsvst[NUM_PC_MAX];					// [  600] プレイヤーキャラクタ
	PcPartySaveStructure _pcptysvst;						// [    8] PC パーティ
	// [31472]
	CharUnitSaveStructure _cusvstNpc[NUM_NPC_MAX];			// [ 5632] NPC
	// [37104]
	Score::ScoreSaveStructure _scoresvst;					// [   64] スコア
	Trophy::TrophySaveStructure _trophysvst;				// [  192] トロフィー
	// [37360]
	MakingSaveStructure _mksvst;							// [   16] 作成用ストラクチャ（※必ず末尾に入れる）
	// [37376]

	//======================================================================
	// メソッド
public:
	SaveStructure() { TFW_ZERO_MEMORY(this, sizeof(SaveStructure)); }
	bool storePlacementObjData(const List<PlacementObj*>* listPlaceObj);
	bool restorePlacementObjData(List<PlacementObj*>* listPlaceObj) const;
	bool isSamePlacementObj(const PlacementObj::SaveStructure* pobjsvst, const PlacementObj* pobj) const;
	void calcCheckSum(u32* sumF, u32* sumR) const;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_SAVE_SAVE_STRUCTURE_H_
