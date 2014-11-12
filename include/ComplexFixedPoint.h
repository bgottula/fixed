#ifndef COMPLEX_FIXED_POINT_H
#define COMPLEX_FIXED_POINT_H

#include <complex>
#include <cstdint>
#include <stdexcept>
#include "FixedPoint.h"

class ComplexFixedPoint;
typedef ComplexFixedPoint CFxp;

class ComplexFixedPoint : public std::complex<std::int64_t>
{
public:

	static const int MAX_WIDTH = 64;

	ComplexFixedPoint(std::int64_t r, std::int64_t i, unsigned int width,
		unsigned int fractionalBits = 0);

	ComplexFixedPoint(std::complex<int64_t> c, unsigned int width,
		unsigned int fractionalBits = 0);

	static CFxp quantize(std::complex<double> c, unsigned int width, 
		unsigned int fractionalBits);

	unsigned int width(void) const { return m_width; }
	unsigned int fracBits(void) const { return m_fracBits; }
	int64_t minVal(void) const { return m_minVal; }
	int64_t maxVal(void) const { return m_maxVal; }
	int64_t minHeldVal(void) const { return m_minHeldVal; }
	int64_t maxHeldVal(void) const { return m_maxHeldVal; }

	CFxp &operator = (const CFxp &rhs);
	friend bool operator == (const CFxp &lhs, const CFxp & rhs);
	bool operator != (const CFxp &rhs);
	friend CFxp operator + (const CFxp &lhs, const CFxp &rhs);
	friend CFxp operator * (const CFxp &lhs, const FixedPoint &rhs);
	friend CFxp operator * (const FixedPoint &lhs, const CFxp &rhs);
	friend CFxp operator * (const CFxp &lhs, const CFxp &rhs);

	CFxp &truncateBy(unsigned int numLsbsToRemove);
	CFxp &truncateTo(unsigned int newWidth);
	CFxp &saturateTo(unsigned int newWidth);
	CFxp &saturateBy(unsigned int numMsbsToRemove);
	CFxp &roundBy(unsigned int numLsbsToRemove);
	CFxp &roundTo(unsigned int newWidth);
	CFxp &signExtendBy(unsigned int numMsbsToAdd);
	CFxp &signExtendTo(unsigned int newWidth);
	std::complex<float> toFloat(void) const;
	std::complex<double> toDouble(void) const;

private:

	unsigned int m_width;
	unsigned int m_fracBits;
	std::int64_t m_maxVal;
	std::int64_t m_minVal;
	std::int64_t m_minHeldVal;
	std::int64_t m_maxHeldVal;

	void setWidth(unsigned int width);
	void setFractionalBits(unsigned int fractionalBits);
	void checkSize(void);
	void updateMinMaxHeldVals(void);
};


#endif
