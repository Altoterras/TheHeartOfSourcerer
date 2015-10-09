/***********************************************************************//**
 *  XmlParser.cpp
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
#include "XmlParser.h"

// Friends
#include "../string/VcString.h"

// External

// Library
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <wctype.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define TOKEN_UNKNOWN				0
#define TOKEN_TAG_BEGIN				1	// <tag
#define TOKEN_TAG_END				2	// </tag
#define TOKEN_BRACKET_CLOSE			3	// >
#define TOKEN_BRACKET_CLOSE_EMPTY	4	// />
#define TOKEN_PCDATA				5	// PCDATA
#define TOKEN_CDATA					6	// CDATA
#define TOKEN_EOF					7

#define CDATA_START					"<![CDATA["
#define CDATA_END					"]]>"

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// XmlParser メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	XML 解析
**//*---------------------------------------------------------------------*/
const XmlNode* XmlParser::parseXmlDocument(const CStringBase* strXmlData)
{
	// 初期化
	reset();

	///// バッファから UTF8 エンコーディングを削除する
	///removeNonAsciiUtf8FromBuffer(buf);

	// 最初のトークンを得る
	_posCur = strXmlData->getRaw();
	getNextToken();
    
	// 解析する
	_pnodeRoot = parseElement();

	// 解析用トークン名を削除する
	delete[] _tokennameCur;

	return _pnodeRoot;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
XmlParser::XmlParser() :
	_posCur(0L),
	_tokennameCur(0L),
	_tokensizeMax(-1),
	_pnodeRoot(0L)
{
	
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
XmlParser::~XmlParser()
{
	delete _pnodeRoot;
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
void XmlParser::reset()
{
	delete _pnodeRoot;

	_posCur = 0L;
	_tokennameCur = 0L;
	_tokensizeMax = -1;
	_pnodeRoot = 0L;
}

/*---------------------------------------------------------------------*//**
	要素を解析
**//*---------------------------------------------------------------------*/
XmlNode* XmlParser::parseElement()
{
	XmlNode* node = 0L;
	XmlAttribute* attr = 0L;
    
	if(_tokentypeCur == TOKEN_TAG_BEGIN)	// <
	{
		// 新しいタグノードを作成
		node = new XmlNode(XMLNODETYPE_TAG, _tokennameCur);
    
		// 属性を解析（EOF, >, /> が来るまで）
		attr = parseAttribute();
		while(attr != 0L)
		{
			node->addAttribute(attr);
			attr = parseAttribute();
		}

		// EOF, >, /> などで終わるはず
		getNextToken();
		ASSERT(_tokentypeCur == TOKEN_BRACKET_CLOSE_EMPTY ||  _tokentypeCur == TOKEN_BRACKET_CLOSE ||  _tokentypeCur  == TOKEN_EOF);
        
		if(_tokentypeCur == TOKEN_BRACKET_CLOSE_EMPTY)	// />
		{
			getNextToken();
		}
		else if(_tokentypeCur == TOKEN_BRACKET_CLOSE)	// >
		{
			getNextToken();
            
			// 終了タグが見つかるまで解析
			node->setChild( parseElement() );
            
			if(_tokentypeCur == TOKEN_TAG_END)	// </
			{
				// 終了タグ </ が見つかった
				do { getNextToken(); } while((_tokentypeCur != TOKEN_EOF) && (_tokentypeCur != TOKEN_BRACKET_CLOSE));
				getNextToken();
			}  
		}
	}
	else if(_tokentypeCur == TOKEN_PCDATA)	// PCDATA 開始
	{
		// PC データノードを作成
		node = new XmlNode(XMLNODETYPE_PCDATA, _tokennameCur);
		getNextToken();
	}
	else if(_tokentypeCur == TOKEN_TAG_END)	// </
	{
		// 兄弟パート解析終了
		return 0L;
	}
	else
	{
		// 不正か？
		ASSERT(false);
		return 0L;
	}
    
	// 兄弟パートを解析
	if(_tokentypeCur != TOKEN_EOF)
	{
		node->setSibling( parseElement() );
	}
	return node;
}

/*---------------------------------------------------------------------*//**
	属性を解析
**//*---------------------------------------------------------------------*/
XmlAttribute* XmlParser::parseAttribute()
{
	XmlAttribute* attr = 0L;
	const char* q;
	char quoteChar;

	// 空白をスキップ
	skipFilling();

	// 処理終了判定
	if((_posCur[0] == '\0') || (_posCur[0] == '>') || ((_posCur[0] == '/') && (_posCur[1] == '>'))) return 0L;

	// 末端を見つける
	q = _posCur;
	while(*q && !::iswspace(*q) && (*q !='=')) { q++; }

	setToken(TOKEN_UNKNOWN, _posCur, q);
	VcString name(_tokennameCur);

	// 空白をスキップ
	_posCur = q;
	_posCur = skipFilling();
       
	// 次は '=' が来るはずだが、来なかった場合（本来はエラーだが）は無視して処理を続行する
	if(*_posCur != '=')
	{
		return parseAttribute();
	}   
        
	_posCur++;
	_posCur = skipWhiteSpace(_posCur);
	// 属性値の CDATA 部の解析
	if((*_posCur == '\"') || (*_posCur == '\''))
	{
		quoteChar = *_posCur;
		q = ++_posCur;
		while((*q != '\0') && (*q != quoteChar)) { q++; }
		setToken(TOKEN_CDATA, _posCur, q);
		_posCur = q + 1;
	}
	else
	{
		q = _posCur;
		while((*q != '\0') && !isspace(*q)) { q++; }
		setToken(TOKEN_CDATA, _posCur, q);
		_posCur = q;
	}

	attr  = new XmlAttribute(name, _tokennameCur);
    
	return attr;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 解析ユーティリティ

/*---------------------------------------------------------------------*//**
	空白をスキップ
**//*---------------------------------------------------------------------*/
const char* XmlParser::skipWhiteSpace(const char* p) const
{
	if(p == 0L)	{	return 0L;	}
	while((*p == 0x20) || (*p == 0x09) || (*p == 0x0d) || (*p == 0x0a)) { p++; }
	return p;
}

/*---------------------------------------------------------------------*//**
	名前をスキップ
**//*---------------------------------------------------------------------*/
const char* XmlParser::skipXmlName(const char* p) const
{
	char c = *p;
	
	if((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (c == '_') || (c == ':'))	//トークンの開始チェック
	{
		while( (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (('0' <= c) && (c <= '9')) || (c == '_') || (c == ':') || (c == '.') || (c == '-') )
		{
			p++;
			c = *p;
			if(c == '\0') { break; }
		}        
	}
	return p;
}

/*---------------------------------------------------------------------*//**
	コメントをスキップ
**//*---------------------------------------------------------------------*/
const char* XmlParser::skipXmlComment(const char* p) const
{
	if(p == 0L)	{	return 0L;	}

	if(::strncmp(p, "<!--", 4) == 0)
	{
		p += 4;
		do
		{
			if(strncmp(p, "-->", 3) == 0) { return p + 3; }
			p++;
		}
		while(*p != '\0');
	}
	return p;
}

/*---------------------------------------------------------------------*//**
	DOCTYPE をスキップ
**//*---------------------------------------------------------------------*/
const char* XmlParser::skipXmlDocType(const char *p) const
{
	if(p == 0L)	{	return 0L;	}

	if(::strncmp(p, "<!", 2) == 0)
	{
		p += 2;
		while((*p != '\0') && (*p != '>')) { p++; }
		if(*p == '>') { p++; }
	}
	return p;
}

/*---------------------------------------------------------------------*//**
	<? ～ ?> をスキップ
**//*---------------------------------------------------------------------*/
const char* XmlParser::skipXmlProlog(const char *p) const
{
	if(p == 0L)	{	return 0L;	}

	if(::strncmp(p, "<?", 2) == 0)
	{
		p += 2;
		do
		{
			if(::strncmp(p, "?>", 2) == 0) { return p + 2; }
			p++;
		} 
		while(*p != '\0');
	}
	return p;
}

/*---------------------------------------------------------------------*//**
	PCDATA をスキップ
**//*---------------------------------------------------------------------*/
const char* XmlParser::skipPcData(const char *p) const
{
	const char* end = ::strstr(p, CDATA_END);
	if(end != 0L) { return end + sizeof(CDATA_END); }
	return (++p);
}

/*---------------------------------------------------------------------*//**
	PCDATA かを判定する
**//*---------------------------------------------------------------------*/
int XmlParser::isPcData(const char *p) const
{
	return (::strncmp(CDATA_START, p, sizeof(CDATA_START)) == 0);
}

/*---------------------------------------------------------------------*//**
	空白、コメント、DOCTYPE などをスキップする
**//*---------------------------------------------------------------------*/
const char* XmlParser::skipFilling()
{
	const char* q = 0L;

	do
	{
		q = _posCur;        
		_posCur = skipWhiteSpace(_posCur);
		_posCur = skipXmlComment(_posCur);	// DOCTYPE の前に呼ぶ必要がある
		_posCur = skipXmlDocType(_posCur);	// <! ～ >
		_posCur = skipXmlProlog(_posCur);	// <? ～ ?>
	}
	while(_posCur != q);

	return _posCur;
}

/*---------------------------------------------------------------------*//**
	ビルトインエンティティの変換
	&amp; → &, &apos; → ', &lt; → <, &gt; → >, &quote → "
**//*---------------------------------------------------------------------*/
void XmlParser::convertBuiltInEntities(char* p) const
{
	char* q;
	q = p;
	while(*p)
	{
		if(isPcData(p))
		{
			const char* end;
			end = skipPcData(p);
			while(p < end) { *q++ = *p++; }
		}
		else
		{
			if(::strncmp(p, "&amp;", 5) == 0)
			{
				*q = '&';
				q++;
				p += 5;
			}
			else if(::strncmp(p, "&lt;", 4)  == 0)
			{
				*q = '<';
				p += 4;
			}
			else if(::strncmp(p, "&gt;", 4)  == 0)
			{
				*q = '>';
				p += 4;
			}
			else if(::strncmp(p, "&apos;", 6)  == 0)
			{
				*q = '\'';
				p += 6;
			}
			else if(::strncmp(p, "&quote;", 7)  == 0)
			{
				*q = '\"';
				p += 7;
			}
			else
			{
				*q++ = *p++;
			}
		}
	}
	*q = '\0';
}

/*---------------------------------------------------------------------*//**
	トークンの値をバッファからコピーする
**//*---------------------------------------------------------------------*/
void XmlParser::setToken(int type, const char* start, const char* end)
{
	int len = (int)(end - start);
	if (len > _tokensizeMax)
	{
		if(_tokennameCur != 0L)
		{
			delete[] _tokennameCur;
		}
		_tokennameCur = new char[len + 1];
		_tokensizeMax = len;
	}
    
	_tokentypeCur = type;
	#if defined(_WINDOWS)
		::strncpy_s(_tokennameCur, len + 1, start, len);
	#else
		::strncpy(_tokennameCur, start, len);
	#endif
	_tokennameCur[len] = '\0';    
}

/*---------------------------------------------------------------------*//**
	次のトークンを得る
	<!-- ～ --> や <! ～ > は自動的にスキップする
**//*---------------------------------------------------------------------*/
void XmlParser::getNextToken()
{
	const char* p;
	const char* q;

	// 空白やコメントをスキップ
	p = skipFilling();

	if ((p == 0L) || (*p == '\0'))
	{
		_tokentypeCur = TOKEN_EOF;
		return;
	}
	else if((p[0] == '<') && (p[1] == '/'))
	{
		// 終了タグ
		q = skipXmlName(p + 2);        
		setToken(TOKEN_TAG_END, p + 2, q);        
		p = q;
	}
	else  if (*p == '<')
	{
		// 開始タグ
		q = skipXmlName(p + 1);        
		setToken(TOKEN_TAG_BEGIN, p + 1, q);
		p = q;
	}
	else if (p[0] == '>')
	{
		// > 終了
		_tokentypeCur = TOKEN_BRACKET_CLOSE;                
		p++;
	}
	else if (p[0] == '/' && p[1] == '>')
	{
		// /> での終了
		_tokentypeCur = TOKEN_BRACKET_CLOSE_EMPTY;        
		p += 2;
	}
	else
	{
		// 内容データの終了までを走査
		q = p + 1;
		while(*q && (*q != '<'))
		{
			if(isPcData(q))	{ q = skipPcData(q); }
			else { q++; }
		}
		setToken(TOKEN_PCDATA, p, q);
		// トークン内のエンティティの変換
		convertBuiltInEntities(_tokennameCur);
		p = q;
	}

	_posCur = p;	// 次のトークン位置までポインタを移動させておく
}

#if 0
/*---------------------------------------------------------------------*//**
	名前で同レベルの要素を検索
**//*---------------------------------------------------------------------*/
const XmlNode* XmlParser::findNextNode(const XmlNode* pnode, const CStringBase* name)
{
	if(pnode->_next == 0L) { return 0L; }
	pnode = pnode->_next;
	if((pnode->_type == XMLNODETYPE_TAG) && pnode->_name->equals(name))
	{
		return pnode;
	} 

	return findNextNode(pnode, name);
}

/*---------------------------------------------------------------------*//**
	名前で要素を検索
**//*---------------------------------------------------------------------*/
const XmlNode* XmlParser::findNodeDive(const XmlNode* pnode, const CStringBase* name)
{
	if(pnode == 0L) { return 0L; }
	if((pnode->_type == XMLNODETYPE_TAG) && pnode->_name->equals(name))
	{
		return pnode;
	} 

	if(pnode->_child != 0L)
	{
		const XmlNode* pnodeRet = findNodeDive(pnode->_child, name);
		if(pnodeRet != 0L)	{	return pnodeRet;	}
	}
	if(pnode->_next != 0L)
	{
		const XmlNode* pnodeRet = findNodeDive(pnode->_next, name);
		if(pnodeRet != 0L)	{	return pnodeRet;	}
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	属性を名前で検索して値を返す
	見つからない場合は 0L を返す
**//*---------------------------------------------------------------------*/
const VcString* XmlParser::findAttribute(const XmlAttribute* attr, const CStringBase* name)
{
	if(attr == 0L) { return 0L; }
	if(attr->_name->equals(name)) { return attr->_value; }
	return findAttribute(attr->_next, name);
}
#endif

/*---------------------------------------------------------------------*//**
	非 ASCII の UTF8 エンコーディングの削除
	
	UTF-8 encoding:
	
	 範囲 1:
			0x0000 - 0x007F 
				0 | bits 0 - 7
	 範囲 2:
			0x0080 - 0x07FF  : 
				110 | bits 6 - 10
				10  | bits 0 - 5
	 範囲 3:
			0x0800 - 0xFFFF  :        
				1110 | bits 12-15
				10   | bits  6-11
				10   | bits  0-5
**//*---------------------------------------------------------------------*/
void XmlParser::removeNonAsciiUtf8FromBuffer(char* dst, const char* src)
{
	char c;
	while(*src != '\0')
	{
		c = *src;
		if( (c & 0x80) == 0)
		{
			// 範囲 1
			*dst++ = *src++;
		}
		else if((c & 0xE0) == 0xC0)
		{
			// 範囲 2
			*dst++ = (char)0xFF;
			src++;
			if(*src) { src++; }
		}
		else
		{
			// 範囲 3
			*dst++ = (char)0xFF;
			src++;
			if(*src) { src++; }
			if(*src) { src++; }
		}        
	}
	
	*dst = '\0';    // 0L 終端
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// デバッグ

/*---------------------------------------------------------------------*//**
	デバッグ表示
**//*---------------------------------------------------------------------*/
void XmlParser::printXmlDocument(const XmlNode* node, int indt)
{
	if(node == 0L) { return; }

	if(node->getType() == XMLNODETYPE_TAG)
	{
		::printf("\n");
		indent(indt);
		::printf("<%s", node->getName()->getRaw());
		printAttributes(node->getFirstAttribute());

		if(node->getChildNode() == 0L)
		{
			::printf("/>\n");
		}
		else
		{
			::printf(">");
			printXmlDocument(node->getChildNode(), indt + 1);
			indent(indt);
			::printf("</%s>", node->getName()->getRaw());       
		}
	}
	else
	{
		::printf("%s", node->getName()->getRaw());
	}
	printXmlDocument(node->getSiblingNode(), indt);
}

/*---------------------------------------------------------------------*//**
	デバッグ表示（属性）
**//*---------------------------------------------------------------------*/
void XmlParser::printAttributes(const XmlAttribute* attr)
{
	if(attr == 0L) { return; }

	::printf(" %s=\"%s\"", attr->getName()->getRaw(), (attr->getValue() != 0L) ? attr->getValue()->getRaw() : "");
	printAttributes(attr->getNextAttribute());
}

/*---------------------------------------------------------------------*//**
	デバッグ表示（インデント）
**//*---------------------------------------------------------------------*/
void XmlParser::indent(int indt)
{
	int i;  
	for(i = 0; i < indt; i++) { ::printf("  "); }
}

//==========================================================================
// XmlNode メソッド

/*---------------------------------------------------------------------*//**
	子ノードの PCDATA を得る
**//*---------------------------------------------------------------------*/
const VcString* XmlNode::getChildPcdata() const
{
	if(_child == 0L)									{	return 0L;	}
	if(_child->_type != XmlParser::XMLNODETYPE_PCDATA)	{	return 0L;	}
	return _child->_name;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
XmlNode::XmlNode(int type, const char* name) :
	_type(type),
	_name(new VcString(name)),
	_sibling(0L),
	_child(0L),
	_atrbs(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
XmlNode::~XmlNode()
{
	// 再帰的削除
	delete _child;
	delete _sibling;
	delete _atrbs;

	// 自らのメンバ削除
	delete _name;
}

/*---------------------------------------------------------------------*//**
	属性を追加
**//*---------------------------------------------------------------------*/
void XmlNode::addAttribute(XmlAttribute* atrb)
{
	atrb->setNext(_atrbs);
	_atrbs = atrb;
}

/*---------------------------------------------------------------------*//**
	子ノードを検索
**//*---------------------------------------------------------------------*/
const XmlNode* XmlNode::findChildNode(const CStringBase* name) const
{
	if(_child == 0L) { return 0L; }
	if((_child->_type == XmlParser::XMLNODETYPE_TAG) && _child->_name->equals(name))
	{
		return _child;
	}

	return _child->findSiblingNode(name);
}

/*---------------------------------------------------------------------*//**
	兄弟ノードを検索
**//*---------------------------------------------------------------------*/
const XmlNode* XmlNode::findSiblingNode(const CStringBase* name) const
{
	if(_sibling == 0L) { return 0L; }
	if((_sibling->_type == XmlParser::XMLNODETYPE_TAG) && _sibling->_name->equals(name))
	{
		return _sibling;
	} 

	return _sibling->findSiblingNode(name);
}

/*---------------------------------------------------------------------*//**
	ノードを検索（子、兄弟共に下層までを検索対象とする）
**//*---------------------------------------------------------------------*/
const XmlNode* XmlNode::findNodeDive(const CStringBase* name) const
{
	if((_type == XmlParser::XMLNODETYPE_TAG) && _name->equals(name))
	{
		return this;
	} 

	if(_child != 0L)
	{
		const XmlNode* pnodeRet = _child->findNodeDive(name);
		if(pnodeRet != 0L)	{	return pnodeRet;	}
	}
	if(_sibling != 0L)
	{
		const XmlNode* pnodeRet = _sibling->findNodeDive(name);
		if(pnodeRet != 0L)	{	return pnodeRet;	}
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	属性を検索
**//*---------------------------------------------------------------------*/
const VcString* XmlNode::findAttribute(const CStringBase* name) const
{
	if(_atrbs == 0L)	{	return 0L;	}
	return _atrbs->findAttribute(name);
}

//==========================================================================
// XmlAttribut メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
XmlAttribute::XmlAttribute(const char* name, const char* val) :
	_name(new VcString(name)),
	_value((val != 0) ? new VcString(val) : 0L),
	_next(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
XmlAttribute::~XmlAttribute()
{
	// 再帰的削除
	delete _next;

	// 自らのメンバ削除
	delete _name;
	delete _value;
}

/*---------------------------------------------------------------------*//**
	属性を検索
**//*---------------------------------------------------------------------*/
const VcString* XmlAttribute::findAttribute(const CStringBase* name) const
{
	const XmlAttribute* atrb = this;
	while(atrb != 0L)
	{
		if(atrb->_name->equals(name))
		{
			return atrb->_value;
		}
		atrb = atrb->_next;
	}
	return 0L;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS