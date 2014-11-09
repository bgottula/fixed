#include "boost_test.h"
#include "ComplexFixedPoint.h"
#include "FixedPoint.h"
#include <complex>

using namespace std;


BOOST_AUTO_TEST_CASE( CFxpConstructors )
{
	/* Check constructors */
	CFxp b(1, -3, 4, 1);
	BOOST_CHECK_EQUAL(b.width(), 4);
	BOOST_CHECK_EQUAL(b.fracBits(), 1);
	BOOST_CHECK_EQUAL(b.real(), 1);
	BOOST_CHECK_EQUAL(b.imag(), -3);
	CFxp c(complex<int64_t>(5, -13), 6, 3);
	BOOST_CHECK_EQUAL(c.width(), 6);
	BOOST_CHECK_EQUAL(c.fracBits(), 3);
	BOOST_CHECK_EQUAL(c.real(), 5);
	BOOST_CHECK_EQUAL(c.imag(), -13);

	/* Values too large for width */
	BOOST_CHECK_THROW(CFxp(128, 0, 8), range_error);
	BOOST_CHECK_THROW(CFxp(0, -129, 8), range_error);

	/* Width larger than max allowed width */
	BOOST_CHECK_THROW(CFxp(0, 0, CFxp::MAX_WIDTH + 1), range_error);

	/* More fractional bits than total bits */
	BOOST_CHECK_THROW(CFxp(0, 0, 2, 3), range_error);
}

BOOST_AUTO_TEST_CASE( CFxpAccessors )
{
	CFxp a(1, 2, 8, 3);

	BOOST_CHECK_EQUAL(a.real(), 1);
	BOOST_CHECK_EQUAL(a.imag(), 2);
	BOOST_CHECK_EQUAL(a.width(), 8);
	BOOST_CHECK_EQUAL(a.fracBits(), 3);
	BOOST_CHECK_EQUAL(a.minVal(), -128);
	BOOST_CHECK_EQUAL(a.maxVal(), 127);
}

BOOST_AUTO_TEST_CASE( CFxpAssignment )
{
	CFxp a(1, 2, 8);
	CFxp b(1, 2, 10);
	CFxp c(0, 0, 8);
	CFxp d(1, 2, 8, 3);

	/* lhs and rhs widths must match by default */
	BOOST_CHECK_THROW(a = b, SizeMismatchException);

	/* lhs and rhs binary point must be in same position */
	BOOST_CHECK_THROW(a = d, SizeMismatchException);

	/* Valid assignment */
	BOOST_CHECK_NO_THROW(c = a);
	BOOST_CHECK_EQUAL(c, a);
}

BOOST_AUTO_TEST_CASE( CFxpEquality )
{
	CFxp a(1, 2, 8, 3);
	CFxp b(1, 2, 8, 3);
	CFxp c(1, 2, 10, 3);
	CFxp d(13, 2, 8, 3);
	CFxp e(1, 13, 8, 3);
	CFxp f(1, 2, 8, 1);
	CFxp g(24, 38, 21, 7);

	/* same object as lhs and rhs */
	BOOST_CHECK_EQUAL(a == a, true);
	BOOST_CHECK_EQUAL(a != a, false);

	/* different equal objects */
	BOOST_CHECK_EQUAL(a == b, true);
	BOOST_CHECK_EQUAL(a != b, false);

	/* same value, different widths */
	BOOST_CHECK_EQUAL(a == c, false);
	BOOST_CHECK_EQUAL(a != c, true);

	/* different values, same widths */
	BOOST_CHECK_EQUAL(a == d, false);
	BOOST_CHECK_EQUAL(a != d, true);
	BOOST_CHECK_EQUAL(a == e, false);
	BOOST_CHECK_EQUAL(a != e, true);

	/* same value, same size, different number of fractional bits */
	BOOST_CHECK_EQUAL(a == f, false);
	BOOST_CHECK_EQUAL(a != f, true);

	/* everything different */
	BOOST_CHECK_EQUAL(a == g, false);
	BOOST_CHECK_EQUAL(a != g, true);
}

BOOST_AUTO_TEST_CASE( CFxpAddition )
{
	CFxp a(1, 2, 8);
	CFxp b(2, 5, 5);
	CFxp c(4, 12, 8, 3);

	/* addition is commutative */
	BOOST_CHECK_EQUAL(a + b, b + a);

	/* chaining works */
	BOOST_CHECK_EQUAL(a + a + a, a + a + a);

	/* width after addition */
	BOOST_CHECK_EQUAL((a + b).width(), 9);

	/* result check */
	BOOST_CHECK_EQUAL(a + b, CFxp(3, 7, 9));

	/* different number of fractional bits */
	CFxp d = a + c;
	BOOST_CHECK_EQUAL(d.width(), 12);
	BOOST_CHECK_EQUAL(d.fracBits(), 3);
	BOOST_CHECK_EQUAL(d, CFxp(12, 28, 12, 3));
	d = c + a;
	BOOST_CHECK_EQUAL(d.width(), 12);
	BOOST_CHECK_EQUAL(d.fracBits(), 3);
	BOOST_CHECK_EQUAL(d, CFxp(12, 28, 12, 3));
}

BOOST_AUTO_TEST_CASE( CFxpScalarMultiplication )
{
	CFxp a(1, 2, 8);
	Fxp b(2, 5);

	/* multiplication by scalar is commutative */
	BOOST_CHECK_EQUAL(a * b, b * a);

	/* width after multiplication by scalar */
	BOOST_CHECK_EQUAL((a * b).width(), 13);

	/* result check */
	BOOST_CHECK_EQUAL(a * b, CFxp(2, 4, 13));
}

BOOST_AUTO_TEST_CASE( CFxpMultiplication )
{
	CFxp a(1, 2, 8);
	CFxp b(2, 5, 5);

	/* complex multiplication is commutative */
	BOOST_CHECK_EQUAL(a * b, b * a);

	/* chaining works */
	BOOST_CHECK_EQUAL(a * a * a, a * a * a);

	/* width after multiplication */
	BOOST_CHECK_EQUAL((a * b).width(), 14);

	/* result check */
	BOOST_CHECK_EQUAL(a * b, CFxp(-8, 9, 14));
}


BOOST_AUTO_TEST_CASE( CFxpTruncation )
{
	CFxp a(15, -32, 10, 2);

	/* Check results for normal use */
	BOOST_CHECK_EQUAL(a.truncateBy(2), CFxp(3, -8, 8, 0));
	BOOST_CHECK_EQUAL(a.truncateTo(7), CFxp(1, -4, 7, 0));

	/* Go beyond allowed range */
	BOOST_CHECK_THROW(a.truncateTo(0), std::range_error);
	BOOST_CHECK_THROW(a.truncateBy(a.width()), std::range_error);
}

BOOST_AUTO_TEST_CASE( CFxpSaturation )
{
	CFxp a(432, -397, 10);

	/* Check results for normal use */
	BOOST_CHECK_EQUAL(a.saturateBy(2), CFxp(127, -128, 8));
	BOOST_CHECK_EQUAL(a.saturateTo(6), CFxp(31, -32, 6));

	/* Go beyond allowed range */
	BOOST_CHECK_THROW(a.saturateTo(0), std::range_error);
	BOOST_CHECK_THROW(a.saturateBy(a.width()), std::range_error);
}

BOOST_AUTO_TEST_CASE( CFxpRounding )
{
	CFxp a(15, -32, 10, 2);

	/* Check results for normal use */
	BOOST_CHECK_EQUAL(a.roundBy(2), CFxp(4, -8, 8, 0));
	BOOST_CHECK_EQUAL(a.roundTo(7), CFxp(2, -4, 7, 0));

	/* Go beyond allowed range */
	BOOST_CHECK_THROW(a.roundTo(0), std::range_error);
	BOOST_CHECK_THROW(a.roundBy(a.width()), std::range_error);
}

BOOST_AUTO_TEST_CASE( CFxpSignExtension )
{
	CFxp a(15, -32, 10);

	/* Check results for normal use */
	BOOST_CHECK_EQUAL(a.signExtendBy(2), CFxp(15, -32, 12));
	BOOST_CHECK_EQUAL(a.signExtendTo(51), CFxp(15, -32, 51));
	
	/* Try to sign extend beyond allowed range */
	BOOST_CHECK_THROW(a.signExtendTo(CFxp::MAX_WIDTH + 1), std::range_error);
	BOOST_CHECK_THROW(a.signExtendBy(CFxp::MAX_WIDTH - a.width() + 1), std::range_error);
}

BOOST_AUTO_TEST_CASE( CFxpToFloat )
{
	/* check a simple case */
	CFxp a(15, -32, 10, 1);
	BOOST_CHECK_CLOSE(a.toFloat().real(), 15.0/2.0, 0.001);
	BOOST_CHECK_CLOSE(a.toFloat().imag(), -32.0/2.0, 0.001);

	/* check an extreme case */
	CFxp b(15, -32, 64, 64);
	BOOST_CHECK_CLOSE(b.toFloat().real(), 15.0/pow(2.0, 64.0), 0.001);
	BOOST_CHECK_CLOSE(b.toFloat().imag(), -32.0/pow(2.0, 64.0), 0.001);
}
