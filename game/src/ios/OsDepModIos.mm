/***********************************************************************//**
 *	OsDepModIos.mm
 *	Enlight iOS Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#import "OsDepModIos.h"

// Friends
#import "EnlightAppDelegate.h"
#import "GameCenterManipulator.h"
#import "MainGlView.h"
#import "MainViewController.h"

// External
#include "../etk/body/Env.h"
#include "../tfw/file/File.h"
#include "../tfw/gcmn/View.h"
#include "../tfw/lib/Rect.h"
#include "../tfw/string/VcString.h"
#include "../tfw/string/StringUtils.h"
#include "../tfw/txt/TxtUtils.h"
#include "../tfw/txt/implement/SjisTxt.h"
#include "../tfw/txt/implement/Utf8Txt.h"
#if defined(_ENLIGHT_V01) || defined(_ENLIGHT_V02)
#include "../sourcerer/Base.h"
#endif

// Library
#import <mach/mach.h>
#import <sys/sysctl.h>
#import <GameKit/GameKit.h>
#import <CoreMotion/CMMotionManager.h>

////////////////////////////////////////////////////////////////////////////
// マクロ

#define USE_CMMOTIONMANAGER		(1)	// CMMotionManager を使用する

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// OsDepModIos メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	アプリケーションのバージョン文字列を得る
**//*---------------------------------------------------------------------*/
void OsDepModIos::getAppVersionString(VcString* ver)
{
	NSString *version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];
	*ver = [version UTF8String];
}

/*---------------------------------------------------------------------*//**
	リソースパスを得る
**//*---------------------------------------------------------------------*/
bool OsDepModIos::getResourceDirPath(tfw::VcString* path, const CcString prod)
{
	// リソースパスの取得
	#if 1
		NSString* pstrResourcePath = [[NSBundle mainBundle] bundlePath];
		//NSString* pstrResourcePath = [[NSBundle mainBundle] resourcePath];
		//NSString* pstrResourcePath = NSHomeDirectory();
		if(pstrResourcePath == nil)	{	return false;	}
	#else
		// 【旧仕様】
		// ！！！　注意　！！！
		// リソースパスのルートに
		// 　data_location.txt
		// というファイルが置かれていることが大前提条件である
		#if defined(_ENLIGHT_V01)
			NSString* pstrResourcePath = [[NSBundle mainBundle] pathForResource:@"enlight1010_location.txt" ofType:@""];
		#else
			NSString* pstrResourcePath = [[NSBundle mainBundle] pathForResource:@"enlight1010_location.txt" ofType:@""];
		#endif
		if(pstrResourcePath == nil)	{	return false;	}
		pstrResourcePath = [pstrResourcePath stringByDeletingLastPathComponent];
	#endif

	// String 化
	const char* szResourcePath = [pstrResourcePath UTF8String];
	*path = szResourcePath;
	*path += "/";
	TRACELN("{OsDepModIos::getResourceDirPath} res-path=%s", path->getRaw());

	return true;
}

/*---------------------------------------------------------------------*//**
	ドキュメントパスを得る
**//*---------------------------------------------------------------------*/
bool OsDepModIos::getDocumentDirPath(tfw::VcString* path, bool isMakeDirIfNoExist)
{
	// パスを得る
	NSArray* arrPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* pstrDocPath = [arrPaths objectAtIndex:0];
	
	// フォルダが存在しないとき、書き込み用途の場合はフォルダを作成する
	if(isMakeDirIfNoExist)
	{
		if(![[NSFileManager defaultManager] fileExistsAtPath:pstrDocPath])
		{
			[[NSFileManager defaultManager] createDirectoryAtPath:pstrDocPath withIntermediateDirectories:YES attributes:nil error:nil];
		}
	}

	// String 化
	const char* szDocPath = [pstrDocPath UTF8String];
	*path = szDocPath;
	if(path->getLength() > 1)
	{
		char c = path->getAt(path->getLength() - 1);
		if((c != '/') && (c != '\\'))	{	*path += '/';	}
	}
	//TRACELN("{OsDepModIos::getDocumentDirPath} doc-path=%s", path->getRaw());

	#if 0
	{
		TRACELN("");
		TRACELN("");
		TRACELN("");
		TRACELN("===");
		NSArray* arrPathsTest = NSSearchPathForDirectoriesInDomains(NSDocumentationDirectory, NSAllDomainsMask, YES);
		for(int i = 0; i < [arrPathsTest count]; i++)
		{
			NSString* pstrTest = [arrPathsTest objectAtIndex:i];
			const char* szTest = [pstrTest UTF8String];
			TRACELN("[%d]%s", i, szTest);
		}
		TRACELN("===");
		NSString* pstrResPath = [[NSBundle mainBundle] pathForResource:@"sourcerer01_save_f01.dat" ofType:@""];
		const char* szResPath = [pstrResPath UTF8String];
		TRACELN("r:%s", szResPath);
		TRACELN("===");
		TRACELN("");
		TRACELN("");
		TRACELN("");
	}
	#endif
	
	return true;
}

/*---------------------------------------------------------------------*//**
	一時ディレクトリのパスを得る
**//*---------------------------------------------------------------------*/
bool OsDepModIos::getTemporaryDirPath(VcString* path)
{
	NSString* nspathTemp = NSTemporaryDirectory();
	*path = [nspathTemp UTF8String];
	if(path->getLength() > 1)
	{
		char c = path->getAt(path->getLength() - 1);
		if((c != '/') && (c != '\\'))	{	*path += '/';	}
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	ホームページを開く
**//*---------------------------------------------------------------------*/
void OsDepModIos::gotoHp(CStringBase* str) const
{
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithCString:str->getRaw() encoding:NSUTF8StringEncoding]]];
}

/*---------------------------------------------------------------------*//**
	広告バナー表示制御
**//*---------------------------------------------------------------------*/
void OsDepModIos::showAdBanner(bool show) const
{
	EnlightAppDelegate* delg = (EnlightAppDelegate*)[[UIApplication sharedApplication] delegate];
	if(!delg)				{	return;	}
	MainViewController* vcMain = delg.mainViewController;
	if(!vcMain)				{	return;	}
	[vcMain showAdBanner: show ? YES : NO];
}

/*---------------------------------------------------------------------*//**
	クリップボードからペーストするテキストを得る
**//*---------------------------------------------------------------------*/
bool OsDepModIos::getPasteTextFromClipboard(Txt** txt)
{
	TRACELN("{OsDepModIos::getPasteTextFromClipboard}");
	
	// アプリケーションペーストボードを得る
	UIPasteboard* pbGen = [UIPasteboard generalPasteboard];
	TRACELN("    paste_board=%p", pbGen);
	if(!pbGen)
	{
		return false;
	}

	// 文字列を得る
	NSString* str = pbGen.string;
	if(!str)
	{
		return false;
	}

	// Shift-JIS に変換して C 文字列を得る
	const char* szSjis = (const char*)[str cStringUsingEncoding:NSShiftJISStringEncoding];
	if(!szSjis)
	{
		return false;
	}
	*txt = new SjisTxt();
	if(!txt)
	{
		return false;
	}
	VcString strSjis(szSjis);
	(*txt)->setSingleString(&strSjis);

	return true;
}

/*---------------------------------------------------------------------*//**
	クリップボードへコピーするテキストを設定する
**//*---------------------------------------------------------------------*/
bool OsDepModIos::setCopyTextToClipboard(const Txt* txt)
{
	TRACELN("{OsDepModIos::setCopyTextToClipboard}");
	
	// UTF8 テキストを得る
	Utf8Txt utftxt;
	if(txt->getEncode() == TxtDef::ENC_SJIS)
	{
		TxtUtils::convSjisToUtf8Text(&utftxt, (SjisTxt*)txt);
		txt = &utftxt;
	}
	VcString strwk;
	txt->takeRawString(&strwk);
	NSString* str = [NSString stringWithCString:strwk.getRaw() encoding:NSUTF8StringEncoding];
	if(!str)
	{
		return false;
	}

	// アプリケーションペーストボードを得る
	UIPasteboard* pbGen = [UIPasteboard generalPasteboard];
	TRACELN("    paste_board=%p", pbGen);
	if(!pbGen)
	{
		return false;
	}
	
	// コピーテキストをセット
	[pbGen setValue:str forPasteboardType:@"public.utf8-plain-text"];
	
	return true;
}

/*---------------------------------------------------------------------*//**
	テキストフィールドを作成する
**//*---------------------------------------------------------------------*/
u32 OsDepModIos::createTextField(const Txt* txt, const RectF32* rect, const View* view, u32 flags)
{
	EnlightAppDelegate* delg = (EnlightAppDelegate*)[[UIApplication sharedApplication] delegate];
	if(!delg)				{	return 0;	}
	MainGlView* viewGl = delg.glView;
	if(!viewGl)				{	return 0;	}
	UITextField* txtfldSourceEdit = viewGl.txtfldSourceEdit;
	if(!txtfldSourceEdit)	{	return 0;	}

	// UTF8 テキストを得る
	Utf8Txt utftxt;
	if(txt->getEncode() == TxtDef::ENC_SJIS)
	{
		//TRACELOGV("### 599 ### text=%s\n", txt->getRawString()->getRaw());
		TxtUtils::convSjisToUtf8Text(&utftxt, (SjisTxt*)txt);
		txt = &utftxt;
	}
	//TRACELOGV("### 600 ### text=%s\n", txt->getRawString()->getRaw());
	VcString strwk;
	txt->takeRawString(&strwk);
	NSString* str = [NSString stringWithCString:strwk.getRaw() encoding:NSUTF8StringEncoding];
	if(!str)
	{
		return 0;
	}
	
	// テキストフィールドへテキストを設定する
	txtfldSourceEdit.text = str;
	
	// 矩形設定
	f32 sx = view->getInvCoordinatesScale() * view->getLogicalScaleX();
	f32 sy = view->getInvCoordinatesScale() * view->getLogicalScaleY();
	//TRACELOGV("### 601 ### %f, %f, %f, %f\n", rect->x() * sx, rect->y() * sy, rect->w() * sx, rect->h() * sy);
	txtfldSourceEdit.frame = CGRectMake(rect->x() * sx, rect->y() * sy, rect->w() * sx, rect->h() * sy);
	
	// 表示
	txtfldSourceEdit.hidden = NO;
	
	return etk::OSCMPID_SOURCE_TEXT_FIELD;
}

/*---------------------------------------------------------------------*//**
	テキストフィールドを破棄する
**//*---------------------------------------------------------------------*/
bool OsDepModIos::destroyTextField(u32 oscmpid)
{
	if(oscmpid != etk::OSCMPID_SOURCE_TEXT_FIELD)	{	return false;	}
	EnlightAppDelegate* delg = (EnlightAppDelegate*)[[UIApplication sharedApplication] delegate];
	if(!delg)										{	return false;	}
	MainGlView* viewGl = delg.glView;
	if(!viewGl)										{	return false;	}
	UITextField* txtfldSourceEdit = viewGl.txtfldSourceEdit;
	if(!txtfldSourceEdit)							{	return false;	}
	
	// 非表示
	txtfldSourceEdit.hidden = YES;
	
	return true;
}

/*---------------------------------------------------------------------*//**
	テキストフィールドを表示（非表示）する
**//*---------------------------------------------------------------------*/
bool OsDepModIos::showTextField(u32 oscmpid, bool isShow)
{
	if(oscmpid != etk::OSCMPID_SOURCE_TEXT_FIELD)	{	return false;	}
	EnlightAppDelegate* delg = (EnlightAppDelegate*)[[UIApplication sharedApplication] delegate];
	if(!delg)										{	return false;	}
	MainGlView* viewGl = delg.glView;
	if(!viewGl)										{	return false;	}
	UITextField* txtfldSourceEdit = viewGl.txtfldSourceEdit;
	if(!txtfldSourceEdit)							{	return false;	}
	
	// 表示／非表示
	txtfldSourceEdit.hidden = !isShow;

	return true;
}

/*---------------------------------------------------------------------*//**
	テキストフィールドの文字列を得る
**//*---------------------------------------------------------------------*/
bool OsDepModIos::getTextFieldString(u32 oscmpid, Txt** txt)
{
	if(oscmpid != etk::OSCMPID_SOURCE_TEXT_FIELD)	{	return false;	}
	EnlightAppDelegate* delg = (EnlightAppDelegate*)[[UIApplication sharedApplication] delegate];
	if(!delg)										{	return false;	}
	MainGlView* viewGl = delg.glView;
	if(!viewGl)										{	return false;	}
	UITextField* txtfldSourceEdit = viewGl.txtfldSourceEdit;
	if(!txtfldSourceEdit)							{	return false;	}
	
	// テキストフィールドからテキストを得る
	NSString* str = txtfldSourceEdit.text;
	if(!str)
	{
		return false;
	}
	
	// Shift-JIS に変換して C 文字列を得る
	const char* szSjis = (const char*)[str cStringUsingEncoding:NSShiftJISStringEncoding];
	if(!szSjis)
	{
		return false;
	}
	*txt = new SjisTxt();
	if(!txt)
	{
		return false;
	}
	VcString strSjis(szSjis);
	(*txt)->setSingleString(&strSjis);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	プロダクトのスキーマを得る
**//*---------------------------------------------------------------------*/
bool OsDepModIos::getAppSchema(tfw::VcString* schema, s32 proddefid)
{
#if defined(_ENLIGHT_V01) || defined(_ENLIGHT_V02)
	switch(proddefid)
	{
	case sourcerer::PRODDEFID_V01:		*schema = "sourcerer01";		return true;
	case sourcerer::PRODDEFID_V01_LITE:	*schema = "sourcerer01lite";	return true;
	case sourcerer::PRODDEFID_V02:		*schema = "sourcerer02";		return true;
	}
#endif
	
	return false;
}

/*---------------------------------------------------------------------*//**
	他のプロダクトがインストールされているかチェックする
**//*---------------------------------------------------------------------*/
bool OsDepModIos::isInstalledApp(s32 proddefid)
{
	tfw::VcString schema;
	if(!getAppSchema(&schema, proddefid)) { return false; }
	NSString* strUrl = [NSString stringWithFormat:@"%@:", [NSString stringWithCString:schema.getRaw() encoding:NSASCIIStringEncoding]];
	NSURL* url = [NSURL URLWithString:strUrl];
	bool res = [[UIApplication sharedApplication] canOpenURL:url];
	return res;
}

/*---------------------------------------------------------------------*//**
	他のプロダクトを起動する
**//*---------------------------------------------------------------------*/
bool OsDepModIos::runApp(s32 proddefid, const tfw::CStringBase* param)
{
	tfw::VcString schema;
	if(!getAppSchema(&schema, proddefid)) { return false; }
	NSString* strUrl = [NSString stringWithFormat:@"%@://%@", [NSString stringWithCString:schema.getRaw() encoding:NSASCIIStringEncoding], [NSString stringWithCString:param->getRaw() encoding:NSASCIIStringEncoding]];
	TRACELN("{OsDepModIos::runApp} url=%s", [strUrl cStringUsingEncoding:NSASCIIStringEncoding]);
	NSURL* url = [NSURL URLWithString:strUrl];
	bool res = [[UIApplication sharedApplication] openURL:url];
	return res;
}

/*---------------------------------------------------------------------*//**
	Lite 版より有料版へセーブデータをコピーする
**//*---------------------------------------------------------------------*/
bool OsDepModIos::transferSavesTo(const tfw::VcString* namePasteBoard, const tfw::VcString* nameFiles, const tfw::VcString* keyFiles, s32 numFiles)
{
	TRACELN("{OsDepModIos::transferSavesTo}");
	
	// アプリケーションペーストボードを得る
	NSString* namePb = [NSString stringWithCString:namePasteBoard->getRaw() encoding:NSASCIIStringEncoding];
	UIPasteboard* pbApp = [UIPasteboard pasteboardWithName:namePb create:YES];
	TRACELN("    paste_board_name=%s, paste_board=%p", [namePb cStringUsingEncoding:NSASCIIStringEncoding], pbApp);
	if(!pbApp)
	{
		return false;
	}
	pbApp.persistent = true;	// 永続

	// ドキュメントパスを得る
	NSArray* arrDocumentPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* pathDocDir = [arrDocumentPaths objectAtIndex:0];
	TRACELN("    doc_dir=%s", [pathDocDir cStringUsingEncoding:NSASCIIStringEncoding]);
	
///	// テンポラリディレクトリを得る
///	NSString* pathTmpDir = NSTemporaryDirectory();
///	TRACELN("    tmp_dir=%s", [pathTmpDir cStringUsingEncoding:NSASCIIStringEncoding]);

	bool isSucceeded = false;
	NSMutableArray* items = [NSMutableArray array];///WithCapacity:numFiles];
	for(int i = 0; i < numFiles; i++)
	{
		// ファイルパスを得る
		VcString nameFileWk(nameFiles[i]);
		if(nameFileWk.find('%', 0) != -1)
		{
			// Lite 版は GameCenter に対応していないと想定する
			VcString::format(&nameFileWk, nameFileWk, "");
		}
		NSString* pathDocFile = [NSString stringWithFormat:@"%@/%@", pathDocDir, [NSString stringWithCString:nameFileWk.getRaw() encoding:NSASCIIStringEncoding]];

///		// ファイルをテンポラリディレクトリにコピーする
///		NSString* pathTmpFile = [NSString stringWithFormat:@"%@%@", pathTmpDir, [NSString stringWithCString:nameFiles[i].getRaw() encoding:NSASCIIStringEncoding]];
///		[[NSFileManager defaultManager] copyItemAtPath:pathDocFile toPath:pathTmpFile error:NULL];
		
		// ファイルデータを得る
///		NSData* dataFile = [NSData dataWithContentsOfMappedFile:pathTmpFile];
		NSData* dataFile = [NSData dataWithContentsOfMappedFile:pathDocFile];
		if(dataFile)
		{
			NSDictionary* dict = [NSDictionary dictionaryWithObject:dataFile forKey:[NSString stringWithCString:keyFiles[i].getRaw() encoding:NSASCIIStringEncoding]];
///			TRACELN("    [%d] file=%s, key=%s", i, [pathTmpFile cStringUsingEncoding:NSASCIIStringEncoding], keyFiles[i].getRaw());
			TRACELN("    [%d] file=%s, key=%s", i, [pathDocFile cStringUsingEncoding:NSASCIIStringEncoding], keyFiles[i].getRaw());
			[items addObject:dict];
			isSucceeded = true;	// １つでも成功したら、全体の成功とする
		}
	}
	
	// ペーストボードにアイテムを設定
	pbApp.items = items;
	
	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	有料版にて Lite 版からのセーブデータをペーストする
**//*---------------------------------------------------------------------*/
bool OsDepModIos::transferSavesFrom(const tfw::VcString* namePasteBoard, const tfw::VcString* nameFiles, const tfw::VcString* keyFiles, s32 numFiles)
{
	TRACELN("{OsDepModIos::transferSavesFrom}");
	
	// アプリケーションペーストボードを得る
	NSString* namePb = [NSString stringWithCString:namePasteBoard->getRaw() encoding:NSASCIIStringEncoding];
	UIPasteboard* pbApp = [UIPasteboard pasteboardWithName:namePb create:/*NO*/YES];
	TRACELN("    paste_board_name=%s, paste_board=%p", [namePb cStringUsingEncoding:NSASCIIStringEncoding], pbApp);
	if(!pbApp)
	{
		return false;
	}
	
	// ドキュメントパスを得る
	NSArray* arrDocumentPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* pathDocDir = [arrDocumentPaths objectAtIndex:0];
	TRACELN("    doc_dir=%s", [pathDocDir cStringUsingEncoding:NSASCIIStringEncoding]);

	// フォルダが存在しないとき、書き込み用途の場合はフォルダを作成する
	if(![[NSFileManager defaultManager] fileExistsAtPath:pathDocDir])
	{
		[[NSFileManager defaultManager] createDirectoryAtPath:pathDocDir withIntermediateDirectories:YES attributes:nil error:nil];
	}
	
	// ユーザー ID を得る
	VcString userid;
	if([GameCenterManipulator isGameCenterApiAvailable])
	{
		GKLocalPlayer* lp = [GKLocalPlayer localPlayer];
		if(lp && lp.authenticated)	// GameCenter 認証済み
		{
			userid = [lp.playerID cStringUsingEncoding:NSASCIIStringEncoding];
		}
	}

	// ペーストボードのデータごとに処理し、ファイルを上書き保存する
	NSMutableArray* types = [NSMutableArray array];
	for(int i = 0; i < numFiles; i++)
	{
		[types addObject:[NSString stringWithCString:keyFiles[i].getRaw() encoding:NSASCIIStringEncoding]];
	}
	NSIndexSet* idxset = [pbApp itemSetWithPasteboardTypes:types];
	if(!idxset)
	{
		TRACELN("    !1! itemSetWithPasteboardTypes error.");
		return false;
	}
	bool isSucceeded = false;
	for(int i = 0; i < numFiles; i++)
	{
		NSArray* items = [pbApp dataForPasteboardType:[NSString stringWithCString:keyFiles[i].getRaw() encoding:NSASCIIStringEncoding] inItemSet:idxset];
		if(!items)
		{
			TRACELN("    !2! dataForPasteboardType error.");
			continue;
		}
		u32 cnt = (u32)[items count];
		if(cnt <= 0)
		{
			TRACELN("    !3! item count error.");
			continue;
		}
		TRACELN("    items are found. [%d] cnt=%d, key=%s", i, cnt, keyFiles[i].getRaw());
		
		// ファイルパスを得る
		NSString* pathFile;
		{
			VcString nameFileWk(nameFiles[i]);
			if(nameFileWk.find('%', 0) != -1)
			{
				if(userid.getLength() > 0)
				{
					VcString postfix("_");
					postfix += userid;
					VcString::format(&nameFileWk, nameFileWk.getRaw(), postfix.getRaw());
				}
				else
				{
					VcString::format(&nameFileWk, nameFileWk.getRaw(), "");
				}
			}
			
			pathFile = [NSString stringWithFormat:@"%@/%@", pathDocDir, [NSString stringWithCString:nameFileWk.getRaw() encoding:NSASCIIStringEncoding]];
		}
			
		// ファイルデータを書き込む
		NSFileHandle* fhandle = [NSFileHandle fileHandleForWritingAtPath:pathFile];
		if(!fhandle)
		{
			// 開けなかったのはファイルが存在しないからかもしれないので、作成→開くを試みる
			[[NSFileManager defaultManager] createFileAtPath:pathFile contents:nil attributes:nil];
			fhandle = [NSFileHandle fileHandleForWritingAtPath:pathFile];
			if(!fhandle)
			{
				TRACELN("    !4! file open error. [%d] file=%s, key=%s", i, [pathFile cStringUsingEncoding:NSASCIIStringEncoding], keyFiles[i].getRaw());
				continue;
			}
		}
		TRACELN("    file is opened. [%d] file=%s, key=%s", i, [pathFile cStringUsingEncoding:NSASCIIStringEncoding], keyFiles[i].getRaw());
		for(u32 j = 0; j < cnt; j++)
		{
			[fhandle writeData:[items objectAtIndex:j]];
		}
		[fhandle closeFile];
		TRACELN("    write file is done. [%d] cnt=%d, file=%s, key=%s", i, cnt, [pathFile cStringUsingEncoding:NSASCIIStringEncoding], keyFiles[i].getRaw());
		isSucceeded = true;	// １つでも成功したら、全体の成功とする
	}
	
	// ペーストボードを無効にする
	if(isSucceeded)
	{
		[UIPasteboard removePasteboardWithName:namePb];
	}
	
	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	セーブデータ移行のためのペーストボードが有効化どうかを判定する
**//*---------------------------------------------------------------------*/
bool OsDepModIos::isTransferSavesPasterboardValid(const tfw::VcString* namePasteBoard, const tfw::VcString* keyFiles, s32 numFiles)
{
	//TRACELN("{OsDepModIos::isTransferSavesPasterboardValid}");
	
	// アプリケーションペーストボードを得る
	NSString* namePb = [NSString stringWithCString:namePasteBoard->getRaw() encoding:NSASCIIStringEncoding];
	UIPasteboard* pbApp = [UIPasteboard pasteboardWithName:namePb create:/*NO*/YES];
	//TRACELN("    paste_board_name=%s, paste_board=%p", [namePb cStringUsingEncoding:NSASCIIStringEncoding], pbApp);
	if(!pbApp)
	{
		return false;
	}

	// ペーストボードのデータを参照する
	NSMutableArray* types = [NSMutableArray array];
	for(int i = 0; i < numFiles; i++)
	{
		[types addObject:[NSString stringWithCString:keyFiles[i].getRaw() encoding:NSASCIIStringEncoding]];
	}
	NSIndexSet* idxset = [pbApp itemSetWithPasteboardTypes:types];
	if(!idxset)
	{
		//TRACELN("{OsDepModIos::isTransferSavesPasterboardValid} * 1 *");
		return false;
	}
	for(int i = 0; i < numFiles; i++)
	{
		NSArray* items = [pbApp dataForPasteboardType:[NSString stringWithCString:keyFiles[i].getRaw() encoding:NSASCIIStringEncoding] inItemSet:idxset];
		if(items)
		{
			return true;	// 一つでもデータが存在すれば成功
		}
	}		
	
	//TRACELN("{OsDepModIos::isTransferSavesPasterboardValid} * 2 *");
	return false;
}

/*---------------------------------------------------------------------*//**
	セーブデータ移行のためのペーストボードを削除する
**//*---------------------------------------------------------------------*/
void OsDepModIos::removeTransferSavesPasterboard(const tfw::VcString* namePasteBoard)
{
	NSString* namePb = [NSString stringWithCString:namePasteBoard->getRaw() encoding:NSASCIIStringEncoding];
	[UIPasteboard removePasteboardWithName:namePb];
}

/*---------------------------------------------------------------------*//**
	デバイスタイプを得る
**//*---------------------------------------------------------------------*/
u32 OsDepModIos::getDeviceType()
{
	size_t size;
	::sysctlbyname("hw.machine", NULL, &size, NULL, 0);
	char* machine = (char*)::malloc(size);
	::sysctlbyname("hw.machine", machine, &size, NULL, 0);
	NSString* platform = [NSString stringWithCString:machine encoding:NSASCIIStringEncoding];
	//NSString* platform = [[UIDevice currentDevice]model];
	
	u32 dtype = DeviceType_Unknown;
	do
	{
		if([platform isEqualToString:@"iFPGA"])		{	dtype = DeviceType_IFPGA;			break;	}

		if([platform hasPrefix:@"iPhone5"])			{	dtype = DeviceType_iPhone5;			break;	}
		if([platform hasPrefix:@"iPhone4"])			{	dtype = DeviceType_iPhone4;			break;	}	// iPhone4S
		if([platform hasPrefix:@"iPhone3"])			{	dtype = DeviceType_iPhone4;			break;	}	// iPhone4
		if([platform hasPrefix:@"iPhone2"])			{	dtype = DeviceType_iPhone3GS;		break;	}
		if([platform isEqualToString:@"iPhone1,2"])	{	dtype = DeviceType_iPhone3G;		break;	}
		if([platform isEqualToString:@"iPhone1,1"])	{	dtype = DeviceType_iPhone1G;		break;	}
	
		if([platform hasPrefix:@"iPod5"])			{	dtype = DeviceType_iPod5G;			break;	}
		if([platform hasPrefix:@"iPod4"])			{	dtype = DeviceType_iPod4G;			break;	}
		if([platform hasPrefix:@"iPod3"])			{	dtype = DeviceType_iPod3G;			break;	}
		if([platform hasPrefix:@"iPod2"])			{	dtype = DeviceType_iPod2G;			break;	}
		if([platform hasPrefix:@"iPod1"])			{	dtype = DeviceType_iPod1G;			break;	}
		
		if([platform isEqualToString:@"iPad3,4"])	{	dtype = DeviceType_iPad4G;			break;	}
		if([platform hasPrefix:@"iPad3"])			{	dtype = DeviceType_iPad3G;			break;	}
		if([platform hasPrefix:@"iPad2"])			{	dtype = DeviceType_iPad2G;			break;	}
		if([platform hasPrefix:@"iPad1"])			{	dtype = DeviceType_iPad1G;			break;	}

		if([platform hasPrefix:@"iPhone"])			{	dtype = DeviceType_iPhone_Unknown;	break;	}
		if([platform hasPrefix:@"iPod"])			{	dtype = DeviceType_iPod_Unknown;	break;	}
		if([platform hasPrefix:@"iPad"])			{	dtype = DeviceType_iPad_Unknown;	break;	}
	
		if([platform hasSuffix:@"86"] || [platform isEqual:@"x86_64"])
		{
			float w = [[UIScreen mainScreen] bounds].size.width;
			float h = [[UIScreen mainScreen] bounds].size.height;
			float aspect = h / w;
			if((1.33f <= aspect) && (aspect <= 1.34f))	{	dtype = DeviceType_iPhoneSimulator_iPad;	break;	}
			else										{	dtype = DeviceType_iPhoneSimulator_iPhone;	break;	}

			dtype = DeviceType_iPhoneSimulator;	break;
		}
	}
	while(false);
	
	::free(machine);
	
	#if defined(_DEBUG)
		TRACELOGV("{OsDepModIos::getDeviceType} platform=%s, dtype=%d, width=%f, height=%f, aspect=%f\n", [platform cStringUsingEncoding:NSASCIIStringEncoding], dtype, [[UIScreen mainScreen] bounds].size.width, [[UIScreen mainScreen] bounds].size.height, [[UIScreen mainScreen] bounds].size.height / (float)[[UIScreen mainScreen] bounds].size.width);
	#endif
	
	return dtype;
}

/*---------------------------------------------------------------------*//**
	モーションデバイスが有効かを調べる
**//*---------------------------------------------------------------------*/
bool OsDepModIos::isAvailableMotionDevice()
{
#if defined(_SUPPORT_MOTION_EVENT) && USE_CMMOTIONMANAGER
	CMMotionManager* momng = [[CMMotionManager alloc] init];
	bool b = momng.deviceMotionAvailable == TRUE;
	momng = nil;
	return b;
#else
	return false;
#endif
}

/*---------------------------------------------------------------------*//**
	加速度センサデバイスが有効かを調べる
**//*---------------------------------------------------------------------*/
bool OsDepModIos::isAvailableAccelerometerDevice()
{
#if defined(_SUPPORT_MOTION_EVENT) && USE_CMMOTIONMANAGER
	CMMotionManager* momng = [[CMMotionManager alloc] init];
	bool b = momng.accelerometerAvailable == TRUE;
	momng = nil;
	return b;
#else
	return false;
#endif
}

/*---------------------------------------------------------------------*//**
	磁気センサデバイスが有効かを調べる
**//*---------------------------------------------------------------------*/
bool OsDepModIos::isAvailableMagnetometerDevice()
{
#if defined(_SUPPORT_MOTION_EVENT) && USE_CMMOTIONMANAGER
	CMMotionManager* momng = [[CMMotionManager alloc] init];
	bool b = momng.magnetometerAvailable == TRUE;
	momng = nil;
	return b;
#else
	return false;
#endif
}

/*---------------------------------------------------------------------*//**
	ジャイロデバイスが有効かを調べる
**//*---------------------------------------------------------------------*/
bool OsDepModIos::isAvailableGyroDevice()
{
#if defined(_SUPPORT_MOTION_EVENT) && USE_CMMOTIONMANAGER
	CMMotionManager* momng = [[CMMotionManager alloc] init];
	bool b = momng.gyroAvailable == TRUE;
	momng = nil;
	return b;
#else
	return false;
#endif
}

/*---------------------------------------------------------------------*//**
	メモリ情報を得る
**//*---------------------------------------------------------------------*/
bool OsDepModIos::getMemoryInfo(u32* sizeFree, u32* sizeTotal, u32* sizeResident)
{
	// 確保済みメモリ量
	if(sizeResident != 0L)
	{
		struct task_basic_info basicInfo;
		mach_msg_type_number_t basicInfoCount = TASK_BASIC_INFO_COUNT;
		if (task_info(current_task(), TASK_BASIC_INFO, (task_info_t)&basicInfo, &basicInfoCount) != KERN_SUCCESS)
		{
			return false;
		}
		*sizeResident = (u32)basicInfo.resident_size;;
	}
	
	// メモリ情報を得る
	mach_port_t hostPort = mach_host_self();;
	mach_msg_type_number_t hostSize = sizeof(vm_statistics_data_t) / sizeof(integer_t);
	vm_size_t pageSize;
	host_page_size(hostPort, &pageSize);
	vm_statistics_data_t vmStat;
	if (host_statistics(hostPort, HOST_VM_INFO, (host_info_t)&vmStat, &hostSize) != KERN_SUCCESS)
	{
		return false;
	}
	
	// 空きメモリ量
	u32 freesize = (u32)((vmStat.free_count) * pageSize);
	if(sizeFree != 0L)
	{
		*sizeFree = freesize;
	}
	
	// 最大メモリ量
	if(sizeTotal != 0L)
	{
		*sizeTotal = (u32)((vmStat.active_count + vmStat.inactive_count + vmStat.wire_count) * pageSize) + freesize;
	}

	//TRACELOGV("resident_size=%fMB, free_size=%fMB\n", resident_size / (1024.0f * 1024.0f), free_size / (1024.0f * 1024.0f));
	return true;
}

/*---------------------------------------------------------------------*//**
	アプリケーションデリゲートを取得する
**//*---------------------------------------------------------------------*/
EnlightAppDelegate* OsDepModIos::getEnlightAppDelegate()
{
	return (EnlightAppDelegate*)[[UIApplication sharedApplication] delegate];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
OsDepModIos::OsDepModIos()
	: OsDepMod(0L)
{
}

////////////////////////////////////////////////////////////////////////////
// グローバル関数

//==========================================================================
// test

#if 0
/*---------------------------------------------------------------------*//**
	ムービー再生テスト
**//*---------------------------------------------------------------------*/
#import <MediaPlayer/MPMoviePlayerController.h>
void _test_playSimpleMovie()
{
	MPMoviePlayerController* moviePlayer = [MPMoviePlayerController alloc];
	
	NSURL* url = [[NSURL alloc] initFileURLWithPath: [[NSBundle mainBundle] pathForResource:@"movie_test" ofType:@"mp4"]];
	[moviePlayer initWithContentURL: url];
	
	moviePlayer.controlStyle = MPMovieControlStyleNone;
	
	[moviePlayer play];
}
#endif

////////////////////////////////////////////////////////////////////////////
