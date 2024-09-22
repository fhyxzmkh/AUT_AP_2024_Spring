#ifndef AUT_AP_2024_Spring_HW1
#define AUT_AP_2024_Spring_HW1

#include <format>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <vector>

namespace algebra {

    // 矩阵数据结构
    template<typename T>
    using MATRIX = std::vector<std::vector<T>>;

    // 矩阵初始化类型
    enum class MatrixType { Zeros,
                            Ones,
                            Identity,
                            Random };

    // 矩阵初始化函数模板
    template<typename T>
    MATRIX<T> create_matrix(std::size_t rows, std::size_t columns,
                            std::optional<MatrixType> type = MatrixType::Zeros,
                            std::optional<T> lowerBound = std::nullopt,
                            std::optional<T> upperBound = std::nullopt) {

        if (rows <= 0 || columns <= 0)
            throw std::invalid_argument("Matrix dimensions must be positive.");

        MATRIX<T> mtx{};

        if (type.value() == MatrixType::Zeros) {
            mtx.resize(rows, std::vector<T>(columns, 0));
        } else if (type.value() == MatrixType::Ones) {
            mtx.resize(rows, std::vector<T>(columns, 1));
        } else if (type.value() == MatrixType::Identity) {
            if (rows != columns)
                throw std::invalid_argument("Identity matrix must be square.");

            mtx.resize(rows, std::vector<T>(columns, 0));

            for (std::size_t i = 0; i < rows; ++i)
                for (std::size_t j = 0; j < columns; ++j)
                    if (i == j)
                        mtx[i][j] = 1;
        } else {
            if (lowerBound >= upperBound)
                throw std::invalid_argument("Invalid bounds for random matrix.");

            mtx.resize(rows, std::vector<T>(columns, 0));

            std::random_device rd{};// 用于生成随机种子
            std::mt19937 rand(rd());// 使用 Mersenne Twister 引擎
            std::uniform_real_distribution<> dis(lowerBound.value(), upperBound.value() + std::numeric_limits<double>::epsilon());

            for (std::size_t i = 0; i < rows; ++i)
                mtx[i][i] = dis(rand);
        }

        return std::move(mtx);
    }

    template<typename T>
    void display(const MATRIX<T> &matrix) {
        for (const auto &row: matrix) {
            std::string row_str;
            for (const auto &elem: row) {
                row_str += std::format("|{:^7}", elem);// 每个元素占 4 个字符宽度
            }
            std::cout << row_str << '|' << std::endl;
        }
    }

    template<typename T>
    MATRIX<T> sum_sub(const MATRIX<T> &matrixA,
                      const MATRIX<T> &matrixB,
                      std::optional<std::string> operation = "sum") {

        if (matrixA.empty() && matrixB.empty())
            return {};

        int row_a = matrixA.size(), col_a = matrixA[0].size();
        int row_b = matrixB.size(), col_b = matrixB[0].size();

        if (row_a != row_b || col_a != col_b)
            throw std::invalid_argument("Matrix dimension mismatch.");

        MATRIX<T> res{matrixA};

        if (operation.value() == "sub") {
            for (int i = 0; i < row_a; ++i)
                for (int j = 0; j < col_a; ++j)
                    res[i][j] -= matrixB[i][j];
        } else {
            for (int i = 0; i < row_a; ++i)
                for (int j = 0; j < col_a; ++j)
                    res[i][j] += matrixB[i][j];
        }

        return std::move(res);
    }

    template<typename T>
    MATRIX<T> multiply(const MATRIX<T> &matrix, const T scalar) {
        if (matrix.empty())
            return {};

        int rows = matrix.size(), cols = matrix[0].size();

        MATRIX<T> res{matrix};

        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                res[i][j] *= scalar;

        return std::move(res);
    }

    template<typename T>
    MATRIX<T> multiply(const MATRIX<T> &matrixA, const MATRIX<T> &matrixB) {
        if (matrixA.empty() && matrixB.empty())
            throw std::invalid_argument("Matrices must not be empty.");

        int row_a = matrixA.size(), col_a = matrixA[0].size();
        int row_b = matrixB.size(), col_b = matrixB[0].size();

        if (col_a != row_b)
            throw std::invalid_argument("Matrix dimension mismatch.");

        MATRIX<T> res(row_a, std::vector<T>(col_b, 0));

        for (int i = 0; i < row_a; ++i) {
            for (int j = 0; j < col_b; ++j) {
                for (int k = 0; k < col_a; ++k)
                    res[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }

        return std::move(res);
    }

    template<typename T>
    MATRIX<T> hadamard_product(const MATRIX<T> &matrixA, const MATRIX<T> &matrixB) {
        if (matrixA.empty() && matrixB.empty())
            return {};

        int row_a = matrixA.size(), col_a = matrixA[0].size();
        int row_b = matrixB.size(), col_b = matrixB[0].size();

        if (row_a != row_b || col_a != col_b)
            throw std::invalid_argument("Matrix dimension mismatch.");

        MATRIX<T> res(row_a, std::vector<T>(col_a, 0));

        for (int i = 0; i < row_a; ++i)
            for (int j = 0; j < col_a; ++j)
                res[i][j] = matrixA[i][j] * matrixB[i][j];

        return std::move(res);
    }

    template<typename T>
    MATRIX<T> transpose(const MATRIX<T> &matrix) {
        if (matrix.empty())
            return {};

        int rows = matrix.size();
        int cols = matrix[0].size();

        MATRIX<T> zeroMtx(rows, std::vector<T>(cols, 0));
        if (matrix == zeroMtx)
            return std::move(zeroMtx);

        MATRIX<T> res(cols, std::vector<T>(rows, 0));

        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                res[j][i] = matrix[i][j];

        return std::move(res);
    }

    template<typename T>
    T trace(const MATRIX<T> &matrix) {
        if (matrix.empty())
            throw std::invalid_argument("Matrices must not be empty.");

        int rows = matrix.size();
        int cols = matrix[0].size();

        if (rows != cols)
            throw std::invalid_argument("Identity matrix must be square.");

        T res{};

        for (int i = 0; i < rows; ++i)
            res += matrix[i][i];

        return res;
    }

    template<typename T>
    MATRIX<T> getSubMatrix(const MATRIX<T> &matrix, int skipRow, int skipCol) {
        int n = matrix.size();
        MATRIX<T> subMatrix(n - 1, std::vector<T>(n - 1));
        int rowIndex = 0, colIndex = 0;

        for (int i = 0; i < n; ++i) {
            if (i == skipRow) continue;
            colIndex = 0;
            for (int j = 0; j < n; ++j) {
                if (j == skipCol) continue;
                subMatrix[rowIndex][colIndex] = matrix[i][j];
                ++colIndex;
            }
            ++rowIndex;
        }

        return std::move(subMatrix);
    }

    template<typename T>
    double determinant(const MATRIX<T> &matrix) {
        if (matrix.empty())
            throw std::invalid_argument("Matrices must not be empty.");

        int rows = matrix.size();
        int cols = matrix[0].size();

        if (rows != cols)
            throw std::invalid_argument("Identity matrix must be square.");

        if (rows == 1)
            return matrix[0][0];

        if (rows == 2)
            return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];

        double det{};
        int sign = 1;

        for (int j = 0; j < rows; ++j) {
            MATRIX<T> subMatrix = getSubMatrix(matrix, 0, j);
            det += static_cast<double>(sign * matrix[0][j] * determinant(subMatrix));
            sign = -sign;
        }

        return det;
    }

    template<typename T>
    MATRIX<T> getAdjointMatrix(const MATRIX<T> &matrix) {
        int n = matrix.size();
        MATRIX<T> adj(n, std::vector<T>(n));

        if (n == 1) {
            adj[0][0] = 1;
            return adj;
        }

        int sign = 1;
        MATRIX<T> temp(n, std::vector<T>(n));

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                MATRIX<T> subMatrix = getSubMatrix(matrix, i, j);
                sign = ((i + j) % 2 == 0) ? 1 : -1;
                adj[j][i] = sign * determinant(subMatrix);
            }
        }

        return adj;
    }

    template<typename T>
    MATRIX<double> inverse(const MATRIX<T> &matrix) {
        if (matrix.empty())
            throw std::invalid_argument("Matrices must not be empty.");

        int rows = matrix.size();
        int cols = matrix[0].size();

        if (rows != cols)
            throw std::invalid_argument("Identity matrix must be square.");

        auto det = determinant(matrix);
        if (det == 0.)
            throw std::invalid_argument("Singular matrix.");

        MATRIX<double> adj = getAdjointMatrix(matrix);
        MATRIX<double> inv(rows, std::vector<double>(cols));

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                inv[i][j] = adj[i][j] / det;
            }
        }

        return inv;
    }

}// namespace algebra

#endif// AUT_AP_2024_Spring_HW1
