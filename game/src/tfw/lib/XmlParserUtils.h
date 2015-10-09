/***********************************************************************//**
 *  XmlParserUtils.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2010/02/10.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_LIB_XML_PARSER_UTILS_H_
#define _TFW_LIB_XML_PARSER_UTILS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../string/CcString.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
template<class TYPE> class Vector4;
typedef Vector4<f32> Vector4F;
class XmlNode;
class XmlAttribute;
class CStringBase;
class VcString;

/*---------------------------------------------------------------------*//**
 *	XML ユーティリティ
 *
**//*---------------------------------------------------------------------*/
class XmlParserUtils
{
	//======================================================================
	// メソッド
public:
	static bool						analyzeData(VcString* data, const XmlNode* node);
	static bool						analyzeData(s32* data, const XmlNode* node);
	static bool						analyzeData(f32* data, const XmlNode* node);
	static bool						analyzeData(bool* data, const XmlNode* node);
	static bool						analyzeData(Vector3F* data, const XmlNode* node);
	static bool						analyzeData(Vector4F* data, const XmlNode* node);
	static const XmlNode*			analyzeChildData(VcString* data, const XmlNode* node, const CStringBase* nameNode);
	inline static const XmlNode*	analyzeChildData(VcString* data, const XmlNode* node, CcString nameNode) { return analyzeChildData(data, node, &nameNode); }
	static const XmlNode*			analyzeChildData(s32* data, const XmlNode* node, const CStringBase* nameNode);
	inline static const XmlNode*	analyzeChildData(s32* data, const XmlNode* node, CcString nameNode) { return analyzeChildData(data, node, &nameNode); }
	static const XmlNode*			analyzeChildData(f32* data, const XmlNode* node, const CStringBase* nameNode);
	inline static const XmlNode*	analyzeChildData(f32* data, const XmlNode* node, CcString nameNode) { return analyzeChildData(data, node, &nameNode); }
	static const XmlNode*			analyzeChildData(bool* data, const XmlNode* node, const CStringBase* nameNode);
	inline static const XmlNode*	analyzeChildData(bool* data, const XmlNode* node, CcString nameNode) { return analyzeChildData(data, node, &nameNode); }
	static const XmlNode*			analyzeChildData(Vector3F* data, const XmlNode* node, const CStringBase* nameNode);
	inline static const XmlNode*	analyzeChildData(Vector3F* data, const XmlNode* node, CcString nameNode) { return analyzeChildData(data, node, &nameNode); }
	static const XmlNode*			analyzeChildData(Vector4F* data, const XmlNode* node, const CStringBase* nameNode);
	inline static const XmlNode*	analyzeChildData(Vector4F* data, const XmlNode* node, CcString nameNode) { return analyzeChildData(data, node, &nameNode); }
	static bool						analyzeAtrb(VcString* data, const XmlNode* node, const CStringBase* nameAtrb);
	inline static bool				analyzeAtrb(VcString* data, const XmlNode* node, CcString nameAtrb) { return analyzeAtrb(data, node, &nameAtrb); }
	static bool						analyzeAtrb(s32* data, const XmlNode* node, const CStringBase* nameAtrb);
	inline static bool				analyzeAtrb(s32* data, const XmlNode* node, CcString nameAtrb) { return analyzeAtrb(data, node, &nameAtrb); }
	static bool						analyzeAtrb(f32* data, const XmlNode* node, const CStringBase* nameAtrb);
	inline static bool				analyzeAtrb(f32* data, const XmlNode* node, CcString nameAtrb) { return analyzeAtrb(data, node, &nameAtrb); }
	static bool						analyzeAtrb(bool* data, const XmlNode* node, const CStringBase* nameAtrb);
	inline static bool				analyzeAtrb(bool* data, const XmlNode* node, CcString nameAtrb) { return analyzeAtrb(data, node, &nameAtrb); }
	static bool						analyzeAtrb(Vector3F* data, const XmlNode* node, const CStringBase* nameAtrb);
	inline static bool				analyzeAtrb(Vector3F* data, const XmlNode* node, CcString nameAtrb) { return analyzeAtrb(data, node, &nameAtrb); }
	static bool						analyzeAtrb(Vector4F* data, const XmlNode* node, const CStringBase* nameAtrb);
	inline static bool				analyzeAtrb(Vector4F* data, const XmlNode* node, CcString nameAtrb) { return analyzeAtrb(data, node, &nameAtrb); }
	static bool						analyzeAtrb(s32** arrdata, s32* num, const XmlNode* node, const CStringBase* nameAtrb);
	inline static bool				analyzeAtrb(s32** arrdata, s32* num, const XmlNode* node, CcString nameAtrb) { return analyzeAtrb(arrdata, num, node, &nameAtrb); }
	static bool						analyzeChildAtrb(VcString* data, const XmlNode* node, const CStringBase* nameNode, const CStringBase* nameAtrb);
	inline static bool				analyzeChildAtrb(VcString* data, const XmlNode* node, CcString nameNode, CcString nameAtrb) { return analyzeChildAtrb(data, node, &nameNode, &nameAtrb); }
	static bool						analyzeChildAtrb(s32* data, const XmlNode* node, const CStringBase* nameNode, const CStringBase* nameAtrb);
	inline static bool				analyzeChildAtrb(s32* data, const XmlNode* node, CcString nameNode, CcString nameAtrb) { return analyzeChildAtrb(data, node, &nameNode, &nameAtrb); }
	static bool						analyzeChildAtrb(f32* data, const XmlNode* node, const CStringBase* nameNode, const CStringBase* nameAtrb);
	inline static bool				analyzeChildAtrb(f32* data, const XmlNode* node, CcString nameNode, CcString nameAtrb) { return analyzeChildAtrb(data, node, &nameNode, &nameAtrb); }
	static bool						analyzeChildAtrb(bool* data, const XmlNode* node, const CStringBase* nameNode, const CStringBase* nameAtrb);
	inline static bool				analyzeChildAtrb(bool* data, const XmlNode* node, CcString nameNode, CcString nameAtrb) { return analyzeChildAtrb(data, node, &nameNode, &nameAtrb); }
	static bool						analyzeChildAtrb(Vector3F* data, const XmlNode* node, const CStringBase* nameNode, const CStringBase* nameAtrb);
	inline static bool				analyzeChildAtrb(Vector3F* data, const XmlNode* node, CcString nameNode, CcString nameAtrb) { return analyzeChildAtrb(data, node, &nameNode, &nameAtrb); }
	static bool						analyzeChildAtrb(Vector4F* data, const XmlNode* node, const CStringBase* nameNode, const CStringBase* nameAtrb);
	inline static bool				analyzeChildAtrb(Vector4F* data, const XmlNode* node, CcString nameNode, CcString nameAtrb) { return analyzeChildAtrb(data, node, &nameNode, &nameAtrb); }
	static const XmlNode*			findChild(const XmlNode* node, const CStringBase* nameNode);
	inline static const XmlNode*	findChild(const XmlNode* node, CcString nameNode) { return findChild(node, &nameNode); }

private:
	static bool						toVector3F(Vector3F* v, const VcString* str);
	static bool						toVector4F(Vector4F* v, const VcString* str);
	static bool						toArrayValue(s32** arr, s32* num, const VcString* str);
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_LIB_XML_PARSER_UTILS_H_