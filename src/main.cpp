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

    auto curry = abs<1>( abs<1>( abs<1>( app( var<3,1>()
                                            , var<2,1>()
                                            , var<1,1>()
                                            )
                                       )
                               )
                       );
    auto uncurry = abs<1>( abs<2>( app( app( var<2,1>()
                                           , var<1,1>()
                                           )
                                      , var<1,2>()
                                      )
                                 )
                         );

    auto bug = abs<1>( abs<1>( abs<1>( 12 ) ) );
//    bug( 1 );

    std::cout << id( "a" )
              << '\n' << always33( "asdf" )
              << '\n' << const_( "asdf" )( 12 )
              << '\n' << minusTwelve( 24 )
              << '\n' << flip( minus )( 10, 2 )
              << '\n' << compose( times2, times3 )( 4 )
              << '\n' << negFour()
              //gcc-4.5 barfs without ICE on these cases.
//              << '\n' << curry(minus)(3)(2)
//              << '\n' << uncurry(curriedF)(3,2)
              << '\n';
}
