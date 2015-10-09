/***********************************************************************//**
 *	MyUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MyUnit.h"

// Friends
#include "HelperUnit.h"
#include "MySpirit.h"
#include "PartnerUnit.h"
#include "../CharIdDef.h"
#include "../../../debug/GameTest.h"
#include "../../../episode/char/CharIdDef.h"
#include "../../../episode/party/MyParty.h"
#include "../../../episode/save/SaveStructure.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/MoveMap.h"
#include "../../../item/Item.h"
#include "../../../item/ItemEquip.h"
#include "../../../map/Map.h"
#include "../../../souma/Souma.h"
#include "../../../spirit/Spirit.h"
#include "../../../status/CharStat.h"
#include "../../../status/EquipStat.h"
#include "../../../status/TransStat.h"
#include "../../../unit/char/CharUnitSaveStructure.h"

// External
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../etk/sg/TbfDefineEx.h"
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/collision/Gcol.h"
#include "../../../../tfw/lib/Matrix.h"
#include "../../../../tfw/lib/Point.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../../../tfw/g3d/sg/node/implement/ModelLoader.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define VEL_MIN				(150.0f / (f32)FRAMERATE)	// 移動最小速さ
#define VEL_MAX				(430.0f / (f32)FRAMERATE)	// 移動最大速さ
#define RATIO_RUN			(0.79f)						// 走り速さ率
#define RATIO_FRICT_NRM		(0.45f)						// 速さ摩擦率（非加速フレーム時）
#define RATIO_FRICT_ACC		(0.65f)//(1.0f)				// 速さ摩擦率（加速フレーム時）

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MyUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	ソウマモジュールを得る
**//*---------------------------------------------------------------------*/
Souma* MyUnit::getSouma() const
{
	MySpirit* sprt = (MySpirit*)getSpirit(); ASSERT(sprt != 0L);
	Souma* souma = sprt->getSouma(); ASSERT(souma != 0L);
	return souma;
}

/*---------------------------------------------------------------------*//**
	フレンド関係にあるユニットの配列を得る
**//*---------------------------------------------------------------------*/
UnitArray MyUnit::getFriends() const
{
	UnitArray uarr;
	CharUnit* unitPc2 = _partyRef->pcUnit(MyParty::PC_PTN);
	if((unitPc2 != 0L) && unitPc2->isEnable())
	{
		uarr.addUnit(unitPc2);
	}
	CharUnit* unitPc3 = _partyRef->pcUnit(MyParty::PC_HLP);
	if((unitPc3 != 0L) && unitPc3->isEnable())
	{
		uarr.addUnit(unitPc3);
	}
	return uarr;
}

/*---------------------------------------------------------------------*//**
	増分元素エネルギーを得る
**//*---------------------------------------------------------------------*/
bool MyUnit::getEvokableElene(Elene* elene, EvokableElneCategory evecat) const
{
	#define ELENE_EFF_SELF(ev_)		((ev_) * 1)
	#define ELENE_EFF_PTNR(ev_)		((ev_) / 4)
	#define ELENE_EFF_HLPR(ev_)		((ev_) * 1)
	#define ELENE_EFF_ITEM(ev_)		((ev_) * 16)
	#define ELENE_EFF_MAP(ev_)		((ev_) / 4)

	Elene eleneSelf;	// 自らの値
	Elene elenePc2;		// パートナーからの影響値
	Elene elenePc3;		// 支援モンスターからの影響値
	Elene eleneItem;	// 装備アイテムからの影響値
	Elene eleneMap;		// マップからの影響値

	const CharParam* cprmSelf = this->getCharStat()->getCharParam();
	f32 scale = 0.05f + cprmSelf->getUnity() / 4096.0f;	//2048.0f;

	if((evecat == EVECAT_TOTAL) || (evecat == EVECAT_SELF))
	{
		// 追加分は自らの原質に基づく
		for(s32 i = 0; i < Elene::NUM_EL; i++)
		{
			s32 elval = ELENE_EFF_SELF(_eestat->getNql()->get(i));
			eleneSelf.set(i, eleneSelf.get(i) + elval);
		}
		eleneSelf.scale(scale);
	}

	// パートナーからの影響
	if((evecat == EVECAT_TOTAL) || (evecat == EVECAT_PARTY))
	{
		const CharUnit* unitPc2 = _partyRef->getPcUnit(MyParty::PC_PTN);
		if((unitPc2 != 0L) && unitPc2->isEnable())
		{
			ASSERT(unitPc2->getEleneStat() != 0L);
			const Elene* eleneEff = unitPc2->getEleneStat()->getNql();
			for(s32 i = 0; i < Elene::NUM_EL; i++)
			{
				if(eleneSelf.get(i) < eleneEff->get(i))	// パートナーの方が強い元素
				{
					s32 elval = ELENE_EFF_PTNR(eleneEff->get(i) - eleneSelf.get(i));	// パートナーからの影響分
					elenePc2.set(i, elval);
				}
			}
		}
		elenePc2.scale(scale);
	}

	// 支援モンスターからの影響
	if((evecat == EVECAT_TOTAL) || (evecat == EVECAT_HELPER))
	{
		const CharUnit* unitPc3 = _partyRef->getPcUnit(MyParty::PC_HLP);
		if((unitPc3 != 0L) && unitPc3->isEnable())
		{
			ASSERT(unitPc3->getEleneStat() != 0L);
			const Elene* eleneEff = unitPc3->getEleneStat()->getNql();
			for(s32 i = 0; i < Elene::NUM_EL; i++)
			{
				s32 elval = ELENE_EFF_HLPR(eleneEff->get(i));		// ヘルパーからの影響分
				elenePc3.set(i, elval);
			}
		}
		elenePc3.scale(scale);
	}

	// アイテムからの影響
	if((evecat == EVECAT_TOTAL) || (evecat == EVECAT_EQUIP))
	{
		const Item* itemAccsr = _itmeq->getEquipStat()->getEquipItem(EquipPlace::EQP_ACCSR);
		if(itemAccsr != 0L)
		{
			ASSERT(!itemAccsr->isEmpty());	// 装備されている以上は空で無いはず
			if(!itemAccsr->isEmpty())	// 念のため
			{
				ASSERT(itemAccsr->getEleneStat() != 0L);
				const Elene* eleneEff = itemAccsr->getEleneStat()->getCql();
				for(s32 i = 0; i < Elene::NUM_EL; i++)
				{
					if(i == Elene::EL_AE)	{	continue;	}	// 空元素は除外
					s32 elval = ELENE_EFF_ITEM(eleneEff->get(i));	// アイテムからの影響分
					eleneItem.set(i, elval);
				}
			}
		}
		eleneItem.scale(scale);
	}

	// マップからの影響
	if((evecat == EVECAT_TOTAL) || (evecat == EVECAT_MAP))
	{
		const Map* map = Game::getGame()->getMap();
		const MapConfig* mconf = (map != 0L) ? map->getMapConfig() : 0L;
		if(mconf != 0L)
		{
			const Elene* eleneEff = mconf->_eestatEff.getCql();
			for(s32 i = 0; i < Elene::NUM_EL; i++)
			{
				s32 elval = ELENE_EFF_MAP(eleneEff->get(i));	// マップからの影響分
				eleneMap.set(i, elval);
			}
		}
		eleneMap.scale(scale);
	}

	// 合算
	for(s32 i = 0; i < Elene::NUM_EL; i++)
	{
		s32 elval = eleneSelf.get(i);
		elval += elenePc2.get(i);	// パートナーからの影響分
		elval += elenePc3.get(i);	// ヘルパーからの影響分
		elval += eleneItem.get(i);	// アイテムからの影響分
		elval += eleneMap.get(i);	// マップからの影響分
		elene->set(i, elval);
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MyUnit::MyUnit()
	: PcUnit(UNITTYPE_MY_CHAR)
	, _itmeq(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MyUnit::~MyUnit()
{
	ASSERT(_itmeq == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MyUnit::create(Spirit* sprtRef, Party* party)
{
	// モデルを読み込み、リソースプールに追加（シーングラフには追加しない）
	ShapeModel* mdl = 0L;
	if(!ModelLoader::loadModelFromTbf(Game::getGame()->getTfw(), CHARACTER_CHAR1101_TBMB, TbfDefineEx::MEXF_CHAR, ModelLoader::LFLAG_NULL, (Model**)&mdl))
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
	if(!PcUnit::create(mdl, sprtRef, party, CharIdDef::CHARID_PC1, CHARCLS_SOURCERER))
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
void MyUnit::destroy()
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
void MyUnit::exec(ExecRes* res, const ExecCtx* ec)
{
	// 装備品の更新
	_itmeq->exec(ec);

	if(!isEnable())	{	return;	}

	// キャラユニットとしての移動処理など
	PcUnit::exec(res, ec);

	#if defined(_DEBUG)
		if(IS_TEST_FLAG(GameTest::F_SHOW_MY_POS))	{	STRACE("My{%f,%f,%f}\n", getPosition()->x(), getPosition()->y(), getPosition()->z());	}
	#endif
}

#if DEBUG_SHOW_MVCURSOR_POS
/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
SRCR_END_NS
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
SRCR_BEGIN_NS
void MyUnit::render(const RenderCtx* rc)
{
	if(_mvcsrRef->isTouching())
	{
		const PointF32* ptSt = _mvcsrRef->getStartPt();
		const PointF32* ptNw = _mvcsrRef->getLastPt();
		Vector2F pt[2] = { Vector2F((f32)ptSt->x(), (f32)ptSt->y()), Vector2F((f32)ptNw->x(), (f32)ptNw->y()) };
		Vector3F pos[2];
		f32 zPos[2] = { - (f32)ptSt->y() * 0.001f, - (f32)ptNw->y() * 0.001f };
		CalcUtils::calcScreenPosTo3dPos(pos, zPos, pt, 2);

		Renderer* rdr = rc->getRenderer();

		rdr->bindTexture(0L);
		rdr->setCulling(false);
		Vector3F vtx[3];
		for(int i = 0; i < 2; i++)
		{
			rdr->setSolidColor(255, 0, i * 255, 127);
			vtx[0].set(&pos[i]);
			vtx[1].set(pos[i].x() - 200.0f, pos[i].y() + 500.0f, pos[i].z());
			vtx[2].set(pos[i].x() + 200.0f, pos[i].y() + 500.0f, pos[i].z());
			RendererUtils::render3dTriangles(rdr, vtx, 0L, 0L, 0L, 3, RendererUtils::TRIANGLES);
			STRACE("[%d]{%f, %f, %f}\n", i, pos[i].x(), pos[i].y(), pos[i].z());
		}
		rdr->setCulling(true);
///		rdr->setSolidColor(255, 255, 255, 255);	// 色を戻す
	}
}
#endif	// #if DEBUG_SHOW_MVCURSOR_POS

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool MyUnit::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		SaveStructure::PcSaveStructure* pcsvst = &svst->_pcsvst[0];
		
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
bool MyUnit::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		const SaveStructure* svst = (SaveStructure*)ser->getStructure();
		const SaveStructure::PcSaveStructure* pcsvst = &svst->_pcsvst[0];

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

	return true;
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void MyUnit::notifyChangedMap()
{
	PcUnit::notifyChangedMap();	// 失敗した場合は CharUnit による配置に任せる
}

/*---------------------------------------------------------------------*//**
	対決状態変更通知
**//*---------------------------------------------------------------------*/
void MyUnit::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition)
{
	PcUnit::notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);
}

/*---------------------------------------------------------------------*//**
	イベントゲームモード変更通知
**//*---------------------------------------------------------------------*/
void MyUnit::notifyChangedEventMode(bool isEventMode)
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
void MyUnit::calcTargetPos()
{
	if(isOperatedForMoving())	// 移動操作時
	{
		setTargetPosition(_pos);
	}
	else
	{
		Vector3F posLeaderDiff(-134.0f, 0.0f, -100.0f);	// PC1 はリーダーの右後ろにいる
		PcUnit::calcTargetPosRelLeader(&posLeaderDiff);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
