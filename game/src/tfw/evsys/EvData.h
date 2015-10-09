/***********************************************************************//**
 *	EvData.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/09/28.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_EVSYS_EV_DATA_H_
#define _TFW_EVSYS_EV_DATA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "EvMatterCreateParam.h"

#include "../string/CcString.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class VcString;
class VcStringArray;
class EvMatter;
class EvMatterFactory;
class EvSys;
class MsgDataSet;
class OverrideEvMatter;
class XmlNode;

/*---------------------------------------------------------------------*//**
 *	イベント データ
 *
**//*---------------------------------------------------------------------*/
class EvData
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// マターの数を得る
	int getMatterNum() const;
	// マターを得る
	EvMatter* getMatter(int index) const;
	// マターをイベント ID から得る
	EvMatter* getMatterFromEvid(u16 evid) const;
	// メッセージデータセットを得る
	const MsgDataSet* getMsgDataSet() const { return _msgdatset; }
	// イベントシステムを得る
	EvSys* getOwner() const { return _evsysRef; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	u16 addFixedMatter(u16 evid, const CStringBase* nameMatter);
	inline u16 addFixedMatter(u16 evid, CcString nameMatter) { return addFixedMatter(evid, &nameMatter); }
	u16 addChildMatter(u16 evidParent, const CStringBase* nameMatter);
	inline u16 addChildMatter(u16 evidParent, CcString nameMatter) { return addChildMatter(evidParent, &nameMatter); }
	void removeMatter(u16 evid);
	void removeChildMatter(u16 evidParent);
	bool overrideMatter(const OverrideEvMatter* oemInf);
	bool unoverrideMatter(const OverrideEvMatter* oemInf);
	bool isOverrideMatter(const OverrideEvMatter* oemInf) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EvData();
	virtual ~EvData();
	virtual bool create(EvMatterFactory* evmfactRef, u16 numMsgKind);
	virtual void destroy();
	void bindEvSys(EvSys* evsysRef) { _evsysRef = evsysRef; }

protected:
	bool createMatterListFromXml(u32 fileidXml, addr_size paramPresv, const EvMatterCreateParam* cparam);
	virtual EvMatter* createMatter(const XmlNode* nodeEventMatter, const EvMatterCreateParam* cparam);
	void unloadMatter(u16 evidStart, u16 evidEnd, u16 evidParent);
	bool getMatterAttribute(const VcString** strwk, const XmlNode* nodeEventMatter, const CStringBase* atrbname);
	inline bool getMatterAttribute(const VcString** strwk, const XmlNode* nodeEventMatter, CcString atrbname) { return getMatterAttribute(strwk, nodeEventMatter, &atrbname); }
	bool getMatterAttributeArray(VcStringArray* strarrwk, s32 numRequire, const XmlNode* nodeEventMatter, const CStringBase* atrbname);
	inline bool getMatterAttributeArray(VcStringArray* strarrwk, s32 numRequire, const XmlNode* nodeEventMatter, CcString atrbname) { return getMatterAttributeArray(strarrwk, numRequire, nodeEventMatter, &atrbname); }
	bool getMatterChildData(const VcString** strwk, const XmlNode* nodeEventMatter, const CStringBase* nameChildNode);
	inline bool getMatterChildData(const VcString** strwk, const XmlNode* nodeEventMatter, CcString nameChildNode) { return getMatterChildData(strwk, nodeEventMatter, &nameChildNode); }
	virtual u16 getUnusedAutoEventId();

	//======================================================================
	// 変数
protected:
	EvSys* _evsysRef;
	EvMatterFactory* _evmfactRef;
	List<EvMatter*>* _listMatter;
	MsgDataSet* _msgdatset;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_EVSYS_EV_DATA_H_
