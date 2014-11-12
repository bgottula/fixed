#include "FixedPoint.h"
#include <math.h>

using namespace std;

Fxp::FixedPoint(int64_t v, unsigned int width, unsigned int fractionalBits)
	: m_val(v),
	m_minHeldVal(INT64_MAX),
	m_maxHeldVal(INT64_MIN)
{
	setWidth(width);
	setFractionalBits(fractionalBits);
	checkSize();
	updateMinMaxHeldVals();
}

Fxp Fxp::quantize(double v, unsigned int width, unsigned int fractionalBits)
{
	double multiplier = pow(2.0, fractionalBits);
	return Fxp(
		(int64_t)floor(v * multiplier + 0.5),
		width, fractionalBits
	);
}

Fxp &Fxp::operator = (const Fxp &rhs)
{
	if (rhs.m_width != m_width || rhs.m_fracBits != m_fracBits)
	{
		throw runtime_error("Size of lhs and rhs of assignment must match");
	}
	m_val = rhs.m_val;
	updateMinMaxHeldVals();
	return *this;
}

bool operator == (const Fxp &lhs, const Fxp & rhs)
{
	return lhs.m_val == rhs.m_val 
		&& lhs.m_width == rhs.m_width
		&& lhs.m_fracBits == rhs.m_fracBits;
}

bool Fxp::operator != (const Fxp &rhs)
{
	return !((*this) == rhs);
}

Fxp operator + (const Fxp &lhs, const Fxp &rhs)
{
	int64_t sum;
	int fracBitsDifference = rhs.m_fracBits - lhs.m_fracBits;
	int sumFracBits = max(rhs.m_fracBits, lhs.m_fracBits);
	int sumWidth = max(lhs.m_width, rhs.m_width) + 1 + abs(fracBitsDifference);
	if (fracBitsDifference > 0)
	{
		sum = lhs.m_val * (1L << fracBitsDifference) + rhs.m_val;
	}
	else
	{
		sum = lhs.m_val	+ rhs.m_val * (1L << -fracBitsDifference);
	}

	return Fxp(sum, sumWidth, sumFracBits);
}

Fxp operator * (const Fxp &lhs, const Fxp &rhs)
{
	return Fxp(lhs.m_val * rhs.m_val, lhs.m_width + rhs.m_width, 
		lhs.m_fracBits + rhs.m_fracBits);
}

std::ostream& operator << (std::ostream& os, const Fxp &obj)
{
	return os << obj.toDouble();
}

Fxp &Fxp::truncateBy(unsigned int numLsbsToRemove)
{
	if (numLsbsToRemove >= m_width)
	{
		throw range_error("Truncation width out of range");
	}

	setWidth(m_width - numLsbsToRemove);
	setFractionalBits(max((int)m_fracBits - (int)numLsbsToRemove, 0));
	m_val >>= numLsbsToRemove;
	return *this;
}

Fxp &Fxp::truncateTo(unsigned int newWidth)
{
	return truncateBy(m_width - newWidth);
}

Fxp &Fxp::saturateTo(unsigned int newWidth)
{
	if ((newWidth <= 0) || (newWidth > m_width))
	{
		throw range_error("Saturation width out of range");
	}

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

Fxp &Fxp::saturateBy(unsigned int numMsbsToRemove)
{
	return saturateTo(m_width - numMsbsToRemove);
}

Fxp &Fxp::roundBy(unsigned int numLsbsToRemove)
{
	if (numLsbsToRemove >= m_width)
	{
		throw range_error("Round width out of range");
	}

	int roundUp = (m_val >> (numLsbsToRemove - 1)) & 0x1;
	m_val = (m_val >> numLsbsToRemove) + roundUp;

	setWidth(m_width - numLsbsToRemove);
	setFractionalBits(max((int)m_fracBits - (int)numLsbsToRemove, 0));

	return *this;
}

Fxp &Fxp::roundTo(unsigned int newWidth)
{
	return roundBy(m_width - newWidth);
}

Fxp &Fxp::signExtendBy(unsigned int numMsbsToAdd)
{
	if (numMsbsToAdd + m_width > MAX_WIDTH)
	{
		throw range_error("Sign extend width out of range");
	}

	setWidth(m_width + numMsbsToAdd);
	return *this;
}

Fxp &Fxp::signExtendTo(unsigned int newWidth)
{
	return signExtendBy(newWidth - m_width);
}

float Fxp::toFloat(void) const
{
	return (float)m_val / (float)pow(2.0, m_fracBits);
}

double Fxp::toDouble(void) const
{
	return (double)m_val / (double)pow(2.0, m_fracBits);
}

void Fxp::setWidth(unsigned int width)
{
	if (width > MAX_WIDTH)
	{
		throw std::range_error("Width outside allowed range");
	}
	m_width = width;
	m_minVal = -(1LL << (width - 1));
	m_maxVal = (1LL << (width - 1)) - 1;
}

void Fxp::setFractionalBits(unsigned int fractionalBits)
{
	if (fractionalBits > m_width)
	{
		throw std::range_error("Fractional bits outside allowed range");
	}
	m_fracBits = fractionalBits;
}

void Fxp::checkSize(void)
{
	if ((m_val < m_minVal) || (m_val > m_maxVal))
	{
		throw std::range_error("Values exceed size");
	}
}

void Fxp::updateMinMaxHeldVals(void)
{
	if (m_val > m_maxHeldVal)
	{
		m_maxHeldVal = m_val;
	}

	if (m_val < m_minHeldVal)
	{
		m_minHeldVal = m_val;
	}
}