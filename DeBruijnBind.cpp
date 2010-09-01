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
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/at_c.hpp>
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
        >
struct App;

template< typename F, typename A1 >
struct App< F
          , A1
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

struct app
{
    typedef app this_type;

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
            >
    typename result< this_type( F, A1 ) >::type
    operator()( F f, A1 a1 ) const
    {
        return App< F, A1 >( f, a1 );
    }
};

template< int numArgs_
        , typename AbsBody
        >
struct Abs;

template< typename Context
        , typename Abs >
struct AbsIn;

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
/** TODO: left off here. I just defined AbsIn for an Abstraction that
 *        has a context. Continuing:
 *        - Make AbsIn handle variables (I think this is finished with
 *                                       an optimization for var<1,1>)
 *        - Make AbsIn handle values
 *        - Make AbsIn handle abstraction (nested AbsIn or Abs? )
 *        - Make AbsIn handle application
 *
 *        - See if AbsIn and Abs can be joined. Note that Right now Abs doesn't
 *          handle application.
 *        - Think about how to handle Abstractions that have no arguments. This
 *          is pretty neat actually. abs<0>
 */
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
    typedef typename boost::fusion::result_of::at_c
             < typename boost::remove_reference
                < typename boost::fusion::result_of::at_c
                  < Context
                  , var_depth - 2
                  >::type
                >::type
             , var_argument - 1
             >::type result_type;
    AbsIn( Context c
         , Abs< 1, var< var_depth, var_argument > >
         )
        : r( boost::fusion::at_c<var_argument-1>
             ( boost::fusion::at_c<var_depth - 2>
               ( c )
             )
           )
    {
    }
    result_type r;

    template< typename A1 >
    result_type operator()( A1 a1 ) const
    {
        return r;
    }
};

template<>
struct Abs<1,var<1,1> >
{
    Abs( var<1,1> ) { }
    typedef Abs<1,var<1,1> > this_type;

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


template< int innerNumArgs
        , typename InnerBody
        >
struct Abs< 1
          , Abs< innerNumArgs
               , InnerBody
               >
          >
{
    typedef Abs< innerNumArgs
               , InnerBody
               > InnerAbs;
    typedef Abs< 1, InnerAbs> this_type;
    Abs( InnerAbs iAbs_ )
        : iAbs( iAbs_ )
    {
    }
    InnerAbs iAbs;

    template< typename T >
    struct result;

    template< typename A1 >
    struct result< this_type( A1 ) >
    {
        typedef AbsIn< boost::fusion::vector< boost::fusion::vector< A1 > >
                     , InnerAbs
                     > type;
    };

    template< typename A1 >
    typename result< this_type( A1 ) >::type
    operator()( A1 a1 ) const
    {
        return AbsIn< boost::fusion::vector< boost::fusion::vector< A1 > >
                    , InnerAbs
                    >( boost::fusion::make_vector
                        ( boost::fusion::make_vector
                          ( a1
                          )
                        )
                     , iAbs
                     );
    }
};

template< typename Value >
struct Abs< 1, Value >
{
    Abs( Value v_ )
        : v( v_ )
    {
    }
    Value v;

    typedef Abs< 1, Value > this_type;

    template< typename T >
    struct result;

    template< typename A1 >
    struct result< this_type( A1 ) >
    {
        typedef Value type;
    };

    template< typename A1 >
    typename result< this_type( A1 ) >::type
    operator()( A1 ) const
    {
        return v;
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
    std::cout << id( "a" )
              << '\n' << always33( "asdf" )
              << '\n' << const_( "asdf" )( 12 )
              << '\n';
//    auto id = var<1,1>();
}
