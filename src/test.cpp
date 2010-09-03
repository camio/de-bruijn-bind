#include "DeBruijnBind.hpp"

static void testTDepth()
{
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< arg<3,1> >::type
                        , boost::mpl::int_<3>
                        >
                     ));
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< int >::type
                        , boost::mpl::int_<0>
                        >
                     ));
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< Abs<1, arg<1,1> >
                                       >::type
                        , boost::mpl::int_<0>
                        >
                     ));
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< Abs< 1
                                            , Abs< 1
                                                 , arg<1,1>
                                                 >
                                            >
                                       >::type
                        , boost::mpl::int_<0>
                        >
                     ));
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< Abs< 1
                                            , Abs< 1
                                                 , arg<3,1>
                                                 >
                                            >
                                       >::type
                        , boost::mpl::int_<1>
                        >
                     ));
    BOOST_MPL_ASSERT(( boost::mpl::equal_to
                        < tdepth::apply< App< boost::mpl::void_
                                            , boost::fusion::vector
                                               < arg<3,1>
                                               , arg<5,1>
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
                                                    < arg<3,1>
                                                    , arg<4,1>
                                                    >
                                                 >
                                            >
                                       >::type
                        , boost::mpl::int_<3>
                        >
                     ));
}
