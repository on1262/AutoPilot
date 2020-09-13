#include "Floyd.h"

using namespace std;
using namespace autopilot;

Floyd::Floyd(QVector<autopilot::ViewPath*> paths, QVector<ViewNode*> nodes)
{
	this->paths = paths;
	//��ȡ�ڵ�������ʼ��mMatrix
	mMatrix = new ViewMatrix(nodes.size(), nodes.size(),FLOYD_INF);
	distMat = new ViewMatrix(nodes.size(), nodes.size(),0);
	pathMat = new ViewMatrix(nodes.size(), nodes.size(),0);
	//����������node����
	for (auto k = nodes.begin(); k != nodes.end(); k++) {
		mVexs.push_back((*k)->ID); //��ʼ��Ϊ�ڵ��Ӧ��ID
	}
	mVexNum = mVexs.size();
	//��ʼ������
	for (auto i = paths.begin(); i != paths.end(); i++) {
		int sumLen = ViewPath::getPathSumLength(*i);
		mMatrix->set(getIndexOfID((*i)->startID), getIndexOfID((*i)->endID), sumLen);
		mMatrix->set(getIndexOfID((*i)->endID), getIndexOfID((*i)->startID), sumLen);
	}
	for (int j = 0; j < nodes.size(); j++) {
		mMatrix->set(j, j, 0);
	}

	//��������paths��С
	mEdgNum = paths.size();
	//���¾���
	init();
}


/*
 * ����ͼ(�����ṩ�ľ���)
 *
 * ����˵����
 *     vexs  -- ��������
 *     vlen  -- ��������ĳ���
 *     matrix-- ����(����)
 
Floyd::Floyd(char vexs[], int vlen, int matrix[][9])
{
	int i, j;

	// ��ʼ��"������"��"����"
	mVexNum = vlen;
	// ��ʼ��"����"
	for (i = 0; i < mVexNum; i++)
		mVexs[i] = vexs[i];

	// ��ʼ��"��"
	for (i = 0; i < mVexNum; i++)
		for (j = 0; j < mVexNum; j++)
			mMatrix[i][j] = matrix[i][j];

	// ͳ�Ʊߵ���Ŀ
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
 * ��ȡͼ�еı�
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
 * �Ա߰���Ȩֵ��С��������(��С����)
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
				// ����"��i"��"��j"
				swap(edges[i], edges[j]);
			}
		}
	}
}


/*
 * init���·����
 * ����ͳ��ͼ�и������������·����
 *
 * ����˵����
 *     cachePath -- ·����cachePath[i][j]=k��ʾ��"����i"��"����j"�����·���ᾭ������k��
 *     dist -- �������顣����dist[i][j]=sum��ʾ��"����i"��"����j"�����·���ĳ�����sum��
 */
void Floyd::init()
{
	int i, j, k;
	int tmp;

	// ��ʼ��
	for (i = 0; i < mVexNum; i++)
	{
		for (j = 0; j < mVexNum; j++)
		{
			distMat->set(i,j,mMatrix->get(i,j));    // "����i"��"����j"��·������Ϊ"i��j��Ȩֵ"��
			pathMat->set(i,j,j);                // "����i"��"����j"�����·���Ǿ�������j��
		}
	}

	// �������·��
	for (k = 0; k < mVexNum; k++)
	{
		for (i = 0; i < mVexNum; i++)
		{
			for (j = 0; j < mVexNum; j++)
			{
				// ��������±�Ϊk����·����ԭ�����·�����̣������dist[i][j]��cachePath[i][j]
				tmp = (distMat->get(i,k) == FLOYD_INF || distMat->get(k,j) == FLOYD_INF) ? FLOYD_INF : (distMat->get(i, k) + distMat->get(k, j));
				if (distMat->get(i, j) > tmp)
				{
					// "i��j���·��"��Ӧ��ֵ�裬Ϊ��С��һ��(������k)
					distMat->set(i, j,tmp);
					// "i��j���·��"��Ӧ��·��������k
					pathMat->set(i,j,pathMat->get(i,k));
				}
			}
		}
	}

	// ��ӡinit���·���Ľ��
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
	shortestPath.clear(); //���
	findPath(startID, endID); //�ݹ�õ�·��
	//���·���Ƿ���ȷ
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
	if (pass == start || pass == end) { //�����������
		addPath(startID, endID);
		return;
	}
	else { //�����������
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
