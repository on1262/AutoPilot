#include "ViewMatrix.h"
#include "Utils.h"

ViewMatrix::ViewMatrix(int rows, int cols,int initValue)
{
	this->rows = rows;
	this->cols = cols;
	for (int i = 0; i < rows; i++) {
		QVector<int> rowVec;
		for (int j = 0; j < cols; j++) {
			rowVec.push_back(initValue);
		}
		vec.push_back(rowVec);
	}
}

int ViewMatrix::get(int rows, int cols)
{
	if (abs(rows) > this->rows - 1 || abs(cols) > this->cols - 1) {
		Utils::log(true, "ViewMatrix: illegal position.");
		return 0;
	}
	else {
		return vec[rows][cols];
	}
}

void ViewMatrix::set(int rows, int cols, int value)
{
	this->vec[rows][cols] = value;
}
