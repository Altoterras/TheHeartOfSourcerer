/***********************************************************************//**
 *	FixedData.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/27.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "FixedData.h"

// Friends
#include "../../body/Game.h"
#include "../../file/GameFileIdDef.h"

// External
#include "../../../tfw/file/IdFile.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// FixedData 静的変数

StringTable FixedData::_strtblSys;
CharDefTbl FixedData::_chrdftbl;
FixedCharParamTbl FixedData::_fcprmtbl;
CharLogicDataTbl FixedData::_nldtbl;
ItemDefTbl FixedData::_itmdftbl;
FixedSourceDefTbl FixedData::_fsrcdftbl;
ParticleDefTbl FixedData::_ptcldftbl;
QuestDefTbl FixedData::_qsdftbl;

//==========================================================================
// FixedData メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool FixedData::init()
{
	ASSERT(!_strtblSys.isValid());

	// システム固定文字列テーブルを初期化
	initFixedSystemStringTable();

	// キャラクタ定義テーブルを初期化
	_chrdftbl.init(CHARACTER_CHARDEF01_DAT, CHARACTER_CHARSTR01_TXT);

	// 固定キャラクタパラメータテーブルを初期化
	_fcprmtbl.init(CHARACTER_PCLV01_DAT);

	// NPC ロジックデータテーブルを初期化
	_nldtbl.init(CHARACTER_CHARLOGIC01_DAT);

	// アイテム定義テーブルを初期化
	_itmdftbl.init(SYSTEM_ITEMDEF01_DAT, SYSTEM_ITEMSTR01_TXT);

	// 固定ソース定義テーブルを初期化
	_fsrcdftbl.init(SYSTEM_FIXEDSOUMASRCDEF01_DAT, SYSTEM_FIXEDSOUMASRCSTR01_TXT);

	// パーティクル定義テーブルを初期化
	_ptcldftbl.init(PARTICLE_PARTICLEDEF01_DAT);

	// クエスト定義テーブルを初期化
	_qsdftbl.init(QUESTEVENT_QUESTDEF01_DAT, QUESTEVENT_QUESTTXT01_DAT);

	#if 0
		// キャラクタパラメータ
		_cprmMin[MyParty::PC_MY ].set(    0,  1150,   732,   596);	// 第一引数のエネルギー値は実際の 1/10 であることに注意
	//	_cprmMin[MyParty::PC_MY ].set(    0, 10000,   732,   596);	// 第一引数のエネルギー値は実際の 1/10 であることに注意
		_cprmMin[MyParty::PC_PTN].set(    0,   634,   901,   994);
		_cprmMin[MyParty::PC_HLP].set(    0,  1100,  1003,  1071);
		_cprmMax[MyParty::PC_MY ].set(20216,  9984,  7653,  7911);
		_cprmMax[MyParty::PC_PTN].set(36120,  7024,  8627,  9895);
		_cprmMax[MyParty::PC_HLP].set(18973,  8177,  7819,  8047);
	#endif

	#if 0
	// クエスト定義
	{
		// ファイルを開く
		IdFile file(QSEVENT_QUESTDEF01_DAT);
		if(file.isOpened())
		{
			do
			{
				// クエストの数
				s32 numQuest = 0;
				u32 sizeRead = file.read(&numQuest, sizeof(u32));
				if(sizeRead != sizeof(u32))
				{
					ASSERT(false);
					break;
				}

				// クエストごとに読み込む
				for(s32 i = 0; i < numQuest; i++)
				{
					sizeRead = file.read(&_qsdef[i], sizeof(QuestDef));
					if(sizeRead != sizeof(QuestDef))
					{
						ASSERT(false);
						break;
					}
				}
			}
			while(false);

			// ファイルを閉じる
			file.close();
		}
	}
	#endif

	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void FixedData::cleanup()
{
	// クエスト定義テーブルを解放する
	_qsdftbl.cleanup();

	// パーティクル定義テーブルを解放する
	_ptcldftbl.cleanup();

	// 固定ソース定義テーブルを解放する
	_fsrcdftbl.cleanup();

	// アイテム定義テーブルを解放する
	_itmdftbl.cleanup();

	// NPC ロジックデータテーブルを解放する
	_nldtbl.cleanup();

	// 固定キャラクタパラメータテーブルを解放する
	_fcprmtbl.cleanup();

	// キャラクタ定義テーブルを解放する
	_chrdftbl.cleanup();

	// システム固定文字列テーブルを解放する
	_strtblSys.destroy();
}

/*---------------------------------------------------------------------*//**
	言語変更通知
**//*---------------------------------------------------------------------*/
void FixedData::notifyChangeLanguage()
{
	if(_strtblSys.isValid())
	{
		// クエスト定義テーブルを解放する
		_qsdftbl.cleanup();

		// 固定ソース定義テーブルを解放する
		_fsrcdftbl.cleanup();

		// システム固定文字列テーブルを一度解放する
		_strtblSys.destroy();

		// システム固定文字列テーブルを再初期化する
		initFixedSystemStringTable();

		// 固定ソース定義テーブルを初期化
		_fsrcdftbl.init(SYSTEM_FIXEDSOUMASRCDEF01_DAT, SYSTEM_FIXEDSOUMASRCSTR01_TXT);
		
		// クエスト定義テーブルを初期化
		_qsdftbl.init(QUESTEVENT_QUESTDEF01_DAT, QUESTEVENT_QUESTTXT01_DAT);
	}
}

/*---------------------------------------------------------------------*//**
	システム固定文字列テーブルを初期化する
**//*---------------------------------------------------------------------*/
void FixedData::initFixedSystemStringTable()
{
	// ファイルを開く
	IdFile file(SYSTEM_FIXEDSTRTBL_DAT);
	if(file.isOpened())
	{
		s32 colValidString[1];
		switch(Env_getLangId())
		{
		case LANGID_EN:	colValidString[0] = 2;		break;
		case LANGID_JP:	colValidString[0] = 1;		break;
		}
		_strtblSys.loadFromFile(&file, colValidString, 1);
	}

	// ファイルを閉じる
	file.close();
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
