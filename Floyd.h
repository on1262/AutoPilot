#pragma once
#include <iomanip>
#include <iostream>
#include "qvector.h"
#include "ViewPath.h"
#include "ViewMatrix.h"
#include "ViewNode.h"
namespace autopilot {
#define FLOYD_MAX    100
#define FLOYD_INF    (~(0x1<<31))        // �����(��0X7FFFFFFF)

	// �ߵĽṹ��
	class EData
	{
	public:
		char start; // �ߵ����
		char end;   // �ߵ��յ�
		int weight; // �ߵ�Ȩ��

	public:
		EData() {}
		EData(char s, char e, int w) :start(s), end(e), weight(w) {}
	};

	class Floyd
	{
	public:
		// ����ͼ
		Floyd(QVector<autopilot::ViewPath*> paths, QVector<ViewNode*> nodes);
		//��ȡ���·��
		QVector<autopilot::ViewPath*> getShortestPath(int startID, int endID);
	private:
		QVector<int> mVexs;    // ���㼯��
		int mVexNum;             // ������
		int mEdgNum;             // ����
		ViewMatrix* mMatrix;   // �ڽӾ���
		ViewMatrix* distMat;   //�������
		ViewMatrix* pathMat;   //·������
		QVector<autopilot::ViewPath*> shortestPath; //���·�����ݴ�
		QVector<autopilot::ViewPath*> paths;
		// Floyd���·��
		void init();
		// ��ID��ȡindex
		int getIndexOfID(int _ID);
		// ��ȡͼ�еı�
		EData* getEdges();
		// �Ա߰���Ȩֵ��С��������(��С����)
		void sortEdges(EData* edges, int elen);
		//�Ӿ�������ȡ���·��
		void findPath(int startID, int endID);
		void addPath(int startID, int endID);
	};
}