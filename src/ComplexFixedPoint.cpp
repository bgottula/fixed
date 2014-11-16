#include "ComplexFixedPoint.h"
#include <algorithm>

using namespace std;

CFxp::ComplexFixedPoint(int64_t r, int64_t i, unsigned int width,
	unsigned int fractionalBits)
	: complex<int64_t>(r, i),
	m_minHeldVal(INT64_MAX),
	m_maxHeldVal(INT64_MIN)
{
	setWidth(width);
	setFractionalBits(fractionalBits);
	checkSize();
	updateMinMaxHeldVals();
}

CFxp::ComplexFixedPoint(complex<int64_t> c, unsigned int width,
	unsigned int fractionalBits)
	: complex<int64_t>(c),
	m_minHeldVal(0),
	m_maxHeldVal(0)
{
	setWidth(width);
	setFractionalBits(fractionalBits);
	checkSize();
	updateMinMaxHeldVals();
}

CFxp CFxp::quantize(complex<double> c, unsigned int width, 
	unsigned int fractionalBits)
{
	double multiplier = pow(2.0, fractionalBits);
	return CFxp(
		(int64_t)floor(c.real() * multiplier + 0.5),
		(int64_t)floor(c.imag() * multiplier + 0.5),
		width, fractionalBits
	);
}

CFxp &CFxp::operator = (const CFxp &rhs)
{
	if (rhs.m_width != m_width || rhs.m_fracBits != m_fracBits)
	{
		throw runtime_error("Size of lhs and rhs of assignment must match");
	}
	complex<int64_t>::operator=(rhs);
	updateMinMaxHeldVals();
	return *this;
}

bool operator == (const CFxp &lhs, const CFxp & rhs)
{
	return lhs.real() == rhs.real() 
		&& lhs.imag() == rhs.imag() 
		&& lhs.m_width == rhs.m_width
		&& lhs.m_fracBits == rhs.m_fracBits;
}

bool CFxp::operator != (const CFxp &rhs)
{
	return !((*this) == rhs);
}

CFxp operator + (const CFxp &lhs, const CFxp &rhs)
{
	complex<int64_t> sum;
	int fracBitsDifference = rhs.m_fracBits - lhs.m_fracBits;
	int sumFracBits = max(rhs.m_fracBits, lhs.m_fracBits);
	int sumWidth = max(lhs.m_width, rhs.m_width) + 1 + abs(fracBitsDifference);
	if (fracBitsDifference > 0)
	{
		sum = (complex<int64_t>)lhs * (int64_t)(1LL << fracBitsDifference) 
			+ (complex<int64_t>)rhs;
	}
	else
	{
		sum = (complex<int64_t>)lhs 
			+ (complex<int64_t>)rhs * (int64_t)(1LL << -fracBitsDifference);
	}

	return CFxp(sum, sumWidth, sumFracBits);
}

CFxp operator * (const CFxp &lhs, const FixedPoint &rhs)
{
	return CFxp((complex<int64_t>)lhs * rhs.val(), 
		lhs.width() + rhs.width(), lhs.fracBits() + rhs.fracBits());
}

CFxp operator * (const FixedPoint &lhs, const CFxp &rhs)
{
	return CFxp((complex<int64_t>)rhs * lhs.val(), 
		lhs.width() + rhs.width(), lhs.fracBits() + rhs.fracBits());
}

CFxp operator * (const CFxp &lhs, const CFxp &rhs)
{
	complex<int64_t> product = (complex<int64_t>)lhs * (complex<int64_t>)rhs;
	int productWidth = lhs.m_width + rhs.m_width + 1;
	int productFracBits = lhs.m_fracBits + rhs.m_fracBits;
	return CFxp(product, productWidth, productFracBits);
}

std::ostream& operator << (std::ostream& os, const CFxp &obj)
{
	return os << obj.toDouble();
}

CFxp &CFxp::truncateBy(unsigned int numLsbsToRemove)
{
	if (numLsbsToRemove >= m_width)
	{
		throw range_error("Truncation width out of range");
	}

	setWidth(m_width - numLsbsToRemove);
	setFractionalBits(max((int)m_fracBits - (int)numLsbsToRemove, 0));
	real(real() >> numLsbsToRemove);
	imag(imag() >> numLsbsToRemove);
	return *this;
}

CFxp &CFxp::truncateTo(unsigned int newWidth)
{
	return truncateBy(m_width - newWidth);
}

CFxp &CFxp::saturateTo(unsigned int newWidth)
{
	if ((newWidth <= 0) || (newWidth > m_width))
	{
		throw range_error("Saturation width out of range");
	}

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

CFxp &CFxp::saturateBy(unsigned int numMsbsToRemove)
{
	return saturateTo(m_width - numMsbsToRemove);
}

CFxp &CFxp::roundBy(unsigned int numLsbsToRemove)
{
	if (numLsbsToRemove >= m_width)
	{
		throw range_error("Round width out of range");
	}

	int roundUp = (real() >> (numLsbsToRemove - 1)) & 0x1;
	real((real() >> numLsbsToRemove) + roundUp);

	roundUp = (imag() >> (numLsbsToRemove - 1)) & 0x1;
	imag((imag() >> numLsbsToRemove) + roundUp);

	setWidth(m_width - numLsbsToRemove);
	setFractionalBits(max((int)m_fracBits - (int)numLsbsToRemove, 0));

	return *this;
}

CFxp &CFxp::roundTo(unsigned int newWidth)
{
	return roundBy(m_width - newWidth);
}

CFxp &CFxp::signExtendBy(unsigned int numMsbsToAdd)
{
	if (numMsbsToAdd + m_width > MAX_WIDTH)
	{
		throw range_error("Sign extend width out of range");
	}

	setWidth(m_width + numMsbsToAdd);
	return *this;
}

CFxp &CFxp::signExtendTo(unsigned int newWidth)
{
	return signExtendBy(newWidth - m_width);
}

complex<float> CFxp::toFloat(void) const
{
	complex<float> result((float)real(), (float)imag());
	return result / (float)pow(2.0, m_fracBits);
}

complex<double> CFxp::toDouble(void) const
{
	complex<double> result((double)real(), (double)imag());
	return result / (double)pow(2.0, m_fracBits);
}

void CFxp::setWidth(unsigned int width)
{
	if (width > MAX_WIDTH)
	{
		throw std::range_error("Width outside allowed range");
	}
	m_width = width;
	m_minVal = -(1LL << (width - 1));
	m_maxVal = (1LL << (width - 1)) - 1;
}

void CFxp::setFractionalBits(unsigned int fractionalBits)
{
	if (fractionalBits > m_width)
	{
		throw std::range_error("Fractional bits outside allowed range");
	}
	m_fracBits = fractionalBits;
}

void CFxp::checkSize(void)
{
	if ((real() < m_minVal) || (real() > m_maxVal) || (imag() < m_minVal)
		|| (imag() > m_maxVal))
	{
		throw std::range_error("Values exceed size");
	}
}

void CFxp::updateMinMaxHeldVals(void)
{
	if (real() > m_maxHeldVal)
	{
		m_maxHeldVal = real();
	}
	
	if (imag() > m_maxHeldVal)
	{
		m_maxHeldVal = imag();
	}

	if (real() < m_minHeldVal)
	{
		m_minHeldVal = real();
	}
	
	if (imag() < m_minHeldVal)
	{
		m_minHeldVal = imag();
	}
}