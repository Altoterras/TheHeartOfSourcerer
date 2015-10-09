/***********************************************************************//**
 *	QuestDefTbl.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/25.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QuestDefTbl.h"

// Friends

// External
#include "../../tfw/file/IdFile.h"
#include "../../tfw/lib/StringTable.h"
#include "../../tfw/string/StringUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	クエスト定義を取得する
**//*---------------------------------------------------------------------*/
const QuestDef* QuestDefTbl::getDef(u16 questid) const
{
	for(s32 i = 0; i < NUM_QUESTDEF_MAX; i++)
	{
		if(_qsdefarr[i].getQuestId() == questid)
		{
			return &_qsdefarr[i];
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	クエスト定義を取得する
**//*---------------------------------------------------------------------*/
const QuestDef* QuestDefTbl::getDefFromIndex(s32 index) const
{
	if((index < 0) || (NUM_QUESTDEF_MAX <= index))
	{
		return 0L;
	}

	return &_qsdefarr[index];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QuestDefTbl::QuestDefTbl()
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
QuestDefTbl::~QuestDefTbl()
{
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool QuestDefTbl::init(u32 fileidData, u32 fileidTxt)
{
	s32 numQuestDef = 0;

	// データファイルを読み込む
	{
		IdFile file(fileidData);
		if(file.isOpened())
		{
			do
			{
				// クエスト定義の数
				u32 sizeRead = file.read(&numQuestDef, sizeof(u32));
				if(sizeRead != sizeof(u32))
				{
					ASSERT(false);
					break;
				}
				ASSERT(numQuestDef <= NUM_QUESTDEF_MAX);

				// クエスト定義ごとにデータを読み込む
				for(s32 i = 0; i < numQuestDef; i++)
				{
					if(!_qsdefarr[i].loadFromDataFile(&file))
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

	// テキストファイルを読み込む
	{
		IdFile file(fileidTxt);
		if(file.isOpened())
		{
			// 有効な文字列ファイルの列
			s32 colValidString[4];
			switch(Env_getLangId())
			{
			case LANGID_EN:
				colValidString[0] = 1;
				colValidString[1] = 3;
				colValidString[2] = 5;
				colValidString[3] = 7;
				break;
			case LANGID_JP:
				colValidString[0] = 2;
				colValidString[1] = 4;
				colValidString[2] = 6;
				colValidString[3] = 8;
				break;
			}

			StringTable strtbl;
			if(strtbl.loadFromFile(&file, colValidString, 4))
			{
				s32 idxDefCur = 0;
				for(int irow = 0; irow < strtbl.getRowNum(); irow++)
				{
					s32 idxWk = idxDefCur;
					idxDefCur = -1;
					do
					{
						if(idxWk >= numQuestDef)	{	idxWk = 0;	}
						if(strtbl.getIdByIndex(irow) == _qsdefarr[idxWk].getQuestId())
						{
							idxDefCur = idxWk;
							_qsdefarr[idxDefCur].setText(
								strtbl.getStringByIndex(irow, 0),
								strtbl.getStringByIndex(irow, 1),
								strtbl.getStringByIndex(irow, 2),
								strtbl.getStringByIndex(irow, 3)	);
							idxWk++;
							break;
						}
						idxWk++;
					}
					while(idxWk != idxDefCur);
				}
				strtbl.destroy();
			}

			file.close();
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void QuestDefTbl::cleanup()
{
	for(s32 i = 0; i < NUM_QUESTDEF_MAX; i++)
	{
		_qsdefarr[i].cleanup();
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
