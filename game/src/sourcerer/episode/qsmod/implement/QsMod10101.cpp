/***********************************************************************//**
 *	QsMod10101.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/06.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QsMod10101.h"

// Friends
#include "../../MapIdDef.h"
#include "../../../common/CalcUtils.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../body/MoveMap.h"
#include "../../../quest/QuestArticle.h"
#include "../../../quest/QuestDef.h"
#include "../../../quest/QuestManager.h"
#include "../../../particle/GameParticleDef.h"
#include "../../../sound/GameSoundDef.h"
#include "../../../status/EleneStat.h"
#include "../../../status/TransStat.h"
#include "../../../unit/Unit.h"

// External
#include "../../../../etk/particle/Particle.h"
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../etk/sound/SoundMngr.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QsMod10101 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool QsMod10101::create(QuestArticle* qstaRef)
{
	if(!QsMod::create(qstaRef))
	{
		return false;
	}

	// クエスト要素のセットアップ
	_ent[0].setup(CTRLEVID_HERB_01);
	_ent[1].setup(CTRLEVID_HERB_02);
	_ent[2].setup(CTRLEVID_HERB_03);

	return true;
}

/*---------------------------------------------------------------------*//**
	マップ変更後通知
**//*---------------------------------------------------------------------*/
void QsMod10101::notifyChangedMap(bool isDeserializeCase)
{
	// クエスト要素の状態をクリア
	for(int i = 0; i < NUM_ENTITY; i++) { _ent[i].clear(); }

	QsMod::notifyChangedMap(isDeserializeCase);
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void QsMod10101::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	if(_qstaRef->getStat() == QuestArticle::STAT_ACCEPT)	// 受諾中のみ
	{
		u32 eff = (tstat->getFuncFlags() & EleneStat::FF_MASK_EFF);
		switch(eff)
		{
		case EleneStat::FF_EFF_MIST:		// 靄
		case EleneStat::FF_EFF_MIST_L:		// 光靄
		case EleneStat::FF_EFF_MIST_H:		// 温靄
		case EleneStat::FF_EFF_WGRAIN:		// 水粒
		case EleneStat::FF_EFF_WGRAIN_L:	// 水粒（光）
		case EleneStat::FF_EFF_WGRAIN_H:	// 水粒（温）
		case EleneStat::FF_EFF_SPRAY:		// 飛沫
		case EleneStat::FF_EFF_SPRAY_L:		// 飛沫（光）
		case EleneStat::FF_EFF_SPRAY_H:		// 飛沫（温）
			{
				// クエスト要素判定
				bool isNext = false;
				for(int i = 0; i < NUM_ENTITY; i++)
				{
					if(unitRecv->getCtrlEvid() == _ent[i]._ctrlevid)	// 対象がクエスト要素である
					{
						// エフェクト表示
						ParticleSys* ptclsys = Game::getGame()->getParticleSys();
						Vector3F posBase(unitRecv->getCenterPos());
						for(int j = 0; j < 10; j++)
						{
							Vector3F pos(
								posBase.x() + (CalcUtils::randF() * 240.0f) - 120.0f,	// 左右均等の乱数
								posBase.y() + (CalcUtils::randF() * 280.0f),			// 上方にくるように
								posBase.z() + (CalcUtils::randF() * 280.0f));			// 手前にくるように
							u16 ptclid = ptclsys->appearParticle(GameParticleDef::PTCLK_WATER, &pos, 0);
							if(ptclid != 0)
							{
								Particle* ptcl = ptclsys->getParticle(ptclid);
								if(ptcl != 0L)
								{
									s32 frameDelay = (::rand() & 0x1f) * 2;	// 0 ～ 62 フレームでばらける
									ptcl->setDelayFrame(frameDelay);
								}
							}
						}
						// SE 再生
						Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_ENE_CURE, false, 1.0f, &posBase, 0L);

						// OK
						_ent[i]._ok = true;
						isNext = true;
						break;
					}
				}

				// クエストをクリアしたか
				if(isNext)	// 状態更新
				{
					isNext = true;
					for(int i = 0; i < NUM_ENTITY; i++)
					{
						if(!_ent[i]._ok)
						{
							isNext = false;
							break;
						}
					}

					// 条件達成
					if(isNext)
					{
						QuestManager* qsmng = Game::getGame()->getQuestManager(); ASSERT(qsmng != 0L);
						qsmng->reserveAchieveQuest(_qstaRef->getDef()->getQuestId());
					}
				}
			}
			break;
		}
	}

	QsMod::notifyUnitInfluenceStat(unitRecv, scres, tstat);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
