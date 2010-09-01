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

#include <boost/fusion/include/vector.hpp>
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

struct AppVoid{};

template< typename F
        , typename A1 = AppVoid
        , typename A2 = AppVoid
        , typename A3 = AppVoid
        >
struct App;

template< typename F, typename A1 >
struct App< F
          , A1
          , AppVoid
          , AppVoid
          >
{
    App( F f_
       , A1 a1_
       )
       : f( f_ )
       , a1( a1_ )
    {
    }
    F f;
    A1 a1;
};

template< typename F
        , typename A1
        , typename A2
        >
struct App< F
          , A1
          , A2
          , AppVoid
          >
{
    App( F f_
       , A1 a1_
       , A2 a2_
       )
       : f( f_ )
       , a1( a1_ )
       , a2( a2_ )
    {
    }
    F f;
    A1 a1;
    A2 a2;
};

struct App0
{
    typedef App0 this_type;

    template< typename T >
    struct result;

    template< typename F
            , typename A1
            >
    struct result< this_type( F, A1 ) >
    {
        typedef App<F,A1> type;
    };

    template< typename F
            , typename A1
            , typename A2
            >
    struct result< this_type( F, A1, A2 ) >
    {
        typedef App<F,A1,A2> type;
    };

    template< typename F
            , typename A1
            >
    typename result< this_type( F, A1 ) >::type
    operator()( F f, A1 a1 ) const
    {
        return App< F, A1 >( f, a1 );
    }
    template< typename F
            , typename A1
            , typename A2
            >
    typename result< this_type( F, A1, A2 ) >::type
    operator()( F f, A1 a1, A2 a2 ) const
    {
        return App< F, A1, A2 >( f, a1, a2 );
    }
} const app;

template< int numArgs_
        , typename AbsBody
        >
struct Abs;

template< typename Context
        , typename Abs >
struct AbsIn;

/** Helper function for contexts
 */
template< int var_depth
        , int var_argument
        , typename Context
        >
auto cLookup( const Context & c )
    -> decltype( boost::fusion::at_c<var_argument-1>
                 ( boost::fusion::at_c<var_depth - 2>
                   ( c )
                 )
               )
{
    return boost::fusion::at_c<var_argument-1>
             ( boost::fusion::at_c<var_depth - 2>
               ( c )
             );
}

template< typename Context
        , typename AbsBody
        >
AbsIn< Context, AbsBody >
absIn( const Context & c
     , const AbsBody & b
     )
{
    return AbsIn< Context, AbsBody >( c, b );
}

//TODO: make this handle var<1,n>. Use static assert and convert the arguments
//      to a fusion vector (view?) before using at_c.
//
//      Also for Abs< n, var<1,p> >, static assert that p ≤ n.
template< typename Context >
struct AbsIn< Context
            , Abs< 1
                 , var<1,1>
                 >
            >
{
    AbsIn( Context, Abs< 1, var<1,1> > ) { }
    typedef AbsIn< Context
                 , Abs< 1
                      , var<1,1>
                      >
                 > this_type;

    template< typename T >
    struct result;

    template< typename A1 >
    struct result< this_type( A1 ) >
    {
        typedef A1 type;
    };

    template< typename A1 >
    typename result< this_type( A1 ) >::type
    operator()( A1 a1 ) const
    {
        return a1;
    }
};

//TODO: handle value abstractions with different numArgs
template< typename Context
        , int var_depth
        , int var_argument
        >
struct AbsIn< Context
            , Abs< 1
                 , var< var_depth, var_argument >
                 >
            >
{
    typedef decltype( cLookup< var_depth
                             , var_argument
                             >( *(Context*)(0) )
                    ) result_type;
    AbsIn( Context c
         , Abs< 1, var< var_depth, var_argument > >
         )
        : r( cLookup< var_depth, var_argument >( c ) )
    {
    }
    result_type r;

    template< typename A1 >
    result_type operator()( A1 a1 ) const
    {
        return r;
    }
};

//TODO: handle value abstractions with different numArgs
template< typename Context
        , typename Value
        >
struct AbsIn< Context
            , Abs< 1
                 , Value
                 >
            >
{
    AbsIn( Context
         , Abs< 1, Value > abs
         ) : v( abs.b )
    {
    }
    Value v;

    typedef Value result_type;

    template< typename A1 >
    result_type
    operator()( A1 ) const
    {
        return v;
    }
};

//An Abs Abs. TODO: make versions that have different numArgs. Maybe use
//static_assert in the body of the function to ensure numArgs matches
//the number of arguments
template< typename Context
        , int innerNumArgs
        , typename InnerBody
        >
struct AbsIn< Context
            , Abs< 1
                 , Abs< innerNumArgs
                      , InnerBody
                      >
                 >
            >
{
    typedef Abs< innerNumArgs
               , InnerBody
               > InnerAbs;
    AbsIn( Context c_
         , Abs< 1
              , InnerAbs
              > abs
         )
         : c( c_)
         , iAbs( abs.b )
    {
    }
    Context c;
    InnerAbs iAbs;

    template< typename A1 >
    auto operator()( A1 a1 ) const
        -> decltype( absIn( boost::fusion::as_vector
                             ( boost::fusion::push_front
                               ( c
                               , boost::fusion::make_vector( a1 )
                               )
                             )
                          , iAbs
                          )
                   )
    {
        return absIn( boost::fusion::as_vector
                       ( boost::fusion::push_front
                         ( c
                         , boost::fusion::make_vector( a1 )
                         )
                       )
                    , iAbs
                    );
    }
};

/** Applies an abstraction to an application with two args
 */
//TODO: this needs more thought. The application can have nested
//      abstractions/vars too. Is there an easy way to handle this?
//template< typename Context
//        , int f_var_depth
//        , int f_var_argument
//        , int a1_var_depth
//        , int a1_var_argument
//        >
//struct AbsIn< Context
//            , Abs< 1
//                 , App< var< f_var_depth
//                           , f_var_argument
//                           >
//                      , var< a1_var_depth
//                           , a1_var_argument
//                           >
//                      , AppVoid
//                      >
//                 >
//            >
//{
//    AbsIn( Context c_
//         , Abs< 1
//              , App< var< f_var_depth
//                        , f_var_argument
//                        >
//                   , var< a1_var_depth
//                        , a1_var_argument
//                        >
//                   , AppVoid
//                   >
//              >
//         )
//        : c( c_ )
//    {
//    }
//    typedef decltype( cLookup< f_var_depth
//                             , f_var_argument
//                             >( *(Context*)(0) )
//                             ( cLookup< a1_var_depth
//                                      , a1_var_argument
//                                      >( *(Context*)(0) )
//                             )
//                    ) result_type;
//    template< typename A1 >
//    result_type operator()( ) const
//    {
//        auto f = cLookup< f_var_depth
//                        , f_var_argument
//                        >( c );
//        auto a1 = cLookup< a1_var_depth
//                         , a1_var_argument
//                         >( c );
//        return f( a1 );
//    }
//    Context c;
//};

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
        -> decltype( absIn( boost::fusion::make_vector()
                          , *(this_type*)(0)//*this
                          )( a1 )
                   )
    {
        //TODO: Is it better to have AbsIn take in the body and numArgs instead
        //      of *this?
        return absIn( boost::fusion::make_vector()
                    , *this
                    )( a1 );
    }
};

template< int numArgs_
        , typename AbsBody
        >
Abs< numArgs_, AbsBody >
abs( AbsBody b )
{
    return Abs< numArgs_, AbsBody >( b );
}

#include <iostream>
void main()
{
    auto id = abs<1>( var<1,1>() );
    auto always33 = abs<1>( 33 );
    auto const_ = abs<1>( abs<1>( var<2,1>() ) );
//    auto flip   = abs<1>( abs<2>( app( var<2,1>()
//                                     , var<1,2>()
//                                     , var<1,1>()
//                                     )
//                                )
//                        );
    std::cout << id( "a" )
              << '\n' << always33( "asdf" )
              << '\n' << const_( "asdf" )( 12 )
              << '\n';
}
