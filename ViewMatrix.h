#pragma once
#include "qvector.h"
/*实现int的可变矩阵*/
class ViewMatrix
{
public:
	int rows, cols;
	QVector<QVector<int>> vec; //内层是cols,外层是rows
	ViewMatrix(int rows, int cols, int initValue);
	int get(int rows, int cols);
	void set(int rows, int cols,int value);
};

