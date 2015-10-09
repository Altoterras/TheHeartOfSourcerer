/***********************************************************************//**
 *	SetPos.cpp
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
#include "SetPos.h"

// Friends
#include "../../ActMatter.h"
#include "../../../../../body/Game.h"
#include "../../../../../unit/char/CharUnit.h"
#include "../../../../../unit/UnitManager.h"

// External
#include "../../../../../../etk/sg/SorSgm.h"
#include "../../../../../../tfw/g3d/sg/node/implement/LocAngCamera.h"
#include "../../../../../../tfw/lib/Vector.h"
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SetPos メソッド

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SetPos::~SetPos()
{
	delete _pos;
	delete _dir;
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool SetPos::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// ［オプション］設定位置
	Vector3F pos;
	if(XmlParserUtils::analyzeAtrb(&pos, node, "pos"))
	{
		_pos = new Vector3F(pos);
	}

	// ［オプション］設定位置
	Vector3F dir;
	if(XmlParserUtils::analyzeAtrb(&dir, node, "dir"))
	{
		_dir = new Vector3F(TFW_DEG_TO_RAD(dir._v[0]), TFW_DEG_TO_RAD(dir._v[1]), TFW_DEG_TO_RAD(dir._v[2]));
	}

	// ［オプション］キャラクタ ID
	if(!XmlParserUtils::analyzeAtrb(&_charid, node, "char_id"))
	{
		// 取得できなかった場合は特殊オブジェかもしれない
		VcString wk;
		if(XmlParserUtils::analyzeAtrb(&wk, node, "obj"))
		{
			// カメラの場合
			if(wk.equals("CAM"))
			{
				_charid = CHARID_CAM;
			}
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void SetPos::play(ActMatter* owner)
{
	if(_charid == CHARID_CAM)	// カメラの場合
	{
		SorSgm* sgm = Game::getGame()->getSceneGraph();	ASSERT(sgm != 0L);
		Camera* cam = sgm->getVisibleCamera();	ASSERT(cam != 0L);
		if((cam != 0L) && (cam->getCameraClass() == Camera::CAMCLS_LOCANG))
		{
			LocAngCamera* lacam = (LocAngCamera*)cam;
			if(_pos != 0L)
			{
				lacam->setLoc(_pos);
			}
			if(_dir != 0L)
			{
				lacam->setAngle(_dir);
			}
		}
		else
		{
			ASSERT(false);	// しばらくエラーを出して様子見
		}
	}
	else
	{
		// ユニットを取得する
		CharUnit* unit = 0L;
		if(_charid != 0)
		{
			UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
			unit = unitmng->findCharUnitFromCharId(_charid);
		}

		// ユニットの位置設定
		if(unit != 0L)
		{
			if(_pos != 0L)
			{
				unit->setPosition(_pos);
			}
			if(_dir != 0L)
			{
				unit->setDirection(_dir);
			}
		}
	}

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
