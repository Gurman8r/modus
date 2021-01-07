#ifndef _ML_FLOW_HPP_
#define _ML_FLOW_HPP_

#include <modus_core/Standard.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml::impl
{
    // invoke function in constructor
    template <class Fn> struct block_impl final
    {
        block_impl(Fn && fn) noexcept { ML_forward(fn)(); }
    };

    enum class block_tag {};

    template <class Fn> auto operator+(block_tag, Fn && fn) noexcept
    {
        return block_impl<Fn>{ ML_forward(fn) };
    }
}

// block ex
#define ML_block_ex(...) \
    _ML impl::block_tag{} + [##__VA_ARGS__]() noexcept

// block
#define ML_block(...) \
    auto ML_anon = ML_block_ex(##__VA_ARGS__)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml::impl
{
    // invoke function in destructor
    template <class Fn> struct defer_impl final
    {
        defer_impl(Fn && fn) noexcept : m_fn{ ML_forward(fn) } {}

        ~defer_impl() noexcept { m_fn(); }

    private: Fn const m_fn;
    };

    enum class defer_tag {};

    template <class Fn> auto operator+(defer_tag, Fn && fn) noexcept
    {
        return defer_impl<Fn>{ ML_forward(fn) };
    }
}

// defer ex
#define ML_defer_ex(...) \
    _ML impl::defer_tag{} + [##__VA_ARGS__]() noexcept

// defer
#define ML_defer(...) \
    auto ML_anon = ML_defer_ex(##__VA_ARGS__)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_FLOW_HPP_