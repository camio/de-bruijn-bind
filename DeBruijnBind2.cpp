/*
Grammar
=======
abstraction = 'abs<', int, '>', '(', expression, ')';
expression not value = abstraction
                     | application
                     | value
                     | variable

application = 'app(', expression, { ',', expression }, ')'
variable = 'var<', int, ',', int, '>'

value = any c++ value that isn't a variable or a tdepth 0
int = digit, {digit};
digit = "0" | "1" | "2" | "3" ... "9";

The abstraction here requires an int to declare how many arguments the newly created function has.

Variables have two ints, the first corresponds to which nested abstraction we're referring to. 0 is inner most one and so on outward. The second int corresponds to which argument within the abstraction is being referred to, 0 being the first.

Examples
========

In lambda and then De Bruijn terms

id = λx. x = λ1
const = λx.λy.x = λλ2
flip = λx.λ(y,z).x(z,y) = λλ2(1₂, 1₁)
compose = λx.λy.λz.x(y(z)) = λλλ3(2(1))


auto id = abs<1>( var<1,1>() );
auto const_ = abs<1>( abs<1>( var<2,1>() ) );
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
*/

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

template< int depth_
        , int argument_
        >
struct var
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
abs( AbsBody b )
{
    return Abs< numArgs_, AbsBody >( b );
}

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

/** tdepth type function **/
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
    struct apply< var< depth
                      , argument
                      >
                 >
                 : boost::mpl::int_< depth + 1 >
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

struct keep_App
{
    template< typename App>
    App operator()( App a ) const
    {
        return a;
    }
};

struct Reduce
{
    /** Vars **/
    //TODO: I'm not sure why the var handler selection works so well, but
    //      I'd like to know.
    template< int depth
            , int argument
            , typename Context
            >
    auto operator()( boost::fusion::pair< boost::mpl::int_<depth>
                                        , Context
                                        > c
                  , var< depth, argument >
                  ) const
        -> decltype( boost::fusion::at_c<argument>( c.second ) )
    {
        return boost::fusion::at_c<argument>( c.second );
    }

    template< int depth0
            , int depth1
            , int argument1
            , typename Context
            >
    auto operator()( boost::fusion::pair< boost::mpl::int_<depth0>
                                        , Context
                                        > c
                  , var< depth1, argument1 > v
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
        -> decltype( abs<numArgs>( reduce( boost::fusion::make_pair
                                            < boost::mpl::int_<cdepth+1> >
                                            ( c.second )
                                         , a.b
                                         )
                                 )
                   )
    {
        return abs<numArgs>( reduce( boost::fusion::make_pair
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
/**creduce is a curried version of reduce
 */
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

    template< typename A1 >
    auto operator()( A1 a1 ) const
        -> decltype( reduce( boost::fusion::make_pair
                              < boost::mpl::int_<0> >
                              ( boost::fusion::make_vector( a1 ) )
                           , b
                           )
                   )
    {
        static_assert( numArgs_ == 1
                     , "Calling abstraction with too many arguments"
                     );
        return reduce( boost::fusion::make_pair
                        < boost::mpl::int_<0> >
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
                              < boost::mpl::int_<0> >
                              ( boost::fusion::make_vector( a1
                                                          , a2
                                                          )
                              )
                           , b
                           )
                   )
    {
        static_assert( numArgs_ == 2
                     , "Calling abstraction with incorrect number of arguments"
                     );
        return reduce( boost::fusion::make_pair
                        < boost::mpl::int_<0> >
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
                              < boost::mpl::int_<0> >
                              ( boost::fusion::make_vector() )
                           , b
                           )
                   )
    {
        return reduce( boost::fusion::make_pair
                        < boost::mpl::int_<0> >
                        ( boost::fusion::make_vector() )
                     , b
                     );
    }

};

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
