#pragma once
#include <iomanip>
#include <iostream>
#include "qvector.h"
#include "ViewPath.h"
#include "ViewMatrix.h"
#include "ViewNode.h"
namespace autopilot {
#define FLOYD_MAX    100
#define FLOYD_INF    (~(0x1<<31))        // 无穷大(即0X7FFFFFFF)

	// 边的结构体
	class EData
	{
	public:
		char start; // 边的起点
		char end;   // 边的终点
		int weight; // 边的权重

	public:
		EData() {}
		EData(char s, char e, int w) :start(s), end(e), weight(w) {}
	};

	class Floyd
	{
	public:
		// 创建图
		Floyd(QVector<autopilot::ViewPath*> paths, QVector<ViewNode*> nodes);
		//获取最短路径
		QVector<autopilot::ViewPath*> getShortestPath(int startID, int endID);
	private:
		QVector<int> mVexs;    // 顶点集合
		int mVexNum;             // 顶点数
		int mEdgNum;             // 边数
		ViewMatrix* mMatrix;   // 邻接矩阵
		ViewMatrix* distMat;   //距离矩阵
		ViewMatrix* pathMat;   //路径矩阵
		QVector<autopilot::ViewPath*> shortestPath; //最短路径，暂存
		QVector<autopilot::ViewPath*> paths;
		// Floyd最短路径
		void init();
		// 由ID获取index
		int getIndexOfID(int _ID);
		// 获取图中的边
		EData* getEdges();
		// 对边按照权值大小进行排序(由小到大)
		void sortEdges(EData* edges, int elen);
		//从矩阵中提取最近路径
		void findPath(int startID, int endID);
		void addPath(int startID, int endID);
	};
}