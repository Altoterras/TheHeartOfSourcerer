/***********************************************************************//**
 *	AppearParticle.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_PARTICLE_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_PARTICLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	パーティクル出現
 *	
**//*---------------------------------------------------------------------*/
class AppearParticle : public Act
{
public:
	u16				_ptcldid;		// ［必須］パーティクル定義 ID
	Vector3F*		_pos;			// ［オプション］位置
	ColorU8*		_color;			// ［オプション］色

public:
	AppearParticle() : Act(ACT_APPEAR_PARTICLE), _ptcldid(0), _pos(0L), _color(0L) {}
	~AppearParticle();
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_PARTICLE_H_
