/***********************************************************************//**
 *	ValFaderMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/08.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ValFaderMatter.h"

// Friends
#include "ActMatter.h"
#include "SpriteMatter.h"
#include "act/Act.h"
#include "../EventMatter.h"
#include "../EventMatterType.h"
#include "../../EventData.h"
#include "../../EventSys.h"
#include "../../../body/Game.h"
#include "../../../model/spid/Spid.h"
#include "../../../unit/Unit.h"
#include "../../../unit/UnitManager.h"

// External
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/g3d/Shape.h"
#include "../../../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ValFaderMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	動作設定
**//*---------------------------------------------------------------------*/
void ValFaderMatter::setting(const Setting* s)
{
	_setting.copy(s);
}

/*---------------------------------------------------------------------*//**
	設定を XML から解析する
**//*---------------------------------------------------------------------*/
bool ValFaderMatter::analyzeSettingFromXml(Setting* s, const XmlNode* node)
{
	VcString strwk;
	s32 nwk;
	f32 fwk;

	// ［必須］要素
	if(!XmlParserUtils::analyzeAtrb(&strwk, node, "element"))
	{
		return false;
	}
	if(strwk.equalsIgnoreCase("pos_x"))			{	s->_telmf = TARGET_ELMF_POS_X;		}
	else if(strwk.equalsIgnoreCase("pos_y"))	{	s->_telmf = TARGET_ELMF_POS_Y;		}
	else if(strwk.equalsIgnoreCase("pos_z"))	{	s->_telmf = TARGET_ELMF_POS_Z;		}
	else if(strwk.equalsIgnoreCase("color_r"))	{	s->_telmf = TARGET_ELMF_COLOR_R;	}
	else if(strwk.equalsIgnoreCase("color_g"))	{	s->_telmf = TARGET_ELMF_COLOR_G;	}
	else if(strwk.equalsIgnoreCase("color_b"))	{	s->_telmf = TARGET_ELMF_COLOR_B;	}
	else if(strwk.equalsIgnoreCase("color_a"))	{	s->_telmf = TARGET_ELMF_COLOR_A;	}

	// ［オプション］名前
	if(XmlParserUtils::analyzeAtrb(&strwk, node, "name"))
	{
		s->_name = new VcString(strwk);
	}

	// ［オプション］SPID
	if(XmlParserUtils::analyzeAtrb(&nwk, node, "spid"))
	{
		s->_spid = nwk;
	}

	// ［オプション］開始値
	if(XmlParserUtils::analyzeAtrb(&fwk, node, "value_from"))
	{
		s->_valFrom = fwk;
	}

	// ［オプション］終了値
	if(XmlParserUtils::analyzeAtrb(&fwk, node, "value_to"))
	{
		s->_valTo = fwk;
	}
	
	// ［オプション］継続フレーム
	s32 duration = 1;
	if(XmlParserUtils::analyzeAtrb(&duration, node, "frame_num"))
	{
		s->_nframe = duration;
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ValFaderMatter::ValFaderMatter()
	: EventMatter(EMTYPE_VALFADER)
	, _target(0L)
	, _ttype(0)
	, _value(0.0f)
	, _frmcnt(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ValFaderMatter::~ValFaderMatter()
{
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ValFaderMatter::destroy()
{
	EventMatter::destroy();
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void ValFaderMatter::begin(u32 param)
{
	EventMatter::begin(param);

	// 対象の検索
	searchTarget();

	// 値の初期化
	_value = _setting._valFrom;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void ValFaderMatter::end()
{
	// 念のため再適用
	searchTarget();
	_value = (f32)_setting._valTo;
	applyValue();

	EventMatter::end();
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool ValFaderMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
{
	if(!EventMatter::setupFromXmlNode(node, cparam))	{	return false;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// 動作設定解析

	analyzeSettingFromXml(&_setting, node);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理実装
**//*---------------------------------------------------------------------*/
void ValFaderMatter::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	// フレームを更新
	_frmcnt += ec->getDeltaFrame();
	f32 nfrmf = (f32)_setting._nframe;

	if(_frmcnt >= nfrmf)
	{
		// 終了（最終フレームは end 内で値を反映している）
		end();
	}
	else
	{
		// 値を更新
		f32 valfTo = (f32)_setting._valTo;
		f32 valfFrom = (f32)_setting._valFrom;
		f32 valueNew = ((valfTo - valfFrom) * _frmcnt / nfrmf) + valfFrom;
		if(valfTo > valfFrom)
		{
			if(valueNew > valfTo)	{	valueNew = valfTo;	}
		}
		else if(valfTo < valfFrom)
		{
			if(valueNew < valfTo)	{	valueNew = valfTo;	}
		}
		_value = valueNew;

		// 最終フレームあたりはオブジェクトの生存があやしくなる可能性が高いので重いけど再検索する
		if(_frmcnt >= (nfrmf - 1.1f))
		{
			searchTarget();
		}

		// 値を反映
		applyValue();
	}
}

/*---------------------------------------------------------------------*//**
	SPID から対象を探す
**//*---------------------------------------------------------------------*/
bool ValFaderMatter::findTargetBySpid(u16 spid)
{
	_target = 0L;
	_ttype = 0;

	Model* mdl;
	if(Spid::find(&mdl, _setting._spid, Game::getGame()->getSceneGraph()))
	{
		// 発見
		_target = mdl;
		_ttype = TARGET_TYPE_MODEL;
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	名前から対象を探す
**//*---------------------------------------------------------------------*/
bool ValFaderMatter::findTargetByName(const VcString* name)
{
	_target = 0L;
	_ttype = 0;

	// イベントから該当名のものを探す
	EventData* evdat = getOwner()->getEventData(); ASSERT(evdat != 0L);
	for(int ied = 0; ied < evdat->getEventMatterNum(); ied++)
	{
		EventMatter* matterWk = evdat->getEventMatter(ied);
		if(matterWk->getMatterType() == EMTYPE_ACT)
		{
			// Act リストから該当名のものを探す
			List<Act*>* listAct = ((ActMatter*)matterWk)->getPlayingActList();
			for(ListIterator<Act*> itAct = listAct->iterator(); itAct.has(); itAct.next())
			{
				Act* act = itAct.object();
				if((act->_name != 0L) && (act->_name->equals(name)))
				{
					// その Act が指し示しているマター
					EventMatter* matterTrg = evdat->getEventMatterFromEvid(act->_evid);
					if(matterTrg != 0L)
					{
						switch(matterTrg->getMatterType())
						{
						case EMTYPE_SPRITE:
							// 発見
							_target = matterTrg;
							_ttype = TARGET_TYPE_SPRITE;
							return true;
						default:
							break;
						}
					}
				}
			}
		}
	}

	// モデルから該当名のものを探す
	UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
	for(int iuit = 0; iuit < unitmng->getUnitNum(); iuit++)
	{
		Unit* unitWk = unitmng->getUnitFromIndex(iuit);
		for(int imdl = 0; imdl < unitWk->getModelNum(); imdl++)
		{
			Model* mdl = unitWk->getModel(imdl);
			if(mdl->getModelClass() == Model::MODELCLASS_SHAPE)
			{
				ShapeModel* shpmdl = (ShapeModel*)mdl;
				for(int ishp = 0; ishp < shpmdl->getShapeNum(); ishp++)
				{
					Shape* shp = shpmdl->getShape(ishp);
					if((shp->getDefineName() != 0L) && (shp->getDefineName()->equals(name)))
					{
						// 発見
						_target = mdl;
						_ttype = TARGET_TYPE_MODEL;
						return true;
					}
				}
			}
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	対象を検索する（重い処理）
**//*---------------------------------------------------------------------*/
void ValFaderMatter::searchTarget()
{
	// SPID から対象を探す
	if(_setting._spid != 0)
	{
		if(!findTargetBySpid(_setting._spid))
		{
			TRACE("{ValFaderMatter::begin} no tareget. - spid=%d\n", _setting._spid);
		}
	}
	// 名前から対象を探す
	else if(_setting._name != 0L)
	{
		if(!findTargetByName(_setting._name))
		{
			TRACE("{ValFaderMatter::begin} no tareget. - name=%s\n", (_setting._name != 0L) ? _setting._name->getRaw() : "(NULL)");
		}
	}
}

/*---------------------------------------------------------------------*//**
	値の反映
**//*---------------------------------------------------------------------*/
void ValFaderMatter::applyValue()
{
	// 値を反映
	switch(_ttype)
	{
	case TARGET_TYPE_SPRITE:
		{
			SpriteMatter* spm = (SpriteMatter*)_target;
			if(TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_POS_X))	{	spm->rect()->x() = _value;		}
			if(TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_POS_Y))	{	spm->rect()->y() = _value;		}
			if(TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_POS_Z))	{	spm->rect()->z() = _value;		}
		}
		break;
	case TARGET_TYPE_MODEL:
		{
			Model* mdl = (Model*)_target;
			if(	TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_POS_X) ||
				TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_POS_Y) ||
				TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_POS_Z) )
			{
				Vector3F pos(mdl->getPosition());
				if(TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_POS_X))		{	pos.x() = _value;		}
				if(TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_POS_Y))		{	pos.y() = _value;		}
				if(TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_POS_Z))		{	pos.z() = _value;		}
				mdl->setPosition(&pos, 0L);
			}
			if(	TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_COLOR_R) ||
				TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_COLOR_G) ||
				TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_COLOR_B) ||
				TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_COLOR_A) )
			{
				ColorU8 color((mdl->getColor() != 0L) ? *mdl->getColor() : ColorU8(255, 255, 255, 255));
				if(TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_COLOR_R))	{	color.r() = (u8)_value;	}
				if(TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_COLOR_G))	{	color.g() = (u8)_value;	}
				if(TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_COLOR_B))	{	color.b() = (u8)_value;	}
				if(TFW_IS_FLAG(_setting._telmf, TARGET_ELMF_COLOR_A))	{	color.a() = (u8)_value;	}
				mdl->setColor(&color);
			}
		}
		break;
	}
}


////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
