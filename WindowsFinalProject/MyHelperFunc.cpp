#include "framework.h"
#include "MyHelperFunc.h"
BOOL ImageFromIDResource(HINSTANCE hInst, UINT nID, LPCTSTR sTR, Image*& pImg)
{
	HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nID), sTR); // type
	if (!hRsrc)
		return FALSE;
	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (!lpRsrc)
		return FALSE;
	// Allocate global memory on which to create stream
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem, lpRsrc, len);
	GlobalUnlock(m_hMem);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem, FALSE, &pstm);
	// load from stream
	pImg = Gdiplus::Image::FromStream(pstm);
	// free/release stuff
	pstm->Release();
	FreeResource(lpRsrc);
	GlobalFree(m_hMem);
	return TRUE;
}

void DrawSurfboard(HINSTANCE hInst, Graphics* bufMem, int status,int timeStatusn) {
	static Image* pimage; // Construct an image
	if(!pimage)ImageFromIDResource(hInst, IDB_PNG1, _T("PNG"), pimage);
	RectF dest(512/2, 512/4,32,32);
	bufMem->DrawImage(pimage, dest,status*64, timeStatusn*64,64,64, UnitPixel);
}

void DrawPlayer(HINSTANCE hInst, Graphics* bufMem, int status) {
	static Image* pimage; // Construct an image
	if (!pimage)ImageFromIDResource(hInst, IDB_PNG2, _T("PNG"), pimage);
	RectF dest(512 / 2, 512 / 4, 32, 32);
	bufMem->DrawImage(pimage, dest, status * 64, 7 * 64, 64, 64, UnitPixel);
}

double getRadian(int angle) {
	return ((double)angle) / 180 * acos(-1.0);
}