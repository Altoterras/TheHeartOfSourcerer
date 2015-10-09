/***********************************************************************//**
 *	Party.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Party.h"

// Friends
#include "../body/Game.h"
#include "../body/GameNotifier.h"
#include "../spirit/Spirit.h"
#include "../unit/char/pc/PcUnit.h"

// External
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Party メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	プレイヤーキャラクタのユニットを得る
**//*---------------------------------------------------------------------*/
const PcUnit* Party::getPcUnit(int ipc) const
{
	return _pcarr[ipc]._unitRef;
}

/*---------------------------------------------------------------------*//**
	プレイヤーキャラクタのユニットを得る
**//*---------------------------------------------------------------------*/
PcUnit* Party::pcUnit(int ipc)
{
	return _pcarr[ipc]._unitRef;
}

/*---------------------------------------------------------------------*//**
	プレイヤーキャラクタのスピリットを得る
**//*---------------------------------------------------------------------*/
const PcSpirit* Party::getPcSpirit(s32 ipc) const
{
	return (_pcarr[ipc]._unitRef != 0L) ? (const PcSpirit*)_pcarr[ipc]._unitRef->getSpirit() : 0L;
}

/*---------------------------------------------------------------------*//**
	プレイヤーキャラクタのスピリットを得る
**//*---------------------------------------------------------------------*/
PcSpirit* Party::pcSpirit(s32 ipc)
{
	return (_pcarr[ipc]._unitRef != 0L) ? (PcSpirit*)_pcarr[ipc]._unitRef->spirit() : 0L;
}

/*---------------------------------------------------------------------*//**
	リーダープレイヤーキャラクタのユニットを得る
**//*---------------------------------------------------------------------*/
const PcUnit* Party::getLeaderPcUnit() const
{
	if((_ipcLeader < 0) || (_numPc <= _ipcLeader))	{	return 0L;	}
	return _pcarr[_ipcLeader]._unitRef;
}

/*---------------------------------------------------------------------*//**
	リーダープレイヤーキャラクタのユニットを得る
**//*---------------------------------------------------------------------*/
PcUnit* Party::leaderPcUnit()
{
	if((_ipcLeader < 0) || (_numPc <= _ipcLeader))	{	return 0L;	}
	return _pcarr[_ipcLeader]._unitRef;
}

/*---------------------------------------------------------------------*//**
	ユニットから PC を探して、PC インデックスを返す
**//*---------------------------------------------------------------------*/
s32 Party::findPcUnit(const Unit* pcunit) const
{
	for(int i = 0; i < _numPc; i++)
	{
		if(_pcarr[i]._unitRef == pcunit)
		{
			return i;
		}
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	イベントによりあるユニットに注目する
**//*---------------------------------------------------------------------*/
void Party::animUnitOnEvent(const Unit* unitTrg, u16 evid)
{
	_evidAim = evid;
	animUnit(unitTrg);	// 対象ユニットに注目する
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	キャラクタの ON / OFF スイッチを行う
**//*---------------------------------------------------------------------*/
void Party::setEnablePcChar(int ipc, bool isEnable)
{
	ASSERT((0 <= ipc) && (ipc < _numPc));
	PcUnit* unit = _pcarr[ipc]._unitRef;
	if(unit != 0L)
	{
		if(unit->isEnable() != isEnable)
		{
			unit->setEnable(isEnable);
		}
	}
}

/*---------------------------------------------------------------------*//**
	リーダー設定
**//*---------------------------------------------------------------------*/
bool Party::setLeader(s32 ipc)
{
	if((ipc < 0) || (_numPc <= ipc))	{	return false;	}

	if(_ipcLeader != ipc)
	{
		// リーダー変更可能か調べる
		if(!_pcarr[ipc]._unitRef->isValidLeader())
		{
			return false;
		}

		// リーダー変更
		_ipcLeader = ipc;

		// 各ユニットに設定
		for(int i = 0; i < _numPc; i++)
		{
			PcUnit* unit = _pcarr[i]._unitRef;
			if(unit != 0L)
			{
				bool isLeader = (i == _ipcLeader);
				if(unit->isLeader() != isLeader)
				{
					unit->setLeader(isLeader);
				}
			}
		}

		// リーダー変更通知
		GameNotifier* gnotif = Game::getGame()->getGameNotifier(); ASSERT(gnotif != 0L);
		gnotif->notifyChangeLeader();

		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	リーダーになれるか調べる
**//*---------------------------------------------------------------------*/
bool Party::isValidLeader(s32 ipc)
{
	if((ipc < 0) || (_numPc <= ipc))	{	return false;	}
	return _pcarr[ipc]._unitRef->isValidLeader();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Party::Party()
	: _pcarr(0L)
	, _numPc(0)
	, _ipcLeader(0)
	, _evidAim(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Party::~Party()
{
	ASSERT(_pcarr == 0);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Party::create(s32 numPc)
{
	// PC 配列の作成
	Pc* pcarr = new Pc[numPc];
	if(pcarr == 0)	{	return false;	}
	_pcarr = pcarr;
	_numPc = numPc;
	_ipcLeader = -1;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Party::destroy()
{
	// PC 配列の削除
	if(_pcarr != 0L)
	{
		delete[] _pcarr;
		_pcarr = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	マップ変更後通知
**//*---------------------------------------------------------------------*/
void Party::notifyChangedMap(bool isDeserializeCase)
{
	moveMapInitLeaderPos();

	// 注目を解く
	if(_evidAim != 0)
	{
		animUnit(0L);
	}
}

/*---------------------------------------------------------------------*//**
	一連動作イベントマターの開始・終了通知
**//*---------------------------------------------------------------------*/
void Party::notifyEventActMatterTrigger(u16 evid, bool start, const Unit* unitTrg)
{
	// 注目を解く
	if((_evidAim == evid) && !start)
	{
		animUnit(0L);
	}
}

/*---------------------------------------------------------------------*//**
	あるユニットに注目する
**//*---------------------------------------------------------------------*/
void Party::animUnit(const Unit* unitTrg)
{
	for(int i = 0; i < _numPc; i++)
	{
		Spirit* sprt = ((_pcarr[i]._unitRef != 0L) && _pcarr[i]._unitRef->isEnable()) ? _pcarr[i]._unitRef->spirit() : 0L;
		if(sprt != 0L)
		{
			sprt->setTalkUnit(unitTrg);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
