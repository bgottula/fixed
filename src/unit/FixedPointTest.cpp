#include "boost_test.h"
#include "FixedPoint.h"

using namespace std;

BOOST_AUTO_TEST_CASE( FxpConstructors )
{
	/* Check constructor */
	Fxp b(-3, 4, 1);
	BOOST_CHECK_EQUAL(b.width(), 4);
	BOOST_CHECK_EQUAL(b.fracBits(), 1);
	BOOST_CHECK_EQUAL(b.val(), -3);
	
	/* Values too large for width */
	BOOST_CHECK_THROW(Fxp(128, 8), range_error);
	BOOST_CHECK_THROW(Fxp(-129, 8), range_error);

	/* Width larger than max allowed width */
	BOOST_CHECK_THROW(Fxp(0, Fxp::MAX_WIDTH + 1), range_error);

	/* More fractional bits than total bits */
	BOOST_CHECK_THROW(Fxp(0, 2, 3), range_error);
}

BOOST_AUTO_TEST_CASE( FxpQuantize )
{
	double a = 2.34;
	
	/* Normal quantization */
	Fxp b = Fxp::quantize(a, 12, 4);
	BOOST_CHECK_EQUAL(b.val(), 37);
	BOOST_CHECK_EQUAL(b.width(), 12);
	BOOST_CHECK_EQUAL(b.fracBits(), 4);
	BOOST_CHECK_CLOSE(b.toDouble(), 2.3125, 0.001);

	/* Try to quantize with too few integer bits */
	BOOST_CHECK_THROW(Fxp::quantize(a, 12, 10), range_error);
}

BOOST_AUTO_TEST_CASE( FxpAccessors )
{
	Fxp a(1, 8, 3);

	BOOST_CHECK_EQUAL(a.val(), 1);
	BOOST_CHECK_EQUAL(a.width(), 8);
	BOOST_CHECK_EQUAL(a.fracBits(), 3);
	BOOST_CHECK_EQUAL(a.minVal(), -128);
	BOOST_CHECK_EQUAL(a.maxVal(), 127);
	BOOST_CHECK_EQUAL(a.maxHeldVal(), 1);
	BOOST_CHECK_EQUAL(a.minHeldVal(), 1);
	a = Fxp(123, 8, 3);
	BOOST_CHECK_EQUAL(a.maxHeldVal(), 123);
	BOOST_CHECK_EQUAL(a.minHeldVal(), 1);
	a = Fxp(-110, 8, 3);
	BOOST_CHECK_EQUAL(a.maxHeldVal(), 123);
	BOOST_CHECK_EQUAL(a.minHeldVal(), -110);
}

BOOST_AUTO_TEST_CASE( FxpAssignment )
{
	Fxp a(1, 8);
	Fxp b(1, 10);
	Fxp c(0, 8);
	Fxp d(1, 8, 3);

	/* lhs and rhs widths must match by default */
	BOOST_CHECK_THROW(a = b, runtime_error);

	/* lhs and rhs binary point must be in same position */
	BOOST_CHECK_THROW(a = d, runtime_error);

	/* Valid assignment */
	BOOST_CHECK_NO_THROW(c = a);
	BOOST_CHECK_EQUAL(c, a);
}

BOOST_AUTO_TEST_CASE( FxpEquality )
{
	Fxp a(1, 8, 3);
	Fxp b(1, 8, 3);
	Fxp c(1, 10, 3);
	Fxp d(13, 8, 3);
	Fxp e(5, 8, 3);
	Fxp f(1, 8, 1);
	Fxp g(24, 21, 7);

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

BOOST_AUTO_TEST_CASE( FxpAddition )
{
	Fxp a(1, 8);
	Fxp b(2, 5);
	Fxp c(4, 8, 3);

	/* addition is commutative */
	BOOST_CHECK_EQUAL(a + b, b + a);

	/* chaining works */
	BOOST_CHECK_EQUAL(a + a + a, a + a + a);

	/* width after addition */
	BOOST_CHECK_EQUAL((a + b).width(), 9);

	/* result check */
	BOOST_CHECK_EQUAL(a + b, Fxp(3, 9));

	/* different number of fractional bits */
	Fxp d = a + c;
	BOOST_CHECK_EQUAL(d.width(), 12);
	BOOST_CHECK_EQUAL(d.fracBits(), 3);
	BOOST_CHECK_EQUAL(d, Fxp(12, 12, 3));
	d = c + a;
	BOOST_CHECK_EQUAL(d.width(), 12);
	BOOST_CHECK_EQUAL(d.fracBits(), 3);
	BOOST_CHECK_EQUAL(d, Fxp(12, 12, 3));
}

BOOST_AUTO_TEST_CASE( FxpMultiplication )
{
	Fxp a(-13, 8);
	Fxp b(2, 5);
	Fxp c(4, 8, 3);
	Fxp d(-3, 6, 1);

	/* complex multiplication is commutative */
	BOOST_CHECK_EQUAL(a * b, b * a);

	/* chaining works */
	BOOST_CHECK_EQUAL(a * a * a, a * a * a);

	/* width after multiplication */
	BOOST_CHECK_EQUAL((a * b).width(), 13);

	/* result check */
	BOOST_CHECK_EQUAL(a * b, Fxp(-26, 13));

	/* handling of fractional bits */
	Fxp e = c * d;
	BOOST_CHECK_EQUAL(e.width(), 14);
	BOOST_CHECK_EQUAL(e.fracBits(), 4);
	BOOST_CHECK_EQUAL(e, Fxp(-12, 14, 4));
	e = d * c;
	BOOST_CHECK_EQUAL(e.width(), 14);
	BOOST_CHECK_EQUAL(e.fracBits(), 4);
	BOOST_CHECK_EQUAL(e, Fxp(-12, 14, 4));
}

BOOST_AUTO_TEST_CASE( FxpStreamInsertion )
{
	stringstream out;
	Fxp a(5, 8, 1);
	out << a;
	BOOST_CHECK_EQUAL(out.str(), "2.5");
}

BOOST_AUTO_TEST_CASE( FxpTruncation )
{
	Fxp a(15, 10, 2);

	/* Check results for normal use */
	BOOST_CHECK_EQUAL(a.truncateBy(2), Fxp(3, 8, 0));
	BOOST_CHECK_EQUAL(a.truncateTo(7), Fxp(1, 7, 0));

	/* Go beyond allowed range */
	BOOST_CHECK_THROW(a.truncateTo(0), std::range_error);
	BOOST_CHECK_THROW(a.truncateBy(a.width()), std::range_error);
}

BOOST_AUTO_TEST_CASE( FxpSaturation )
{
	Fxp a(432, 10);
	Fxp b(-467, 10);

	/* Check results for normal use */
	BOOST_CHECK_EQUAL(a.saturateBy(2), Fxp(127, 8));
	BOOST_CHECK_EQUAL(a.saturateTo(6), Fxp(31, 6));
	BOOST_CHECK_EQUAL(b.saturateBy(2), Fxp(-128, 8));
	BOOST_CHECK_EQUAL(b.saturateTo(6), Fxp(-32, 6));

	/* Go beyond allowed range */
	BOOST_CHECK_THROW(a.saturateTo(0), std::range_error);
	BOOST_CHECK_THROW(a.saturateBy(a.width()), std::range_error);
}

BOOST_AUTO_TEST_CASE( FxpRounding )
{
	Fxp a(15, 10, 2);

	/* Check results for normal use */
	BOOST_CHECK_EQUAL(a.roundBy(2), Fxp(4, 8, 0));
	BOOST_CHECK_EQUAL(a.roundTo(7), Fxp(2, 7, 0));

	/* Go beyond allowed range */
	BOOST_CHECK_THROW(a.roundTo(0), std::range_error);
	BOOST_CHECK_THROW(a.roundBy(a.width()), std::range_error);
}

BOOST_AUTO_TEST_CASE( FxpSignExtension )
{
	Fxp a(15, 10);
	Fxp b(-32, 10);

	/* Check results for normal use */
	BOOST_CHECK_EQUAL(a.signExtendBy(2), Fxp(15, 12));
	BOOST_CHECK_EQUAL(a.signExtendTo(51), Fxp(15, 51));
	BOOST_CHECK_EQUAL(b.signExtendBy(2), Fxp(-32, 12));
	BOOST_CHECK_EQUAL(b.signExtendTo(51), Fxp(-32, 51));
	
	/* Try to sign extend beyond allowed range */
	BOOST_CHECK_THROW(a.signExtendTo(Fxp::MAX_WIDTH + 1), std::range_error);
	BOOST_CHECK_THROW(a.signExtendBy(Fxp::MAX_WIDTH - a.width() + 1), std::range_error);
}

BOOST_AUTO_TEST_CASE( FxpToFloat )
{
	/* check a simple case */
	Fxp a(15, 10, 1);
	BOOST_CHECK_CLOSE(a.toFloat(), 15.0/2.0, 0.001);

	/* check an extreme case */
	Fxp b(15, 64, 64);
	BOOST_CHECK_CLOSE(b.toFloat(), 15.0/pow(2.0, 64.0), 0.001);
}

BOOST_AUTO_TEST_CASE( FxpToDouble )
{
	/* check a simple case */
	Fxp a(15, 10, 1);
	BOOST_CHECK_CLOSE(a.toDouble(), 15.0/2.0, 0.001);

	/* check an extreme case */
	Fxp b(15, 64, 64);
	BOOST_CHECK_CLOSE(b.toDouble(), 15.0/pow(2.0, 64.0), 0.001);
}
