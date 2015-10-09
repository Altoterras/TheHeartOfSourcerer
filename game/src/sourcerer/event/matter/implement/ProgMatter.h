/***********************************************************************//**
 *	ProgMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_PROG_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_PROG_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../EventMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class XmlNode;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	カスタム プログラム マター
 *	
**//*---------------------------------------------------------------------*/
class ProgMatter : public EventMatter
{
	//======================================================================
	// 定数
public:	
	// プログラム マター プログラム タイプ
	enum ProgType
	{
		PROGTYPE_NULL = 0,
	};
	
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	キー＆バリュー
	 *	
	**//*-----------------------------------------------------------------*/
	class KeyValue
	{
	public:
		VcString* _key;
		VcString* _value;
		KeyValue(VcString* val);
		KeyValue(VcString* key, VcString* val);
		KeyValue(const KeyValue& src);
		~KeyValue();
	};
	
	/*-----------------------------------------------------------------*//**
	 *	動作設定
	 *	
	**//*-----------------------------------------------------------------*/
	class Setting
	{
	public:
		List<KeyValue*>* _listParams;	// ［オプション］パラメータ
		Setting();
		~Setting();
		void copy(const Setting* s);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
protected:
	// パラメータの数を得る
	s32 getParamNum() const;
	// パラメータキーを得る
	const VcString* getParamKey(s32 index) const;
	// パラメータ値を得る
	const VcString* getParamValue(s32 index) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setting(const Setting* setting);
	static bool	analyzeSettingFromXml(Setting* setting, const XmlNode* node);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ProgMatter(u16 emptype);

	//======================================================================
	// 変数
private:
	u16 _emptype;
	Setting _setting;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_PROG_MATTER_H_
