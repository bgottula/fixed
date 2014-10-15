#include <complex>
#include <cstdint>
#include <cassert>
#include <algorithm>


/* TODO: Perhaps throw exceptions in addition to or in place of
 * assertions.
 */

/* TODO: Add support for a binary point. This would necessitate
 * overloading various operations to adjust it. Could also
 * add methods to convert to floating-point types.
 */

class ComplexFixedPoint : public std::complex<std::int64_t>
{
public:

	ComplexFixedPoint(std::int64_t r, std::int64_t i, unsigned int width)
		: std::complex<int64_t>(r, i)
	{
		setWidth(width);
		checkSize();	
	}

	ComplexFixedPoint(std::complex<int64_t> &c, unsigned int width)
		: std::complex<int64_t>(c)
	{
		setWidth(width);
		checkSize();
	}

	static const int MAX_WIDTH = 64;

	int getWidth(void) { return m_width; }
	int64_t getMinVal(void) { return m_minVal; }
	int64_t getMaxVal(void) { return m_maxVal; }


	ComplexFixedPoint &operator = (const ComplexFixedPoint &rhs)
	{
		assert(rhs.m_width == m_width);
		std::complex<int64_t>::operator=(rhs);
		return (*this);
	}


	ComplexFixedPoint &operator >> (const int nbits)
	{
		real(real() >> nbits);
		imag(imag() >> nbits);
		return (*this);
	}

	ComplexFixedPoint &operator << (const int nbits)
	{
		real(real() << nbits);
		imag(imag() << nbits);
		return (*this);
	}

	
	// add ComplexFixedPoint to this
	ComplexFixedPoint &operator += (const ComplexFixedPoint &rhs)
	{
		setWidth(std::max(m_width, rhs.m_width) + 1);
		std::complex<int64_t>::operator+=(rhs);
		checkSize();
		return *this;
	}


	friend ComplexFixedPoint operator+(ComplexFixedPoint lhs, const ComplexFixedPoint &rhs)
	{
		return lhs += rhs;
	}

	// complex multiplication
	ComplexFixedPoint &operator *= (const ComplexFixedPoint &rhs)
	{
		setWidth(m_width + rhs.m_width + 1);
		std::complex<int64_t>::operator*=(rhs);
		checkSize();
		return *this;
	}

	friend ComplexFixedPoint operator * (ComplexFixedPoint lhs, const ComplexFixedPoint &rhs)
	{
		return lhs *= rhs;
	}
	

	ComplexFixedPoint &truncateBy(unsigned int numLsbsToRemove)
	{
		assert(numLsbsToRemove < m_width);
		setWidth(m_width - numLsbsToRemove);
		return (*this) >> numLsbsToRemove;
	}

	ComplexFixedPoint &truncateTo(unsigned int newWidth)
	{
		return truncateBy(m_width - newWidth);
	}

	ComplexFixedPoint &saturateTo(unsigned int newWidth)
	{
		assert(newWidth > 0);
		assert(newWidth <= m_width);
		setWidth(newWidth);
		
		if (real() > m_maxVal)
		{
			real(m_maxVal);
		}
		else if (real() < m_minVal)
		{
			real(m_minVal);
		}

		if (imag() > m_maxVal)
		{
			imag(m_maxVal);
		}
		else if (imag() < m_minVal)
		{
			imag(m_minVal);
		}

		return *this;
	}

	ComplexFixedPoint &saturateBy(unsigned int numMsbsToRemove)
	{
		return saturateTo(m_width - numMsbsToRemove);
	}

	ComplexFixedPoint &roundBy(unsigned int numLsbsToRemove)
	{
		assert(numLsbsToRemove < m_width);

		int roundUp = (real() >> (numLsbsToRemove - 1)) & 0x1;
		real((real() >> numLsbsToRemove) + roundUp);

		roundUp = (imag() >> (numLsbsToRemove - 1)) & 0x1;
		imag((imag() >> numLsbsToRemove) + roundUp);

		setWidth(m_width - numLsbsToRemove);

		return *this;
	}

	ComplexFixedPoint &roundTo(unsigned int newWidth)
	{
		return roundBy(m_width - newWidth);
	}

	ComplexFixedPoint &signExtendBy(unsigned int numMsbsToAdd)
	{
		assert(numMsbsToAdd >= 0);
		assert(numMsbsToAdd + m_width <= MAX_WIDTH);
		setWidth(m_width + numMsbsToAdd);
		return *this;
	}

	ComplexFixedPoint &signExtendTo(unsigned int newWidth)
	{
		return signExtendBy(newWidth - m_width);
	}



private:

	unsigned int m_width;
	std::int64_t m_maxVal;
	std::int64_t m_minVal;


	void setWidth(unsigned int width)
	{
		assert(width > 0);
		assert(width <= MAX_WIDTH);
		m_width = width;
		m_minVal = -(1LL << (width - 1));
		m_maxVal = (1LL << (width - 1)) - 1;
	}

	// throw exceptions?
	void checkSize(void)
	{
		assert(real() >= m_minVal);
		assert(real() <= m_maxVal);
		assert(imag() >= m_minVal);
		assert(imag() <= m_maxVal);
	}
};
