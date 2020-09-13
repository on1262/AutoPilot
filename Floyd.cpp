#include "Floyd.h"

using namespace std;
using namespace autopilot;

Floyd::Floyd(QVector<autopilot::ViewPath*> paths, QVector<ViewNode*> nodes)
{
	this->paths = paths;
	//获取节点数，初始化mMatrix
	mMatrix = new ViewMatrix(nodes.size(), nodes.size(),FLOYD_INF);
	distMat = new ViewMatrix(nodes.size(), nodes.size(),0);
	pathMat = new ViewMatrix(nodes.size(), nodes.size(),0);
	//顶点数等于node数量
	for (auto k = nodes.begin(); k != nodes.end(); k++) {
		mVexs.push_back((*k)->ID); //初始化为节点对应的ID
	}
	mVexNum = mVexs.size();
	//初始化矩阵
	for (auto i = paths.begin(); i != paths.end(); i++) {
		int sumLen = ViewPath::getPathSumLength(*i);
		mMatrix->set(getIndexOfID((*i)->startID), getIndexOfID((*i)->endID), sumLen);
		mMatrix->set(getIndexOfID((*i)->endID), getIndexOfID((*i)->startID), sumLen);
	}
	for (int j = 0; j < nodes.size(); j++) {
		mMatrix->set(j, j, 0);
	}

	//边数等于paths大小
	mEdgNum = paths.size();
	//更新矩阵
	init();
}


/*
 * 创建图(用已提供的矩阵)
 *
 * 参数说明：
 *     vexs  -- 顶点数组
 *     vlen  -- 顶点数组的长度
 *     matrix-- 矩阵(数据)
 
Floyd::Floyd(char vexs[], int vlen, int matrix[][9])
{
	int i, j;

	// 初始化"顶点数"和"边数"
	mVexNum = vlen;
	// 初始化"顶点"
	for (i = 0; i < mVexNum; i++)
		mVexs[i] = vexs[i];

	// 初始化"边"
	for (i = 0; i < mVexNum; i++)
		for (j = 0; j < mVexNum; j++)
			mMatrix[i][j] = matrix[i][j];

	// 统计边的数目
	for (i = 0; i < mVexNum; i++)
		for (j = 0; j < mVexNum; j++)
			if (i != j && mMatrix[i][j] != FLOYD_INF)
				mEdgNum++;
	mEdgNum /= 2;
}*/


int Floyd::getIndexOfID(int _ID)
{
	int i;
	for (i = 0; i < mVexNum; i++)
		if (mVexs[i] == _ID)
			return i;
	return -1;
}



/*
 * 获取图中的边
 */
EData* Floyd::getEdges()
{
	int i, j;
	int index = 0;
	EData *edges;

	edges = new EData[mEdgNum];
	for (i = 0; i < mVexNum; i++)
	{
		for (j = i + 1; j < mVexNum; j++)
		{
			if (mMatrix->get(i,j) != FLOYD_INF)
			{
				edges[index].start = mVexs[i];
				edges[index].end = mVexs[j];
				edges[index].weight = mMatrix->get(i,j);
				index++;
			}
		}
	}

	return edges;
}

/*
 * 对边按照权值大小进行排序(由小到大)
 */
void Floyd::sortEdges(EData* edges, int elen)
{
	int i, j;

	for (i = 0; i < elen; i++)
	{
		for (j = i + 1; j < elen; j++)
		{
			if (edges[i].weight > edges[j].weight)
			{
				// 交换"边i"和"边j"
				swap(edges[i], edges[j]);
			}
		}
	}
}


/*
 * init最短路径。
 * 即，统计图中各个顶点间的最短路径。
 *
 * 参数说明：
 *     cachePath -- 路径。cachePath[i][j]=k表示，"顶点i"到"顶点j"的最短路径会经过顶点k。
 *     dist -- 长度数组。即，dist[i][j]=sum表示，"顶点i"到"顶点j"的最短路径的长度是sum。
 */
void Floyd::init()
{
	int i, j, k;
	int tmp;

	// 初始化
	for (i = 0; i < mVexNum; i++)
	{
		for (j = 0; j < mVexNum; j++)
		{
			distMat->set(i,j,mMatrix->get(i,j));    // "顶点i"到"顶点j"的路径长度为"i到j的权值"。
			pathMat->set(i,j,j);                // "顶点i"到"顶点j"的最短路径是经过顶点j。
		}
	}

	// 计算最短路径
	for (k = 0; k < mVexNum; k++)
	{
		for (i = 0; i < mVexNum; i++)
		{
			for (j = 0; j < mVexNum; j++)
			{
				// 如果经过下标为k顶点路径比原两点间路径更短，则更新dist[i][j]和cachePath[i][j]
				tmp = (distMat->get(i,k) == FLOYD_INF || distMat->get(k,j) == FLOYD_INF) ? FLOYD_INF : (distMat->get(i, k) + distMat->get(k, j));
				if (distMat->get(i, j) > tmp)
				{
					// "i到j最短路径"对应的值设，为更小的一个(即经过k)
					distMat->set(i, j,tmp);
					// "i到j最短路径"对应的路径，经过k
					pathMat->set(i,j,pathMat->get(i,k));
				}
			}
		}
	}

	// 打印init最短路径的结果
	cout << "floyd: dist" << endl;
	for (i = 0; i < mVexNum; i++)
	{
		for (j = 0; j < mVexNum; j++)
			cout << setw(2) << distMat->get(i,j) << "  ";
		cout << endl;
	}
	cout << "floyd: path" << endl;
	for (i = 0; i < mVexNum; i++)
	{
		for (j = 0; j < mVexNum; j++)
			cout << setw(2) << pathMat->get(i,j) << "  ";
		cout << endl;
	}
}

QVector<ViewPath*> Floyd::getShortestPath(int startID, int endID)
{
	shortestPath.clear(); //清空
	findPath(startID, endID); //递归得到路径
	//检查路径是否正确
	if ((*shortestPath.begin())->startID != startID || shortestPath.back()->endID != endID) {
		Utils::log(true, "getShortestPath: error ID index");
	}
	for (auto i = shortestPath.begin() + 1; i < shortestPath.end(); i++) {
		if ((*i)->startID != (*(i + 1))->endID) {
			Utils::log(true, "getShortestPath: error ID index");
		}
	}
	return shortestPath;
}

void Floyd::findPath(int startID, int endID)
{
	int start = getIndexOfID(startID);
	int end = getIndexOfID(endID);
	int pass = pathMat->get(start, end);
	if (pass == start || pass == end) { //如果两者相邻
		addPath(startID, endID);
		return;
	}
	else { //如果并不相邻
		findPath(startID, mVexs[pass]);
		findPath(mVexs[pass], endID);
	}
}

void Floyd::addPath(int startID, int endID)
{
	int reversePathIndex = -1;
	for (int i = 0; i < paths.size(); i++) {
		if (paths[i]->startID == startID && paths[i]->endID == endID) {
			shortestPath.push_back(paths[i]);
			return;
		}
		if (paths[i]->endID == startID && paths[i]->startID == endID) {
			reversePathIndex = i;
		}
	}
	if (reversePathIndex != -1) {
		shortestPath.push_back(ViewPath::getReversePath(paths[reversePathIndex]));
	}
	else {
		Utils::log(true, "addPath: can't find path or reversed path");
	}
}
