#include <dbb/dbb.hpp>
#include <iostream>

int minus( int a, int b )
{
  return a - b;
}

int times2( int a )
{
  return a*2;
}

#define BOOST_TEST_MODULE odillo test suite
#define BOOST_TEST_ALTERNATIVE_INIT_API
#define BOOST_TEST_MAIN
//#define BOOST_TEST_NO_MAIN
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE( test_dbb )
{
  auto id = lam<1>( _1_1 );
  BOOST_CHECK_EQUAL( id( 'a' ), 'a' );

  auto always33 = lam<1>( 33 );
  BOOST_CHECK_EQUAL( always33( "asdf" ), 33 );

  auto const_ = lam<1>( lam<1>( _2_1 ) );
  BOOST_CHECK_EQUAL( const_( "asdf" )( 12 ), "asdf" );

  auto minusTwelve = lam<1>( app( minus
                                , _1_1
                                , 12
                                )
                           );
  BOOST_CHECK_EQUAL( minusTwelve( 24 ), 12 );

  auto flip   = lam<1>( lam<2>( app( _2_1
                                   , _1_2
                                   , _1_1
                                   )
                              )
                      );
  BOOST_CHECK_EQUAL( flip( minus )( 10, 2 ), -8 );

  auto negFour = lam<0>( app( times2
                            , app( minus, 2, 4 )
                            )
                       );

  BOOST_CHECK_EQUAL( negFour( ), -4 );
}


int times3( int a )
{
  return a*3;
}
BOOST_AUTO_TEST_CASE( test_dbb_compose )
{
  auto compose = lam<2>( lam<1>( app( _2_1
                                    , app( _2_2
                                         , _1_1
                                         )
                                    )
                               )
                       );
  BOOST_CHECK_EQUAL( compose( times2, times3 )( 4 ), 24 );
}

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

BOOST_AUTO_TEST_CASE( test_dbb_currying )
{
  auto curry = lam<1>( lam<1>( lam<1>( app( _3_1
                                          , _2_1
                                          , _1_1
                                          )
                                     )
                             )
                     );

  BOOST_CHECK_EQUAL( curry( minus )( 3 )( 2 ), 1 );

  auto uncurry = lam<1>( lam<2>( app( app( _2_1
                                         , _1_1
                                         )
                                    , _1_2
                                    )
                               )
                       );
  BOOST_CHECK_EQUAL( uncurry( curriedF )( 3, 2 ), 4 );
}

namespace larryEvans
{
  int next( int i )
  {
    return i+1;
  }
  int foo()
  {
    auto op = lam<1>( app(next,_1_1 ) );
    int op_1 = op(1);
    auto lam_app = lam<1>( app(op,_1_1) );
    int lam_app_1 = lam_app(1);
    return op_1 + lam_app_1;
  }
}
BOOST_AUTO_TEST_CASE( test_dbb_Larry_Evans )
{
  BOOST_CHECK_EQUAL( larryEvans::foo(), 4 );
}

#include <boost/function.hpp>

//TODO: Iff really needs to compute the nearest supertype of a1 and a2 and make that
//      the result. Perhaps another template function that computes the supertype?
//      Maybe the nearest supertype should be specified with a template parameter?
struct Iff
{
//  template< typename A, Typename B >
//  typename nearest_supertype<A,B>::type
//  operator()( bool c, A a1, B a2 ) const
//  {
//      return c ? a1 : a2;
//  }
  typedef boost::function<int(int)> result_type;

  boost::function<int(int)> operator()
      ( bool c
      , boost::function<int(int)> a1
      , boost::function<int(int)> a2
      ) const
  {
    return c ? a1 : a2;
  }
} const iff = Iff();

struct Equals
{
  typedef bool result_type;
  template< typename A >
  bool operator()( const A a1, const A a2 ) const
  {
    return a1 == a2;
  }
} const equals = Equals();

int mult( int a, int b )
{
  return a*b;
}

BOOST_AUTO_TEST_CASE( test_dbb_factorial )
{
  //TODO: move this to a shared Place
  auto const_ = lam<1>( lam<1>( _2_1 ) );
  //λ₁λ₁(iff( equals(1₁,0)
  //        , const(1)
  //        , λ₁ mult(2₁, 3₁(minus(2₁, 1₁)))
  //        )
  //    )1
  auto factInner = lam<1>( lam<1>( app( app( iff
                                           , app( equals
                                               , _1_1
                                               , 1
                                               )
                                           , const_( 1 )
                                           , lam<1>( app( mult
                                                        , _2_1
                                                        , app( _3_1
                                                             , app( minus
                                                                  , _2_1
//                                                                  using the argument here
//                                                                  instead of 1 ensures this
//                                                                  function won't be applied until
//                                                                  after iff has returned.
//                                                                    , 1
                                                                  , _1_1
                                                                  )
                                                             )
                                                        )
                                                   )
                                           )
                                      , 1 //could be void
                                      )
                                 )
                         );
  auto fact = fix( factInner );
  BOOST_CHECK_EQUAL( fact( 1 ), 1 );
  BOOST_CHECK_EQUAL( fact( 2 ), 2 );
  BOOST_CHECK_EQUAL( fact( 4 ), 24 );
  BOOST_CHECK_EQUAL( fact( 10 ), 3628800 );
}
