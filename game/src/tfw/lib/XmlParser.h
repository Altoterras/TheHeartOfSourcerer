/***********************************************************************//**
 *  XmlParser.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/20.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_LIB_XML_PARSER_H_
#define _TFW_LIB_XML_PARSER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../string/CcString.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

class CStringBase;
class VcString;

class XmlNode;
class XmlAttribute;

/*---------------------------------------------------------------------*//**
 *	XML パーサー
 *
**//*---------------------------------------------------------------------*/
class XmlParser
{
	//======================================================================
	// 定数

public:
	static const int XMLNODETYPE_TAG	= 0;
	static const int XMLNODETYPE_PCDATA	= 1;

	//======================================================================
	// アクセサ
public:
	// ルートノードの取得
	const XmlNode* getRootNode() const { return _pnodeRoot; }

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス

	const XmlNode* parseXmlDocument(const CStringBase* strXmlData);
	inline const XmlNode* parseXmlDocument(CcString strXmlData) { return parseXmlDocument(&strXmlData); }
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	XmlParser();
	~XmlParser();

	static void removeNonAsciiUtf8FromBuffer(char* dst, const char* src);
	static void printXmlDocument(const XmlNode* node, int indt = 0);	// デバッグ

private:
	void reset();
	const char* skipWhiteSpace(const char* p) const;
	const char* skipXmlName(const char* p) const;
	const char* skipXmlComment(const char* p) const;
	const char* skipXmlDocType(const char* p) const;
	const char* skipXmlProlog(const char* p) const;
	const char* skipPcData(const char* p) const;
	int isPcData(const char* p) const;
	const char* skipFilling();
	void convertBuiltInEntities(char* p) const;
	void setToken(int type, const char* start, const char* end);
	void getNextToken(void);
	XmlNode* parseElement();
	XmlAttribute* parseAttribute();

	// デバッグ
	static void printAttributes(const XmlAttribute* attr);
	static void indent(int indt);

	//======================================================================
	// メンバ変数
private:
	const char* _posCur;
	char* _tokennameCur;
	int _tokentypeCur;
	int _tokensizeMax;
	XmlNode* _pnodeRoot;
};

/*---------------------------------------------------------------------*//**
	XML ノード

**//*---------------------------------------------------------------------*/
class XmlNode
{
public:
	int getType() const { return _type; }
	const VcString* getName() const { return _name; }
	const VcString* getChildPcdata() const;
	const XmlNode* getChildNode() const { return _child; }
	const XmlNode* getSiblingNode() const { return _sibling; }
	const XmlAttribute* getFirstAttribute() const { return _atrbs; }

public:
	const XmlNode* findChildNode(const CStringBase* name) const;
	inline const XmlNode* findChildNode(CcString name) const { return findChildNode(&name); }
	const XmlNode* findSiblingNode(const CStringBase* name) const;
	inline const XmlNode* findSiblingNode(CcString name) const { return findSiblingNode(&name); }
	const XmlNode* findNodeDive(const CStringBase* name) const;
	inline const XmlNode* findNodeDive(CcString name) const { return findNodeDive(&name); }
	const VcString* findAttribute(const CStringBase* name) const;
	inline const VcString* findAttribute(CcString name) const { return findAttribute(&name); }

public:
	XmlNode(int type, const char* name);
	~XmlNode();
	void setChild(XmlNode* child) { _child = child; }
	void setSibling(XmlNode* sibling) { _sibling = sibling; }
	void addAttribute(XmlAttribute* atrb);

private:
	int _type;				// ノードタイプ（タグ，PDDATA, CDATA)
	VcString* _name;		// 名前
	XmlNode* _child;		// 子ノードの先頭ノード
	XmlNode* _sibling;		// 兄弟ノードの先頭ノード
	XmlAttribute* _atrbs;	// 属性リスト
};

/*---------------------------------------------------------------------*//**
	XML 属性

**//*---------------------------------------------------------------------*/
class XmlAttribute
{
public:
	const VcString* getName() const { return _name; }
	const VcString* getValue() const { return _value; }
	const XmlAttribute* getNextAttribute() const { return _next; }

public:
	const VcString* findAttribute(const CStringBase* name) const;
	inline const VcString* findAttribute(CcString name) const { return findAttribute(&name); }

public:
	XmlAttribute(const char* name, const char* value);
	~XmlAttribute();
	void setNext(XmlAttribute* atrb) { _next = atrb; }

private:
	VcString* _name;		// 名前
	VcString* _value;		// 値
	XmlAttribute* _next;	// 次の属性
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_LIB_XML_PARSER_H_