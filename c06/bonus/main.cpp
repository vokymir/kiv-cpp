#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

template <typename T, size_t Dim> class SquareMatrix {
private: // member attributes
  T values[Dim][Dim]{};

private: // helper functions for print
  size_t Get_Longest_Print_Len() {
    size_t longest_len = 0;
    for (int i = 0; i < Dim; i++) {
      for (int j = 0; j < Dim; j++) {
        std::ostringstream sx;
        sx << values[i][j];
        size_t len = sx.str().size();
        if (longest_len < len) {
          longest_len = len;
        }
      }
    }
    return longest_len;
  }
  void Get_Nice_Header(std::stringstream &ss, size_t line_len) {
    ss << "┌";
    for (size_t i = 0; i < line_len; i++) {
      ss << "-";
    }
    ss << "┐\n";
  }
  void Get_Nice_Footer(std::stringstream &ss, size_t line_len) {
    ss << "└";
    for (size_t i = 0; i < line_len; i++) {
      ss << "-";
    }
    ss << "┘";
  }

public: // naplneni & tisk
  void Fill_XY(size_t x, size_t y, T value) { values[x][y] = value; }
  void Print(std::string matrix_name = "") {
    // Zjisti delku nejdelsiho
    size_t longest_len = Get_Longest_Print_Len();

    // hezka hlavicka
    std::stringstream ss;
    size_t line_len = Dim * longest_len + (Dim - 1); // value, commas
    ss << matrix_name << "\n";
    Get_Nice_Header(ss, line_len);

    // hodnoty
    for (size_t i = 0; i < Dim; i++) {
      ss << "|";
      for (size_t j = 0; j < Dim; j++) {
        ss << std::setw(longest_len)
           << values[i][j]; // sirka podle nejdelsi hodnoty

        if (j == Dim - 1) {
          ss << "|\n";
        } else {
          ss << ",";
        }
      }
    }

    // hezka paticka
    Get_Nice_Footer(ss, line_len);

    // vypis
    std::cout << ss.str() << std::endl;
  }

public: // move semantika
  SquareMatrix<T, Dim>() {}

  SquareMatrix<T, Dim>(SquareMatrix<T, Dim> &&other) {
    for (size_t i = 0; i < Dim; i++) {
      for (size_t j = 0; j < Dim; j++) {
        values[i][j] = other.values[i][j];
        other.values[i][j] = 0;
      }
    }
  }

  SquareMatrix<T, Dim> &operator=(SquareMatrix<T, Dim> &&other) {
    for (size_t i = 0; i < Dim; i++) {
      for (size_t j = 0; j < Dim; j++) {
        values[i][j] = other.values[i][j];
        other.values[i][j] = 0;
      }
    }
    return *this;
  }

public: // nasobeni matice
  T Get_Mult_XY(const SquareMatrix<T, Dim> &L, const SquareMatrix<T, Dim> &R,
                size_t row, size_t col) {
    T sum = 0;
    for (size_t k = 0; k < Dim; k++) {
      sum += L.values[row][k] * R.values[k][col];
    }
    return sum;
  }

  SquareMatrix<T, Dim> operator*(SquareMatrix<T, Dim> &other) {
    SquareMatrix<T, Dim> res{};
    for (size_t i = 0; i < Dim; i++) {
      for (size_t j = 0; j < Dim; j++) {
        res.values[i][j] = Get_Mult_XY(*this, other, i, j);
      }
    }
    return res;
  }
};

int main(void) {
  SquareMatrix<double, 2> a;
  SquareMatrix<double, 2> b;

  std::cout << "PRED NAPLNENIM" << std::endl;
  a.Print("a");
  b.Print("b");

  a.Fill_XY(0, 0, 1);
  a.Fill_XY(1, 0, 2);
  a.Fill_XY(0, 1, 3);
  a.Fill_XY(1, 1, 4);

  std::cout << "PO NAPLNENIM" << std::endl;
  a.Print("a");
  b.Print("b");

  b = std::move(a);

  std::cout << "MOVE A->B" << std::endl;
  a.Print("a");
  b.Print("b");

  auto c(std::move(b));

  std::cout << "MOVE B->C" << std::endl;
  a.Print("a");
  b.Print("b");
  c.Print("c");

  a.Fill_XY(0, 0, 1);
  a.Fill_XY(1, 0, 2);
  a.Fill_XY(0, 1, 3);
  a.Fill_XY(1, 1, 4);

  std::cout << "FILL A & D = A * C" << std::endl;
  auto d = a * c;

  a.Print("a");
  b.Print("b");
  c.Print("c");
  d.Print("d");
}
