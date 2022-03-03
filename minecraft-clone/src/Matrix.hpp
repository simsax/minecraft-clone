#include <stdexcept>

template <typename T>
class Matrix {
public:
	Matrix(unsigned int xSize, unsigned int ySize, unsigned int zSize);
	~Matrix();
	Matrix(const Matrix& other);
	Matrix(Matrix&& other);
	Matrix& operator=(Matrix other);
	void fill(const T& data);

	friend void swap(Matrix& a, Matrix& b) {
		std::swap(a.m_xSize, b.m_xSize);
		std::swap(a.m_ySize, b.m_ySize);
		std::swap(a.m_zSize, b.m_zSize);
		std::swap(a.m_Data, b.m_Data);
	}

	T& operator()(unsigned int i, unsigned int j, unsigned int k);
	T operator()(unsigned int i, unsigned int j, unsigned int k) const;
private:
	unsigned int m_xSize;
	unsigned int m_ySize;
	unsigned int m_zSize;
	T* m_Data;
};

template<typename T>
inline Matrix<T>::Matrix(unsigned int xSize, unsigned int ySize, unsigned int zSize) :
	m_xSize(xSize), m_ySize(ySize), m_zSize(zSize)
{
	if (xSize <= 0 || ySize <= 0 || zSize <= 0)
		throw std::logic_error("Size must be higher than 0.");
	m_Data = new T[xSize * ySize * zSize];
}

template<typename T>
inline Matrix<T>::~Matrix()
{
	delete[] m_Data;
}

template<typename T>
inline Matrix<T>::Matrix(const Matrix& other)
{
	this->m_xSize = other.m_xSize;
	this->m_ySize = other.m_ySize;
	this->m_zSize = other.m_zSize;
	this->m_Data = new T[m_xSize * m_ySize * m_zSize];
	memcpy(this->m_Data, other.m_Data, m_xSize * m_ySize * m_zSize);
}

template<typename T>
inline Matrix<T>::Matrix(Matrix&& other) :
	m_xSize(0), m_ySize(0), m_zSize(0), m_Data(nullptr)
{
	swap(*this, other);
}

template<typename T>
inline Matrix<T>& Matrix<T>::operator=(Matrix other)
{
	swap(*this, other);
	return *this;
}

template<typename T>
inline void Matrix<T>::fill(const T& data)
{
	for (unsigned int i = 0; i < m_xSize * m_ySize * m_zSize; i++)
		m_Data[i] = data;
}

template<typename T>
inline T& Matrix<T>::operator()(unsigned int i, unsigned int j, unsigned int k)
{
	if (i < 0 || i >= m_xSize || j < 0 || j >= m_ySize || k < 0 || k >= m_zSize)
		throw std::logic_error("Maitrx index out of bounds.");
	return m_Data[i * m_ySize * m_zSize + j * m_zSize + k];
}

template<typename T>
inline T Matrix<T>::operator()(unsigned int i, unsigned int j, unsigned int k) const
{
	if (i < 0 || i >= m_xSize || j < 0 || j >= m_ySize || k < 0 || k >= m_zSize)
		throw std::logic_error("Maitrx index out of bounds.");
	return m_Data[i * m_ySize * m_zSize + j * m_zSize + k];
}
