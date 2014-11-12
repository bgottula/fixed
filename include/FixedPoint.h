#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <cstdint>
#include <stdexcept>
#include <iostream>

class FixedPoint;
typedef FixedPoint Fxp;

class FixedPoint
{
public:

	static const int MAX_WIDTH = 64;

	FixedPoint(std::int64_t v, unsigned int width, 
		unsigned int fractionalBits = 0);

	static Fxp quantize(double v, unsigned int width, 
		unsigned int fractionalBits);

	int64_t val(void) const { return m_val; }
	unsigned int width(void) const { return m_width; }
	unsigned int fracBits(void) const { return m_fracBits; }
	int64_t minVal(void) const { return m_minVal; }
	int64_t maxVal(void) const { return m_maxVal; }
	int64_t minHeldVal(void) const { return m_minHeldVal; }
	int64_t maxHeldVal(void) const { return m_maxHeldVal; }

	Fxp &operator = (const Fxp &rhs);
	friend bool operator == (const Fxp &lhs, const Fxp & rhs);
	bool operator != (const Fxp &rhs);
	friend Fxp operator + (const Fxp &lhs, const Fxp &rhs);
	friend Fxp operator * (const Fxp &lhs, const Fxp &rhs);
	friend std::ostream& operator << (std::ostream& os, const Fxp &obj);

	Fxp &truncateBy(unsigned int numLsbsToRemove);
	Fxp &truncateTo(unsigned int newWidth);
	Fxp &saturateTo(unsigned int newWidth);
	Fxp &saturateBy(unsigned int numMsbsToRemove);
	Fxp &roundBy(unsigned int numLsbsToRemove);
	Fxp &roundTo(unsigned int newWidth);
	Fxp &signExtendBy(unsigned int numMsbsToAdd);
	Fxp &signExtendTo(unsigned int newWidth);
	float toFloat(void) const;
	double toDouble(void) const;

private:

	std::int64_t m_val;
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
