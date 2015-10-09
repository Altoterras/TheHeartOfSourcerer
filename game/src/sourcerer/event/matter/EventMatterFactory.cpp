/***********************************************************************//**
 *	EventMatterFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EventMatterFactory.h"

// Friends
#include "EventMatter.h"
#include "implement/ActMatter.h"
#include "implement/IntvlSaveMatter.h"
#include "implement/MapconfMatter.h"
#include "implement/MovieCutMatter.h"
#include "implement/QuestconfMatter.h"
#include "implement/RealtimeMovieMatter.h"
#include "implement/ScrEffMatter.h"
#include "implement/ScrModeMatter.h"
#include "implement/SeMatter.h"
#include "implement/ShadowCtrlMatter.h"
#include "implement/SpriteMatter.h"
#include "implement/StoryconfMatter.h"
#include "implement/TalkMatter.h"
#include "implement/TexFacialMatter.h"
#include "implement/ValFaderMatter.h"
#include "../../episode/mapemp/MapEmpFactory.h"
#include "../../episode/stemp/StEmpFactory.h"
#include "../../quest/qsemp/QsEmpFactory.h"

// External
#include "../../../tfw/string/VcString.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	イベントマター作成
**//*---------------------------------------------------------------------*/
EvMatter* EventMatterFactory::newEventMatter(const CStringBase* nameMatter)
{
	const char* name = nameMatter->getRaw();
	EventMatter* matter = 0L;

	// システム固定マター
	if(::strcmp(name, "ACT") == 0)							{	matter = new ActMatter();						}
	else if(::strcmp(name, "INTVLSAVE") == 0)				{	matter = new IntvlSaveMatter();					}
	else if(::strcmp(name, "MAPCONF") == 0)					{	matter = new MapconfMatter();					}
	else if(::strcmp(name, "MOVIECUT") == 0)				{	matter = new MovieCutMatter();					}
	else if(::strcmp(name, "RTMOVIE") == 0)					{	matter = new RealtimeMovieMatter();				}
	else if(::strcmp(name, "SCREFF") == 0)					{	matter = new ScrEffMatter();					}
	else if(::strcmp(name, "SCRMODE") == 0)					{	matter = new ScrModeMatter();					}
	else if(::strcmp(name, "SE") == 0)						{	matter = new SeMatter();						}
	else if(::strcmp(name, "SHADOWCTRL") == 0)				{	matter = new ShadowCtrlMatter();				}
	else if(::strcmp(name, "SPRITE") == 0)					{	matter = new SpriteMatter();					}
	else if(::strcmp(name, "STORYCONF") == 0)				{	matter = new StoryconfMatter();					}
	else if(::strcmp(name, "QUESTCONF") == 0)				{	matter = new QuestconfMatter();					}
	else if(::strcmp(name, "TALK") == 0)					{	matter = new TalkMatter();						}
	else if(::strcmp(name, "TEXFACIAL") == 0)				{	matter = new TexFacialMatter();					}
	else if(::strcmp(name, "VALFADER") == 0)				{	matter = new ValFaderMatter();					}

	// その他ストーリープログラム
	if(matter == 0L)
	{
		if(	
			(name[0] == 'S') &&
			(name[1] == 't') &&
			(name[2] == 'E') &&
			(name[3] == 'm') &&
			(name[4] == 'p') )
		{
			matter = StEmpFactory::newEventMatter(nameMatter);
		}
		else if(
			(name[0] == 'M') &&
			(name[1] == 'a') &&
			(name[2] == 'p') &&
			(name[3] == 'E') &&
			(name[4] == 'm') &&
			(name[5] == 'p') )
		{
			matter = MapEmpFactory::newEventMatter(nameMatter);
		}
		else if(
			(name[0] == 'Q') &&
			(name[1] == 's') &&
			(name[2] == 'E') &&
			(name[3] == 'm') &&
			(name[4] == 'p') )
		{
			matter = QsEmpFactory::newEventMatter(nameMatter);
		}
	}
	
	return matter;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
