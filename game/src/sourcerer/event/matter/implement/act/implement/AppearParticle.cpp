/***********************************************************************//**
 *	AppearParticle.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "AppearParticle.h"

// Friends
#include "../../ActMatter.h"
#include "../../../../EventData.h"
#include "../../../../EventSys.h"
#include "../../../../../body/Game.h"

// External
#include "../../../../../../etk/particle/Particle.h"
#include "../../../../../../etk/particle/ParticleSys.h"
#include "../../../../../../tfw/lib/Color.h"
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"
#include "../../../../../../tfw/string/StringUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// AppearParticle メソッド

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
AppearParticle::~AppearParticle()
{
	delete _pos;
	delete _color;
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool AppearParticle::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// ［必須］パーティクル定義 ID
	s32 wk;
	if(!XmlParserUtils::analyzeAtrb(&wk, node, "particle_id"))
	{
		return false;
	}
	_ptcldid = (u16)wk;

	// ［オプション］位置
	Vector3F pos;
	if(XmlParserUtils::analyzeAtrb(&pos, node, "pos"))
	{
		_pos = new Vector3F(pos);
	}

	// ［オプション］色
	VcString strWk;
	if(XmlParserUtils::analyzeAtrb(&strWk, node, "color"))
	{
		if(strWk.getLength() <= 8)
		{
			u32 rgba = StringUtils::toInteger(&strWk, 16);
			_color = new ColorU8((u8)((rgba >> 24) & 0xff), (u8)((rgba >> 16) & 0xff), (u8)((rgba >> 8) & 0xff), (u8)(rgba & 0xff));
		}
		else if(strWk.getLength() <= 6)
		{
			u32 rgb = StringUtils::toInteger(&strWk, 16);
			_color = new ColorU8((u8)((rgb >> 16) & 0xff), (u8)((rgb >> 8) & 0xff), (u8)(rgb & 0xff), 255);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void AppearParticle::play(ActMatter* owner)
{
	if(_ptcldid != 0)
	{
		Vector3F pos;
		if(_pos != 0L)	{	pos.copy(_pos);	}

		ParticleSys* ptclsys = Game::getGame()->getParticleSys();
		s16 ptclid = ptclsys->appearParticle(_ptcldid, &pos, (_color != 0L) ? Particle::CF_EXT_COLOR : 0);
		if(ptclid > 0)
		{
			Particle* ptcl = ptclsys->getParticle(ptclid);
			if(ptcl != 0L)
			{
				// 色設定
				if(_color != 0L)
				{
					ptcl->setColor(_color);
				}
			}
		}
	}

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
