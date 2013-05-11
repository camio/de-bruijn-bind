#include <dbb/test/common.hpp>
#include <dbb/algorithms.hpp>

namespace {
  struct Foo
  {
    typedef int result_type;
    int operator()( int a ) const
    {
      return a*2;
    }
  } const foo = Foo();

  Foo curriedF( int i )
  {
    return foo;
  }
}

BOOST_AUTO_TEST_CASE( test_dbb_currying )
{
  BOOST_CHECK_EQUAL( dbb::curry( dbb::test::minus )( 3 )( 2 ), 1 );
  BOOST_CHECK_EQUAL( dbb::uncurry( curriedF )( 3, 2 ), 4 );
}

BOOST_AUTO_TEST_CASE( test_dbb_compose )
{
  BOOST_CHECK_EQUAL( dbb::compose( dbb::test::times2, dbb::test::times3 )( 4 ), 24 );
}

BOOST_AUTO_TEST_CASE( test_dbb_id )
{
  BOOST_CHECK_EQUAL( dbb::id( 'a' ), 'a' );
}

BOOST_AUTO_TEST_CASE( test_dbb_const )
{
  BOOST_CHECK_EQUAL( dbb::const_( "asdf" )( 12 ), "asdf" );
}

BOOST_AUTO_TEST_CASE( test_dbb_flip )
{
  BOOST_CHECK_EQUAL( dbb::flip( dbb::test::minus )( 10, 2 ), -8 );
}
