#include "boost_test.h"
#include "ComplexFixedPoint.h"

BOOST_AUTO_TEST_CASE( complexFixedPointTest )
{
	ComplexFixedPoint a(1, 2, 8);

	BOOST_CHECK_EQUAL(a.width(), 8);
}
