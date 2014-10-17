#include "boost_test.h"
#include "ComplexFixedPoint.h"
#include <complex>

using namespace std;


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

	BOOST_CHECK_THROW(a = b, SizeMismatchException);
	BOOST_CHECK_NO_THROW(c = a);
	BOOST_CHECK_EQUAL(c, a);
	BOOST_CHECK_NO_THROW(d = b);
	BOOST_CHECK_EQUAL(d, b);
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
