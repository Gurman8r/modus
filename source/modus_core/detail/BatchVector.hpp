#ifndef _ML_BATCH_VECTOR_HPP_
#define _ML_BATCH_VECTOR_HPP_

#include <modus_core/detail/List.hpp>

namespace ml::ds
{
	// tuple<vector<Ts>...>
	template <class ... _Ts
	> struct batch_vector final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type			= typename _ML ds::batch_vector<_Ts...>;
		using allocator_type	= typename pmr::polymorphic_allocator<byte_t>;
		using value_types		= typename meta::list<_Ts...>;
		using value_tuple		= typename meta::tuple<value_types>;
		using vector_types		= typename meta::remap<ds::list, value_types>;
		using vector_tuple		= typename meta::tuple<vector_types>;
		using init_type			= typename std::initializer_list<value_tuple>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr size_t npos
		{
			static_cast<size_t>(-1)
		};

		static constexpr size_t tuple_size
		{
			std::tuple_size_v<value_tuple>
		};

		static constexpr auto tuple_sequence
		{
			std::make_index_sequence<tuple_size>{}
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I>	using value_i			= typename meta::nth<I, value_types>;
		template <class  T> using value_t			= typename T;
		template <size_t I>	using vector_i			= typename meta::nth<I, vector_types>;
		template <class  T> using vector_t			= typename ds::list<T>;
		template <size_t I>	using iterator_i		= typename vector_i<I>::iterator;
		template <class  T>	using iterator_t		= typename ds::list<T>::iterator;
		template <size_t I>	using const_iterator_i	= typename vector_i<I>::const_iterator;
		template <class  T>	using const_iterator_t	= typename ds::list<T>::const_iterator;

		using iterator_types		= typename meta::remap<iterator_t, value_types>;
		using iterator_tuple		= typename meta::tuple<iterator_types>;

		using const_iterator_types	= typename meta::remap<const_iterator_t, value_types>;
		using const_iterator_tuple	= typename meta::tuple<const_iterator_types>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		batch_vector() noexcept
			: m_data{}
		{
		}

		batch_vector(allocator_type alloc) noexcept
			: m_data{ std::allocator_arg, alloc }
		{
		}

		batch_vector(init_type value, allocator_type alloc = {}) noexcept
			: m_data{ std::allocator_arg, alloc }
		{
			this->reserve(value.size());

			for (auto const & elem : value)
			{
				this->emplace_back(elem);
			}
		}

		explicit batch_vector(vector_tuple const & value, allocator_type alloc = {})
			: m_data{ std::allocator_arg, alloc, value }
		{
		}

		explicit batch_vector(vector_tuple && value, allocator_type alloc = {}) noexcept
			: m_data{ std::allocator_arg, alloc, std::move(value) }
		{
		}

		batch_vector(self_type const & value, allocator_type alloc = {})
			: m_data{ std::allocator_arg, alloc, value.m_data }
		{
		}

		batch_vector(self_type && value, allocator_type alloc = {}) noexcept
			: m_data{ std::allocator_arg, alloc, std::move(value.m_data) }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		self_type & operator=(init_type value)
		{
			self_type temp{ init };
			this->swap(temp);
			return (*this);
		}

		self_type & operator=(vector_tuple const & value)
		{
			self_type temp{ value };
			this->swap(temp);
			return (*this);
		}

		self_type & operator=(vector_tuple && value) noexcept
		{
			this->swap(std::move(value));
			return (*this);
		}

		self_type & operator=(self_type const & value)
		{
			self_type temp{ value };
			this->swap(temp);
			return (*this);
		}

		self_type & operator=(self_type && value) noexcept
		{
			this->swap(std::move(value));
			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD decltype(auto) data() & noexcept { return m_data; }

		ML_NODISCARD decltype(auto) data() const & noexcept { return m_data; }

		ML_NODISCARD decltype(auto) data() && noexcept { return std::move(m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I> ML_NODISCARD decltype(auto) get() & noexcept
		{
			return std::get<I>(m_data);
		}

		template <size_t I> ML_NODISCARD decltype(auto) get() const & noexcept
		{
			return std::get<I>(m_data);
		}

		template <size_t I> ML_NODISCARD decltype(auto) get() && noexcept
		{
			return std::move(std::get<I>(m_data));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T> ML_NODISCARD decltype(auto) get() & noexcept
		{
			return std::get<ds::list<T>>(m_data);
		}

		template <class T> ML_NODISCARD decltype(auto) get() const & noexcept
		{
			return std::get<ds::list<T>>(m_data);
		}

		template <class T> ML_NODISCARD decltype(auto) get() && noexcept
		{
			return std::move(std::get<ds::list<T>>(m_data));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t ... Is
		> ML_NODISCARD decltype(auto) at(size_t const i, std::index_sequence<Is...>) noexcept
		{
			if constexpr (1 == sizeof...(Is))
			{
				return this->get<Is...>()[i];
			}
			else
			{
				return std::forward_as_tuple(this->get<Is>()[i]...);
			}
		}

		template <size_t ... Is
		> ML_NODISCARD decltype(auto) at(size_t const i, std::index_sequence<Is...>) const noexcept
		{
			if constexpr (1 == sizeof...(Is))
			{
				return this->get<Is...>()[i];
			}
			else
			{
				return std::forward_as_tuple(this->get<Is>()[i]...);
			}
		}

		template <size_t ... Is
		> ML_NODISCARD decltype(auto) at(size_t const i) noexcept
		{
			if constexpr (1 == sizeof...(Is))
			{
				return this->get<Is...>()[i];
			}
			else
			{
				return std::forward_as_tuple(this->get<Is>()[i]...);
			}
		}

		template <size_t ... Is
		> ML_NODISCARD decltype(auto) at(size_t const i) const noexcept
		{
			if constexpr (1 == sizeof...(Is))
			{
				return this->get<Is...>()[i];
			}
			else
			{
				return std::forward_as_tuple(this->get<Is>()[i]...);
			}
		}

		template <class ... Ts
		> ML_NODISCARD decltype(auto) at(size_t const i) noexcept
		{
			if constexpr (1 == sizeof...(Ts))
			{
				return this->get<Ts...>()[i];
			}
			else
			{
				return std::forward_as_tuple(this->get<Ts>()[i]...);
			}
		}

		template <class ... Ts
		> ML_NODISCARD decltype(auto) at(size_t const i) const noexcept
		{
			if constexpr (1 == sizeof...(Ts))
			{
				return this->get<Ts...>()[i];
			}
			else
			{
				return std::forward_as_tuple(this->get<Ts>()[i]...);
			}
		}

		ML_NODISCARD decltype(auto) at(size_t const i) noexcept
		{
			return this->at(i, tuple_sequence);
		}

		ML_NODISCARD decltype(auto) at(size_t const i) const noexcept
		{
			return this->at(i, tuple_sequence);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t ... Is
		> ML_NODISCARD decltype(auto) back(std::index_sequence<Is...>) noexcept
		{
			if constexpr (1 == sizeof...(Is))
			{
				return this->get<Is...>().back();
			}
			else
			{
				return std::forward_as_tuple(this->get<Is>().back()...);
			}
		}

		template <size_t ... Is
		> ML_NODISCARD decltype(auto) back(std::index_sequence<Is...>) const noexcept
		{
			if constexpr (1 == sizeof...(Is))
			{
				return this->get<Is...>().back();
			}
			else
			{
				return std::forward_as_tuple(this->get<Is>().back()...);
			}
		}

		template <size_t ... Is
		> ML_NODISCARD decltype(auto) back() noexcept
		{
			if constexpr (1 == sizeof...(Is))
			{
				return this->get<Is...>().back();
			}
			else
			{
				return std::forward_as_tuple(this->get<Is>().back()...);
			}
		}

		template <size_t ... Is
		> ML_NODISCARD decltype(auto) back() const noexcept
		{
			if constexpr (1 == sizeof...(Is))
			{
				return this->get<Is...>().back();
			}
			else
			{
				return std::forward_as_tuple(this->get<Is>().back()...);
			}
		}

		template <class ... Ts
		> ML_NODISCARD decltype(auto) back() noexcept
		{
			if constexpr (1 == sizeof...(Ts))
			{
				return this->get<Ts...>().back();
			}
			else
			{
				return std::forward_as_tuple(this->get<Ts>().back()...);
			}
		}

		template <class ... Ts
		> ML_NODISCARD decltype(auto) back() const noexcept
		{
			if constexpr (1 == sizeof...(Ts))
			{
				return this->get<Ts...>().back();
			}
			else
			{
				return std::forward_as_tuple(this->get<Ts>().back()...);
			}
		}

		ML_NODISCARD decltype(auto) back() noexcept
		{
			return this->back(tuple_sequence);
		}

		ML_NODISCARD decltype(auto) back() const noexcept
		{
			return this->back(tuple_sequence);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t ... Is
		> ML_NODISCARD decltype(auto) front(std::index_sequence<Is...>) noexcept
		{
			if constexpr (1 == sizeof...(Is))
			{
				return this->get<Is...>().front();
			}
			else
			{
				return std::forward_as_tuple(this->get<Is>().front()...);
			}
		}

		template <size_t ... Is
		> ML_NODISCARD decltype(auto) front(std::index_sequence<Is...>) const noexcept
		{
			if constexpr (1 == sizeof...(Is))
			{
				return this->get<Is...>().front();
			}
			else
			{
				return std::forward_as_tuple(this->get<Is>().front()...);
			}
		}

		template <size_t ... Is
		> ML_NODISCARD decltype(auto) front() noexcept
		{
			if constexpr (1 == sizeof...(Is))
			{
				return this->get<Is...>().front();
			}
			else
			{
				return std::forward_as_tuple(this->get<Is>().front()...);
			}
		}

		template <size_t ... Is
		> ML_NODISCARD decltype(auto) front() const noexcept
		{
			if constexpr (1 == sizeof...(Is))
			{
				return this->get<Is...>().front();
			}
			else
			{
				return std::forward_as_tuple(this->get<Is>().front()...);
			}
		}

		template <class ... Ts
		> ML_NODISCARD decltype(auto) front() noexcept
		{
			if constexpr (1 == sizeof...(Ts))
			{
				return this->get<Ts...>().front();
			}
			else
			{
				return std::forward_as_tuple(this->get<Ts>().front()...);
			}
		}

		template <class ... Ts
		> ML_NODISCARD decltype(auto) front() const noexcept
		{
			if constexpr (1 == sizeof...(Ts))
			{
				return this->get<Ts...>().front();
			}
			else
			{
				return std::forward_as_tuple(this->get<Ts>().front()...);
			}
		}

		ML_NODISCARD decltype(auto) front() noexcept
		{
			return this->front(tuple_sequence);
		}

		ML_NODISCARD decltype(auto) front() const noexcept
		{
			return this->front(tuple_sequence);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn, size_t ... Is
		> void expand(Fn && fn, std::index_sequence<Is...>) noexcept
		{
			std::invoke(ML_forward(fn), this->get<Is>()...);
		}

		template <class Fn, size_t ... Is
		> void expand(Fn && fn, std::index_sequence<Is...>) const noexcept
		{
			std::invoke(ML_forward(fn), this->get<Is>()...);
		}

		template <size_t ... Is, class Fn
		> void expand(Fn && fn) noexcept
		{
			std::invoke(ML_forward(fn), this->get<Is>()...);
		}

		template <size_t ... Is, class Fn
		> void expand(Fn && fn) const noexcept
		{
			std::invoke(ML_forward(fn), this->get<Is>()...);
		}

		template <class ... Ts, class Fn
		> void expand(Fn && fn) noexcept
		{
			std::invoke(ML_forward(fn), this->get<Ts>()...);
		}

		template <class ... Ts, class Fn
		> void expand(Fn && fn) const noexcept
		{
			std::invoke(ML_forward(fn), this->get<Ts>()...);
		}

		template <class Fn> void expand_all(Fn && fn) noexcept
		{
			this->expand(ML_forward(fn), tuple_sequence);
		}

		template <class Fn> void expand_all(Fn && fn) const noexcept
		{
			this->expand(ML_forward(fn), tuple_sequence);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn, size_t ... Is
		> void expand(size_t const i, Fn && fn, std::index_sequence<Is...>) noexcept
		{
			std::invoke(ML_forward(fn), this->at<Is>(i)...);
		}

		template <class Fn, size_t ... Is
		> void expand(size_t const i, Fn && fn, std::index_sequence<Is...>) const noexcept
		{
			std::invoke(ML_forward(fn), this->at<Is>(i)...);
		}

		template <size_t ... Is, class Fn
		> void expand(size_t const i, Fn && fn) noexcept
		{
			std::invoke(ML_forward(fn), this->at<Is>(i)...);
		}

		template <size_t ... Is, class Fn
		> void expand(size_t const i, Fn && fn) const noexcept
		{
			std::invoke(ML_forward(fn), this->at<Is>(i)...);
		}

		template <class ... Ts, class Fn
		> void expand(size_t const i, Fn && fn) noexcept
		{
			std::invoke(ML_forward(fn), this->at<Ts>(i)...);
		}

		template <class ... Ts, class Fn
		> void expand(size_t const i, Fn && fn) const noexcept
		{
			std::invoke(ML_forward(fn), this->at<Ts>(i)...);
		}

		template <class Fn> void expand_all(size_t const i, Fn && fn) noexcept
		{
			this->expand(i, ML_forward(fn), tuple_sequence);
		}

		template <class Fn> void expand_all(size_t const i, Fn && fn) const noexcept
		{
			this->expand(i, ML_forward(fn), tuple_sequence);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn
		> void for_tuple(Fn && fn) noexcept
		{
			meta::for_tuple(m_data, ML_forward(fn));
		}

		template <class Fn
		> void for_tuple(Fn && fn) const noexcept
		{
			meta::for_tuple(m_data, ML_forward(fn));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t ... Is, class Fn
		> void for_indices(Fn && fn) noexcept
		{
			this->expand<Is...>([&](auto && ... vs) noexcept
			{
				meta::for_args([&](auto & v) noexcept
				{
					std::invoke(ML_forward(fn), v);
				}
				, ML_forward(vs)...);
			});
		}

		template <class ... Ts, class Fn
		> void for_types(Fn && fn) noexcept
		{
			this->expand<Ts...>([&](auto && ... vs) noexcept
			{
				meta::for_args([&](auto & v) noexcept
				{
					std::invoke(ML_forward(fn), v);
				}
				, ML_forward(vs)...);
			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I = 0> ML_NODISCARD size_t capacity() const noexcept
		{
			return this->get<I>().capacity();
		}

		template <class T> ML_NODISCARD size_t capacity() const noexcept
		{
			return this->get<T>().capacity();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I = 0> ML_NODISCARD bool empty() const noexcept
		{
			return this->get<I>().empty();
		}

		template <class T> ML_NODISCARD bool empty() const noexcept
		{
			return this->get<T>().empty();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I = 0> ML_NODISCARD size_t max_size() const noexcept
		{
			return this->get<I>().max_size();
		}

		template <class T> ML_NODISCARD size_t max_size() const noexcept
		{
			return this->get<T>().max_size();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I = 0> ML_NODISCARD size_t size() const noexcept
		{
			return this->get<I>().size();
		}

		template <class T> ML_NODISCARD size_t size() const noexcept
		{
			return this->get<T>().size();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I
		> ML_NODISCARD size_t index_of(const_iterator_i<I> it) const noexcept
		{
			return (size_t)std::distance(this->cbegin<I>(), it);
		}

		template <class T
		> ML_NODISCARD size_t index_of(const_iterator_t<T> it) const noexcept
		{
			return (size_t)std::distance(this->cbegin<T>(), it);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void clear() noexcept
		{
			this->for_tuple([&](auto & v) noexcept { v.clear(); });
		}

		template <size_t ... Is
		> void clear() noexcept
		{
			this->for_indices<Is...>([&](auto & v) noexcept { v.clear(); });
		}

		template <class ... Ts
		> void clear() noexcept
		{
			this->for_types<Ts...>([&](auto & v) noexcept { v.clear(); });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void pop_back() noexcept
		{
			this->for_tuple([&](auto & v) noexcept { v.pop_back(); });
		}

		template <size_t ... Is
		> void pop_back() noexcept
		{
			this->for_indices<Is...>([&](auto & v) noexcept { v.pop_back(); });
		}

		template <class ... Ts
		> void pop_back() noexcept
		{
			this->for_types<Ts...>([&](auto & v) noexcept { v.pop_back(); });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void reserve(size_t const value)
		{
			this->for_tuple([&](auto & v) { v.reserve(value); });
		}

		template <size_t ... Is
		> void reserve(size_t const value)
		{
			this->for_indices<Is...>([&](auto & v) { v.reserve(value); });
		}

		template <class ... Ts
		> void reserve(size_t const value)
		{
			this->for_types<Ts...>([&](auto & v) { v.reserve(value); });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void resize(size_t const value)
		{
			this->for_tuple([&](auto & v) { v.resize(value); });
		}

		template <size_t ... Is
		> void resize(size_t const value)
		{
			this->for_indices<Is...>([&](auto & v) { v.resize(value); });
		}

		template <class ... Ts
		> void resize(size_t const value)
		{
			this->for_types<Ts...>([&](auto & v) { v.resize(value); });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void shrink_to_fit() noexcept
		{
			this->for_tuple([&](auto & v) noexcept { v.shrink_to_fit(); });
		}

		template <size_t ... Is
		> void shrink_to_fit() noexcept
		{
			this->for_indices<Is...>([&](auto & v) noexcept { v.shrink_to_fit(); });
		}

		template <class ... Ts
		> void shrink_to_fit() noexcept
		{
			this->for_types<Ts...>([&](auto & v) noexcept { v.shrink_to_fit(); });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void erase(size_t const i)
		{
			this->for_tuple([&](auto & v)
			{
				v.erase(v.begin() + i);
			});
		}

		void erase(size_t const first, size_t const last)
		{
			this->for_tuple([&](auto & v)
			{
				v.erase(v.begin() + first, v.begin() + last);
			});
		}

		template <size_t ... Is
		> void erase(size_t const i)
		{
			this->for_indices<Is...>([&](auto & v)
			{
				v.erase(v.begin() + i);
			});
		}

		template <size_t ... Is
		> void erase(size_t const first, size_t const last)
		{
			this->for_indices<Is...>([&](auto & v)
			{
				v.erase(v.begin() + first, v.begin() + last);
			});
		}

		template <class ... Ts
		> void erase(size_t const i)
		{
			this->for_types<Ts...>([&](auto & v)
			{
				v.erase(v.begin() + i);
			});
		}

		template <class ... Ts
		> void erase(size_t const first, size_t const last)
		{
			this->for_types<Ts...>([&](auto & v)
			{
				v.erase(v.begin() + first, v.begin() + last);
			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I, class U = value_i<I>
		> ML_NODISCARD iterator_i<I> find(U && value) noexcept
		{
			return std::find(this->begin<I>(), this->end<I>(), ML_forward(value));
		}

		template <size_t I, class U = value_i<I>
		> ML_NODISCARD const_iterator_i<I> find(U && value) const noexcept
		{
			return std::find(this->cbegin<I>(), this->cend<I>(), ML_forward(value));
		}

		template <class T, class U = T
		> ML_NODISCARD iterator_t<T> find(U && value) noexcept
		{
			return std::find(this->begin<T>(), this->end<T>(), ML_forward(value));
		}

		template <class T, class U = T
		> ML_NODISCARD const_iterator_t<T> find(U && value) const noexcept
		{
			return std::find(this->cbegin<T>(), this->cend<T>(), ML_forward(value));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I, class Pr
		> ML_NODISCARD iterator_i<I> find_if(Pr && pr) noexcept
		{
			return std::find_if(this->begin<I>(), this->end<I>(), ML_forward(pr));
		}

		template <size_t I, class Pr
		> ML_NODISCARD const_iterator_i<I> find_if(Pr && pr) const noexcept
		{
			return std::find_if(this->cbegin<I>(), this->cend<I>(), ML_forward(pr));
		}

		template <class T, class Pr
		> ML_NODISCARD iterator_t<T> find_if(Pr && pr) noexcept
		{
			return std::find_if(this->begin<T>(), this->end<T>(), ML_forward(pr));
		}

		template <class T, class Pr
		> ML_NODISCARD const_iterator_t<T> find_if(Pr && pr) const noexcept
		{
			return std::find_if(this->cbegin<T>(), this->cend<T>(), ML_forward(pr));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I, class U = value_i<I>
		> ML_NODISCARD bool binary_search(U && value) const noexcept
		{
			return std::binary_search(this->cbegin<I>(), this->cend<I>(), ML_forward(value));
		}

		template <class T, class U = T
		> ML_NODISCARD bool binary_search(U && value) const noexcept
		{
			return std::binary_search(this->cbegin<T>(), this->cend<T>(), ML_forward(value));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I, class U = value_i<I>
		> ML_NODISCARD bool contains(U && value) const noexcept
		{
			return this->end<T>() != this->find<I>(ML_forward(value));
		}

		template <class T, class U = T
		> ML_NODISCARD bool contains(U && value) const noexcept
		{
			return this->end<T>() != this->find<T>(ML_forward(value));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I, class U = value_i<I>
		> ML_NODISCARD size_t lookup(U && value) const noexcept
		{
			if (auto const it{ this->find<I>(ML_forward(value)) }
			; it == this->end<I>()) { return npos; }
			else { return this->index_of<I>(it); }
		}

		template <class T, class U = value_t<T>
		> ML_NODISCARD size_t lookup(U && value) const noexcept
		{
			if (auto const it{ this->find<T>(ML_forward(value)) }
			; it == this->end<T>()) { return npos; }
			else { return this->index_of<T>(it); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I, class It = const_iterator_i<I>, class U = value_i<I>
		> iterator_i<I> insert(It it, U && value) noexcept
		{
			if constexpr (std::is_scalar_v<It>)
			{
				return this->get<I>().emplace(this->begin<I>() + it, ML_forward(value));
			}
			else
			{
				return this->get<I>().emplace(it, ML_forward(value));
			}
		}

		template <class T, class It = const_iterator_t<T>, class U = value_t<T>
		> iterator_t<T> insert(It it, U && value) noexcept
		{
			if constexpr (std::is_scalar_v<It>)
			{
				return this->get<T>().emplace(this->begin<T>() + it, ML_forward(value));
			}
			else
			{
				return this->get<T>().emplace(it, ML_forward(value));
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I, class U = value_i<I>
		> decltype(auto) push_back(U && value) noexcept
		{
			return this->get<I>().emplace_back(ML_forward(value));
		}

		template <class T, class U = value_t<T>
		> decltype(auto) push_back(U && value)
		{
			return this->get<T>().emplace_back(ML_forward(value));
		}

		template <
			class Tpl	= value_tuple,
			size_t I	= tuple_size,
			size_t N	= std::tuple_size_v<Tpl>
		> decltype(auto) push_back(Tpl && value)
		{
			static_assert(tuple_size <= N);
			if constexpr (I < N)
			{
				this->push_back<I>(std::get<I>(ML_forward(value)));

				return this->push_back<Tpl, I + 1, N>(ML_forward(value));
			}
			else
			{
				return this->back();
			}
		}

		template <class ... Args
		> decltype(auto) push_back(Args && ... args)
		{
			return this->push_back<value_tuple, 0, tuple_size
			>(value_tuple{ ML_forward(args)... });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void swap(self_type & value) noexcept
		{
			if (this != std::addressof(value))
			{
				m_data.swap(value.m_data);
			}
		}

		void swap(vector_tuple & value) noexcept
		{
			if (std::addressof(m_data) != std::addressof(value))
			{
				m_data.swap(value);
			}
		}

		template <size_t I, class U = vector_i<I>
		> void swap(U & value) noexcept
		{
			this->get<I>().swap(value);
		}

		template <class T, class U = T
		> void swap(U & value) noexcept
		{
			this->get<T>().swap(value);
		}

		void swap(size_t const lhs, size_t const rhs) noexcept
		{
			this->for_tuple([&](auto & v) noexcept { std::swap(v[lhs], v[rhs]); });
		}

		template <size_t ... Is
		> void swap(size_t const lhs, size_t const rhs) noexcept
		{
			this->for_indices<Is...>([&](auto & v) noexcept { std::swap(v[lhs], v[rhs]); });
		}

		template <class ... Ts
		> void swap(size_t const lhs, size_t const rhs) noexcept
		{
			this->for_types<Ts...>([&](auto & v) noexcept { std::swap(v[lhs], v[rhs]); });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I> ML_NODISCARD auto begin() noexcept { return this->get<I>().begin(); }

		template <size_t I> ML_NODISCARD auto begin() const noexcept { return this->get<I>().begin(); }

		template <size_t I> ML_NODISCARD auto cbegin() const noexcept { return this->get<I>().cbegin(); }

		template <class T> ML_NODISCARD auto begin() noexcept { return this->get<T>().begin(); }

		template <class T> ML_NODISCARD auto begin() const noexcept { return this->get<T>().begin(); }

		template <class T> ML_NODISCARD auto cbegin() const noexcept { return this->get<T>().cbegin(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I> ML_NODISCARD auto end() noexcept { return this->get<I>().end(); }

		template <size_t I> ML_NODISCARD auto end() const noexcept { return this->get<I>().end(); }

		template <size_t I> ML_NODISCARD auto cend() const noexcept { return this->get<I>().cend(); }

		template <class T> ML_NODISCARD auto end() noexcept { return this->get<T>().end(); }

		template <class T> ML_NODISCARD auto end() const noexcept { return this->get<T>().end(); }

		template <class T> ML_NODISCARD auto cend() const noexcept { return this->get<T>().cend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I> ML_NODISCARD auto rbegin() noexcept { return this->get<I>().rbegin(); }

		template <size_t I> ML_NODISCARD auto rbegin() const noexcept { return this->get<I>().rbegin(); }

		template <size_t I> ML_NODISCARD auto crbegin() const noexcept { return this->get<I>().crbegin(); }

		template <class T> ML_NODISCARD auto rbegin() noexcept { return this->get<T>().rbegin(); }

		template <class T> ML_NODISCARD auto rbegin() const noexcept { return this->get<T>().rbegin(); }

		template <class T> ML_NODISCARD auto crbegin() const noexcept { return this->get<T>().crbegin(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t I> ML_NODISCARD auto rend() noexcept { return this->get<I>().rend(); }

		template <size_t I> ML_NODISCARD auto rend() const noexcept { return this->get<I>().rend(); }

		template <size_t I> ML_NODISCARD auto crend() const noexcept { return this->get<I>().crend(); }

		template <class T> ML_NODISCARD auto rend() noexcept { return this->get<T>().rend(); }

		template <class T> ML_NODISCARD auto rend() const noexcept { return this->get<T>().rend(); }

		template <class T> ML_NODISCARD auto crend() const noexcept { return this->get<T>().crend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class U = self_type
		> ML_NODISCARD auto compare(U const & value) const noexcept
		{
			if constexpr (std::is_same_v<U, self_type>)
			{
				return this->compare(value.m_data);
			}
			else
			{
				static_assert(std::is_same_v<U, vector_tuple>);
				return ML_compare(m_data, value);
			}
		}

		template <class U = self_type
		> ML_NODISCARD bool operator==(U const & value) const noexcept
		{
			return this->compare(value) == 0;
		}

		template <class U = self_type
		> ML_NODISCARD bool operator!=(U const & value) const noexcept
		{
			return this->compare(value) != 0;
		}

		template <class U = self_type
		> ML_NODISCARD bool operator<(U const & value) const noexcept
		{
			return this->compare(value) < 0;
		}

		template <class U = self_type
		> ML_NODISCARD bool operator>(U const & value) const noexcept
		{
			return this->compare(value) > 0;
		}

		template <class U = self_type
		> ML_NODISCARD bool operator<=(U const & value) const noexcept
		{
			return this->compare(value) <= 0;
		}

		template <class U = self_type
		> ML_NODISCARD bool operator>=(U const & value) const noexcept
		{
			return this->compare(value) >= 0;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		vector_tuple m_data;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_BATCH_VECTOR_HPP_