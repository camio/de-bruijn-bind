#ifndef DBB_ALGORITHMS_HPP_
#define DBB_ALGORITHMS_HPP_

namespace dbb {
  const auto id = lam<1>( _1_1 );
  const auto const_ = lam<1>( lam<1>( _2_1 ) );
  const auto curry = lam<1>( lam<1>( lam<1>( app( _3_1
                                                , _2_1
                                                , _1_1
                                                )
                                           )
                                   )
                           );
  const auto uncurry = lam<1>( lam<2>( app( app( _2_1
                                               , _1_1
                                               )
                                          , _1_2
                                          )
                                     )
                             );
  const auto compose = lam<2>( lam<1>( app( _2_1
                                          , app( _2_2
                                               , _1_1
                                               )
                                          )
                                     )
                             );
  const auto flip = lam<1>( lam<2>( app( _2_1
                                       , _1_2
                                       , _1_1
                                       )
                                  )
                          );
}
#endif
