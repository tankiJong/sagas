#pragma once
#include <memory>
#include <concepts>

namespace detail
{
template< typename T >
struct self_type
{
   using type = T;
};
}

template< typename T >
using self_t = typename detail::self_type<T>::type;

template< typename T >
struct value_wrapper
{
   T v;
};

using tid_t = size_t;

namespace detail
{
struct tid_base
{
   tid_base* self;
};
}

template< typename ...T >
struct tid: detail::tid_base, value_wrapper<tid_t>
{
protected:
   static const char _id;

   tid()
      : tid_base( this )
      , value_wrapper( (tid_t)&_id ) {}

public:
   static const tid_t value;
};

template< typename ...T >
const char tid<T...>::_id = 0;

template< typename ...T >
const tid_t tid<T...>::value = (tid_t)&tid<T...>::_id;

template< typename T >
using S = std::shared_ptr<T>;

template< typename T >
using U = std::unique_ptr<T>;

template< typename T >
using W = std::weak_ptr<T>;
