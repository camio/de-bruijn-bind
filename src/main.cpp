#include "DeBruijnBind.hpp"
#include <iostream>

int minus( int a, int b )
{
    return a - b;
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

struct Iff
{
    template< typename A >
    A operator()( bool c, A a1, A a2 ) const
    {
        return c ? a1 : a2;
    }
} const iff = Iff();

struct Equals
{
    template< typename A >
    bool operator()( const A a1, const A a2 ) const
    {
        return a1 == a2;
    }
} const equals = Equals();


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

    //fix λ₁λ₁(iff( equals(1₁,0)
    //            , const(1)
    //            , 2₁
    //            )
    //        )
    //        (minus1, 1₁)
    auto tst = fix( lam<1>( lam<1>( app( app( iff
                                            , app( equals
                                                 , _1_1
                                                 , 0
                                                 )
                                            , const_( 1 )
                                            , _2_1
                                            )
                                       , app( minusOne
                                            , _1_1
                                            )
                                       )
                                   )
                          )
                  );
    tst( 0 );// ⇒ 
    std::cout << id( "a" )
              << '\n' << always33( "asdf" )
              << '\n' << const_( "asdf" )( 12 )
              << '\n' << minusTwelve( 24 )
              << '\n' << flip( minus )( 10, 2 )
              << '\n' << compose( times2, times3 )( 4 )
              << '\n' << negFour()
              << '\n' << curry(minus)(3)(2)
              << '\n' << uncurry(curriedF)(3,2)
              << '\n';
}
