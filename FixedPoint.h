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

class FixedPoint : public std::complex<std::int64_t>
{
public:

	FixedPoint(std::int64_t r, std::int64_t i, unsigned int width)
		: std::complex<int64_t>(r, i)
	{
		setWidth(width);
		checkSize();	
	}

	FixedPoint(std::complex<int64_t> &c, unsigned int width)
		: std::complex<int64_t>(c)
	{
		setWidth(width);
		checkSize();
	}

	static const int MAX_WIDTH = 64;

	int getWidth(void) { return m_width; }
	int64_t getMinVal(void) { return m_minVal; }
	int64_t getMaxVal(void) { return m_maxVal; }


	FixedPoint &operator = (const FixedPoint &rhs)
	{
		assert(rhs.m_width == m_width);
		std::complex<int64_t>::operator=(rhs);
		return (*this);
	}


	FixedPoint &operator >> (const int nbits)
	{
		this->_Val[_RE] >>= nbits;
		this->_Val[_IM] >>= nbits;
		return (*this);
	}

	FixedPoint &operator << (const int nbits)
	{
		this->_Val[_RE] <<= nbits;
		this->_Val[_IM] <<= nbits;
		return (*this);
	}

	
	// add FixedPoint to this
	FixedPoint &operator += (const FixedPoint &rhs)
	{
		/* Should the size change? */
		//setWidth(std::max(m_width, rhs.m_width) + 1);
		
		std::complex<int64_t>::operator+=(rhs);
		checkSize();
		return *this;
	}


	friend FixedPoint operator+(const FixedPoint &lhs, const FixedPoint &rhs)
	{
		return lhs += rhs;
	}
	

	FixedPoint &truncateBy(unsigned int numLsbsToRemove)
	{
		assert(numLsbsToRemove < m_width);
		setWidth(m_width - numLsbsToRemove);
		this->_Val[_RE] >>= numLsbsToRemove;
		this->_Val[_IM] >>= numLsbsToRemove;
		return *this;
	}

	FixedPoint &truncateTo(unsigned int newWidth)
	{
		return truncateBy(m_width - newWidth);
	}

	FixedPoint &saturateTo(unsigned int newWidth)
	{
		assert(newWidth > 0);
		assert(newWidth <= m_width);
		setWidth(newWidth);
		
		if (this->_Val[_RE] > m_maxVal)
		{
			this->_Val[_RE] = m_maxVal;
		}
		else if (this->_Val[_RE] < m_minVal)
		{
			this->_Val[_RE] = m_minVal;
		}

		if (this->_Val[_IM] > m_maxVal)
		{
			this->_Val[_IM] = m_maxVal;
		}
		else if (this->_Val[_IM] < m_minVal)
		{
			this->_Val[_IM] = m_minVal;
		}

		return *this;
	}

	FixedPoint &saturateBy(unsigned int numMsbsToRemove)
	{
		return saturateTo(m_width - numMsbsToRemove);
	}

	FixedPoint &roundBy(unsigned int numLsbsToRemove)
	{
		assert(numLsbsToRemove < m_width);

		int roundUp = (this->_Val[_RE] >> (numLsbsToRemove - 1)) & 0x1;
		this->_Val[_RE] >>= numLsbsToRemove;
		this->_Val[_RE] += roundUp;

		roundUp = (this->_Val[_IM] >> (numLsbsToRemove - 1)) & 0x1;
		this->_Val[_IM] >>= numLsbsToRemove;
		this->_Val[_IM] += roundUp;

		setWidth(m_width - numLsbsToRemove);

		return *this;
	}

	FixedPoint &roundTo(unsigned int newWidth)
	{
		return roundBy(m_width - newWidth);
	}

	FixedPoint &signExtendBy(unsigned int numMsbsToAdd)
	{
		assert(numMsbsToAdd >= 0);
		assert(numMsbsToAdd + m_width <= MAX_WIDTH);
		setWidth(m_width + numMsbsToAdd);
		return *this;
	}

	FixedPoint &signExtendTo(unsigned int newWidth)
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
		assert(this->real() >= m_minVal);
		assert(this->real() <= m_maxVal);
		assert(this->imag() >= m_minVal);
		assert(this->imag() <= m_maxVal);
	}
};
