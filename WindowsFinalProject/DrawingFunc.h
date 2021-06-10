#include "framework.h"
#include "LogicFunc.h"
BOOL ImageFromIDResource(HINSTANCE,UINT nID, LPCTSTR sTR, Image*& pImg);

void DrawSurfboard(HINSTANCE hInst, Graphics* bufMem, int status, int timeStatusn);

void DrawPlayer(HINSTANCE hInst, Graphics* bufMem, int status);

void DrawObstacles(HINSTANCE hInst, Graphics* bufMem, vector<physicalObj>& obstacles);


