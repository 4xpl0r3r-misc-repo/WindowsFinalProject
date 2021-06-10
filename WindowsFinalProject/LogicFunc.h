#pragma once

#include "framework.h"
#include "LogicFunc.h"

struct physicalObj
{
	//зјБъ
	int x;
	int y;
	int r;
	bool invalidFlag;
	bool displayFlag;
	int display[4];
};

struct operateRecord {
	ULONGLONG frameId;
	int status;
};

double getRadian(int angle);
bool checkCollision(physicalObj a, physicalObj b);
void generateObstacles(vector<physicalObj>& obstacles, int baseX, int baseY);