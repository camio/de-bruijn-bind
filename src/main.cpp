#include "DeBruijnBind.hpp"
#include <iostream>

int minus( int a, int b )
{
    return a - b;
}

int mult( int a, int b )
{
    return a*b;
}

struct MinusF
{
    int operator()( int a, int b ) const
    {
        return a - b;
    }
} const minusF = MinusF();

int times2( int a )
{
    return a*2;
}

int times3( int a )
{
    return a*3;
}

int always3( int )
{
    return 3;
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
#include <boost/function.hpp>

//TODO: Iff really needs to compute the nearest supertype of a1 and a2 and make that
//      the result. Perhaps another template function that computes the supertype?
//      Maybe the nearest supertype should be specified with a template parameter?
struct Iff
{
//    template< typename A, Typename B >
//    typename nearest_supertype<A,B>::type
//    operator()( bool c, A a1, B a2 ) const
//    {
//        return c ? a1 : a2;
//    }
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

int minusOneF( int i )
{
    return i-1;
}

int main()
{
    auto id = lam<1>( _1_1 );
    auto always33 = lam<1>( 33 );
    auto const_ = lam<1>( lam<1>( _2_1 ) );
    auto minusTwelve = lam<1>( app( minus
                                  , _1_1
                                  , 12
                                  )
                             );

    auto flip   = lam<1>( lam<2>( app( _2_1
                                     , _1_2
                                     , _1_1
                                     )
                                )
                        );

    auto compose = lam<2>( lam<1>( app( _2_1
                                      , app( _2_2
                                           , _1_1
                                           )
                                      )
                                 )
                         );
    auto negFour = lam<0>( app( times2
                              , app( minus, 2, 4 )
                              )
                         );

    auto curry = lam<1>( lam<1>( lam<1>( app( _3_1
                                            , _2_1
                                            , _1_1
                                            )
                                       )
                               )
                       );
    auto uncurry = lam<1>( lam<2>( app( app( _2_1
                                           , _1_1
                                           )
                                      , _1_2
                                      )
                                 )
                         );
    
    auto minusOne = lam<1>( app( minus

                               , _1_1
                               , 1
                               )
                          );

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
    std::cout << id( "a" )
              << '\n' << always33( "asdf" )
              << '\n' << const_( "asdf" )( 12 )
              << '\n' << minusTwelve( 24 )
              << '\n' << flip( minus )( 10, 2 )
              << '\n' << compose( times2, times3 )( 4 )
              << '\n' << negFour()
              << '\n' << curry(minus)(3)(2)
              << '\n' << uncurry(curriedF)(3,2)
              << '\n' << "foo:" << larryEvans::foo( )
              << '\n' << fact( 1 ) // ⇒ 1
              << '\n' << fact( 2 ) // ⇒ 2
              << '\n' << fact( 4 ) // ⇒ 12
              << '\n' << fact( 10 ) // ⇒ 3628800
              << '\n';
}


#define BOOST_TEST_MODULE odillo test suite
#define BOOST_TEST_ALTERNATIVE_INIT_API
#define BOOST_TEST_MAIN
#define BOOST_TEST_NO_MAIN
#include <boost/test/included/unit_test.hpp>
