/***********************************************************************//**
 *	IntvlSaveMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/04/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_INTVL_SAVE_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_INTVL_SAVE_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../EventMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class XmlNode;

TFW_END_NS

SRCR_BEGIN_NS

class SaveLoadWindow;

/*---------------------------------------------------------------------*//**
 *	途中セーブ マター
 *	
**//*---------------------------------------------------------------------*/
class IntvlSaveMatter : public EventMatter
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	動作設定
	 *	
	**//*-----------------------------------------------------------------*/
	class Setting
	{
	public:
		Setting(){}
		void copy(const Setting* s)
		{
		}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
protected:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setting(const Setting* setting);
	static bool	analyzeSettingFromXml(Setting* setting, const XmlNode* node);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	IntvlSaveMatter();
	~IntvlSaveMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void begin(u32 param);
	void end();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

	//======================================================================
	// 変数
private:
	Setting _setting;
	SaveLoadWindow* _wndSave;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_INTVL_SAVE_MATTER_H_
