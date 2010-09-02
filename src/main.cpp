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
    auto id = abs<1>( var<0,0>() );
    auto always33 = abs<1>( 33 );
    auto const_ = abs<1>( abs<1>( var<1,0>() ) );
    auto minusTwelve = abs<1>( app( minus
                                  , var<0,0>()
                                  , 12
                                  )
                             );

    auto flip   = abs<1>( abs<2>( app( var<1,0>()
                                     , var<0,1>()
                                     , var<0,0>()
                                     )
                                )
                        );

    auto compose = abs<2>( abs<1>( app( var<1,0>()
                                      , app( var<1,1>()
                                           , var<0,0>()
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
