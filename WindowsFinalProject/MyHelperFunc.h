#include "framework.h"
BOOL ImageFromIDResource(HINSTANCE,UINT nID, LPCTSTR sTR, Image*& pImg);

void DrawSurfboard(HINSTANCE hInst, Graphics* bufMem, int status, int timeStatusn);

void DrawPlayer(HINSTANCE hInst, Graphics* bufMem, int status);

double getRadian(int angle);

