/***********************************************************************//**
 *	OsDepModWin.cpp
 *	Enlight Windows Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "OsDepModWin.h"

// Friends
#include "App.h"
#include "MainWindow.h"

// External
#include "../etk/body/Env.h"
#include "../tfw/file/File.h"
#include "../tfw/gcmn/View.h"
#include "../tfw/lib/Rect.h"
#include "../tfw/lib/TmpBuf.h"
#include "../tfw/string/VcString.h"
#include "../tfw/string/StringUtils.h"
#include "../tfw/string/VwString.h"
#include "../tfw/txt/TxtUtils.h"
#include "../tfw/txt/implement/SjisTxt.h"
#include "../tfw/txt/implement/UnicodeTxt.h"
#include "../tfw/txt/implement/Utf8Txt.h"

// Library
#include <shlobj.h>

////////////////////////////////////////////////////////////////////////////

namespace app
{

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// OsDepModWin メソッド
	
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	アプリケーションのバージョン文字列を得る
**//*---------------------------------------------------------------------*/
void OsDepModWin::getAppVersionString(VcString* ver)
{
	*ver = "3.0";
}

/*---------------------------------------------------------------------*//**
	リソースパスを得る
**//*---------------------------------------------------------------------*/
bool OsDepModWin::getResourceDirPath(tfw::VcString* path, const CcString prod)
{
#if 1
	STRING_CLASS wk1, wk2, dir;

	// 自身（EXE）のパスを得る
	TCHAR strExePathWk[MAX_PATH];
	::GetModuleFileName(NULL, strExePathWk, MAX_PATH);
	wk1.set(strExePathWk);

	StringUtils::dividePath(&dir, &wk2, &wk1);
	#if defined(UNICODE)
		_strAppParam->setEasily(&wk2);
	#else
		_strAppParam->copy(&wk2);
	#endif

	// デバッグ環境時のパス
	#if defined(_DEBUG)
		#if 1	// proj の中でビルドする場合
			wk1 = dir + _T("..\\..\\..\\..\\data\\");
		#elif 0	// ローカル相対でビルドする場合
			wk1 = dir + _T("..\\..\\game\\data\\");
		#elif 0	// OneDrive でビルドする場合
			wk1 = dir + _T("..\\..\\..\\..\\..\\SkyDrive\\Works\\Enlight\\Src\\game\\data\\");
		#endif
		#ifdef UNICODE
			wk2.setEasily(prod.getRaw());
		#else
			wk2.set(prod.getRaw());
		#endif
		wk1.add(&wk2);
	#endif

	// 製品環境時のパス
	if(!File::isDirExist(wk1.getRaw()))
	{
		wk1 = dir;
	}

	wk1.add(_T("\\"));

	#ifdef UNICODE
		char cbufExePathWk[MAX_PATH];
		size_t len = 0;
		errno_t err = ::wcstombs_s(&len, cbufExePathWk, MAX_PATH, wk1.getRaw(), _TRUNCATE);	// WCHAR -> char 変換
		path->set(cbufExePathWk);
	#else
		path->set(&wk1);
	#endif
	return true;

#else
	// 自身（EXE）のパスを得る
	TCHAR strExePathWk[MAX_PATH];
	::GetModuleFileName(NULL, strExePathWk, MAX_PATH);
	#ifdef UNICODE
		char cbufExePathWk[MAX_PATH];
		size_t len = 0;
		errno_t err = ::wcstombs_s(&len, cbufExePathWk, MAX_PATH, strExePathWk, _TRUNCATE);	// WCHAR -> char 変換
		VcString pathWk(cbufExePathWk);
	#else
		VcString pathWk(strExePathWk);
	#endif

	VcString dir;
	StringUtils::dividePath(&dir, NULL, &pathWk);

	// デバッグ環境時のパス
	#if defined(_DEBUG)
		*path = dir + "..\\..\\..\\..\\data\\";
		path->add(prod.getRaw());
	#endif

	// 製品環境時のパス
	if(!File::isDirExist(path))
	{
		*path = dir;
	}

	path->add("\\");
	return true;
#endif
}

/*---------------------------------------------------------------------*//**
	ドキュメントパスを得る
**//*---------------------------------------------------------------------*/
bool OsDepModWin::getDocumentDirPath(tfw::VcString* path, bool isMakeDirIfNoExist)
{
	TCHAR strDocDirPathWk[MAX_PATH * 2];
	#ifdef UNICODE
		char cbufWk[MAX_PATH];
		TCHAR strWk[MAX_PATH];
		size_t len = 0;
	#endif

	if(FAILED(::SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, strDocDirPathWk)))
	{
		return false;
	}

	// マーカーパス化
	_tcscat(strDocDirPathWk, _T("\\"));
	#ifdef UNICODE
		::mbstowcs_s(&len, strWk, ::strlen(_marker), _marker, _TRUNCATE);	// char -> WCHAR 変換
		_tcscat(strDocDirPathWk, strWk);
	#else
		_tcscat(strDocDirPathWk, _marker);
	#endif
	_tcscat(strDocDirPathWk, _T("\\"));

	// フォルダが存在しないとき、書き込み用途の場合はフォルダを作成する
	if(isMakeDirIfNoExist)
	{
		if (!tfw::File::isDirExist(strDocDirPathWk))
		{
			if(!::CreateDirectory(strDocDirPathWk, NULL))
			{
				return false;
			}
		}
	}

	// ドキュメントパス化
	#ifdef UNICODE
		::mbstowcs_s(&len, strWk, ::strlen(_appnam), _appnam, _TRUNCATE);	// char -> WCHAR 変換
		_tcscat(strDocDirPathWk, strWk);
	#else
		_tcscat(strDocDirPathWk, _appnam);
	#endif
	_tcscat(strDocDirPathWk, _T("\\"));

	// フォルダが存在しないとき、書き込み用途の場合はフォルダを作成する
	if(isMakeDirIfNoExist)
	{
		if(!tfw::File::isDirExist(strDocDirPathWk))
		{
			if(!::CreateDirectory(strDocDirPathWk, NULL))
			{
				return false;
			}
		}
	}

	#ifdef UNICODE
		::wcstombs_s(&len, cbufWk, _tcslen(strWk), strWk, _TRUNCATE);	// WCHAR -> char 変換
		*path = cbufWk;
	#else
		*path = strDocDirPathWk;
	#endif

	return true;
}

/*---------------------------------------------------------------------*//**
	一時パスを得る
**//*---------------------------------------------------------------------*/
bool OsDepModWin::getTemporaryDirPath(VcString* path)
{
	STRING_CLASS wk1, dir;

	// 自身（EXE）のパスを得る
	TCHAR strExePathWk[MAX_PATH];
	::GetModuleFileName(NULL, strExePathWk, MAX_PATH);
	wk1.set(strExePathWk);
	StringUtils::dividePath(&dir, 0L, &wk1);
	wk1 = dir;
	wk1.add(_T("\\tmp\\"));
	#ifdef UNICODE
		char cbufExePathWk[MAX_PATH];
		size_t len = 0;
		errno_t err = ::wcstombs_s(&len, cbufExePathWk, MAX_PATH, wk1.getRaw(), _TRUNCATE);	// WCHAR -> char 変換
		path->set(cbufExePathWk);
	#else
		path->set(&wk1);
	#endif
	return true;
}

/*---------------------------------------------------------------------*//**
	ホームページへのジャンプ動作
**//*---------------------------------------------------------------------*/
void OsDepModWin::gotoHp(CStringBase* str) const
{
	#ifdef UNICODE
		VwString wstr;
		wstr.setEasily(str);
		::ShellExecute(0, _T("open"), wstr.getRaw(), NULL, NULL, SW_SHOWNORMAL);
	#else
		::ShellExecute(0, _T("open"), str->getRaw(), NULL, NULL, SW_SHOWNORMAL);
	#endif
}

/*---------------------------------------------------------------------*//**
	広告バナー表示制御
**//*---------------------------------------------------------------------*/
void OsDepModWin::showAdBanner(bool show) const
{
}

/*---------------------------------------------------------------------*//**
	クリップボードからペーストするテキストを得る
**//*---------------------------------------------------------------------*/
bool OsDepModWin::getPasteTextFromClipboard(Txt** txt)
{
	if(!::OpenClipboard(0L))
	{
		return false;
	}
	HGLOBAL hg = ::GetClipboardData(CF_TEXT);
	if(hg == 0L)
	{
		::CloseClipboard();
		return false;
	}
	u8* gmem = (u8*)::GlobalLock(hg);
	if(gmem == 0L)
	{
		::CloseClipboard();
		return false;
	}
	u32 length = (u32)::GlobalSize(hg);
	VcString str((char*)gmem, length);
	::GlobalUnlock(hg);
	::CloseClipboard();
	SjisTxt* sjistxt = new SjisTxt();
	#if defined(UNICODE)
	{
		Utf8Txt utf8txt;
		utf8txt.setString(&str, Utf8Txt::F_REF_STRING);
		TxtUtils::convUtf8ToSjisText(sjistxt, &utf8txt);
	}
	#else
		sjistxt->setSingleString(&str);
	#endif
	*txt = sjistxt;
	return true;
}

/*---------------------------------------------------------------------*//**
	クリップボードへコピーするテキストを設定する
**//*---------------------------------------------------------------------*/
bool OsDepModWin::setCopyTextToClipboard(const Txt* txt)
{
	SjisTxt sjistxt;
	if(txt->getEncode() == TxtDef::ENC_UTF8)
	{
		TxtUtils::convUtf8ToSjisText(&sjistxt, (Utf8Txt*)txt);
		txt = &sjistxt;
	}

	VcString str;
	txt->takeRawString(&str);
	u8* data = (u8*)str.getRaw();
	u32 length = str.getLength() + 1;
	HGLOBAL hg = ::GlobalAlloc(GHND, length);
	if(hg == 0L)
	{
		return false;
	}
	u8* gmem = (u8*)::GlobalLock(hg);
	if(gmem == 0L)
	{
		::GlobalFree(hg);
		return false;
	}
	::TFW_COPY_MEMORY(gmem, data, length);
	::GlobalUnlock(hg);
	::OpenClipboard(0L);
	::EmptyClipboard();
	::SetClipboardData(CF_TEXT, hg);
	::CloseClipboard();

	return true;
}

/*---------------------------------------------------------------------*//**
	テキストフィールドを作成する
**//*---------------------------------------------------------------------*/
u32 OsDepModWin::createTextField(const Txt* txt, const RectF32* rect, const View* view, u32 flags)
{
	HWND hwndEdit = g_papp->GetMainWindow()->GetSourceEditWindowHandle();
	if(hwndEdit == 0L)		{	return 0;	}
	HWND hwndMain = g_papp->GetMainWindow()->GetMainWindowHandle();
	if(hwndMain == 0L)		{	return 0;	}

	// テキストフィールドへテキストを設定する
	#if defined(UNICODE)
		VwString str;
		txt->takeRawWideString(&str);
	#else
		VcString str;
		txt->takeRawString(&str);
	#endif
	::SetWindowText(hwndEdit, str.getRaw());
	
	// 矩形設定
	RECT rectMainClient;
	::GetClientRect(hwndMain, &rectMainClient);
	::ClientToScreen(hwndMain, (LPPOINT)&rectMainClient);
	f32 sx = 1.0f / (view->getCoordinatesScale() * view->getInvLogicalScaleX());	//view->getInvContentScale() * view->getLogicalScaleX();
	f32 sy = view->getInvCoordinatesScale() * view->getLogicalScaleY();
	int x = (int)(rect->x() * sx) + rectMainClient.left;
	int y = (int)(rect->y() * sy) + rectMainClient.top;
	int w = (int)(rect->w() * sx);
	int h = (int)(rect->h() * sy);
	::MoveWindow(hwndEdit, x, y, w, h, TRUE);
	
	// 表示
	::ShowWindow(hwndEdit, SW_SHOW);
	
	return etk::OSCMPID_SOURCE_TEXT_FIELD;
}

/*---------------------------------------------------------------------*//**
	テキストフィールドを破棄する
**//*---------------------------------------------------------------------*/
bool OsDepModWin::destroyTextField(u32 oscmpid)
{
	HWND hwndEdit = g_papp->GetMainWindow()->GetSourceEditWindowHandle();
	if(hwndEdit == 0L)		{	return false;	}

	// 非表示
	::ShowWindow(hwndEdit, SW_HIDE);

	return true;
}

/*---------------------------------------------------------------------*//**
	テキストフィールドを表示（非表示）する
**//*---------------------------------------------------------------------*/
bool OsDepModWin::showTextField(u32 oscmpid, bool isShow)
{
	HWND hwndEdit = g_papp->GetMainWindow()->GetSourceEditWindowHandle();
	if(hwndEdit == 0L)		{	return false;	}

	// 表示／非表示
	::ShowWindow(hwndEdit, isShow ? SW_SHOW : SW_HIDE);

	return true;
}

/*---------------------------------------------------------------------*//**
	テキストフィールドの文字列を得る
**//*---------------------------------------------------------------------*/
bool OsDepModWin::getTextFieldString(u32 oscmpid, Txt** txt)
{
	HWND hwndEdit = g_papp->GetMainWindow()->GetSourceEditWindowHandle();
	if(hwndEdit == 0L)		{	return false;	}

	int lenWindowStr = ::GetWindowTextLength(hwndEdit);
	const u32 SIZE_SBUF = 64;
	u8 sbuf[SIZE_SBUF];
	TmpBuf tmpbuf(sizeof(TCHAR) * (lenWindowStr + 1), sbuf, SIZE_SBUF);
	TCHAR* bufStr = (TCHAR*)tmpbuf.get();

	::GetWindowText(hwndEdit, bufStr, lenWindowStr);

	#if defined(UNICODE)
		UnicodeTxt* utxt = new UnicodeTxt();
		if(!utxt)
		{
			return false;
		}
		VwString strUnicode(bufStr);
		utxt->setWideString(&strUnicode);
		*txt = utxt;
	#else
		*txt = new SjisTxt();
		if(!txt)
		{
			return false;
		}
		VcString strSjis(bufStr);
		(*txt)->setSingleString(&strSjis);
	#endif

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
OsDepModWin::OsDepModWin(const char* marker, const char* appnam)
	: OsDepMod(0L)
	, _marker(marker)
	, _appnam(appnam)
{
}

////////////////////////////////////////////////////////////////////////////

} // namespace app