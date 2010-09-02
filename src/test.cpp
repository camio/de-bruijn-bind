#include "DeBruijnBind.hpp"

static void testTDepth()
{
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< var<3,0> >::type
                        , boost::mpl::int_<4>
                        >
                     ));
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< int >::type
                        , boost::mpl::int_<0>
                        >
                     ));
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< Abs<1, var<0,0> >
                                       >::type
                        , boost::mpl::int_<0>
                        >
                     ));
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< Abs< 1
                                            , Abs< 1
                                                 , var<0,0>
                                                 >
                                            >
                                       >::type
                        , boost::mpl::int_<0>
                        >
                     ));
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< Abs< 1
                                            , Abs< 1
                                                 , var<2,0>
                                                 >
                                            >
                                       >::type
                        , boost::mpl::int_<1>
                        >
                     ));
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< App< boost::mpl::void_
                                            , boost::fusion::vector
                                               < var<2,0>
                                               , var<4,0>
                                               >
                                            >
                                       >::type
                        , boost::mpl::int_<5>
                        >
                     ));
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< Abs< 1
                                            , App< boost::mpl::void_
                                                 , boost::fusion::vector
                                                    < var<2,0>
                                                    , var<3,0>
                                                    >
                                                 >
                                            >
                                       >::type
                        , boost::mpl::int_<3>
                        >
                     ));
}
