/***********************************************************************//**
 *	MagicSys.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MagicSys.h"

// Friends
#include "MagicCluster.h"
#include "MagicClusterFactory.h"
#include "../body/Game.h"
#include "../body/GameNotifier.h"

// External

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// 静的定数

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ
	
/*---------------------------------------------------------------------*//**
	クラスターを得る
**//*---------------------------------------------------------------------*/
MagicCluster* MagicSys::getCluster(u16 mcid) const
{
	ASSERT((1 <= mcid) && (mcid <= NUM_CLUSTER_MAX));
	if((mcid < 1) || (NUM_CLUSTER_MAX < mcid))	{	return 0L;	}
	return _mcarr[mcid - 1];
}

/*---------------------------------------------------------------------*//**
	クラスターをインデックスから得る
**//*---------------------------------------------------------------------*/
MagicCluster* MagicSys::getClusterFromIndex(s32 index) const
{
	if((index < 0) || (NUM_CLUSTER_MAX <= index))	{	return 0L;	}
	return _mcarr[index];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	マジッククラスターを出現させる
**//*---------------------------------------------------------------------*/
u16 MagicSys::appearCluster(const MagicClusterCreateParam* mccprm)
{
	s32 idx = findUnusedClusterIndex();
	if(idx < 0)
	{
		TRACE("{MagicSys::appearCluster} particle num over.\n");
		return 0;	// 失敗．最大値を超えている
	}
	u16 mcid = (u16)(idx + 1);

	_mcarr[idx] = MagicClusterFactory::makeCluster(this, mcid, mccprm);

	return mcid;
}

/*---------------------------------------------------------------------*//**
	マジッククラスターを消滅させる
**//*---------------------------------------------------------------------*/
bool MagicSys::disappearCluster(u16 mcid, u16 dcflags)
{
	return disappearClusterFromIndex(mcid - 1, dcflags);
}

/*---------------------------------------------------------------------*//**
	全てのマジッククラスターを消滅させる
**//*---------------------------------------------------------------------*/
bool MagicSys::disappearAllCluster()
{
	bool isSuccess = true;

	for(int i = 0; i <NUM_CLUSTER_MAX; i++)
	{
		if(_mcarr[i] != 0L)
		{
			if(!disappearClusterFromIndex(i, 0))	{	isSuccess = false;	}
		}
	}

	return isSuccess;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MagicSys::MagicSys()
{
	for(int i = 0; i < NUM_CLUSTER_MAX; i++)
	{
		_mcarr[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MagicSys::~MagicSys()
{
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_CLUSTER_MAX; i++)
		{
			ASSERT(_mcarr[i] == 0L);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MagicSys::create()
{
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MagicSys::destroy()
{
	// クラスターポインタ配列を削除
	for(int i = 0; i < NUM_CLUSTER_MAX; i++)
	{
		if(_mcarr[i] != 0L)
		{
			_mcarr[i]->destroy();
			delete _mcarr[i];
			_mcarr[i] = 0L;
		}
	}
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理
**//*---------------------------------------------------------------------*/
void MagicSys::exec(ExecRes* res, const ExecCtx* ec)
{
	for(int i = 0; i < NUM_CLUSTER_MAX; i++)
	{
		if(_mcarr[i] != 0L)
		{
			_mcarr[i]->exec(ec);

			if(!_mcarr[i]->isValid() && !TFW_IS_FLAG(_mcarr[i]->getCtrlFlags(), MagicCluster::CTRLF_EXTINCTION))	// 消滅した
			{
				disappearClusterFromIndex(i, _mcarr[i]->getSelfDisappearFlags());
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void MagicSys::notifyChangedMap()
{
	for(int i = 0; i < NUM_CLUSTER_MAX; i++)
	{
		if(_mcarr[i] != 0L)
		{
			_mcarr[i]->destroy();
			delete _mcarr[i];
			_mcarr[i] = 0L;
		}
	}
}

/*---------------------------------------------------------------------*//**
	未使用のパーティクルインデックスを得る
**//*---------------------------------------------------------------------*/
s32 MagicSys::findUnusedClusterIndex() const
{
	for(int i = 0; i < NUM_CLUSTER_MAX; i++)
	{
		if(_mcarr[i] == 0L)	{	return i;	}
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	マジッククラスターを消滅させる
**//*---------------------------------------------------------------------*/
bool MagicSys::disappearClusterFromIndex(s32 index, u16 dcflags)
{
	if((index < 0) || (NUM_CLUSTER_MAX <= index))	{	return false;	}

	if(_mcarr[index] == 0L)							{	return false;	}

	GameNotifier* gnotif = Game::getGame()->getGameNotifier(); ASSERT(gnotif != 0L);
	gnotif->notifyDisappearMagicCluster(_mcarr[index], dcflags);

	_mcarr[index]->destroy();
	delete _mcarr[index];
	_mcarr[index] = 0L;

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
