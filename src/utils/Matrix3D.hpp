#pragma once
#include <cstring>
#include <cassert>
#include <algorithm>
#define assertm(exp, msg) assert(((void)msg, exp))

template <typename T, size_t X, size_t Y, size_t Z> class Matrix3D {
public:
    Matrix3D();
    Matrix3D(T val);

    ~Matrix3D();
    Matrix3D(const Matrix3D& other);
    Matrix3D(Matrix3D&& other) noexcept ;
    Matrix3D& operator=(Matrix3D other);
    T* GetRawPtr();

    friend void swap(Matrix3D& a, Matrix3D& b)
    {
        std::swap(a.m_Log2XZSize, b.m_Log2XZSize);
        std::swap(a.m_Log2Z, b.m_Log2Z);
        std::swap(a.m_Data, b.m_Data);
    }

    T& operator()(uint32_t i, uint32_t j, uint32_t k);
    T operator()(uint32_t i, uint32_t j, uint32_t k) const;

private:
    uint32_t m_Log2XZSize;
    uint32_t m_Log2Z;
    T* m_Data;
};

constexpr uint32_t MyLog2(uint32_t x)
{
    int res = 0;
    while (x >>= 1)
        res++;
    return res;
}

template <typename T, size_t X, size_t Y, size_t Z> inline Matrix3D<T, X, Y, Z>::Matrix3D()
{
    static_assert(X != 0 && Y != 0 && Z != 0, "Size must be higher than 0.");
    static_assert((X & (X - 1)) == 0 && (Z & (Z - 1)) == 0, "X and Z sizes must be power of 2");

    m_Data = new T[X * Y * Z];
    m_Log2Z = MyLog2(Z);
    m_Log2XZSize = MyLog2(X * Z);
}


template<typename T, size_t X, size_t Y, size_t Z>
Matrix3D<T, X, Y, Z>::Matrix3D(T val) : Matrix3D() {
    std::fill(m_Data, m_Data + X * Y * Z, val);
}

template <typename T, size_t X, size_t Y, size_t Z> inline Matrix3D<T, X, Y, Z>::~Matrix3D()
{
    delete[] m_Data;
}

template <typename T, size_t X, size_t Y, size_t Z>
inline Matrix3D<T, X, Y, Z>::Matrix3D(const Matrix3D& other)
{
    this->m_Log2XZSize = other.m_Log2XZSize;
    this->m_Log2Z = other.m_Log2Z;
    this->m_Data = new T[X * Y * Z];
    memcpy(this->m_Data, other.m_Data, X * Y * Z);
}

template <typename T, size_t X, size_t Y, size_t Z>
inline Matrix3D<T, X, Y, Z>::Matrix3D(Matrix3D&& other) noexcept
    : m_Log2Z(0)
    , m_Log2XZSize(0)
    , m_Data(nullptr)
{
    swap(*this, other);
}

template <typename T, size_t X, size_t Y, size_t Z>
inline Matrix3D<T, X, Y, Z>& Matrix3D<T, X, Y, Z>::operator=(Matrix3D other)
{
    swap(*this, other);
    return *this;
}

template <typename T, size_t X, size_t Y, size_t Z> inline T* Matrix3D<T, X, Y, Z>::GetRawPtr()
{
    return m_Data;
}

template <typename T, size_t X, size_t Y, size_t Z>
inline T& Matrix3D<T, X, Y, Z>::operator()(uint32_t i, uint32_t j, uint32_t k)
{
    assertm(i <= X && j <= Y && k <= Z, "Matrix3D index out of bounds.");
    return m_Data[(j << m_Log2XZSize) + (i << m_Log2Z) + k];
}

template <typename T, size_t X, size_t Y, size_t Z>
inline T Matrix3D<T, X, Y, Z>::operator()(uint32_t i, uint32_t j, uint32_t k) const
{
    assertm(i <= X && j <= Y && k <= Z, "Matrix3D index out of bounds.");
    return m_Data[(j << m_Log2XZSize) + (i << m_Log2Z) + k];
}
