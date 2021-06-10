#include "framework.h"
#include "LogicFunc.h"

double getRadian(int angle) {
	return ((double)angle) / 180 * acos(-1.0);
}

bool checkCollision(physicalObj a, physicalObj b) {
	//(a.x-b.x)* (a.x - b.x)
	//+
	//	(a.y - b.y) * (a.y - b.y)
	//	<=
	//	(a.r + b.r) * (a.r + b.r)s
	if (
		abs(a.x-b.x)<a.r+b.r && abs(a.y -b.y) < a.r + b.r
	) {
		return true;
	}
	else {
		return false;
	}
}

void generateObstaclesReal(vector<physicalObj>& obstacles,int baseLeft,int baseRight,int baseTop,int baseBottom) {
	// avoid danger area spawn in the edge
	for (size_t i = 0; i < 4; i++)
	{
		physicalObj newCenter = {0,0,32};
		boolean flag = false;
		while (!flag) {
			flag = true;
			newCenter.x = (rand() % (baseRight - baseLeft + 1 - 2 * 32)) + baseLeft + 32;
			newCenter.y = (rand() % (baseBottom - baseTop + 1 - 2 * 32)) + baseTop + 32;
			for (auto it = obstacles.begin(); it != obstacles.end(); it++)
			{
				if (!it->invalidFlag&&checkCollision(*it,newCenter)) { // �����οռ�
					flag = false;
					break;
				}
			}
		}
		for (int i = 0; i < 4; i++)
		{
			newCenter.display[i]= rand() % 30;
		}
		obstacles.push_back(newCenter);
	}
}

void generateObstacles(vector<physicalObj>& obstacles, int baseX, int baseY) { // baseXΪ��ֱ���ߵ�x����,Y��Ϊ��Ҫ����ҳ���y���꣬Ҳ���ǵ�ǰҳ��ĵײ�+1/2�ŵ�ͼ�߶�
	generateObstaclesReal(obstacles, baseX - 512, baseX, baseY, baseY + 512);
	generateObstaclesReal(obstacles, baseX , baseX + 512, baseY, baseY + 512);
}