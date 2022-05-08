#include <stdexcept>
#include <cstring>

template <typename T, size_t X, size_t Y, size_t Z>
class Matrix3D {
public:
	Matrix3D();
	~Matrix3D();
	Matrix3D(const Matrix3D& other);
	Matrix3D(Matrix3D&& other);
	Matrix3D& operator=(Matrix3D other);
	T* GetRawPtr();

	friend void swap(Matrix3D& a, Matrix3D& b) {
		std::swap(a.m_XZSize, b.m_XZSize);
		std::swap(a.m_Data, b.m_Data);
	}

	T& operator()(unsigned int i, unsigned int j, unsigned int k);
	T operator()(unsigned int i, unsigned int j, unsigned int k) const;
private:
    /* unsigned int m_YZSize; */
    unsigned int m_XZSize;
	T* m_Data;
};

template<typename T, size_t X, size_t Y, size_t Z>
inline Matrix3D<T, X, Y, Z>::Matrix3D() :
	/* m_YZSize(Y * Z) */
	m_XZSize(X * Z)
{
	if (X == 0 || Y == 0 || Z == 0)
		throw std::logic_error("Size must be higher than 0.");
	m_Data = new T[X * Y * Z];
}

template<typename T, size_t X, size_t Y, size_t Z>
inline Matrix3D<T, X, Y, Z>::~Matrix3D()
{
	delete[] m_Data;
}

template<typename T, size_t X, size_t Y, size_t Z>
inline Matrix3D<T, X, Y, Z>::Matrix3D(const Matrix3D& other)
{
    this->m_XZSize = other.m_XZSize;
	this->m_Data = new T[X * Y * Z];
	memcpy(this->m_Data, other.m_Data, X * Y * Z);
}

template<typename T, size_t X, size_t Y, size_t Z>
inline Matrix3D<T, X, Y, Z>::Matrix3D(Matrix3D&& other) :
	m_XZSize(0), m_Data(nullptr)
{
	swap(*this, other);
}

template<typename T, size_t X, size_t Y, size_t Z>
inline Matrix3D<T, X, Y, Z>& Matrix3D<T, X, Y, Z>::operator=(Matrix3D other)
{
	swap(*this, other);
	return *this;
}

template<typename T, size_t X, size_t Y, size_t Z>
inline T* Matrix3D<T, X, Y, Z>::GetRawPtr() {
	return m_Data;
}

template<typename T, size_t X, size_t Y, size_t Z>
inline T& Matrix3D<T, X, Y, Z>::operator()(unsigned int i, unsigned int j, unsigned int k)
{
	if (i >= X || j >= Y || k >= Z)
		throw std::logic_error("Matrix3D index out of bounds.");
    return m_Data[j * m_XZSize + k * X + i];
}

template<typename T, size_t X, size_t Y, size_t Z>
inline T Matrix3D<T, X, Y, Z>::operator()(unsigned int i, unsigned int j, unsigned int k) const
{
	if (i >= X || j >= Y || k >= Z)
		throw std::logic_error("Matrix3D index out of bounds.");
    return m_Data[j * m_XZSize + k * X + i];
}
