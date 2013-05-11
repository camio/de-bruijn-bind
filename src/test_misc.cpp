#include <dbb/test/common.hpp>

using namespace dbb;
using namespace dbb::test;

BOOST_AUTO_TEST_CASE( test_dbb_misc )
{
  auto always33 = lam<1>( 33 );
  BOOST_CHECK_EQUAL( always33( "asdf" ), 33 );


  auto minusTwelve = lam<1>( app( minus
                                , _1_1
                                , 12
                                )
                           );
  BOOST_CHECK_EQUAL( minusTwelve( 24 ), 12 );

  auto negFour = lam<0>( app( times2
                            , app( minus, 2, 4 )
                            )
                       );

  BOOST_CHECK_EQUAL( negFour( ), -4 );
}
