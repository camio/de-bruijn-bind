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

#include <boost/function.hpp>
boost::function<int (int)> curriedF( int i )
{
    return times2;
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

    std::cout << id( "a" )
              << '\n' << always33( "asdf" )
              << '\n' << const_( "asdf" )( 12 )
              << '\n' << minusTwelve( 24 )
              << '\n' << flip( minus )( 10, 2 )
              << '\n' << compose( times2, times3 )( 4 )
              << '\n' << negFour()
              << '\n' << curry(minus)(3)(2)
              //gcc-4.5 barfs in weird ways on this case.
//              << '\n' << uncurry(curriedF)(3,2)
              << '\n';
}
