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

value = any c++ value not in 
int = digit non zero, {digit};
digit = "0" | digit non zero;
digit non zero = "1" | "2" | "3" ... "9";

The abstraction here requires an int to declare how many arguments the newly created function has.

Variables have two ints, the first corresponds to which nested abstraction we're referring to. 1 is inner most one and so on outward. The second int corresponds to which argument within the abstraction is being referred to.

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

#include <boost/mpl/int.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/max.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/mpl/max_element.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/equal_to.hpp>

#include <boost/type_traits/is_same.hpp> 

#include <boost/fusion/include/mpl.hpp>
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

template< typename F
        , typename Args
        >
struct App
{
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

/** TODO: left off here. I just defined AbsIn for an Abstraction that
 *        has a context. Continuing:
 *        - Make AbsIn handle variables (I think this is finished with
 *                                       an optimization for var<1,1>)
 *        - Make AbsIn handle abstraction (nested AbsIn or Abs? ) (done)
 *        - Make AbsIn handle values (done)
 *        - See if AbsIn and Abs can be joined. Note that Right now Abs doesn't
 *          handle application. (done)
 *
 *        - Make AbsIn handle application
 *        - Think about how to handle Abstractions that have no arguments. This
 *          is pretty neat actually. abs<0>
 */

//struct ReduceApp
//{
//    /** Vars **/
//    //f=var
//    template< int depth
//            , int argument
//            , typename A1
//            , typename A2
//            >
//    auto operator()( App< var<depth,argument>
//                        , A1
//                        , A2
//                        , AppVoid
//                        > a
//                   )
//        -> decltype( a )
//    {
//        return a;
//    }
//    //a1=var
//    template< int depth
//            , int argument
//            , typename F
//            , typename A2
//            >
//    auto operator()( App< F
//                        , var<depth,argument>
//                        , A2
//                        , AppVoid
//                        > a
//                   )
//        -> decltype( a )
//    {
//        return a;
//    }
//    //a2=var
//    template< int depth
//            , int argument
//            , typename F
//            , typename A1
//            >
//    auto operator()( App< F
//                        , A1
//                        , var<depth,argument>
//                        , AppVoid
//                        > a
//                   )
//        -> decltype( a )
//    {
//        return a;
//    }
//};

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
                             < typename tdepth::template apply<AbsBody>::type
                             , boost::mpl::int_< 1 >
                             >
                          >
    {
    };
};

void f()
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


struct Reduce
{
    /** Vars **/
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
        -> bool
    {
        auto a2 = sapp( reduce( c, a.f )
                      , boost::fusion::transform
                            ( a.args
                            , creduce( c )
                            )
                      );
        return true;
    }
} const reduce = Reduce();

template< typename Context >
struct CReduce1
{
    CReduce1( const Context & c_ )
        : c( c_ )
    {
    }
    Context c;
    template< typename Arg >
    auto operator()( const Arg & a )
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


//TODO: make this handle multiple numArgs arguments. Add a static_assert
//      in the operator() overloads.
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
};


#include <iostream>

int minus( int a, int b )
{
    return a - b;
}

int main()
{
    auto id = abs<1>( var<0,0>() );
    auto always33 = abs<1>( 33 );
    auto const_ = abs<1>( abs<1>( var<1,0>() ) );
//    auto flip   = abs<1>( abs<2>( app( var<2,1>()
//                                     , var<1,2>()
//                                     , var<1,1>()
//                                     )
//                                )
//                        );

    auto minusTwelve = abs<1>( app( minus
                                  , var<0,0>()
                                  , 12
                                  )
                             );

    std::cout << id( "a" )
              << '\n' << always33( "asdf" )
              << '\n' << const_( "asdf" )( 12 )
              << '\n' << minusTwelve( 0 )
              << '\n';
}
