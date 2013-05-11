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

BOOST_AUTO_TEST_CASE( test_dbb_tdepth )
{
  BOOST_MPL_ASSERT(( boost::mpl::equal_to
                      < tdepth::apply< arg<3,1> >::type
                      , boost::mpl::int_<3>
                      >
                   ));
  BOOST_MPL_ASSERT(( boost::mpl::equal_to
                      < tdepth::apply< int >::type
                      , boost::mpl::int_<0>
                      >
                   ));
  BOOST_MPL_ASSERT(( boost::mpl::equal_to
                      < tdepth::apply< Abs<1, arg<1,1> >
                                     >::type
                      , boost::mpl::int_<0>
                      >
                   ));
  BOOST_MPL_ASSERT(( boost::mpl::equal_to
                      < tdepth::apply< Abs< 1
                                          , Abs< 1
                                               , arg<1,1>
                                               >
                                          >
                                     >::type
                      , boost::mpl::int_<0>
                      >
                   ));
  BOOST_MPL_ASSERT(( boost::mpl::equal_to
                      < tdepth::apply< Abs< 1
                                          , Abs< 1
                                               , arg<3,1>
                                               >
                                          >
                                     >::type
                      , boost::mpl::int_<1>
                      >
                   ));
  BOOST_MPL_ASSERT(( boost::mpl::equal_to
                      < tdepth::apply< App< boost::mpl::void_
                                          , boost::fusion::vector
                                             < arg<3,1>
                                             , arg<5,1>
                                             >
                                          >
                                     >::type
                      , boost::mpl::int_<5>
                      >
                   ));
  BOOST_MPL_ASSERT(( boost::mpl::equal_to
                      < tdepth::apply< Abs< 1
                                          , App< boost::mpl::void_
                                               , boost::fusion::vector
                                                  < arg<3,1>
                                                  , arg<4,1>
                                                  >
                                               >
                                          >
                                     >::type
                      , boost::mpl::int_<3>
                      >
                   ));
}
