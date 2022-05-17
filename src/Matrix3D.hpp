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
		std::swap(a.m_Log2XZSize, b.m_Log2XZSize);
		std::swap(a.m_Log2Z, b.m_Log2Z);
		std::swap(a.m_Data, b.m_Data);
	}

	T& operator()(unsigned int i, unsigned int j, unsigned int k);
	T operator()(unsigned int i, unsigned int j, unsigned int k) const;
private:
    unsigned int m_Log2XZSize;
	unsigned int m_Log2Z;
	T* m_Data;
};

static unsigned int MyLog2(unsigned int x) {
	int res = 0;
	while (x >>= 1) res++;
	return res;
}

template<typename T, size_t X, size_t Y, size_t Z>
inline Matrix3D<T, X, Y, Z>::Matrix3D()
{
	if (X == 0 || Y == 0 || Z == 0)
		throw std::logic_error("Size must be higher than 0.");
	m_Data = new T[X * Y * Z];
	m_Log2Z = MyLog2(Z);
	m_Log2XZSize = MyLog2(X * Z);
}

template<typename T, size_t X, size_t Y, size_t Z>
inline Matrix3D<T, X, Y, Z>::~Matrix3D()
{
	delete[] m_Data;
}

template<typename T, size_t X, size_t Y, size_t Z>
inline Matrix3D<T, X, Y, Z>::Matrix3D(const Matrix3D& other)
{
    this->m_Log2XZSize = other.m_Log2XZSize;
    this->m_Log2Z = other.m_Log2Z;
	this->m_Data = new T[X * Y * Z];
	memcpy(this->m_Data, other.m_Data, X * Y * Z);
}

template<typename T, size_t X, size_t Y, size_t Z>
inline Matrix3D<T, X, Y, Z>::Matrix3D(Matrix3D&& other) :
	m_Log2Z(0), m_Log2XZSize(0), m_Data(nullptr)
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
    return m_Data[(j << m_Log2XZSize) + (i << m_Log2Z) + k];
}

template<typename T, size_t X, size_t Y, size_t Z>
inline T Matrix3D<T, X, Y, Z>::operator()(unsigned int i, unsigned int j, unsigned int k) const
{
	if (i >= X || j >= Y || k >= Z)
		throw std::logic_error("Matrix3D index out of bounds.");
    return m_Data[(j << m_Log2XZSize) + (i << m_Log2Z) + k];
}
