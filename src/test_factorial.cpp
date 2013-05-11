#include <dbb/test/common.hpp>
#include <boost/function.hpp>

using namespace dbb;
using namespace dbb::test;

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
