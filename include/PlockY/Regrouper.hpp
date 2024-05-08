#pragma once
#include "Eigen/Dense"
#include <vector>
#include "Eigen/Sparse"

namespace PlockY {
   template <typename MatrixType>
    class MatrixConcatenator {
    public:
        static MatrixType concatenateHorizontally(const std::vector<MatrixType>& matrices) {
            // Default implementation
            throw std::logic_error("Not implemented for this matrix type.");
        }

        static MatrixType concatenateVertically(const std::vector<MatrixType>& matrices) {
            // Default implementation
            throw std::logic_error("Not implemented for this matrix type.");
        }
    };

// Specialization for Eigen::SparseMatrix
template <typename Scalar>
class MatrixConcatenator<Eigen::SparseMatrix<Scalar>> {
public:
    static Eigen::SparseMatrix<Scalar> concatenateHorizontally(const std::vector<Eigen::SparseMatrix<Scalar>>& matrices) {
        int totalCols = 0;
        int totalNonZeros = 0;
        int rows = matrices[0].rows();

        for (const auto& matrix : matrices) {
            totalCols += matrix.cols();
            totalNonZeros += matrix.nonZeros();
        }

        Eigen::SparseMatrix<double> result(rows, totalCols);
        result.reserve(totalNonZeros);

        int colOffset = 0;
        for (const auto& matrix : matrices) {
            for (int k = 0; k < matrix.outerSize(); ++k) {
                for (typename Eigen::SparseMatrix<Scalar>::InnerIterator it(matrix, k); it; ++it) {
                    result.insert(it.row(), colOffset + it.col()) = it.value();
                }
            }
            colOffset += matrix.cols();
        }

        return result;
    }

    static Eigen::SparseMatrix<Scalar> concatenateVertically(const std::vector<Eigen::SparseMatrix<Scalar>>& matrices) {
        int totalRows = 0;
        int totalNonZeros = 0;
        int cols = matrices[0].cols();

        for (const auto& matrix : matrices) {
            totalRows += matrix.rows();
            totalNonZeros += matrix.nonZeros();
        }

        Eigen::SparseMatrix<double> result(totalRows, cols);
        result.reserve(totalNonZeros);

        int rowOffset = 0;
        for (const auto& matrix : matrices) {
            for (int k = 0; k < matrix.outerSize(); ++k) {
                for (typename Eigen::SparseMatrix<Scalar>::InnerIterator it(matrix, k); it; ++it) {
                    result.insert(rowOffset + it.row(), it.col()) = it.value();
                }
            }
            rowOffset += matrix.rows();
        }

        return result;
    }
};

// Specialization for Eigen::Matrix (dense matrix)
template <typename Scalar>
class MatrixConcatenator<Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>> {
public:
    static Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> concatenateHorizontally(const std::vector<Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>>& matrices) {
        Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> res = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>::Zero(matrices[0].rows(), 0);
        for (const auto& matrix : matrices) {
            Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> temp = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>::Zero(matrix.rows(), res.cols() + matrix.cols());
            temp << res, matrix;
            res = temp;
        }
        return res;
    }

    static Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> concatenateVertically(const std::vector<Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>>& matrices) {
        Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> res = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>::Zero(0, matrices[0].cols());
        for (const auto& matrix : matrices) {
            Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> temp = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>::Zero(res.rows() + matrix.rows(), matrix.cols());
            temp << res, matrix;
            res = temp;
        }
        return res;
    }
};
}