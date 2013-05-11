#ifndef DBB_TEST_COMMON_HPP_
#define DBB_TEST_COMMON_HPP_

#include <boost/test/unit_test.hpp>
#include <dbb/dbb.hpp>

namespace dbb { namespace test {

  inline int times3( int a )
  {
    return a*3;
  }

  inline int minus( int a, int b )
  {
    return a - b;
  }

  inline int times2( int a )
  {
    return a*2;
  }

} }

#endif
