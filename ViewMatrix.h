#pragma once
#include "qvector.h"
/*ʵ��int�Ŀɱ����*/
class ViewMatrix
{
public:
	int rows, cols;
	QVector<QVector<int>> vec; //�ڲ���cols,�����rows
	ViewMatrix(int rows, int cols, int initValue);
	int get(int rows, int cols);
	void set(int rows, int cols,int value);
};

