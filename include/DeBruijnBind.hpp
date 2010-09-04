#ifndef DEBRUIJNBIND_HPP_
#define DEBRUIJNBIND_HPP_

//TODO: enable this for g++ compiler?
//#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/mpl/int.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/max.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/mpl/max_element.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/if.hpp>

#include <boost/type_traits/is_same.hpp> 

#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/include/fused.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/push_front.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>

/** variables, abstractions, and applications make up our grammar.
 */
template< int depth_
        , int argument_
        >
struct arg
{
    const static int depth = depth_;
    const static int argument = argument_;
};

template< int numArgs_
        , typename AbsBody
        >
struct Abs;

template< int numArgs_
        , typename AbsBody
        >
Abs< numArgs_, AbsBody >
lam( AbsBody b )
{
    return Abs< numArgs_, AbsBody >( b );
}

template< typename FixBody_ >
struct Fix
{
    typedef FixBody_ FixBody;
    Fix( FixBody b_ )
        : b( b_ )
    {
    }
    FixBody b;
};

struct Fix0
{
    template< typename AbsBody >
    Fix< Abs<1, AbsBody> > operator()( Abs< 1, AbsBody> b ) const
    {
        return Fix< Abs<1, AbsBody> >( b );
    }
} const fix = Fix0();

//For application, we're encoding the arguments as a fusion
//sequence.
template< typename F_
        , typename Args_
        >
struct App
{
    typedef F_ F;
    typedef Args_ Args;
    App( F f_, Args args_ )
        : f( f_ )
        , args( args_ )
    {
    }
    F f;
    Args args;
};

//sapp creates an application AST based on a fusion sequence.
struct SApp
{
    typedef SApp this_type;

    template< typename T >
    struct result;

    template< typename F, typename Args >
    struct result< this_type( F, Args ) >
    {
        typedef App< F
                   , typename boost::remove_const
                        < typename boost::remove_reference
                            < Args
                            >::type
                        >::type
                   > type;
    };

    template< typename F, typename Args >
    typename result< this_type( F, const Args & )>::type
    operator()( F f, const Args & args ) const
    {
        return App<F,Args>( f, args );
    }
} const sapp = SApp();

/** app creates an application AST based on multiple arguments.
 */
//TODO: add more function call overloads here.
struct App0
{
    typedef App0 this_type;

#ifdef BOOST_NO_VARIADIC_TEMPLATES
    template< typename F
            , typename A1
            >
    auto operator()( F f, A1 a1 ) const
        -> decltype( sapp( f, boost::fusion::make_vector( a1 ) ) )
    {
        return sapp( f, boost::fusion::make_vector( a1 ) );
    }

    template< typename F
            , typename A1
            , typename A2
            >
    auto operator()( F f, A1 a1, A2 a2 ) const
        -> decltype( sapp( f, boost::fusion::make_vector( a1, a2 ) )
                   )
    {
        return sapp( f, boost::fusion::make_vector( a1, a2 ) );
    }

    template< typename F
            , typename A1
            , typename A2
            , typename A3
            >
    auto operator()( F f, A1 a1, A2 a2, A3 a3 ) const
        -> decltype( sapp( f, boost::fusion::make_vector( a1, a2, a3 ) )
                   )
    {
        return sapp( f, boost::fusion::make_vector( a1, a2, a3 ) );
    }
#else
    template< typename F
            , typename... A
            >
    auto operator()( F f, A... a ) const
        -> decltype( sapp( f, boost::fusion::make_vector( a... ) )
                   )
    {
        return sapp( f, boost::fusion::make_vector( a... ) );
    }
#endif    
} const app = App0();

/** tdepth type function
 *
 *  This takes an instance of our grammar and returns the number of 
 *  abstractions it requires to be evaluated.
 *
 *  Assuming α varies over variables, v varies over values, and e,f over
 *  expressions.
 *
 *  tdepth( v ) = 0
 *  tdepth( α ) = α
 *  tdepth( λ.e ) = max( tdepth( e ) - 1, 0 )
 *  tdepth( f(e) ) = max( tdepth( f ), tdepth( e ) )
 *
 *  The following code extends this to our grammer which allows multiple
 *  arguments.
 *
 *  See src/test.cpp for testing of this function.
 **/
struct tdepth
{
    /** Value **/
    template< typename Value >
    struct apply : boost::mpl::int_<0>
    {
    };

    /** Var **/
    template< int depth
            , int argument
            >
    struct apply< arg< depth
                     , argument
                     >
                >
                : boost::mpl::int_< depth >
    {
    };

    /** Application **/
    template< typename F
            , typename Args
            >
    struct apply< App< F
                      , Args
                      >
                 >
    {
        typedef typename tdepth::template apply<F>::type FDepth;
        typedef typename boost::mpl::deref
                          < typename boost::mpl::max_element
                            < boost::mpl::transform_view
                                < Args
                                , tdepth
                                >
                            >::type
                          >::type  MaxArgsDepth;
        typedef typename boost::mpl::max< FDepth
                                        , MaxArgsDepth
                                        >::type type;
    };

    /** Abstraction **/
    template< int numArgs
            , typename AbsBody
            >
    struct apply< Abs< numArgs
                     , AbsBody
                     >
                >
         : boost::mpl::max< boost::mpl::int_< 0 >
                          , boost::mpl::minus
                             < typename boost::mpl::apply< tdepth, AbsBody >::type
                             , boost::mpl::int_< 1 >
                             >
                          >
    {
    };
};

/** The following two functions are helpers for the reduce function.
 */

/** This function takes a tdepth=0 App and runs the function on the arguments
 */
struct reduce_App
{
    typedef reduce_App this_type;

    template< typename T >
    struct result;

    template< typename App >
    struct result< this_type( App ) >
    {
        typedef typename boost::remove_const
                   < typename boost::remove_reference
                       < App
                       >::type
                   >::type App2;

        typedef typename boost::fusion::result_of::invoke
                    < typename App2::F
                    , const typename App2::Args
                    >::type type;
    };

    template< typename App >
    typename result<this_type( const App &) >::type
    operator()( const App & a ) const
    {
        //TODO: add static assertion here that tdepth( App ) = 0
        return boost::fusion::invoke( a.f, a.args );
    }
};

/** This function takes an App and simply returns it.
 */
struct keep_App
{
    typedef keep_App this_type;

    template< typename T >
    struct result;

    template< typename App >
    struct result< this_type( App ) >
    {
        typedef typename boost::remove_const
                   < typename boost::remove_reference
                       < App
                       >::type
                   >::type App2;
        typedef App2 type;
    };

    template< typename App>
    typename result<this_type( const App & )>::type
    operator()( const App & a ) const
    {
        return a;
    }
};

/** The reduce function takes an instance of our grammer and a fusion pair
 *  of an integer (representing the depth of the binding) and a fusion
 *  sequence representing the arguments being bound.
 *
 *  The result is the term with the substituted variable and any function
 *  applications done that can now be executed.
 */

/**creduce is a curried version of reduce
 */
struct Reduce;
template< typename Context >
struct CReduce1
{
    CReduce1( const Context & c_ )
        : c( c_ )
    {
    }

    typedef CReduce1<Context> this_type;

    //This result struct is needed for visual c++ 2010 support. :(
    template< typename T >
    struct result;

    template< typename Arg >
    struct result< this_type( Arg ) >
    {
        typedef typename boost::remove_const
                   < typename boost::remove_reference
                       < Arg
                       >::type
                   >::type Arg2;
        typedef typename boost::result_of
                            < Reduce( Context , Arg2 )
                            >::type type;
    };

    Context c;
    template< typename Arg >
    typename result<this_type (const Arg &)>::type
    operator()( const Arg & a ) const
    {
        return reduce( c, a );
    }
};
struct CReduce0
{
    typedef CReduce0 this_type;

    template< typename T >
    struct result;

    template< typename Context >
    struct result< this_type( Context ) >
    {
        typedef CReduce1< typename boost::remove_const
                                < typename boost::remove_reference
                                        < Context
                                        >::type
                                >::type
                        > type;
    };

    template< typename Context >
    typename result< this_type( const Context &c ) >::type
    operator()( const Context & c ) const
    {
        return CReduce1<Context>( c );
    }
} const creduce = CReduce0();

struct Reduce
{
    typedef Reduce this_type;
    template< typename T>
    struct result
    {
    };

    /** Args **/
    template< int depth
            , int argument
            , typename Context
            >
    struct result< this_type( const boost::fusion::pair< boost::mpl::int_<depth>
                                                       , Context
                                                       >
                            , arg< depth, argument >
                            )
                 >
    {
        typedef typename boost::fusion::result_of::at_c
                           < Context const
                           , argument - 1
                           >::type r;
        typedef typename boost::remove_const
                            < typename boost::remove_reference<r>::type
                            >::type type;
    };

    //TODO: I'm not sure why the arg handler selection works so well, but
    //      I'd like to know.
    template< int depth
            , int argument
            , typename Context
            >
    typename result< this_type( const boost::fusion::pair< boost::mpl::int_<depth>
                                                         , Context
                                                         >
                              , arg< depth, argument >
                              )
                   >::type
    operator()( const boost::fusion::pair< boost::mpl::int_<depth>
                                         , Context
                                         > c
              , arg< depth, argument >
              ) const
    {
        return boost::fusion::at_c< argument - 1 >( c.second );
    }

    template< int depth0
            , int depth1
            , int argument1
            , typename Context
            >
    struct result< this_type( const boost::fusion::pair
                                      < boost::mpl::int_<depth0>
                                      , Context
                                      >
                            , arg< depth1, argument1 >
                            )
                 >
    {
        typedef arg< depth1, argument1 > type;
    };

    template< int depth0
            , int depth1
            , int argument1
            , typename Context
            >
    typename result< this_type( const boost::fusion::pair
                                        < boost::mpl::int_<depth0>
                                        , Context
                                        >
                              , arg< depth1, argument1 >
                              )
                   >::type
    operator()( const boost::fusion::pair
                        < boost::mpl::int_<depth0>
                        , Context
                        >
             , arg< depth1, argument1 > v
             ) const
    {
        return v;
    }

    /** Values **/

    template< typename ContextPair
            , typename Value
            >
    struct result< this_type( const ContextPair
                            , const Value
                            )
                 >
    {
        //TODO: is this necessary?
        typedef typename boost::remove_reference<Value>::type Value2;
        typedef Value2 type;
    };

    template< typename ContextPair
            , typename Value
            >
    typename result< this_type( const ContextPair
                              , const Value
                              )
                   >::type
    operator()( const ContextPair
              , const Value v
              ) const
    {
        return v;
    }

    /** Abstractions **/
    template< int cdepth
            , typename Context
            , int numArgs
            , typename AbsBody
            >
    struct result< this_type( const boost::fusion::pair
                                        < boost::mpl::int_<cdepth>
                                        , Context
                                        >
                            , Abs< numArgs, AbsBody >
                            )
                 >
    {  
        typedef typename boost::result_of< Reduce( boost::fusion::pair
                                                   < boost::mpl::int_<cdepth+1>
                                                   , Context
                                                   >
                                                 , AbsBody
                                                 )
                                         >::type R1;
        typedef Abs<numArgs, R1> type;
    };
    template< int cdepth
            , typename Context
            , int numArgs
            , typename AbsBody
            >
    typename result< this_type( const boost::fusion::pair
                                        < boost::mpl::int_<cdepth>
                                        , Context
                                        >
                              , Abs< numArgs, AbsBody >
                              )
                   >::type
    operator()( const boost::fusion::pair
                        < boost::mpl::int_<cdepth>
                        , Context
                        > c
             , Abs< numArgs, AbsBody > a
             ) const
    {
        return lam<numArgs>( reduce( boost::fusion::make_pair
                                      < boost::mpl::int_<cdepth+1> >
                                      ( c.second )
                                   , a.b
                                   )
                           );
    }
    /** Applications **/
    template< int cdepth
            , typename Context
            , typename F
            , typename Args
            >
    struct result< this_type( const boost::fusion::pair
                                      < boost::mpl::int_<cdepth>
                                      , Context
                                      >
                            , App< F
                                 , Args
                                 >
                            )
                 >
    {
        typedef boost::fusion::pair< boost::mpl::int_<cdepth>
                                   , Context
                                   > C;

        typedef typename boost::result_of<Reduce(C,F)>::type F2;

        typedef typename boost::fusion::result_of::transform
            < Args const
            , typename boost::result_of< CReduce0( C ) >::type
            >::type Args2;

        typedef typename boost::fusion::result_of::as_vector< Args2
                                                            >::type Args3;
        typedef typename boost::result_of< SApp( F2, Args3 )>::type A2;

        typedef typename boost::mpl::if_
            < boost::mpl::equal_to
               < typename boost::mpl::apply< tdepth
                                           , A2
                                           >::type
               , boost::mpl::int_<0>
               >
            , reduce_App
            , keep_App
            >::type AppReduction;
        typedef typename boost::result_of< AppReduction( A2 ) >::type type;
    };

    template< int cdepth
            , typename Context
            , typename F
            , typename Args
            >
    typename result< this_type( const boost::fusion::pair
                                        < boost::mpl::int_<cdepth>
                                        , Context
                                        >
                              , App< F
                                   , Args
                                   >
                              )
                   >::type
    operator()( const boost::fusion::pair< boost::mpl::int_<cdepth>
                                         , Context
                                         > c
              , App< F
                   , Args
                   > a
              ) const
    {
        //Reduce f, all the arguments, and then see if we can execute
        //f at this point.
        auto a2 = sapp( reduce( c, a.f )
                      , boost::fusion::as_vector
                          ( boost::fusion::transform
                            ( a.args
                            , creduce( c )
                            )
                          )
                      );
        typedef typename boost::mpl::if_
            < boost::mpl::equal_to
               < typename boost::mpl::apply< tdepth
                                           , decltype( a2 )
                                           >::type
               , boost::mpl::int_<0>
               >
            , reduce_App
            , keep_App
            >::type AppReduction;
        return AppReduction()( a2 );
    }
} const reduce = Reduce();


//TODO: Add more overloads for extra arguments (use boost preprocessor)
template< int numArgs_
        , typename AbsBody_
        >
struct Abs
{
    typedef AbsBody_ AbsBody;
    typedef Abs<numArgs_,AbsBody> this_type;
    static const int numArgs = numArgs_;
    Abs( AbsBody b_ ) : b( b_ )
    {
    }
    AbsBody b;

    //Abstraction terms can accept arguments and we do so below.
    template< typename T >
    struct result;

    template< typename A1 >
    struct result< this_type( A1 ) >
    {
        typedef typename boost::fusion::result_of::make_vector
                    < typename boost::remove_const
                        < typename boost::remove_reference
                            < A1
                            >::type
                        >::type
                    >::type V;
        typedef typename boost::fusion::result_of::make_pair
                    < boost::mpl::int_<1>
                    , V
                    >::type P;
        typedef typename boost::result_of
                    < Reduce( P, AbsBody )
                    >::type type;
    };

    template< typename A1 >
    typename result< this_type( A1 )>::type
    operator()( A1 a1 ) const
    {
        BOOST_MPL_ASSERT(( boost::mpl::equal_to
                             < typename boost::mpl::apply< tdepth
                                                         , decltype( a1 )
                                                         >::type
                             , boost::mpl::int_<0>
                             >
                        ));
        static_assert( numArgs_ == 1
                     , "Calling abstraction (lam) with too many arguments"
                     );
        return reduce( boost::fusion::make_pair
                        < boost::mpl::int_<1> >
                        ( boost::fusion::make_vector( a1 ) )
                     , b
                     );
    }
    template< typename A1
            , typename A2
            >
    auto operator()( A1 a1
                   , A2 a2
                   ) const
        -> decltype( reduce( boost::fusion::make_pair
                              < boost::mpl::int_<1> >
                              ( boost::fusion::make_vector( a1
                                                          , a2
                                                          )
                              )
                           , b
                           )
                   )
    {
        //TODO: repeated code here, clean it up.
        BOOST_MPL_ASSERT(( boost::mpl::equal_to
                             < typename boost::mpl::apply< tdepth
                                                         , decltype( a1 )
                                                         >::type
                             , boost::mpl::int_<0>
                             >
                        ));
        BOOST_MPL_ASSERT(( boost::mpl::equal_to
                             < typename boost::mpl::apply< tdepth
                                                         , decltype( a2 )
                                                         >::type
                             , boost::mpl::int_<0>
                             >
                        ));
        static_assert( numArgs_ == 2
                     , "Calling abstraction (lam) with incorrect number of "
                       "arguments"
                     );
        return reduce( boost::fusion::make_pair
                        < boost::mpl::int_<1> >
                        ( boost::fusion::make_vector( a1
                                                    , a2
                                                    )
                        )
                     , b
                     );
    }
};
/* Abs<0,AbsBody> needs to be specialized because its
 * operator() is not a template function and therefore will
 * always be instanciated.
 */
template< typename AbsBody_
        >
struct Abs<0, AbsBody_>
{
    typedef AbsBody_ AbsBody;
    typedef Abs<0,AbsBody> this_type;
    static const int numArgs = 0;
    Abs( AbsBody b_ ) : b( b_ )
    {
    }
    AbsBody b;

    auto operator()() const
        -> decltype( reduce( boost::fusion::make_pair
                              < boost::mpl::int_<1> >
                              ( boost::fusion::make_vector() )
//                           , b
                           , *(AbsBody*)(0) //a workaround for msvc
                           )
                   )
    {
        return reduce( boost::fusion::make_pair
                        < boost::mpl::int_<1> >
                        ( boost::fusion::make_vector() )
                     , b
                     );
    }

};

/** Some shorthands for arg expressions (please know what they translate to!)
 */

const arg<1,1> _1_1 = arg<1,1>();
const arg<1,2> _1_2 = arg<1,2>();
const arg<1,3> _1_3 = arg<1,3>();
const arg<2,1> _2_1 = arg<2,1>();
const arg<2,2> _2_2 = arg<2,2>();
const arg<2,3> _2_3 = arg<2,3>();
const arg<3,1> _3_1 = arg<3,1>();
const arg<3,2> _3_2 = arg<3,2>();
const arg<3,3> _3_3 = arg<3,3>();

#endif
