/***********************************************************************//**
 *	ProgMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ProgMatter.h"

// Friends
#include "../EventMatterType.h"
#include "../../../body/Game.h"

// External
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"
#include "../../../../tfw/string/StringUtils.h"

// Library

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ProgMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	パラメータの数を得る
**//*---------------------------------------------------------------------*/
s32 ProgMatter::getParamNum() const
{
	return _setting._listParams->getCount();
}

/*---------------------------------------------------------------------*//**
	パラメータキーを得る
**//*---------------------------------------------------------------------*/
const VcString* ProgMatter::getParamKey(s32 index) const
{
	if((index < 0) || (_setting._listParams->getCount() <= (u32)index)) { return 0L; }
	return _setting._listParams->getObject(index)->_key;
}

/*---------------------------------------------------------------------*//**
	パラメータ値を得る
**//*---------------------------------------------------------------------*/
const VcString* ProgMatter::getParamValue(s32 index) const
{
	if((index < 0) || (_setting._listParams->getCount() <= (u32)index)) { return 0L; }
	return _setting._listParams->getObject(index)->_value;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	動作設定
**//*---------------------------------------------------------------------*/
void ProgMatter::setting(const Setting* setting)
{
	_setting.copy(setting);
}

/*---------------------------------------------------------------------*//**
	設定を XML から解析する
**//*---------------------------------------------------------------------*/
bool ProgMatter::analyzeSettingFromXml(Setting* setting, const XmlNode* node)
{
	VcString strwk;

	// ［オプション］パラメータ
	if(XmlParserUtils::analyzeAtrb(&strwk, node, "param"))
	{
		VcStringArray saParams = StringUtils::split(&strwk, '&');
		for(int iparam = 0; iparam < saParams.getCount(); iparam++)
		{
			VcStringArray saKv = StringUtils::split(saParams.get(iparam), '=');
			if((saKv.getCount() == 2) || (saKv.getCount() == 1))
			{
				KeyValue* kv = 0L;
				if(saKv.getCount() == 2)
				{
					kv = new KeyValue(saKv.get(0), saKv.get(1));
				}
				else if(saKv.getCount() == 1)
				{
					kv = new KeyValue(saKv.get(0));
				}
				if(kv != 0L)
				{
					setting->_listParams->addTail(kv);
				}
			}
		}
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ProgMatter::ProgMatter(u16 emptype)
	: EventMatter(EMTYPE_PROG)
	, _emptype(emptype)
{
}

//==========================================================================
// ProgMatter::KeyValue メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*-----------------------------------------------------------------*//**
	コンストラクタ
**//*-----------------------------------------------------------------*/
ProgMatter::KeyValue::KeyValue(VcString* val)
	: _key(0L)
	, _value(new VcString(val))
{
}

/*-----------------------------------------------------------------*//**
	コンストラクタ
**//*-----------------------------------------------------------------*/
ProgMatter::KeyValue::KeyValue(VcString* key, VcString* val)
	: _key(new VcString(key))
	, _value(new VcString(val))
{
}

/*-----------------------------------------------------------------*//**
	コピー コンストラクタ
**//*-----------------------------------------------------------------*/
ProgMatter::KeyValue::KeyValue(const KeyValue& src)
	: _key(new VcString(src._key))
	, _value(new VcString(src._value))
{
}

/*-----------------------------------------------------------------*//**
	デストラクタ
**//*-----------------------------------------------------------------*/
ProgMatter::KeyValue::~KeyValue()
{
	delete _key;
	delete _value;
}

//==========================================================================
// ProgMatter::Setting メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ProgMatter::Setting::Setting()
	: _listParams(new List<ProgMatter::KeyValue*>(true))
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ProgMatter::Setting::~Setting()
{
	delete _listParams;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void ProgMatter::Setting::copy(const Setting* s)
{
	// _listParams のコピー
	{
		_listParams->removeAll();
		ListNode<KeyValue*>* lnode = s->_listParams->getHeadNode();
		while(lnode != 0L)
		{
			_listParams->addTail(new KeyValue(*lnode->getObject()));
			lnode = lnode->getNext();
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
