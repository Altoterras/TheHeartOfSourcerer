/***********************************************************************//**
 *	EvCause.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/09/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_EVSYS_EV_CAUSE_H_
#define _TFW_EVSYS_EV_CAUSE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class EvMatter;

/*---------------------------------------------------------------------*//**
 *	イベント起動原因
 *
**//*---------------------------------------------------------------------*/
class EvCause
{
public:
	static const short	CID_NULL		= 0;
	static const short	CID_ACT			= 1;
	static const short	CID_EVENT		= 2;
	static const short	CID_CONCURRENT	= 3;
	
private:
	short _cid;
	EvMatter* _matter;
	void* _param;

public:
	short getCid() const { return _cid; }
	void* getParameter() const { return _param; }

public:
	EvCause() : _cid(CID_NULL), _param(0L) {}
	EvCause(short cid) : _cid(cid), _param(0L) {}
	EvCause(EvMatter* matter) : _cid(CID_CONCURRENT), _matter(matter), _param(0L) {}

	EvCause(const EvCause& src) { copy(&src); }
	inline EvCause& operator=(const EvCause& src) { copy(&src);	return *this; }

	inline void copy(const EvCause* src)
	{
		this->_cid = src->_cid;
		this->_matter = src->_matter;
		this->_param = src->_param;
	}

	void setParameter(void* v) { _param = v; }
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_EVSYS_EV_CAUSE_H_
