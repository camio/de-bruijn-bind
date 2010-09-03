#include "DeBruijnBind.hpp"
#include <iostream>

int minus( int a, int b )
{
    return a - b;
}

int times2( int a )
{
    return a*2;
}

int times3( int a )
{
    return a*3;
}

int main()
{
    auto id = abs<1>( var<1,1>() );
    auto always33 = abs<1>( 33 );
    auto const_ = abs<1>( abs<1>( var<2,1>() ) );
    auto minusTwelve = abs<1>( app( minus
                                  , var<1,1>()
                                  , 12
                                  )
                             );

    auto flip   = abs<1>( abs<2>( app( var<2,1>()
                                     , var<1,2>()
                                     , var<1,1>()
                                     )
                                )
                        );

    auto compose = abs<2>( abs<1>( app( var<2,1>()
                                      , app( var<2,2>()
                                           , var<1,1>()
                                           )
                                      )
                                 )
                         );
    auto negFour = abs<0>( app( times2
                              , app( minus, 2, 4 )
                              )
                         );

    std::cout << id( "a" )
              << '\n' << always33( "asdf" )
              << '\n' << const_( "asdf" )( 12 )
              << '\n' << minusTwelve( 24 )
              << '\n' << flip( minus )( 10, 2 )
              << '\n' << compose( times2, times3 )( 4 )
              << '\n' << negFour()
              << '\n';
}
