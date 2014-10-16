#ifndef BOOST_TEST_H
#define BOOST_TEST_H

#if USE_BOOST_UTF_DYNAMIC
	#define BOOST_TEST_DYN_LINK
	#include <boost/test/unit_test.hpp>
#elif USE_BOOST_UTF_STATIC
	#include <boost/test/unit_test.hpp>
#elif USE_BOOST_UTF_HEADER
	#include <boost/test/included/unit_test.hpp>
#else
	#error must define one of: USE_BOOST_UTF_{DYNAMIC,STATIC,HEADER}
#endif

#endif
