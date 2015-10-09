/***********************************************************************//**
 *	SeMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/14.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_SE_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_SE_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../EventMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
class Texture;

TFW_END_NS

ETK_BEGIN_NS

class MessageWindow;

ETK_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	SE マター
 *	
**//*---------------------------------------------------------------------*/
class SeMatter : public EventMatter
{
	//======================================================================
	// クラス
public:
	class Setting
	{
	public:
		u16				_seid;			// ［必須］SE ID
		bool			_isLoop;		// ［オプション］ループするかどうか
		f32				_volume;		// ［オプション］音量
		f32				_duration;		// ［オプション］継続フレーム

		Setting();
		void copy(const Setting* s);
	};
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setting(const Setting* setting);
	static bool	analyzeSettingFromXml(Setting* setting, const XmlNode* node);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SeMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void begin(u32 param);
	void end();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

	//======================================================================
	// 変数
private:
	Setting _setting;
	s32 _sech;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_SE_MATTER_H_
