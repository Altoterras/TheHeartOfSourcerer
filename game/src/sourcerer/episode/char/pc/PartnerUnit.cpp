/***********************************************************************//**
 *	PartnerUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PartnerUnit.h"

// Friends
#include "PartnerSpirit.h"
#include "MyUnit.h"
#include "../../party/MyParty.h"
#include "../../../debug/GameTest.h"
#include "../../../episode/MapIdDef.h"
#include "../../../episode/char/CharIdDef.h"
#include "../../../episode/item/ItemIdDef.h"
#include "../../../episode/save/SaveStructure.h"
#include "../../../episode/spid/SpidDef.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/MoveMap.h"
#include "../../../item/ItemArray.h"
#include "../../../item/ItemEquip.h"
#include "../../../item/ItemManager.h"
#include "../../../map/Map.h"
#include "../../../model/spid/Spid.h"
#include "../../../particle/GameParticleDef.h"
#include "../../../status/CharStat.h"
#include "../../../status/EleneStat.h"
#include "../../../status/EquipStat.h"
#include "../../../status/StatusDrawer.h"

// External
#include "../../../../etk/particle/Particle.h"
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../etk/sg/TbfDefineEx.h"
#include "../../../../tfw/collision/Gcol.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/lib/Matrix.h"
#include "../../../../tfw/lib/Point.h"
#include "../../../../tfw/g3d/HrAnimObjCtx.h"
#include "../../../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../../../tfw/g3d/sg/node/implement/ModelLoader.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/ui/TouchPanel.h"
#include "../../../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define VEL_MIN				(150.0f / (f32)FRAMERATE)	// 移動最小速さ
#define VEL_MAX				(440.0f / (f32)FRAMERATE)	// 最大移動速さ
#define RATIO_RUN			(0.89f)//(0.79f)					// 走り速さ率
#define RATIO_FRICT_NRM		(0.45f)								// 速さ摩擦率（非加速フレーム時）
#define RATIO_FRICT_ACC		(0.65f)								// 速さ摩擦率（加速フレーム時）

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PartnerUnit 静的メンバ変数

//==========================================================================
// PartnerUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	フレンド関係にあるユニットの配列を得る
**//*---------------------------------------------------------------------*/
UnitArray PartnerUnit::getFriends() const
{
	UnitArray uarr;
	CharUnit* unitPc1 = _partyRef->pcUnit(MyParty::PC_MY);
	if((unitPc1 != 0L) && unitPc1->isEnable())
	{
		uarr.addUnit(unitPc1);
	}
	CharUnit* unitPc3 = _partyRef->pcUnit(MyParty::PC_HLP);
	if((unitPc3 != 0L) && unitPc3->isEnable())
	{
		uarr.addUnit(unitPc3);
	}
	return uarr;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	アニメーション変更
**//*---------------------------------------------------------------------*/
void PartnerUnit::changeAnim(u16 animid, bool isLoop)
{
	CharUnit::changeAnim(animid, isLoop);
}

/*---------------------------------------------------------------------*//**
	最強装備
**//*---------------------------------------------------------------------*/
void PartnerUnit::equipBest()
{
	ItemManager* itmmng = _partyRef->getItemManager(); ASSERT(itmmng != 0L);

	// 武器装備
	{
		// 所持している最も強い（定義 ID の大きいもの）を装備する
		ItemArray itmarr = itmmng->findByCategory(ItemDef::CAT_WEAPON, true);
		u16 itmdid = 0;
		Item* item = 0L;
		for(int i = 0; i < itmarr.getCount(); i++)
		{
			if(itmarr.getItem(i)->getItemDef()->getItemDefId() > itmdid)	// 大きい定義 ID のものを探す
			{
				item = itmarr.item(i);
				itmdid = item->getItemDef()->getItemDefId();
			}
		}
		if(item != 0L)	{	itemEquip()->equip(EquipPlace::EQP_RHAND, item, SpidDef::SPID_PC2_WEP_ACTIVE);		}	// 見つかったものを装備
	}

	// 盾装備
	{
		// 所持している最も強い（定義 ID の大きいもの）を装備する
		ItemArray itmarr = itmmng->findByCategory(ItemDef::CAT_SHIELD, true);
		u16 itmdid = 0;
		Item* item = 0L;
		for(int i = 0; i < itmarr.getCount(); i++)
		{
			if(itmarr.getItem(i)->getItemDef()->getItemDefId() > itmdid)	// 大きい定義 ID のものを探す
			{
				item = itmarr.item(i);
				itmdid = item->getItemDef()->getItemDefId();
			}
		}
		if(item != 0L)	{	itemEquip()->equip(EquipPlace::EQP_LHAND, item, SpidDef::SPID_PC2_SLD_ACTIVE);		}	// 見つかったものを装備
	}
}

/*---------------------------------------------------------------------*//**
	装備状態のフラフィクスを更新
**//*---------------------------------------------------------------------*/
void PartnerUnit::updateEquipModelObjVisibility()
{
	if(!isEnable())	{	return;		}

	// 制御コンテキストを得る
	HrAnimObjCtx* haocPc2WeaponDummy = 0L;
	HrAnimObjCtx* haocPc2WeaponReal = 0L;
	HrAnimObjCtx* haocPc2ShieldDummy = 0L;
	HrAnimObjCtx* haocPc2ShieldReal = 0L;
	Spid::find(&haocPc2WeaponDummy, SpidDef::SPID_PC2_WEP_INACTIVE, this);
	Spid::find(&haocPc2WeaponReal, SpidDef::SPID_PC2_WEP_ACTIVE, this);
	Spid::find(&haocPc2ShieldDummy, SpidDef::SPID_PC2_SLD_INACTIVE, this);
	Spid::find(&haocPc2ShieldReal, SpidDef::SPID_PC2_SLD_ACTIVE, this);

	// コメサット村のマップであるかによる判定
	bool isWeaponEquip = true;
	bool isShieldEquip = true;
	if(MapIdDef::isBaseVillage(Game::getGame()->getMoveMap()->getCurMapId()))
	{
		isWeaponEquip = false;
		isShieldEquip = false;
	}

	// 盾を持っているか
	bool isHasShield = false;
	ItemManager* itmmng = _partyRef->getItemManager(); ASSERT(itmmng != 0L);
	for(int i = 0; i < itmmng->getItemNum(); i++)
	{
		const Item* item = itmmng->getItem(i);
		const ItemDef* itmdf = item->getItemDef();
		if(	(itmdf != 0L) &&
			(itmdf->getCategory() == ItemDef::CAT_SHIELD) &&
			(itmdf->getItemDefId() >= ItemIdDef::ITEMDID_SHLD_FIRST)	)
		{
			isHasShield = true;
		}
	}

	// オブジェクトの有効・無効化
	if(haocPc2WeaponDummy != 0L)	{	haocPc2WeaponDummy->setFlags(HrAnimObjCtx::F_OFF, isWeaponEquip);					}
	if(haocPc2WeaponReal != 0L)		{	haocPc2WeaponReal->setFlags(HrAnimObjCtx::F_OFF, !isWeaponEquip);					}
	if(haocPc2ShieldDummy != 0L)	{	haocPc2ShieldDummy->setFlags(HrAnimObjCtx::F_OFF, isShieldEquip || !isHasShield);	}
	if(haocPc2ShieldReal != 0L)		{	haocPc2ShieldReal->setFlags(HrAnimObjCtx::F_OFF, !isShieldEquip || !isHasShield);	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PartnerUnit::PartnerUnit()
	: PcUnit(UNITTYPE_PARTNER_CHAR)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PartnerUnit::~PartnerUnit()
{
	ASSERT(_itmeq == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PartnerUnit::create(Spirit* sprtRef, Party* party)
{
	// モデルを読み込み、リソースプールに追加（シーングラフには追加しない）
	ShapeModel* mdl = 0L;
	if(!ModelLoader::loadModelFromTbf(Game::getGame()->getTfw(), CHARACTER_CHAR1102_TBMB, TbfDefineEx::MEXF_CHAR, ModelLoader::LFLAG_NULL, (Model**)&mdl))
	{
		return false;
	}

	// アイテム装備機能の作成
	ItemEquip* itmeq = new ItemEquip();
	if((itmeq == 0L) || (!itmeq->create(this)))
	{
		delete itmeq;
		return false;
	}
	_itmeq = itmeq;

	// 親クラスによる作成
	if(!PcUnit::create(mdl, sprtRef, party, CharIdDef::CHARID_PC2, CHARCLS_FIGHTER))
	{
		return false;
	}

	// 基本速さパラメータの設定
	setVelocityParam(VEL_MAX, RATIO_RUN, RATIO_FRICT_NRM, RATIO_FRICT_ACC);
	setVelocityRange(VEL_MIN, VEL_MAX);

	// 半径、中央位置の調整
	_heightCenter += 30.0f;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PartnerUnit::destroy()
{
	// アイテム装備機能の削除
	if(_itmeq != 0L)
	{
		_itmeq->destroy();
		delete _itmeq;
		_itmeq = 0L;
	}

	PcUnit::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void PartnerUnit::exec(ExecRes* res, const ExecCtx* ec)
{
	// 装備品の更新
	_itmeq->exec(ec);

	if(!isEnable())	{	return;	}

	// キャラユニットとしての移動処理など
	PcUnit::exec(res, ec);

	// こん棒へのエンチャント（ベール）のエフェクト表示　【2010/12/28 r-kishi】いずれは PcUnit などで統一的に制御か？
	GameExecCtx* gec = (GameExecCtx*)ec;
	if(!gec->isLockOperating() && !gec->isOpenedLockGui())
	{
		if((gec->getBodyExecCount() & 0xf) == 0)	// 適当に間引く
		{
			Unit* unitWep = _itmeq->getEquipUnit(EquipPlace::EQP_RHAND);
			if(unitWep != 0L)
			{
				const EleneStat* eestatWep = unitWep->eleneStat();
				const EleneVeil* veilWep = eestatWep->getVeil(EleneStat::VEIL_WP);
				if(veilWep->isValid())
				{
					// 合計元素料が多いほうがパーティクル出現頻度が高い
					const Elene* eleneWep = veilWep->getElene();
					s32 sumVeil = eleneWep->sumExceptAethel();
					sumVeil >>= 10;	// ～ 約 500
					if((::rand() & 0x1ff) < sumVeil)
					{
						ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
						u16 ptclid = ptclsys->appearParticle(GameParticleDef::PTCLK_WEP_ENCHANT, unitWep->getPosition(), Particle::CF_EXT_COLOR);
						if(ptclid != 0)
						{
							Particle* ptcl = ptclsys->getParticle(ptclid);
							if(ptcl != 0L)
							{
								ColorU8 col;
								StatusDrawer::calcEleneColorForParticle(&col, veilWep->getElene());
								ptcl->setColor(&col);
							}
						}
					}
				}
			}
		}
	}

	#if defined(_DEBUG) && 1
		if(IS_TEST_FLAG(GameTest::F_SHOW_MY_POS))	{	STRACE("Pt{%f,%f,%f}\n", getPosition()->x(), getPosition()->y(), getPosition()->z());	}
	#endif
}

/*---------------------------------------------------------------------*//**
	移動アニメーション変更 - 停止へ
**//*---------------------------------------------------------------------*/
void PartnerUnit::execChangeMoveAnimationToStop(const ExecCtx* ec, u16 cmflags)
{
	if(TFW_IS_FLAG(cmflags, CMF_CONFRONT))
	{
		getShapeModel()->setAnimId(ANIMID_CF_STOP, ANIMFRAME_INTP, false, false, true, false, false);
	}
	else
	{
		getShapeModel()->setAnimId(ANIMID_STOP, ANIMFRAME_INTP, false, false, true, false, false);
	}
}

/*---------------------------------------------------------------------*//**
	移動アニメーション変更 - 走行へ
**//*---------------------------------------------------------------------*/
void PartnerUnit::execChangeMoveAnimationToRun(const ExecCtx* ec, u16 cmflags)
{
	if(TFW_IS_FLAG(cmflags, CMF_CONFRONT))
	{
		getShapeModel()->setAnimId(ANIMID_CF_RUN, ANIMFRAME_INTP, true, false, true, true, false);
	}
	else
	{
		getShapeModel()->setAnimId(ANIMID_WALK, ANIMFRAME_INTP, true, false, true, true, false);
	}
}

/*---------------------------------------------------------------------*//**
	移動アニメーション変更 - 歩行へ
**//*---------------------------------------------------------------------*/
void PartnerUnit::execChangeMoveAnimationToWalk(const ExecCtx* ec, u16 cmflags)
{
	if(TFW_IS_FLAG(cmflags, CMF_CONFRONT))
	{
		getShapeModel()->setAnimId(ANIMID_CF_WALK, ANIMFRAME_INTP, true, false, true, true, false);
	}
	else
	{
		getShapeModel()->setAnimId(ANIMID_WALK, ANIMFRAME_INTP, true, false, true, true, false);
	}
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool PartnerUnit::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		SaveStructure::PcSaveStructure* pcsvst = &svst->_pcsvst[1];
		
		CharUnit::storeToStructure(&pcsvst->_cusvst);
		_itmeq->storeToStructure(&pcsvst->_itmeq);
	}
	else						// ダイレクトモード
	{
		CharUnitSaveStructure cusvst;
		CharUnit::storeToStructure(&cusvst);
		ser->store(&cusvst, sizeof(CharUnitSaveStructure));

		if(!_itmeq->serialize(ser)) { return false; }
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアイズ
**//*---------------------------------------------------------------------*/
bool PartnerUnit::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		const SaveStructure* svst = (SaveStructure*)ser->getStructure();
		const SaveStructure::PcSaveStructure* pcsvst = &svst->_pcsvst[1];

		CharUnit::restoreFromStructure(&pcsvst->_cusvst);
		_itmeq->restoreFromStructure(&pcsvst->_itmeq);
	}
	else						// ダイレクトモード
	{
		CharUnitSaveStructure cusvst;
		ser->restore(&cusvst, sizeof(CharUnitSaveStructure));
		CharUnit::restoreFromStructure(&cusvst);

		if(!_itmeq->deserialize(ser)) { return false; }
	}

	updateEquipModelObjVisibility();

	return true;
}

/*---------------------------------------------------------------------*//**
	イベントゲームモード変更通知
**//*---------------------------------------------------------------------*/
void PartnerUnit::notifyChangedEventMode(bool isEventMode)
{
	PcUnit::notifyChangedEventMode(isEventMode);

	// 移動を停止へ
	if(isEventMode)	// イベント開始時に
	{
		// ダウンとストップ以外はストップアニメーションに
		u16 animidCur = getCurrentAnimId();
		switch(animidCur)
		{
		case ANIMID_TO_DOWN:
		case ANIMID_STOP:
			break;
		default:
			changeAnim(ANIMID_STOP, true);
			break;
		}
	}
}

/*---------------------------------------------------------------------*//**
	目標位置計算
**//*---------------------------------------------------------------------*/
void PartnerUnit::calcTargetPos()
{
	if(getSpirit()->isConfront() || isOperatedForMoving())	// 対決時または移動操作時
	{
		setTargetPosition(_pos);
	}
	else										// 移動時
	{
		Vector3F posLeaderDiff(-134.0f, 0.0f, 100.0f);	// PC2 は PC1 の右前にいる
		PcUnit::calcTargetPosRelLeader(&posLeaderDiff);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
