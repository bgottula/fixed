#ifndef COMPLEX_FIXED_POINT_H
#define COMPLEX_FIXED_POINT_H

#include <complex>
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <stdexcept>
#include "FixedPoint.h"


/* TODO: Add support for a binary point. This would necessitate
 * overloading various operations to adjust it. Could also
 * add methods to convert to floating-point types.
 */

class SizeMismatchException : public std::runtime_error
{
public: SizeMismatchException(void) :
	std::runtime_error("Size of lhs and rhs of assignment must match") {};
};

class ComplexFixedPoint : public std::complex<std::int64_t>
{
public:

	ComplexFixedPoint(void)
		: m_widthMutableOnAssign(true)
	{
		setWidth(DEFAULT_WIDTH);
		checkSize();
	}

	ComplexFixedPoint(std::int64_t r, std::int64_t i, unsigned int width, bool widthMutable = false)
		: std::complex<int64_t>(r, i),
		m_widthMutableOnAssign(widthMutable)
	{
		setWidth(width);
		checkSize();
	}

	ComplexFixedPoint(std::complex<int64_t> c, unsigned int width, bool widthMutable = false)
		: std::complex<int64_t>(c),
		m_widthMutableOnAssign(widthMutable)
	{
		setWidth(width);
		checkSize();
	}

	static const int MAX_WIDTH = 64;
	static const int DEFAULT_WIDTH = 8;

	unsigned int width(void) const { return m_width; }
	int64_t minVal(void) const { return m_minVal; }
	int64_t maxVal(void) const { return m_maxVal; }


	ComplexFixedPoint &operator = (const ComplexFixedPoint &rhs)
	{
		/* Only allow width copy on assignment when default constructor was used */
		if (m_widthMutableOnAssign)
		{
			setWidth(rhs.m_width);
		}
		else if (rhs.m_width != m_width)
		{
			throw SizeMismatchException();
		}
		std::complex<int64_t>::operator=(rhs);
		checkSize();
		return *this;
	}


	bool operator == (const ComplexFixedPoint &rhs)
	{
		return real() == rhs.real() 
			&& imag() == rhs.imag() 
			&& m_width == rhs.m_width;
	}

	bool operator != (const ComplexFixedPoint &rhs)
	{
		return !((*this) == rhs);
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


	// scalar multiplication
	ComplexFixedPoint &operator *= (const FixedPoint &rhs)
	{
		setWidth(m_width + rhs.width());
		std::complex<int64_t>::operator*=(rhs.val());
		checkSize();
		return *this;
	}

	friend ComplexFixedPoint operator * (ComplexFixedPoint lhs, const FixedPoint &rhs)
	{
		return lhs *= rhs;
	}

	friend ComplexFixedPoint operator * (const FixedPoint &lhs, ComplexFixedPoint rhs)
	{
		return rhs *= lhs;
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
		assert(numMsbsToAdd + m_width <= MAX_WIDTH);
		setWidth(m_width + numMsbsToAdd);
		return *this;
	}

	ComplexFixedPoint &signExtendTo(unsigned int newWidth)
	{
		return signExtendBy(newWidth - m_width);
	}



private:

	bool m_widthMutableOnAssign;
	unsigned int m_width;
	std::int64_t m_maxVal;
	std::int64_t m_minVal;


	void setWidth(unsigned int width)
	{
		if (width > MAX_WIDTH)
		{
			throw std::range_error("Width outside allowed range");
		}
		m_width = width;
		m_minVal = -(1LL << (width - 1));
		m_maxVal = (1LL << (width - 1)) - 1;
	}

	void checkSize(void)
	{
		if ((real() < m_minVal) || (real() > m_maxVal) || (imag() < m_minVal)
			|| (imag() > m_maxVal))
		{
			throw std::range_error("Values exceed size");
		}
	}
};

typedef ComplexFixedPoint CFxp;

#endif
