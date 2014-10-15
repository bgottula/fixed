#ifndef FIXED_POINT_H
#define FIXED_POINT_H

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

class FixedPoint
{
public:

	FixedPoint(std::int64_t r, unsigned int width)
		: m_val(r)
	{
		setWidth(width);
		checkSize();	
	}

	static const int MAX_WIDTH = 64;

	int64_t val(void) const { return m_val; }
	int width(void) const { return m_width; }
	int64_t minVal(void) const { return m_minVal; }
	int64_t maxVal(void) const { return m_maxVal; }


	FixedPoint &operator = (const FixedPoint &rhs)
	{
		assert(rhs.m_width == m_width);
		m_val = rhs.m_val;
		return *this;
	}


	FixedPoint &operator >> (const int nbits)
	{
		m_val >> nbits;
		return *this;
	}

	FixedPoint &operator << (const int nbits)
	{
		m_val << nbits;
		return *this;
	}

	
	// add FixedPoint to this
	FixedPoint &operator += (const FixedPoint &rhs)
	{
		setWidth(std::max(m_width, rhs.m_width) + 1);
		m_val += rhs.m_val;
		checkSize();
		return *this;
	}


	friend FixedPoint operator+(FixedPoint lhs, const FixedPoint &rhs)
	{
		return lhs += rhs;
	}

	// multiplication
	FixedPoint &operator *= (const FixedPoint &rhs)
	{
		setWidth(m_width + rhs.m_width);
		m_val *= rhs.m_val;
		checkSize();
		return *this;
	}

	friend FixedPoint operator * (FixedPoint lhs, const FixedPoint &rhs)
	{
		return lhs *= rhs;
	}
	

	FixedPoint &truncateBy(unsigned int numLsbsToRemove)
	{
		assert(numLsbsToRemove < m_width);
		setWidth(m_width - numLsbsToRemove);
		return (*this) >> numLsbsToRemove;
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
		
		if (m_val > m_maxVal)
		{
			m_val = m_maxVal;
		}
		else if (m_val < m_minVal)
		{
			m_val = m_minVal;
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

		int roundUp = (m_val >> (numLsbsToRemove - 1)) & 0x1;
		m_val >>= numLsbsToRemove;
		m_val += roundUp;

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

	std::int64_t m_val;

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
		assert(m_val >= m_minVal);
		assert(m_val <= m_maxVal);
	}
};

#endif
