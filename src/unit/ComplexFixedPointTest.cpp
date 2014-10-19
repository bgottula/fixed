#include "boost_test.h"
#include "ComplexFixedPoint.h"
#include <complex>

using namespace std;


BOOST_AUTO_TEST_CASE( CFxpConstructors )
{
	/* Check default constructor */
	CFxp a;
	const unsigned int D_WIDTH = CFxp::DEFAULT_WIDTH;
	BOOST_CHECK_EQUAL(a.width(), D_WIDTH);
	BOOST_CHECK_EQUAL(a.real(), 0);
	BOOST_CHECK_EQUAL(a.imag(), 0);

	/* Check non-default constructors */
	CFxp b(1, -3, 4);
	BOOST_CHECK_EQUAL(b.width(), 4);
	BOOST_CHECK_EQUAL(b.real(), 1);
	BOOST_CHECK_EQUAL(b.imag(), -3);
	CFxp c(complex<int64_t>(5, -13), 6);
	BOOST_CHECK_EQUAL(c.width(), 6);
	BOOST_CHECK_EQUAL(c.real(), 5);
	BOOST_CHECK_EQUAL(c.imag(), -13);

	/* Values too large for width */
	BOOST_CHECK_THROW(CFxp(128, 0, 8), range_error);
	BOOST_CHECK_THROW(CFxp(0, -129, 8), range_error);

	/* Width larger than max allowed width */
	BOOST_CHECK_THROW(CFxp(0, 0, CFxp::MAX_WIDTH + 1), range_error);
}


BOOST_AUTO_TEST_CASE( CFxpAccessors )
{
	CFxp a(1, 2, 8);

	BOOST_CHECK_EQUAL(a.real(), 1);
	BOOST_CHECK_EQUAL(a.imag(), 2);
	BOOST_CHECK_EQUAL(a.width(), 8);
	BOOST_CHECK_EQUAL(a.minVal(), -128);
	BOOST_CHECK_EQUAL(a.maxVal(), 127);
}


BOOST_AUTO_TEST_CASE( CFxpAssignment )
{
	CFxp a(1, 2, 8);
	CFxp b(1, 2, 10);
	CFxp c;
	CFxp d(1, 2, 8, true);
	CFxp e(1, 2, 8, false);

	/* lhs and rhs widths must match by default */
	BOOST_CHECK_THROW(a = b, SizeMismatchException);

	/* Default constructor allows mutable width on assignment */
	BOOST_CHECK_NO_THROW(c = b);
	BOOST_CHECK_EQUAL(c, a);

	/* Explicitly set mutable width on assignment to true in d constructor */
	BOOST_CHECK_NO_THROW(d = b);
	BOOST_CHECK_EQUAL(d, b);

	/* Explicitly set mutable width on assignment to false in e constructor */
	BOOST_CHECK_THROW(e = b, SizeMismatchException);
}


BOOST_AUTO_TEST_CASE( CFxpEquality )
{
	CFxp a(1, 2, 8);
	CFxp b(1, 2, 8);
	CFxp c(1, 2, 10);
	CFxp d(13, 2, 8);
	CFxp e(1, 13, 8);
	CFxp f(24, 38, 21);

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

	/* everything different */
	BOOST_CHECK_EQUAL(a == f, false);
	BOOST_CHECK_EQUAL(a != f, true);
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


BOOST_AUTO_TEST_CASE( CFxpAddition )
{
	CFxp a(1, 2, 8);
	CFxp b(2, 5, 5);

	/* addition is commutative */
	BOOST_CHECK_EQUAL(a + b, b + a);

	/* chaining works */
	BOOST_CHECK_EQUAL(a + a + a, a + a + a);

	/* width after addition */
	BOOST_CHECK_EQUAL((a + b).width(), 9);

	/* result check */
	BOOST_CHECK_EQUAL(a + b, CFxp(3, 7, 9));
}


BOOST_AUTO_TEST_CASE( CFxpTruncation )
{
	CFxp a(15, -32, 10);

	BOOST_CHECK_EQUAL(a.truncateBy(2), CFxp(3, -8, 8));
	BOOST_CHECK_EQUAL(a.truncateTo(7), CFxp(1, -4, 7));
}


BOOST_AUTO_TEST_CASE( CFxpRounding )
{
	CFxp a(15, -32, 10);

	BOOST_CHECK_EQUAL(a.roundBy(2), CFxp(4, -8, 8));
	BOOST_CHECK_EQUAL(a.roundTo(7), CFxp(2, -4, 7));
}
