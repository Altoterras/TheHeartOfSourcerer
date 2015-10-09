/***********************************************************************//**
 *	EvData.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/28.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EvData.h"

// Friends
#include "EvMatter.h"
#include "EvMatterFactory.h"
#include "MsgDataSet.h"
#include "OverrideEvMatter.h"

// External
#include "../collection/List.h"
#include "../file/IdFile.h"
#include "../lib/XmlParser.h"
#include "../string/VcString.h"
#include "../string/VcStringArray.h"
#include "../string/StringUtils.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ
	
/*---------------------------------------------------------------------*//**
	マターの数を得る
**//*---------------------------------------------------------------------*/
int EvData::getMatterNum() const
{
	ASSERT(_listMatter != 0L);
	return _listMatter->getCount();
}
	
/*---------------------------------------------------------------------*//**
	マターを得る
**//*---------------------------------------------------------------------*/
EvMatter* EvData::getMatter(int index) const
{
	ASSERT(_listMatter != 0L);
	return _listMatter->getObject(index);
}
	
/*---------------------------------------------------------------------*//**
	マターをイベント ID から得る
**//*---------------------------------------------------------------------*/
EvMatter* EvData::getMatterFromEvid(u16 evid) const
{
	for(u32 i = 0; i < _listMatter->getCount(); i++)
	{
		EvMatter* matter = _listMatter->getObject(i);
		if(matter->getEvId() == evid)
		{
			return matter;
		}
	}
	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	システム固定マター追加
**//*---------------------------------------------------------------------*/
u16 EvData::addFixedMatter(u16 evid, const CStringBase* nameMatter)
{
	// 作成
	EvMatter* matter = _evmfactRef->newEventMatter(nameMatter);
	if(!matter->create(this, evid))
	{
		return 0;
	}
	
	// リストに追加
	_listMatter->addTail(matter);
	
	return matter->getEvId();
}

/*---------------------------------------------------------------------*//**
	子マター追加
**//*---------------------------------------------------------------------*/
u16 EvData::addChildMatter(u16 evidParent, const CStringBase* nameMatter)
{
	// イベント ID 自動割り当て
	u16 evidChild = getUnusedAutoEventId();
	if(evidChild == 0)
	{
		ASSERT(evidChild != 0);
		return 0;	// 自動割当失敗
	}

	// 作成
	EvMatter* matter = _evmfactRef->newEventMatter(nameMatter);
	if(matter == 0L)
	{
		ASSERT(false);
		return 0;
	}
	if(!matter->create(this, evidChild))
	{
		ASSERT(false);
		return 0;
	}

	// 親のイベント ID を設定
	matter->setParentEvId(evidParent);
	
	// リストに追加
	_listMatter->addTail(matter);

	return evidChild;
}

/*---------------------------------------------------------------------*//**
	マターを削除する
**//*---------------------------------------------------------------------*/
void EvData::removeMatter(u16 evid)
{
	unloadMatter(evid, evid, 0);
}

/*---------------------------------------------------------------------*//**
	子マター全削除
**//*---------------------------------------------------------------------*/
void EvData::removeChildMatter(u16 evidParent)
{
	unloadMatter(0, 0, evidParent);
}

/*---------------------------------------------------------------------*//**
	マターを上書きする
**//*---------------------------------------------------------------------*/
bool EvData::overrideMatter(const OverrideEvMatter* oemInf)
{
	bool isSucceeded = false;
	bool isError = false;

	EvMatter* matter = getMatterFromEvid(oemInf->getOldEventId());
	if((matter != 0L) && matter->overrideMatter(oemInf->getNewEventId()))
	{
		isSucceeded = true;
	}
	else
	{
		isError = true;
	}

	if(isError)	{	return false;	}
	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	マター上書きを戻す
**//*---------------------------------------------------------------------*/
bool EvData::unoverrideMatter(const OverrideEvMatter* oemInf)
{
	bool isSucceeded = false;
	bool isError = false;

	EvMatter* matter = getMatterFromEvid(oemInf->getOldEventId());
	if((matter != 0L) && matter->unoverrideMatter())
	{
		isSucceeded = true;
	}
	else
	{
		isError = true;
	}

	if(isError)	{	return false;	}
	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	マター上書きが行われているかを調べる
**//*---------------------------------------------------------------------*/
bool EvData::isOverrideMatter(const OverrideEvMatter* oemInf) const
{
	const EvMatter* matter = getMatterFromEvid(oemInf->getOldEventId());
	if(matter == 0L)
	{
		return false;
	}
	return matter->isOverrideMatter();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EvData::EvData()
	: _evsysRef(0L)
	, _evmfactRef(0L)
	, _listMatter(0L)
	, _msgdatset(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EvData::~EvData()
{
	ASSERT(_listMatter == 0L);
	ASSERT(_msgdatset == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EvData::create(EvMatterFactory* evmfactRef, u16 numMsgKind)
{
	// ファクトリへの参照を得る
	_evmfactRef = evmfactRef;

	// マターリストの作成
	_listMatter = new List<EvMatter*>;

	// メッセージデータセットの作成
	_msgdatset = new MsgDataSet();
	if(!_msgdatset->create(numMsgKind))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EvData::destroy()
{
	// メッセージデータセットの削除
	if(_msgdatset != 0L)
	{
		_msgdatset->destroy();
		delete _msgdatset;
		_msgdatset = 0L;
	}

	// マターリストの削除
	if(_listMatter != 0L)
	{
		for(ListIterator<EvMatter*> it = _listMatter->iterator(); it.has(); it.next())
		{
			EvMatter* matter = it.object();
			matter->destroy();
			delete matter;
		}
		delete _listMatter;
		_listMatter = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	データ読み込み
**//*---------------------------------------------------------------------*/
bool EvData::createMatterListFromXml(u32 fileidXml, addr_size paramPresv, const EvMatterCreateParam* cparam)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// XML ファイル読み込み

	VcString bufFile;
	{
		// ファイルを開く
		IdFile fileXml(fileidXml);
		if(!fileXml.isOpened())	{	/*ASSERT(false);*/	return false;	}	// 現状ではないこともある
		
		// XML ファイルをバッファに読み込む
		while(true)
		{
			const int SIZE_BUF = 100 * 1024;
			char buf[SIZE_BUF];
			int sizeRead = fileXml.read(buf, SIZE_BUF);
			bufFile.add(buf, sizeRead);
			if(sizeRead < SIZE_BUF)	{	break;	}
		}
		TRACE("{EvData::loadXmlFile} event xml : size=%d\n", bufFile.getLength());
		
		// ファイルを閉じる
		fileXml.close();
	}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// XML 解析

	XmlParser parser;
	parser.parseXmlDocument(&bufFile);
	TRACE("{EvData::loadXmlFile} XmlParser::parseXmlDocument end.\n");
	
	const XmlNode* nodeRoot = parser.getRootNode();
	if(nodeRoot == 0L)								{	TRACE("{EvData::createMatterListFromXml} parse error ! *** 1 ***\n");	return false;	}
	if(!nodeRoot->getName()->equals("EventData"))	{	TRACE("{EvData::createMatterListFromXml} parse error ! *** 2 ***\n");	return false;	}
	const XmlNode* nodeEvent = nodeRoot->findChildNode("Event");
	while(nodeEvent != 0L)
	{
		// マター読み込み・作成
		EvMatter* matter = createMatter(nodeEvent, cparam);
		if(matter != 0L)
		{
			matter->setPreserveParam(paramPresv);	// 保持パラメータをセットする
			_listMatter->addTail(matter);
		}

		nodeEvent = nodeEvent->findSiblingNode("Event");
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	マター読み込み
**//*---------------------------------------------------------------------*/
EvMatter* EvData::createMatter(const XmlNode* nodeEventMatter, const EvMatterCreateParam* cparam)
{
	/* XML の形式

		FormatA：
		<Event>
			<Type></Type>
			<Data></Data>
		</Event>

		FormatB：
		<Event Type="TYPE">
		</Event>
	*/

	const VcString* strwk;
	VcStringArray strarrwk;
	
	// ID 取得
	if(!getMatterAttribute(&strwk, nodeEventMatter, "ID"))	{	return 0L;	}
	u16 evid = (u16)StringUtils::toInteger(strwk);
	
	// その他基本属性取得
	u16 evidNext = 0;
	u16 evidConcurrent = 0;
	if(getMatterAttribute(&strwk, nodeEventMatter, "NextID"))		{	evidNext = (u16)StringUtils::toInteger(strwk);			}
	if(getMatterAttribute(&strwk, nodeEventMatter, "ConcurrentID"))	{	evidConcurrent = (u16)StringUtils::toInteger(strwk);	}

	// タイプからマター作成
	EvMatter* matter = 0L;
	if(getMatterChildData(&strwk, nodeEventMatter, "Type"))			// 子要素からタイプを得る（FormatA）
	{
		matter = _evmfactRef->newEventMatter(strwk);
	}
	else if(getMatterAttribute(&strwk, nodeEventMatter, "Type"))	// 属性からタイプを得る（FormatB）
	{
		matter = _evmfactRef->newEventMatter(strwk);
	}
	if(matter == 0L)	{	return 0L;	}	// タイプが取得できず作成失敗

	// 作成
	if(!matter->create(this, evid))
	{
		ASSERT(false);
		return 0L;
	}

	// オプション設定
	matter->setOption(evidNext, evidConcurrent);

	// XML ノードから作成
	const XmlNode* nodeData = nodeEventMatter->findChildNode("Data");	// 子要素からデータノードを得る（FormatA）
	if(nodeData == 0L)	// 子要素は全てデータノード（FormatB）
	{
		nodeData = nodeEventMatter->getChildNode();
	}
	if(nodeData != 0L)
	{
		if(!matter->setupFromXmlNode(nodeData, cparam))
		{
			ASSERT(false);
			return 0L;
		}
	}
	
	return matter;
}

/*---------------------------------------------------------------------*//**
	範囲内のイベントマターを削除する

	@param evidStart 対象開始イベント ID（この値を含む）
	@param evidEnd 対象開始イベント ID（この値を含む）
		evidStart と evidEnd が共に 0 の場合は開始～終了判定を行わない
	@param evidParent 親イベント ID
		evidParent が 0 の場合は親イベント判定を行わない
**//*---------------------------------------------------------------------*/
void EvData::unloadMatter(u16 evidStart, u16 evidEnd, u16 evidParent)
{
	ListIterator<EvMatter*> itCur = _listMatter->iterator();
	while(itCur.has())
	{
		ListIterator<EvMatter*> itNext = itCur;
		itNext.next();

		EvMatter* matter = itCur.object();
		if(	(((evidStart <= matter->getEvId()) && (matter->getEvId() <= evidEnd)) || ((evidStart == 0) && (evidEnd == 0)))
			&& ((evidParent == matter->getParentEvId()) || (evidParent == 0))	)
		{
			// 実行中のものは強制終了
			if(matter->isBegin())
			{
				matter->end();
			}

			// マターを削除してリストからも削除する
			matter->destroy();
			delete matter;
			_listMatter->removeNode(itCur.node());
		}

		itCur = itNext;
	}
}

/*---------------------------------------------------------------------*//**
	ノードの属性値を得る
**//*---------------------------------------------------------------------*/
bool EvData::getMatterAttribute(const VcString** strwk, const XmlNode* nodeEventMatter, const CStringBase* atrbname)
{
	*strwk = nodeEventMatter->findAttribute(atrbname);
	return *strwk != 0L;
}

/*---------------------------------------------------------------------*//**
	ノードの列挙型属性値を得る

	@param	numRequire	必要な配列要素数．判定不要の場合は -1 を指定
**//*---------------------------------------------------------------------*/
bool EvData::getMatterAttributeArray(VcStringArray* strarrwk, s32 numRequire, const XmlNode* nodeEventMatter, const CStringBase* atrbname)
{
	const VcString* strwk;
	if(!getMatterAttribute(&strwk, nodeEventMatter, atrbname))	{	return false;	}	// 値が取得できない
	*strarrwk = StringUtils::split(strwk, ',');
	if((numRequire != strarrwk->getCount()) && (numRequire != -1))	{	return false;	}	// 配列数が不正
	return true;
}

/*---------------------------------------------------------------------*//**
	子ノードの値を得る
**//*---------------------------------------------------------------------*/
bool EvData::getMatterChildData(const VcString** strwk, const XmlNode* nodeEventMatter, const CStringBase* nameChildNode)
{
	const XmlNode* nodeChild = nodeEventMatter->findChildNode(nameChildNode);
	if(nodeChild == 0L)
	{
		return false;
	}
	*strwk = nodeChild->getChildPcdata();
	return true;
}

/*---------------------------------------------------------------------*//**
	自動割り当てのイベント ID を取得する

	@return	イベント ID（0 は不正な ID）
**//*---------------------------------------------------------------------*/
u16 EvData::getUnusedAutoEventId()
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
