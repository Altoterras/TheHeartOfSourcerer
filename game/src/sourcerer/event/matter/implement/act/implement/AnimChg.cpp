/***********************************************************************//**
 *	AnimChg.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "AnimChg.h"

// Friends
#include "../../ActMatter.h"
#include "../../../../../body/Game.h"
#include "../../../../../unit/char/CharUnit.h"
#include "../../../../../unit/UnitManager.h"

// External
#include "../../../../../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// AnimChg メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool AnimChg::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// ［必須］表示か非表示か
	if(!XmlParserUtils::analyzeAtrb(&_animid, node, "anim_id"))
	{
		return false;
	}

	// ［オプション］キャラクタ ID
	XmlParserUtils::analyzeAtrb(&_charid, node, "char_id");

	// ［オプション］ループフラグ
	XmlParserUtils::analyzeAtrb(&_loop, node, "loop");

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void AnimChg::play(ActMatter* owner)
{
	// ユニットを取得する
	CharUnit* unit = 0L;
	if(_charid != 0)
	{
		UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
		unit = unitmng->findCharUnitFromCharId(_charid);
	}

	// アニメーション変更
	if(unit != 0L)
	{
		for(int i = 0; i < unit->getModelNum(); i++)
		{
			Model* mdl = unit->getModel(i);
			if(mdl->getModelClass() == Model::MODELCLASS_SHAPE)
			{
				ShapeModel* smdl = (ShapeModel*)mdl;
				smdl->setAnimId(_animid, 0, false, false, _loop, false, false);
			}
		}
	}

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
