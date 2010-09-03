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
#include <boost/fusion/include/fused.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/push_front.hpp>
#include <boost/type_traits/remove_reference.hpp>

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
    template< typename F, typename Args >
    auto operator()( F f, const Args & args ) const
        -> decltype( App<F,Args>( f, args )
                   )
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
    template< typename App >
    auto operator()( App a ) const
        -> decltype( boost::fusion::fused< decltype(a.f) >( a.f )( a.args ) )
    {
        //TODO: add static assertion here that tdepth( App ) = 0
        return boost::fusion::fused< decltype(a.f) >( a.f )( a.args );
    }
};

/** This function takes an App and simply returns it.
 */
struct keep_App
{
    template< typename App>
    App operator()( App a ) const
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
struct Reduce
{
    /** Vars **/
    //TODO: I'm not sure why the arg handler selection works so well, but
    //      I'd like to know.
    template< int depth
            , int argument
            , typename Context
            >
    auto operator()( boost::fusion::pair< boost::mpl::int_<depth>
                                        , Context
                                        > c
                  , arg< depth, argument >
                  ) const
        -> decltype( boost::fusion::at_c< argument - 1 >( c.second ) )
    {
        return boost::fusion::at_c< argument - 1 >( c.second );
    }

    template< int depth0
            , int depth1
            , int argument1
            , typename Context
            >
    auto operator()( boost::fusion::pair< boost::mpl::int_<depth0>
                                        , Context
                                        > c
                  , arg< depth1, argument1 > v
                  ) const
        -> decltype( v ) 
    {
        return v;
    }
    /** Values **/
    template< typename ContextPair
            , typename Value
            >
    auto operator()( ContextPair
                   , Value v
                   ) const
        -> decltype( v ) 
    {
        return v;
    }
    /** Abstractions **/
    template< int cdepth
            , typename Context
            , int numArgs
            , typename AbsBody
            >
    auto operator()( boost::fusion::pair< boost::mpl::int_<cdepth>
                                        , Context
                                        > c
                  , Abs< numArgs, AbsBody > a
                  ) const
        -> decltype( lam<numArgs>( reduce( boost::fusion::make_pair
                                            < boost::mpl::int_<cdepth+1> >
                                            ( c.second )
                                         , a.b
                                         )
                                 )
                   )
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
    auto operator()( boost::fusion::pair< boost::mpl::int_<cdepth>
                                        , Context
                                        > c
                  , App< F
                       , Args
                       > a
                  ) const
        -> decltype
        ( typename boost::mpl::if_
            < boost::mpl::equal_to
                            < typename boost::mpl::apply< tdepth
                                                        , decltype( sapp( reduce( c, a.f )
                                                                        , boost::fusion::transform
                                                                                ( a.args
                                                                                , creduce( c )
                                                                                )
                                                                        )
                                                                  )
                                                        >::type
                            , boost::mpl::int_<0>
                            >
            , reduce_App
            , keep_App
            >::type()( sapp( reduce( c, a.f )
                           , boost::fusion::transform
                                   ( a.args
                                   , creduce( c )
                                   )
                           )
                     )
        )
    {
        //Reduce f, all the arguments, and then see if we can execute
        //f at this point.
        auto a2 = sapp( reduce( c, a.f )
                      , boost::fusion::transform
                            ( a.args
                            , creduce( c )
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

/**creduce is a curried version of reduce
 */
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
        typedef typename boost::remove_reference<Arg>::type Arg2;
        typedef decltype( reduce( *(Context*)(0), *(Arg2*)(0) ) ) type;
    };

    Context c;
    template< typename Arg >
    auto operator()( const Arg & a ) const
        -> decltype( reduce( c, a ) )
    {
        return reduce( c, a );
    }
};
struct CReduce0
{
    template< typename Context >
    auto operator()( const Context & c ) const
        -> decltype( CReduce1<Context>( c )
                   )
    {
        return CReduce1<Context>( c );
    }
} const creduce = CReduce0();

//TODO: Add more overloads for extra arguments (use boost preprocessor)
template< int numArgs_
        , typename AbsBody
        >
struct Abs
{
    typedef Abs<numArgs_,AbsBody> this_type;
    Abs( AbsBody b_ ) : b( b_ )
    {
    }
    AbsBody b;

    //Abstraction terms can accept arguments and we do so below.
    template< typename A1 >
    auto operator()( A1 a1 ) const
        -> decltype( reduce( boost::fusion::make_pair
                              < boost::mpl::int_<1> >
                              ( boost::fusion::make_vector( a1 ) )
                           , b
                           )
                   )
    {
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
template< typename AbsBody
        >
struct Abs<0, AbsBody>
{
    typedef Abs<0,AbsBody> this_type;
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

#endif
