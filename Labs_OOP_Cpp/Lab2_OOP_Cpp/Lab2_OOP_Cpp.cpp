#include <iostream>


class RealMatrix {
public:
	// Конструкторы
	RealMatrix();
	RealMatrix(int rw, int clm);
	// Деструкторы


	// Методы
	void reduce_row_count();
	void reduce_column_count();
	void print_submatrix();
	void print_full_matrix();
	void compare_two_matrices();
	
private:
	// Поля
	int rows_ = 5;
	int columns_ = 5;
	double matrix_[5][5] = {};
};

RealMatrix::RealMatrix()
	: rows_    (5)
	, columns_ (5)
	, matrix_  {}
{
	std::cout << "Конструктор по умолчанию" << std::endl;
}

void RealMatrix::reduce_row_count() {

}

void RealMatrix::reduce_column_count() {

}

void RealMatrix::print_submatrix() {

}

void RealMatrix::print_full_matrix() {

}

void RealMatrix::compare_two_matrices() {

}


int main() {
	setlocale(LC_ALL, "RUS");
	RealMatrix obj;
}