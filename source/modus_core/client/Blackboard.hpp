#ifndef _ML_BLACKBOARD_HPP_
#define _ML_BLACKBOARD_HPP_

#include <modus_core/system/Memory.hpp>
#include <modus_core/detail/FlatMap.hpp>

namespace ml
{
	struct blackboard final : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type	= typename pmr::polymorphic_allocator<byte_t>;
		using category_type		= typename ds::map<hash_t, shared<std::any>>;
		using catagories_type	= typename ds::map<hash_t, category_type>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> struct var final : trackable, non_copyable
		{
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

			using value_type		= typename T;
			using self_type			= typename blackboard::var<value_type>;
			using pointer			= typename value_type *;
			using const_pointer		= typename value_type const *;
			using reference			= typename value_type &;
			using const_reference	= typename value_type const &;

			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

			~var() noexcept
			{
				ML_check(m_bb)->erase<value_type>(m_id);
			}

			var() noexcept
				: m_bb	{}
				, m_id	{}
				, m_ptr	{}
			{
			}

			template <class ID
			> var(std::allocator_arg_t, allocator_type alloc, blackboard * bb, ID && id)
				: m_bb	{ bb }
				, m_id	{ hashof(ML_forward(id)) }
				, m_ptr	{ m_bb->load<value_type>(m_id, alloc) }
			{
			}

			template <class ID
			> var(blackboard * bb, ID && id) noexcept
				: self_type{ std::allocator_arg, {}, bb, ML_forward(id) }
			{
				if (auto const v{ lock() }; v && !v->has_value())
				{
					v->emplace<value_type>();
				}
			}

			template <class ID, class ... Args
			> var(blackboard * bb, ID && id, Args && ... args) noexcept
				: self_type{ std::allocator_arg, {}, bb, ML_forward(id) }
			{
				if (auto const v{ lock() }; v && !v->has_value())
				{
					v->emplace<value_type>(ML_forward(args)...);
				}
			}

			var(self_type && other) noexcept : self_type{}
			{
				this->swap(std::move(other));
			}

			self_type & operator=(self_type && other) noexcept
			{
				this->swap(std::move(other));
				return (*this);
			}

			void swap(self_type & other) noexcept
			{
				if (this != std::addressof(other))
				{
					std::swap(m_id, other.m_id);
					std::swap(m_bb, other.m_bb);
					m_ptr.swap(other.m_ptr);
				}
			}

			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

			auto type() const noexcept -> typeof<> { return typeof_v<value_type>; }

			auto id() const noexcept -> hash_t { return m_id; }
			
			auto parent() const noexcept -> blackboard * { return m_bb; }

			bool expired() const noexcept { return m_ptr.expired(); }

			auto lock() const noexcept -> shared<std::any> { return m_ptr.lock(); }

			bool has_value() const noexcept { return !expired() && lock()->has_value(); }
			
			auto get() const noexcept -> reference { return *std::any_cast<value_type>(lock().get()); }

			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

			ML_NODISCARD explicit operator bool() const noexcept { return has_value(); }

			ML_NODISCARD operator reference() const noexcept { return get(); }

			ML_NODISCARD auto operator->() const noexcept -> pointer { return &get(); }

			ML_NODISCARD auto operator*() const noexcept -> reference { return get(); }

			template <class I
			> decltype(auto) operator[](I && i) const noexcept
			{
				return get()[ML_forward(i)];
			}

			template <class ... Args
			> decltype(auto) operator()(Args && ... args) noexcept
			{
				return std::invoke(get(), ML_forward(args)...);
			}

			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

			ML_NODISCARD auto begin() noexcept { return get()->begin(); }
			
			ML_NODISCARD auto begin() const noexcept { return get()->begin(); }
			
			ML_NODISCARD auto cbegin() const noexcept { return get()->cbegin(); }

			ML_NODISCARD auto end() noexcept { return get()->end(); }
			
			ML_NODISCARD auto end() const noexcept { return get()->end(); }
			
			ML_NODISCARD auto cend() const noexcept { return get()->cend(); }

			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		private:
			blackboard *	m_bb	; // 
			hash_t			m_id	; // 
			unown<std::any>	m_ptr	; // 

			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		blackboard(allocator_type alloc = {}) noexcept
			: m_vars{ alloc }
		{
		}

		explicit blackboard(catagories_type const & data, allocator_type alloc = {})
			: m_vars{ data, alloc }
		{
		}

		explicit blackboard(catagories_type && data, allocator_type alloc = {}) noexcept
			: m_vars{ std::move(data), alloc }
		{
		}

		blackboard(blackboard const & other, allocator_type alloc = {})
			: blackboard{ other.m_vars, alloc }
		{
		}

		blackboard(blackboard && other, allocator_type alloc = {}) noexcept
			: blackboard{ std::move(other.m_vars), alloc }
		{
		}

		blackboard & operator=(blackboard const & other)
		{
			blackboard temp{ other };
			this->swap(temp);
			return (*this);
		}

		blackboard & operator=(blackboard && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto data() & noexcept -> catagories_type & { return m_vars; }

		ML_NODISCARD auto data() const & noexcept -> catagories_type const & { return m_vars; }

		ML_NODISCARD auto capacity() const noexcept -> size_t { return m_vars.capacity(); }

		ML_NODISCARD bool empty() const noexcept { return m_vars.empty(); }

		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type { return m_vars.get_allocator(); }

		ML_NODISCARD auto max_size() const noexcept -> size_t { return m_vars.max_size(); }

		ML_NODISCARD auto size() const noexcept -> size_t { return m_vars.size(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto find(hash_t type) noexcept -> catagories_type::optl_iterator_pair
		{
			return m_vars.find(type);
		}

		ML_NODISCARD auto find(hash_t type) const noexcept -> catagories_type::optl_const_iterator_pair
		{
			return m_vars.find(type);
		}

		template <class T
		> ML_NODISCARD auto find() noexcept -> catagories_type::optl_iterator_pair
		{
			return this->find(hashof<T>());
		}

		template <class T
		> ML_NODISCARD auto find() const noexcept -> catagories_type::optl_const_iterator_pair
		{
			return this->find(hashof<T>());
		}

		template <class ID
		> ML_NODISCARD auto find(hash_t type, ID && id) noexcept -> category_type::optl_iterator_pair
		{
			return m_vars[type].find(hashof(ML_forward(id)));
		}

		template <class ID
		> ML_NODISCARD auto find(hash_t type, ID && id) const noexcept -> category_type::optl_const_iterator_pair
		{
			if (auto const it{ this->find(type) }; !it) { return std::nullopt; }
			else { return it->second->find(hashof(ML_forward(id))); }
		}

		template <class T, class ID
		> ML_NODISCARD auto find(ID && id) noexcept -> category_type::optl_iterator_pair
		{
			return this->find(hashof<T>(), ML_forward(id));
		}

		template <class T, class ID
		> ML_NODISCARD auto find(ID && id) const noexcept -> category_type::optl_const_iterator_pair
		{
			return this->find(hashof<T>(), ML_forward(id));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool contains(hash_t type) const noexcept
		{
			return m_vars.contains(type);
		}

		template <class T> ML_NODISCARD bool contains() const noexcept
		{
			return this->contains(hashof<T>());
		}

		template <class ID
		> ML_NODISCARD bool contains(hash_t type, ID && id) const noexcept
		{
			if (auto const it{ this->find(type) }; !it) { return false; }
			else { return it->second->contains(hashof(ML_forward(id))); }
		}

		template <class T, class ID
		> ML_NODISCARD bool contains(ID && id) const noexcept
		{
			return this->contains(hashof<T>(), ML_forward(id));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void erase(hash_t type) noexcept
		{
			if (auto const cat{ this->find(type) })
			{
				m_vars.erase(cat->first);
			}
		}

		template <class T
		> void erase() noexcept
		{
			this->erase(hashof<T>());
		}

		template <class ID
		> void erase(hash_t type, ID && id) noexcept
		{
			if (auto const cat{ this->find(type) })
			{
				if (auto const var{ cat->second->find(hashof(ML_forward(id))) })
				{
					cat->second->erase(var->first);
				}
			}
		}

		template <class T, class ID
		> void erase(ID && id) noexcept
		{
			this->erase(hashof<T>(), ML_forward(id));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class ID
		> ML_NODISCARD shared<std::any> & load(hash_t type, ID && id, allocator_type alloc = {}) noexcept
		{
			return m_vars[type].find_or_add_fn(hashof(ML_forward(id)), [&]() noexcept
			{
				return std::allocate_shared<std::any>(alloc);
			});
		}

		template <class T, class ID
		> ML_NODISCARD shared<std::any> & load(ID && id, allocator_type alloc = {}) noexcept
		{
			return this->load(hashof<T>(), ML_forward(id), alloc);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T, class ID
		> ML_NODISCARD T & get(ID && id, allocator_type alloc = {})
		{
			return *std::any_cast<T>(this->load<T>(ML_forward(id), alloc).get());
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T, class ID, class ... Args
		> T & emplace(allocator_type alloc, ID && id, Args && ... args) noexcept
		{
			return this->load<T>(ML_forward(id), alloc)->emplace<T>(ML_forward(args)..., alloc);
		}

		template <class T, class ID, class ... Args
		> T & emplace(ID && id, Args && ... args) noexcept
		{
			return this->load<T>(ML_forward(id))->emplace<T>(ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class ID
		> void reset(hash_t type, ID && id, allocator_type alloc = {}) noexcept
		{
			this->load(type, ML_forward(id), alloc)->reset();
		}

		template <class T, class ID
		> void reset(ID && id, allocator_type alloc = {}) noexcept
		{
			this->reset(hashof<T>(), ML_forward(id), alloc);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void swap(blackboard & other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_vars.swap(other.m_vars);
			}
		}

		template <class LHS, class RHS = LHS
		> void swap(hash_t type, LHS && lhs, RHS && rhs, allocator_type alloc = {}) noexcept
		{
			this->load(type, ML_forward(lhs), alloc)
				->swap(*this->load(type, ML_forward(rhs), alloc));
		}

		template <class T, class LHS, class RHS = LHS
		> void swap(LHS && lhs, RHS && rhs, allocator_type alloc = {}) noexcept
		{
			this->swap(hashof<T>(), ML_forward(lhs), ML_forward(rhs), alloc);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void clear() noexcept { m_vars.clear(); }

		void clear(hash_t type) noexcept { m_vars[type].clear(); }

		template <class T
		> void clear() { this->clear(hashof<T>()); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void resize(size_t const count) { m_vars.resize(count); }
		
		void resize(hash_t type, size_t const count) { m_vars[type].resize(count); }

		template <class T
		> void resize(size_t const count) { this->resize(hashof<T>(), count); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void reserve(size_t const count) { m_vars.reserve(count); }

		void reserve(hash_t type, size_t const count) { m_vars[type].reserve(count); }
		
		template <class T
		> void reserve(size_t const count) { this->reserve(hashof<T>(), count); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void shrink_to_fit() noexcept { m_vars.shrink_to_fit(); }

		void shrink_to_fit(hash_t type) noexcept { m_vars[type].shrink_to_fit(); }

		template <class T
		> void shrink_to_fit() { this->shrink_to_fit(hashof<T>()); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		catagories_type m_vars; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_BLACKBOARD_HPP_