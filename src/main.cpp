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
    auto id = lam<1>( arg<1,1>() );
    auto always33 = lam<1>( 33 );
    auto const_ = lam<1>( lam<1>( arg<2,1>() ) );
    auto minusTwelve = lam<1>( app( minus
                                  , arg<1,1>()
                                  , 12
                                  )
                             );

    auto flip   = lam<1>( lam<2>( app( arg<2,1>()
                                     , arg<1,2>()
                                     , arg<1,1>()
                                     )
                                )
                        );

    auto compose = lam<2>( lam<1>( app( arg<2,1>()
                                      , app( arg<2,2>()
                                           , arg<1,1>()
                                           )
                                      )
                                 )
                         );
    auto negFour = lam<0>( app( times2
                              , app( minus, 2, 4 )
                              )
                         );

    auto curry = lam<1>( lam<1>( lam<1>( app( arg<3,1>()
                                            , arg<2,1>()
                                            , arg<1,1>()
                                            )
                                       )
                               )
                       );
    auto uncurry = lam<1>( lam<2>( app( app( arg<2,1>()
                                           , arg<1,1>()
                                           )
                                      , arg<1,2>()
                                      )
                                 )
                         );

    auto bug = lam<1>( lam<1>( lam<1>( 12 ) ) );
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
