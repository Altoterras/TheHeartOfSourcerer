/***********************************************************************//**
 *	SaveData.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SaveData.h"

// Friends
#include "../episode/StoryManager.h"
#include "../episode/party/MyParty.h"
#include "../episode/save/SaveStructure.h"
#include "../event/EventSys.h"
#include "../body/Game.h"
#include "../gui/GameGui.h"
#include "../quest/QuestManager.h"
#include "../souma/script/SoumaScript.h"

// External
#include "../../tfw/file/File.h"
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SaveData メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	メジャーバージョン番号を得る
**//*---------------------------------------------------------------------*/
u16 SaveData::getMajorVersionNumber() const
{
	return SaveStructure::VER_MAJOR_SAVE_STRUCTURE;
}

/*---------------------------------------------------------------------*//**
	マイナーバージョン番号を得る
**//*---------------------------------------------------------------------*/
u8 SaveData::getMinorVersionNumber() const
{
	return SaveStructure::VER_MINOR_SAVE_STRUCTURE;
}

/*---------------------------------------------------------------------*//**
	セーブフラグを設定する
**//*---------------------------------------------------------------------*/
void SaveData::setSaveFlags(u32 saveflags)
{
	_saveflagsSave = saveflags;

	// フラグチェック
	if(TFW_IS_FLAG(_saveflagsSave, SVF_CLEARING))	// クリア時
	{	
		if(_cntClear > 0)
		{
			if(_cntClear < U32_MAX)
			{
				_cntClear++;
			}
		}
		else
		{
			_cntClear = 1;
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	データクリア
**//*---------------------------------------------------------------------*/
void SaveData::clear()
{
	for(s32 i = 0; i < NUM_SFID / 8; i++)
	{
		_flagSaveGd[i] = 0;
	}
	for(s32 i = 0; i < NUM_SCID; i++)
	{
		_s8SaveGd[i] = '\0';
	}
	for(s32 i = 0; i < NUM_SNID; i++)
	{
		_s32SaveGd[i] = 0;
	}
	for(s32 i = 0; i < NUM_PFID / 8; i++)
	{
		_flagPlayGd[i] = 0;
	}

	_frameElapsedWork = 0.0f;
	_framePlay = 0;
	_cntClear = 0;
	_saveflagsSave = 0;
	_saveflagsLoad = 0;
}

/*---------------------------------------------------------------------*//**
	ニューゲーム
**//*---------------------------------------------------------------------*/
bool SaveData::newGame()
{
	// プレイヤーキャラクタの読み込み
	if(!((MyParty*)Game::getGame()->getMyParty())->loadPcChars())	{	return false;	}

	// ストーリーのニューゲーム処理
	StoryManager* strymng = Game::getGame()->getStoryManager(); ASSERT(strymng != 0L);
	strymng->newGame(false, false);

	// クエストのニューゲーム処理
	QuestManager* qsmng = Game::getGame()->getQuestManager(); ASSERT(qsmng != 0L);
	qsmng->newGame();

	// ［スコア］ニューゲーム
	ScoreBase* score = Game::getGame()->getScore(); ASSERT(score != 0L);
	score->upScore(Score::SC_NEW_GAME_VOL01);

	return true;
}

/*---------------------------------------------------------------------*//**
	ストーリーをはじめからやり直す
	※このメソッド実行前に deserialize をしてある必要がある
**//*---------------------------------------------------------------------*/
bool SaveData::restartGame()
{
	// ストーリーのニューゲーム処理
	StoryManager* strymng = Game::getGame()->getStoryManager(); ASSERT(strymng != 0L);
	strymng->newGame(true, _cntClear);

	ScoreBase* score = Game::getGame()->getScore(); ASSERT(score != 0L);
	if(TFW_IS_FLAG(_saveflagsLoad, SVF_CLEARING))
	{
		// ［スコア］ニューゲーム
		score->upScore(Score::SC_NEW_GAME_VOL01);
	}
	else
	{
		// ［スコア］リニューゲーム
		score->upScore(Score::SC_RENEW_GAME_VOL01);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	マップに直接入る（デバッグ用）
**//*---------------------------------------------------------------------*/
bool SaveData::debug_enterMap(s32 storyid)
{
#if defined(_DEBUG)
	// プレイヤーキャラクタの読み込み
	if(!((MyParty*)Game::getGame()->getMyParty())->loadPcChars())	{	return false;	}

	// ストーリーのデバッグ設定
	StoryManager* strymng = Game::getGame()->getStoryManager();	ASSERT(strymng != 0L);
	strymng->debug_enterMap(storyid);

	// クエストのデバッグ設定
	QuestManager* qsmng = Game::getGame()->getQuestManager();	ASSERT(qsmng != 0L);
	qsmng->debug_enterMap();

	// スクリプト テスト
	Game::getGame()->getSoumaScript()->_debug_testScript();
#endif

	return true;
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool SaveData::serialize(Serializer* ser) const
{
	bool isSuccess = true;
	Game* game = Game::getGame(); ASSERT(game != 0L);
	ScoreBase* score = game->getScore(); ASSERT(score != 0L);

	if(ser->isStructureMode())	// ストラクチャモード
	{
		// セーブデータストラクチャの用意
		SaveStructure* svst = new SaveStructure();
		VcString* str = new VcString();
		ser->setStructure(svst);
		ser->setString(str);

		// 概略データの書き込み
		svst->_sumsvst._framePlay = svst->_gamesvst._framePlay = _framePlay;		// プレイフレーム
		svst->_sumsvst._scene = TFW_IS_FLAG(_saveflagsSave, SVF_CLEARING) ? SaveStructure::SummarySaveStructure::SCENE_CLEAR : SaveStructure::SummarySaveStructure::SCENE_INGAME;	// セーブケース
		svst->_sumsvst._flags = (_cntClear > 0) ? SaveStructure::SummarySaveStructure::F_CLEARED : 0;	// クリア済みフラグ
		svst->_sumsvst._flags |= TFW_IS_FLAG(_saveflagsSave, SVF_CLEARING) ? SaveStructure::SummarySaveStructure::F_CLEARING : 0;	// クリア直後フラグ
		svst->_sumsvst._flags |= TFW_IS_FLAG(_saveflagsSave, SVF_EVENT_INTVL) ? SaveStructure::SummarySaveStructure::F_EVINTVL : 0;	// イベント途中フラグ
		svst->_sumsvst._cntClear = (_cntClear > U8_MAX) ? U8_MAX : (u8)_cntClear;	// クリア回数
		svst->_sumsvst._score = score->calcScore();									// スコア

		// 汎用データの書き込み
		ASSERT((sizeof(u8) * NUM_SFID / 8) <= (sizeof(u8) * SaveStructure::NUM_FLAGS_MAX));
		::memcpy(svst->_svflags, _flagSaveGd, sizeof(u8) * SaveStructure::NUM_FLAGS_MAX);
	}
	else						// ダイレクトモード
	{
		// 概略データの書き込み
		if(!ser->store(&_saveflagsSave))	{	isSuccess = false;	}		// セーブフラグ
		if(!ser->store(&_framePlay))		{	isSuccess = false;	}		// プレイフレーム
		if(!ser->store(&_cntClear))			{	isSuccess = false;	}		// クリア済みフラグ

		// 汎用データの書き込み
		if(!ser->store(_flagSaveGd, sizeof(u8) * NUM_SFID / 8))	{	isSuccess = false;	}
		if(!ser->store(_s8SaveGd, sizeof(s8) * NUM_SCID))		{	isSuccess = false;	}
		if(!ser->store(_s32SaveGd, sizeof(s32) * NUM_SNID))		{	isSuccess = false;	}
	}

	// ゲームの保存
	if(!game->serialize(ser))		{	isSuccess = false;	}

	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		VcString* str = ser->getString();

///		// 文字列ポインタ値を NULL 化
///		svst->_mksvst._string = 0L;

		// チェックサムの計算
		svst->calcCheckSum(&svst->_mksvst._checksumF, &svst->_mksvst._checksumR);

		// セーブデータの書き込み
		if(!ser->store(svst, sizeof(SaveStructure)))	{	isSuccess = false;	}
		if(!ser->store(str))							{	isSuccess = false;	}

		// セーブデータストラクチャの削除
		delete svst;
		delete str;
	}

	return isSuccess;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool SaveData::deserialize(Serializer* ser)
{
	Game* game = Game::getGame(); ASSERT(game != 0L);
	ScoreBase* score = game->getScore(); ASSERT(score != 0L);

	clear();

	// プレイヤーキャラクタの読み込み
	if(!((MyParty*)game->getMyParty())->loadPcChars())	{	return false;	}

	bool isSuccess = true;
	
	if(ser->isStructureMode())	// ストラクチャモード
	{
		// セーブデータの読み込み
		SaveStructure* svst = new SaveStructure();
		VcString* str = new VcString();
		if(!ser->restore(svst, sizeof(SaveStructure)))	{	isSuccess = false;	}
		if(!ser->restore(str))	{	isSuccess = false;	}

		// チェックサムの確認
		u32 checksumF, checksumR;
		svst->calcCheckSum(&checksumF, &checksumR);
		if((checksumF != svst->_mksvst._checksumF) || (checksumR != svst->_mksvst._checksumR))
		{
			ASSERTM(false, "{SaveData::deserialize} Checksum Error.");
			//return false;	// とりあえずはアサートのみ
		}

		// セーブデータストラクチャの用意
		ser->setStructure(svst);
		ser->setString(str);

		// 概略データの読み込み
		_framePlay = svst->_sumsvst._framePlay;	// プレイフレームの読み込み
		bool isCleared = TFW_IS_FLAG(svst->_sumsvst._flags, SaveStructure::SummarySaveStructure::F_CLEARED);	// クリア済みフラグ
		TFW_SET_FLAG(_saveflagsLoad, SVF_CLEARING, TFW_IS_FLAG(svst->_sumsvst._flags, SaveStructure::SummarySaveStructure::F_CLEARING));		// クリア済直後フラグ
		TFW_SET_FLAG(_saveflagsLoad, SVF_EVENT_INTVL, TFW_IS_FLAG(svst->_sumsvst._flags, SaveStructure::SummarySaveStructure::F_EVINTVL));	// イベント途中フラグ
		// クリア回数は Vol.1 Ver1.3 からの仕様なので補正を入れる
		_cntClear = svst->_sumsvst._cntClear;
		if(isCleared && (_cntClear == 0))	{	_cntClear = 1;	}

		// 汎用データの読み込み
		ASSERT((sizeof(u8) * NUM_SFID / 8) <= (sizeof(u8) * SaveStructure::NUM_FLAGS_MAX));
		::memcpy(_flagSaveGd, svst->_svflags, sizeof(u8) * NUM_SFID / 8);
	}
	else						// ダイレクトモード
	{
		// 概略データの読み込み
		if(!ser->restore(&_saveflagsLoad))	{	isSuccess = false;	}	// セーブフラグ
		if(!ser->restore(&_framePlay))		{	isSuccess = false;	}	// プレイフレーム
		if(!ser->restore(&_cntClear))		{	isSuccess = false;	}	// クリア済みフラグ

		// 汎用データの読み込み
		if(!ser->restore(_flagSaveGd, sizeof(u8) * NUM_SFID / 8))	{	isSuccess = false;	}
		if(!ser->restore(_s8SaveGd, sizeof(s8) * NUM_SCID))		{	isSuccess = false;	}
		if(!ser->restore(_s32SaveGd, sizeof(s32) * NUM_SNID))		{	isSuccess = false;	}
	}

	// ゲームの復元
	if(!game->deserialize(ser))		{	isSuccess = false;	}

	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		VcString* str = ser->getString();

		// セーブデータストラクチャの削除
		delete svst;
		delete str;
	}

	// ［スコア］ロード
	score->upScore(Score::SC_LOAD);

	return true;
}

/*---------------------------------------------------------------------*//**
	シリアライズ可能かを判定する
**//*---------------------------------------------------------------------*/
bool SaveData::isSerializableNow()
{
	// パーティ
	MyParty* party = (MyParty*)Game::getGame()->getMyParty();	ASSERT(party != 0L);
	if(party == 0L)					{	return false;	}
	if(!party->isSerializableNow())	{	return false;	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SaveData::SaveData()
	: _flagSaveGd(0L)
	, _s8SaveGd(0L)
	, _s32SaveGd(0L)
	, _flagPlayGd(0L)
	, _frameElapsedWork(0.0f)
	, _framePlay(0)
	, _cntClear(0)
	, _saveflagsSave(0)
	, _saveflagsLoad(0)
{
	TRACE("{SaveData::SaveData} sizeof(Elene)=%d\n", sizeof(Elene));
	TRACE("{SaveData::SaveData} sizeof(EleneVeil)=%d\n", sizeof(EleneVeil));
	TRACE("{SaveData::SaveData} sizeof(ActStat)=%d\n", sizeof(ActStat));
	TRACE("{SaveData::SaveData} sizeof(EleneStat::SaveStructure)=%d\n", sizeof(EleneStat::SaveStructure));
	TRACE("{SaveData::SaveData} sizeof(CharStat::SaveStructure)=%d\n", sizeof(CharStat::SaveStructure));
	TRACE("{SaveData::SaveData} sizeof(ItemEquip::SaveStructure)=%d\n", sizeof(ItemEquip::SaveStructure));
	TRACE("{SaveData::SaveData} sizeof(PlacementObj::SaveStructure)=%d\n", sizeof(PlacementObj::SaveStructure));
	TRACE("{SaveData::SaveData} sizeof(Item::SaveStructure)=%d\n", sizeof(Item::SaveStructure));
	TRACE("{SaveData::SaveData} sizeof(QuestArticle::SaveStructure)=%d\n", sizeof(QuestArticle::SaveStructure));
	TRACE("{SaveData::SaveData} sizeof(Score::ScoreSaveStructure)=%d\n", sizeof(Score::ScoreSaveStructure));
	TRACE("{SaveData::SaveData} sizeof(Trophy::TrophySaveStructure)=%d\n", sizeof(Trophy::TrophySaveStructure));
	TRACE("{SaveData::SaveData} sizeof(SaveStructure::SummarySaveStructure)=%d\n", sizeof(SaveStructure::SummarySaveStructure));
	TRACE("{SaveData::SaveData} sizeof(SaveStructure::GameSaveStructure)=%d\n", sizeof(SaveStructure::GameSaveStructure));
	TRACE("{SaveData::SaveData} sizeof(SaveStructure::PcSaveStructure)=%d\n", sizeof(SaveStructure::PcSaveStructure));
	TRACE("{SaveData::SaveData} sizeof(SaveStructure::PcPartySaveStructure)=%d\n", sizeof(SaveStructure::PcPartySaveStructure));
	TRACE("{SaveData::SaveData} sizeof(SaveStructure::MakingSaveStructure)=%d\n", sizeof(SaveStructure::MakingSaveStructure));
	TRACE("{SaveData::SaveData} sizeof(SaveStructure)=%d\n", sizeof(SaveStructure));
	ASSERT(sizeof(EleneStat::SaveStructure) == EleneStat::SaveStructure::SIZE_THIS);
	ASSERT(sizeof(CharStat::SaveStructure) == CharStat::SaveStructure::SIZE_THIS);
	ASSERT(sizeof(ItemEquip::SaveStructure) == ItemEquip::SaveStructure::SIZE_THIS);
	ASSERT(sizeof(PlacementObj::SaveStructure) == PlacementObj::SaveStructure::SIZE_THIS);
	ASSERT(sizeof(Item::SaveStructure) == Item::SaveStructure::SIZE_THIS);
	ASSERT(sizeof(QuestArticle::SaveStructure) == QuestArticle::SaveStructure::SIZE_THIS);
	ASSERT(sizeof(Score::ScoreSaveStructure) == Score::ScoreSaveStructure::SIZE_THIS);
	ASSERT(sizeof(Trophy::TrophySaveStructure) == Trophy::TrophySaveStructure::SIZE_THIS);
	ASSERT(sizeof(SaveStructure::SummarySaveStructure) == SaveStructure::SummarySaveStructure::SIZE_THIS);
	ASSERT(sizeof(SaveStructure::GameSaveStructure) == SaveStructure::GameSaveStructure::SIZE_THIS);
	ASSERT(sizeof(SaveStructure::PcSaveStructure) == SaveStructure::PcSaveStructure::SIZE_THIS);
	ASSERT(sizeof(SaveStructure::PcPartySaveStructure) == SaveStructure::PcPartySaveStructure::SIZE_THIS);
	ASSERT(sizeof(SaveStructure::MakingSaveStructure) == SaveStructure::MakingSaveStructure::SIZE_THIS);
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SaveData::~SaveData()
{
	ASSERT(_flagSaveGd == 0L);
	ASSERT(_s8SaveGd == 0L);
	ASSERT(_s32SaveGd == 0L);
	ASSERT(_flagPlayGd == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SaveData::create()
{
	// 汎用データの作成
	_flagSaveGd	= new u8[NUM_SFID / 8];
	TFW_ZERO_MEMORY(_flagSaveGd, sizeof(u8) * NUM_SFID / 8);
	_s8SaveGd		= new s8[NUM_SCID];
	TFW_ZERO_MEMORY(_s8SaveGd, sizeof(s8) * NUM_SCID);
	_s32SaveGd	= new s32[NUM_SNID];
	TFW_ZERO_MEMORY(_s32SaveGd, sizeof(s32) * NUM_SNID);
	_flagPlayGd	= new u8[NUM_PFID / 8];
	TFW_ZERO_MEMORY(_flagPlayGd, sizeof(u8) * NUM_PFID / 8);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SaveData::destroy()
{
	// 汎用データの削除
	delete[] _flagPlayGd;
	_flagPlayGd = 0L;
	delete[] _s32SaveGd;
	_s32SaveGd = 0L;
	delete[] _s8SaveGd;
	_s8SaveGd = 0L;
	delete[] _flagSaveGd;
	_flagSaveGd = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SaveData::exec(const ExecCtx* ec)
{
	// プレイフレーム数をカウント
	_frameElapsedWork += ec->getDeltaFrame();
	if(_frameElapsedWork >= 30.0f)
	{
		_framePlay += 30;
		_frameElapsedWork -= 30.0f;
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
