#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>
#include "../../HuffmanCompressor/src/Compresser.h"


BOOST_AUTO_TEST_CASE(myTestCase)
{
	CompressWriter comp("SDSD");
  BOOST_TEST(comp.fullPathToFile == "SDdSD");
}