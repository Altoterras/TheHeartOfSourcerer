/***********************************************************************//**
 *	RealtimeMovieMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_REALTIME_MOVIE_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_REALTIME_MOVIE_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../EventMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class XmlNode;
class AnimCamera;
class ShapeModel;

TFW_END_NS

ETK_BEGIN_NS

class MessageWindow;

ETK_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	リアルタイムムービー マター
 *	
**//*---------------------------------------------------------------------*/
class RealtimeMovieMatter : public EventMatter
{
	//======================================================================
	// クラス
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	AnimCamera* animCamera() const { return _camRtmvRef; }
	ShapeModel** shapeModelArray() const { return _mdlarrRef; }
	s32 getShapeModelNum() const { return _numMdl; }
	s32* noAnimIntpHint() { return &_hintNoAnimIntp; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setting(u32 fileid);
	static bool	analyzeSettingFromXml(u32* fileid, const XmlNode* node);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	RealtimeMovieMatter();
	~RealtimeMovieMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void destroy();
	void begin(u32 param);
	void end();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

	//======================================================================
	// 変数
private:
	AnimCamera*		_camRtmvRef;
	u16				_camidBk;			// 専用カメラ変更前のカメラ ID
	ShapeModel**	_mdlarrRef;
	s32				_numMdl;
	u32				_fileid;
	s32				_hintNoAnimIntp;	// 補間なしフラグの制御ヒント値
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_REALTIME_MOVIE_MATTER_H_
