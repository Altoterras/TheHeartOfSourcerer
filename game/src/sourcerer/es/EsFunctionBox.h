/***********************************************************************//**
 *	EsFunctionBox.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2010/05/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_FUNCTION_BOX_H_
#define _ES_ES_FUNCTION_BOX_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsObjectBox.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class InsArray;
template<class TYPE> class List;

TFW_END_NS

ES_BEGIN_NS

class EsAtom;
class EsBytecodeBuilder;
class EsClass;
class EsNameItNode;
class EsScope;
class EsScopeProperty;
class EsFunction;
class EsObjectOp;
class EsValue;

/*---------------------------------------------------------------------*//**
 *	ES 関数ボックス ⇒ JSFunctionBox
 *
**//*---------------------------------------------------------------------*/
class EsFunctionBox : public EsObjectBox
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 関数を取得する
	inline EsFunction* getFunction() const { return (EsFunction*)_obj; }
	// 関数ノードを取得する
	inline EsNameItNode* getFuncNode() const { return _node; }
	// ビルドフラグを得る
	inline u32 getBuildFlag() const { return _bflags; }
	inline u32* buildFlag() { return &_bflags; }
	// ループ内かどうかを得る
	inline bool isInLoop() const { return _isInLoop; }
	// ネスト深度を得る
	inline u32 getNestDepth() const { return _depthNest; }
	// ツリーの兄弟を得る
	inline EsFunctionBox* getSibling() const { return _sibling; }
	// ツリーの子を得る
	inline EsFunctionBox* getChild() const { return _child; }
	// ツリーの親を得る
	inline EsFunctionBox* getParent() const { return _parent; }

	// ツリーの兄弟を設定する
	inline void setSibling(EsFunctionBox* fb) { _sibling = fb; }
	// ツリーの子を設定する
	inline void setChild(EsFunctionBox* fb) { _child = fb; }
	// ツリーの親を設定する
	inline void setParent(EsFunctionBox* fb) { _parent = fb; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsFunctionBox();

	void setup(EsNameItNode* nodeFunc, EsFunction* objFunc, EsFunctionBox* funcboxParent, EsFunctionBox* funcboxSibling, bool isInLoop, u32 depthNest, u32 bflags);

	void postprocFunction(u32* bflags);
private:
	u32 pickFuncArgs(EsFunctionBox* funcbox, s32 depth, List<EsFunctionBox*>* listQueue);
	void decideFuncKind(u32* bflags);

	//======================================================================
	// 変数
protected:
	EsNameItNode* _node;
	EsFunctionBox* _sibling;	// ⇒ siblings
	EsFunctionBox* _child;		// ⇒ kids
	EsFunctionBox* _parent;		// ⇒ parent
	bool _isInLoop;				// ⇒ inLoop
	u32 _depthNest;				// ⇒ level
	u32 _bflags;				// ⇒ tcflags
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_FUNCTION_BOX_H_
