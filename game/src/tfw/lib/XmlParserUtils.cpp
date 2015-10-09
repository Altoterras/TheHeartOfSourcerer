/***********************************************************************//**
 *  XmlParserUtils.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/20.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "XmlParserUtils.h"

// Friends
#include "XmlParser.h"
#include "Vector.h"
#include "../string/VcString.h"
#include "../string/StringUtils.h"
#include "../collection/InsList.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// XmlParserUtils メソッド

/*---------------------------------------------------------------------*//**
	XML ノードから文字列データを得る

	@return	見つけたノードのポインタを返す
			見つからなかった場合は NULL を返す
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeData(VcString* data, const XmlNode* node)
{
	const VcString* pcdata = node->getChildPcdata();
	if(pcdata != 0L)
	{
		*data = *pcdata;
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	XML ノードから整数データを得る

	@return	見つけたノードのポインタを返す
			見つからなかった場合は NULL を返す
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeData(s32* data, const XmlNode* node)
{
	VcString strData;
	if(!analyzeData(&strData, node))	{	return false;	}

	*data = StringUtils::toInteger(&strData);
	return true;
}

/*---------------------------------------------------------------------*//**
	XML ノードから浮動小数点値データを得る

	@return	見つけたノードのポインタを返す
			見つからなかった場合は NULL を返す
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeData(f32* data, const XmlNode* node)
{
	VcString strData;
	if(!analyzeData(&strData, node))	{	return false;	}

	*data = (f32)StringUtils::toDouble(&strData, 0L);
	return true;
}

/*---------------------------------------------------------------------*//**
	XML ノードからブーリアンデータを得る

	@return	見つけたノードのポインタを返す
			見つからなかった場合は NULL を返す
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeData(bool* data, const XmlNode* node)
{
	VcString strData;
	if(!analyzeData(&strData, node))	{	return false;	}

	*data = StringUtils::toBoolean(&strData);
	return true;
}

/*---------------------------------------------------------------------*//**
	XML ノードから３要素ベクトルデータを得る

	@return	見つけたノードのポインタを返す
			見つからなかった場合は NULL を返す
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeData(Vector3F* data, const XmlNode* node)
{
	VcString strData;
	if(!analyzeData(&strData, node))	{	return false;	}

	toVector3F(data, &strData);
	return true;
}

/*---------------------------------------------------------------------*//**
	XML ノードから４要素ベクトルデータを得る

	@return	見つけたノードのポインタを返す
			見つからなかった場合は NULL を返す
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeData(Vector4F* data, const XmlNode* node)
{
	VcString strData;
	if(!analyzeData(&strData, node))	{	return false;	}

	toVector4F(data, &strData);
	return true;
}

/*---------------------------------------------------------------------*//**
	XML ノードから文字列データを得る

	@return	見つけたノードのポインタを返す
			見つからなかった場合は NULL を返す
**//*---------------------------------------------------------------------*/
const XmlNode* XmlParserUtils::analyzeChildData(VcString* data, const XmlNode* node, const CStringBase* nameNode)
{
	if(node == 0L)	{	return 0L;	}
	const XmlNode* nodeChild = node->getChildNode();
	while(nodeChild != 0L)
	{
		if(nodeChild->getName()->equals(nameNode))
		{
			const VcString* pcdata = nodeChild->getChildPcdata();
			if(pcdata != 0L)
			{
				*data = *pcdata;
				return nodeChild;
			}
		}
		
		nodeChild = nodeChild->getSiblingNode();
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	XML ノードから整数データを得る

	@return	見つけたノードのポインタを返す
			見つからなかった場合は NULL を返す
**//*---------------------------------------------------------------------*/
const XmlNode* XmlParserUtils::analyzeChildData(s32* data, const XmlNode* node, const CStringBase* nameNode)
{
	VcString strData;
	const XmlNode* nodeFind = analyzeChildData(&strData, node, nameNode);
	if(nodeFind == 0L)	{	return 0L;	}

	*data = StringUtils::toInteger(&strData);
	return nodeFind;
}

/*---------------------------------------------------------------------*//**
	XML ノードから浮動小数点値データを得る

	@return	見つけたノードのポインタを返す
			見つからなかった場合は NULL を返す
**//*---------------------------------------------------------------------*/
const XmlNode* XmlParserUtils::analyzeChildData(f32* data, const XmlNode* node, const CStringBase* nameNode)
{
	VcString strData;
	const XmlNode* nodeFind = analyzeChildData(&strData, node, nameNode);
	if(nodeFind == 0L)	{	return 0L;	}

	*data = (f32)StringUtils::toDouble(&strData, 0L);
	return nodeFind;
}

/*---------------------------------------------------------------------*//**
	XML ノードからブーリアンデータを得る

	@return	見つけたノードのポインタを返す
			見つからなかった場合は NULL を返す
**//*---------------------------------------------------------------------*/
const XmlNode* XmlParserUtils::analyzeChildData(bool* data, const XmlNode* node, const CStringBase* nameNode)
{
	VcString strData;
	const XmlNode* nodeFind = analyzeChildData(&strData, node, nameNode);
	if(nodeFind == 0L)	{	return 0L;	}

	*data = StringUtils::toBoolean(&strData);
	return nodeFind;
}

/*---------------------------------------------------------------------*//**
	XML ノードから３要素ベクトルデータを得る

	@return	見つけたノードのポインタを返す
			見つからなかった場合は NULL を返す
**//*---------------------------------------------------------------------*/
const XmlNode* XmlParserUtils::analyzeChildData(Vector3F* data, const XmlNode* node, const CStringBase* nameNode)
{
	VcString strData;
	const XmlNode* nodeFind = analyzeChildData(&strData, node, nameNode);
	if(nodeFind == 0L)	{	return 0L;	}

	toVector3F(data, &strData);
	return nodeFind;
}

/*---------------------------------------------------------------------*//**
	XML ノードから４要素ベクトルデータを得る

	@return	見つけたノードのポインタを返す
			見つからなかった場合は NULL を返す
**//*---------------------------------------------------------------------*/
const XmlNode* XmlParserUtils::analyzeChildData(Vector4F* data, const XmlNode* node, const CStringBase* nameNode)
{
	VcString strData;
	const XmlNode* nodeFind = analyzeChildData(&strData, node, nameNode);
	if(nodeFind == 0L)	{	return 0L;	}

	toVector4F(data, &strData);
	return nodeFind;
}

/*---------------------------------------------------------------------*//**
	XML ノードから文字列属性値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeAtrb(VcString* data, const XmlNode* node, const CStringBase* nameAtrb)
{
	const VcString* val = node->findAttribute(nameAtrb);
	if(val == 0L)	{	return false;	}

	*data = *val;
	return true;
}

/*---------------------------------------------------------------------*//**
	XML ノードから整数属性値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeAtrb(s32* data, const XmlNode* node, const CStringBase* nameAtrb)
{
	VcString val;
	if(!analyzeAtrb(&val, node, nameAtrb))	{	return false;	}

	if(val.getLength() <= 0)				{	return false;	}

	char cFirst = val.getAt(0);
	if( (('0' <= cFirst) && (cFirst <= '9')) || (cFirst == '-') )
	{
		*data = StringUtils::toInteger(&val);
	}
	else
	{
		*data = (s32)StringUtils::toBoolean(&val);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	XML ノードから浮動小数点値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeAtrb(f32* data, const XmlNode* node, const CStringBase* nameAtrb)
{
	VcString val;
	if(!analyzeAtrb(&val, node, nameAtrb))	{	return false;	}

	*data = (f32)StringUtils::toDouble(&val, 0L);
	return true;
}

/*---------------------------------------------------------------------*//**
	XML ノードからブーリアン属性値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeAtrb(bool* data, const XmlNode* node, const CStringBase* nameAtrb)
{
	VcString val;
	if(!analyzeAtrb(&val, node, nameAtrb))	{	return false;	}

	*data = StringUtils::toBoolean(&val);
	return true;
}

/*---------------------------------------------------------------------*//**
	XML ノードから３要素ベクトル属性値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeAtrb(Vector3F* data, const XmlNode* node, const CStringBase* nameAtrb)
{
	VcString val;
	if(!analyzeAtrb(&val, node, nameAtrb))	{	return false;	}

	return toVector3F(data, &val);
}

/*---------------------------------------------------------------------*//**
	XML ノードから４要素ベクトル属性値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeAtrb(Vector4F* data, const XmlNode* node, const CStringBase* nameAtrb)
{
	VcString val;
	if(!analyzeAtrb(&val, node, nameAtrb))	{	return false;	}

	return toVector4F(data, &val);
}

/*---------------------------------------------------------------------*//**
	XML ノードから配列属性値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeAtrb(s32** arrdata, s32* num, const XmlNode* node, const CStringBase* nameAtrb)
{
	VcString val;
	if(!analyzeAtrb(&val, node, nameAtrb))	{	return false;	}

	return toArrayValue(arrdata, num, &val);
}

/*---------------------------------------------------------------------*//**
	XML 子ノードから文字列属性値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeChildAtrb(VcString* data, const XmlNode* node, const CStringBase* nameNode, const CStringBase* nameAtrb)
{
	if(node == 0L)	{	return 0L;	}
	const XmlNode* nodeChild = node->getChildNode();
	while(nodeChild != 0L)
	{
		if(nodeChild->getName()->equals(nameNode))
		{
			return analyzeAtrb(data, nodeChild, nameAtrb);
		}
		nodeChild = nodeChild->getSiblingNode();
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	XML 子ノードから整数属性値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeChildAtrb(s32* data, const XmlNode* node, const CStringBase* nameNode, const CStringBase* nameAtrb)
{
	if(node == 0L)	{	return 0L;	}
	const XmlNode* nodeChild = node->getChildNode();
	while(nodeChild != 0L)
	{
		if(nodeChild->getName()->equals(nameNode))
		{
			return analyzeAtrb(data, nodeChild, nameAtrb);
		}
		nodeChild = nodeChild->getSiblingNode();
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	XML 子ノードから整数属性値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeChildAtrb(f32* data, const XmlNode* node, const CStringBase* nameNode, const CStringBase* nameAtrb)
{
	if(node == 0L)	{	return 0L;	}
	const XmlNode* nodeChild = node->getChildNode();
	while(nodeChild != 0L)
	{
		if(nodeChild->getName()->equals(nameNode))
		{
			return analyzeAtrb(data, nodeChild, nameAtrb);
		}
		nodeChild = nodeChild->getSiblingNode();
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	XML 子ノードからブーリアン属性値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeChildAtrb(bool* data, const XmlNode* node, const CStringBase* nameNode, const CStringBase* nameAtrb)
{
	if(node == 0L)	{	return 0L;	}
	const XmlNode* nodeChild = node->getChildNode();
	while(nodeChild != 0L)
	{
		if(nodeChild->getName()->equals(nameNode))
		{
			return analyzeAtrb(data, nodeChild, nameAtrb);
		}
		nodeChild = nodeChild->getSiblingNode();
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	XML 子ノードから３要素ベクトル属性値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeChildAtrb(Vector3F* data, const XmlNode* node, const CStringBase* nameNode, const CStringBase* nameAtrb)
{
	if(node == 0L)	{	return 0L;	}
	const XmlNode* nodeChild = node->getChildNode();
	while(nodeChild != 0L)
	{
		if(nodeChild->getName()->equals(nameNode))
		{
			return analyzeAtrb(data, nodeChild, nameAtrb);
		}
		nodeChild = nodeChild->getSiblingNode();
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	XML 子ノードから４要素ベクトル属性値を得る

	@retval true	見つけた場合
	@retval false	見つからなかった場合
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::analyzeChildAtrb(Vector4F* data, const XmlNode* node, const CStringBase* nameNode, const CStringBase* nameAtrb)
{
	if(node == 0L)	{	return 0L;	}
	const XmlNode* nodeChild = node->getChildNode();
	while(nodeChild != 0L)
	{
		if(nodeChild->getName()->equals(nameNode))
		{
			return analyzeAtrb(data, nodeChild, nameAtrb);
		}
		nodeChild = nodeChild->getSiblingNode();
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	XML ノードから指定名の子ノードを探す

	@return 子ノードのポインタ．見つからなかった場合は NULL
**//*---------------------------------------------------------------------*/
const XmlNode* XmlParserUtils::findChild(const XmlNode* node, const CStringBase* nameNode)
{
	if(node == 0L)	{	return 0L;	}
	const XmlNode* nodeChild = node->getChildNode();
	while(nodeChild != 0L)
	{
		if(nodeChild->getName()->equals(nameNode))
		{
			return nodeChild;
		}
		nodeChild = nodeChild->getSiblingNode();
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	文字列から３要素ベクトルデータへ変換
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::toVector3F(Vector3F* v, const VcString* str)
{
	int idxY = str->find(',', 0);
	if(idxY < 0)	{	return false;	}
	idxY++;
	int idxZ = str->find(',', idxY);
	if(idxZ < 0)	{	return false;	}
	idxZ++;

	VcString strX = str->substring(0, idxY - 1);
	VcString strY = str->substring(idxY, idxZ - 1);
	VcString strZ = str->substring(idxZ);
	StringUtils::trim(&strX);
	StringUtils::trim(&strY);
	StringUtils::trim(&strZ);
	v->x() = (f32)StringUtils::toDouble(&strX, 0L);
	v->y() = (f32)StringUtils::toDouble(&strY, 0L);
	v->z() = (f32)StringUtils::toDouble(&strZ, 0L);

	return true;
}

/*---------------------------------------------------------------------*//**
	文字列から４要素ベクトルデータへ変換
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::toVector4F(Vector4F* v, const VcString* str)
{
	int idxY = str->find(',', 0);
	if(idxY < 0)	{	return false;	}
	idxY++;
	int idxZ = str->find(',', idxY);
	if(idxZ < 0)	{	return false;	}
	idxZ++;
	int idxW = str->find(',', idxZ);
	if(idxW < 0)	{	return false;	}
	idxW++;

	VcString strX = str->substring(0, idxY - 1);
	VcString strY = str->substring(idxY, idxZ - 1);
	VcString strZ = str->substring(idxZ, idxW - 1);
	VcString strW = str->substring(idxW);
	StringUtils::trim(&strX);
	StringUtils::trim(&strY);
	StringUtils::trim(&strZ);
	StringUtils::trim(&strW);
	v->x() = (f32)StringUtils::toDouble(&strX, 0L);
	v->y() = (f32)StringUtils::toDouble(&strY, 0L);
	v->z() = (f32)StringUtils::toDouble(&strZ, 0L);
	v->w() = (f32)StringUtils::toDouble(&strW, 0L);

	return true;
}

/*---------------------------------------------------------------------*//**
	文字列から配列データ値へ変換
**//*---------------------------------------------------------------------*/
bool XmlParserUtils::toArrayValue(s32** arr, s32* num, const VcString* str)
{
	InsList<s32> listWk;

	int sidx1 = 0;
	while(true)
	{
		int sidx2 = str->find(',', sidx1);

		VcString strWk;
		if(sidx2 < 0)
		{
			strWk = str->substring(sidx1);
		}
		else
		{
			strWk = str->substring(sidx1, sidx2);
		}
		StringUtils::trim(&strWk);

		s32 val = (s32)StringUtils::toInteger(&strWk);
		listWk.addTail(&val);

		if(sidx2 < 0)	{	break;	}
		sidx1 = sidx2 + 1;
	}

	*num = listWk.getCount();
	if(num == 0L)
	{
		arr = 0L;
	}
	else
	{
		*arr = new s32[*num];
		int aidx = 0;
		for(InsListIterator<s32> it = listWk.iterator(); it.has(); it.next())
		{
			(*arr)[aidx++] = *it.object();
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS