/***********************************************************************//**
 *	ParticleDefTbl.cpp
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2010/05/24.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ParticleDefTbl.h"

// Friends

// External
#include "../../tfw/file/IdFile.h"

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	パーティクル定義を取得する
**//*---------------------------------------------------------------------*/
const ParticleDef* ParticleDefTbl::getDef(u16 ptcldid) const
{
	for(s32 i = 0; i < NUM_PTCLDEF_MAX; i++)
	{
		if(_ptcldefarr[i].getParticleDefId() == ptcldid)
		{
			return &_ptcldefarr[i];
		}
	}
	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ParticleDefTbl::ParticleDefTbl()
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ParticleDefTbl::~ParticleDefTbl()
{
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool ParticleDefTbl::init(u32 fileidData)
{
	s32 numParticleDef = 0;

	// データファイルを開く
	{
		IdFile file(fileidData);
		if(file.isOpened())
		{
			do
			{
				// パーティクル定義の数
				u32 sizeRead = file.read(&numParticleDef, sizeof(u32));
				if(sizeRead != sizeof(u32))
				{
					ASSERT(false);
					break;
				}
				ASSERT(numParticleDef <= NUM_PTCLDEF_MAX);

				// パーティクル定義ごとにデータを読み込む
				for(s32 i = 0; i < numParticleDef; i++)
				{
					if(!_ptcldefarr[i].loadFromDataFile(&file))
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

	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void ParticleDefTbl::cleanup()
{
	for(s32 i = 0; i < NUM_PTCLDEF_MAX; i++)
	{
		_ptcldefarr[i].cleanup();
	}
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
