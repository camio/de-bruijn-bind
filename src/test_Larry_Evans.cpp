#include <dbb/test/common.hpp>

using namespace dbb;
using namespace dbb::test;

static int next( int i )
{
  return i+1;
}

static int foo()
{
  auto op = lam<1>( app(next,_1_1 ) );
  int op_1 = op(1);
  auto lam_app = lam<1>( app(op,_1_1) );
  int lam_app_1 = lam_app(1);
  return op_1 + lam_app_1;
}

BOOST_AUTO_TEST_CASE( test_dbb_Larry_Evans )
{
  BOOST_CHECK_EQUAL( foo(), 4 );
}
