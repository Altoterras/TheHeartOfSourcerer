/***********************************************************************//**
 *	MapMod10221.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MapMod10221.h"

// Friends
#include "../../../episode/party/MyParty.h"
#include "../../../body/Game.h"
#include "../../../sound/GameSoundDef.h"
#include "../../../unit/char/pc/PcUnit.h"

// External
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define KEEPCNT_CHANGE_MUSIC		(15)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapMod10221 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MapMod10221::MapMod10221()
	: _cntKeep(0)
{
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void MapMod10221::exec(ExecRes* res, const ExecCtx* ec)
{
	SoundMngr* sndmng = Game::getGame()->getSoundManager(); ASSERT(sndmng != 0L);
	u16 musicidCur = (sndmng->getReservedMusicId() != 0) ? sndmng->getReservedMusicId() : sndmng->getCurrentPlayMusicId();
	{
		u16 musicidNew = musicidCur;

		// キャラクタの位置によって曲を変える
		Unit* unitMy = Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY);
		if(unitMy != 0L)
		{
			const Vector3F* posMy = unitMy->getPosition();
			///TRACE("> {%f,%f,%f}\n", posMy->x(), posMy->y(), posMy->z());
			if(posMy->z() > 0.0f)
			{
				// 門を出た
				if(_cntKeep < 0)	{	_cntKeep = 0;	}
				else				{	_cntKeep++;		}
			}
			else if(posMy->z() < 0.0f)
			{
				// 村に入った
				if(_cntKeep > 0)	{	_cntKeep = 0;	}
				else				{	_cntKeep--;		}
			}

			if(_cntKeep >= KEEPCNT_CHANGE_MUSIC)
			{
				musicidNew = GameSoundDef::BGM_106_FIELD;
			}
			else if(_cntKeep <= (- KEEPCNT_CHANGE_MUSIC))
			{
				musicidNew = GameSoundDef::BGM_103_VILLAGE;
			}
		}

		// 曲の変更
		if(musicidNew != musicidCur)
		{
			sndmng->playBgm(musicidNew, true, 1.0f, false, 60, 30, 0);
		}
	}

	MapMod10000::exec(res, ec);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
