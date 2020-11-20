#ifndef _ML_LAYERS_HPP_
#define _ML_LAYERS_HPP_

#include <modus_core/detail/Events.hpp>
#include <modus_core/detail/Duration.hpp>

// LAYER
namespace ml
{
	// layer
	struct layer : trackable, event_listener
	{
		explicit layer(event_bus * bus) noexcept : event_listener{ bus } {}

		virtual ~layer() noexcept override = default;

		virtual void on_attach() = 0;

		virtual void on_detach() = 0;

		virtual void on_update(duration const &) = 0;

		virtual void on_event(event const &) override = 0;
	};
}

// LAYER STACK
namespace ml
{
	// layer stack
	struct layer_stack final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type					= typename layer_stack;
		using allocator_type			= typename pmr::polymorphic_allocator<byte>;
		using difference_type			= typename ptrdiff_t;
		using size_type					= typename size_t;
		
		using value_type				= typename layer *;
		using storage_type				= typename ds::list<value_type>;
		using pointer					= typename storage_type::pointer;
		using const_pointer				= typename storage_type::const_pointer;
		using reference					= typename storage_type::reference;
		using const_reference			= typename storage_type::const_reference;

		using iterator					= typename storage_type::iterator;
		using const_iterator			= typename storage_type::const_iterator;
		using reverse_iterator			= typename storage_type::reverse_iterator;
		using const_reverse_iterator	= typename storage_type::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		layer_stack(allocator_type alloc = {}) noexcept
			: m_data{ alloc }
			, m_index{}
		{
		}

		layer_stack(self_type && other, allocator_type alloc = {}) noexcept
			: layer_stack{ alloc }
		{
			this->swap(std::move(other));
		}

		~layer_stack() noexcept final
		{
			for (auto & e : m_data)
			{
				delete e;
			}
			m_data.clear();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		self_type & operator=(self_type && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(self_type & other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_data.swap(other.m_data);
				std::swap(m_index, other.m_index);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T = layer, class ... Args
		> auto emplace_layer(Args && ... args) noexcept -> T *
		{
			static_assert(std::is_base_of_v<layer, T>, "?");

			return static_cast<T *>(*m_data.emplace
			(
				begin() + m_index++, new T{ ML_forward(args)... }
			));
		}

		auto push_layer(layer * value) noexcept -> layer *
		{
			if (!value) { return nullptr; }
			else if (auto const it{ std::find(begin(), end(), value) }
			; it != end()) { return nullptr; }
			else
			{
				return *m_data.emplace(begin() + m_index++, value);
			}
		}

		auto pop_layer(layer * value) noexcept -> iterator
		{
			if (!value) { return end(); }
			else if (auto const it{ std::find(begin(), end(), value) }
			; it == end()) { return it; }
			else
			{
				m_index--;
				return m_data.erase(it);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T = layer, class ... Args
		> auto emplace_overlay(Args && ... args) noexcept -> T *
		{
			static_assert(std::is_base_of_v<layer, T>, "?");

			return static_cast<T *>(m_data.emplace_back
			(
				new T{ ML_forward(args)... }
			));
		}

		auto push_overlay(layer * value) noexcept -> layer *
		{
			if (!value) { return nullptr; }
			else if (auto const it{ std::find(begin(), end(), value) }
			; it != end()) { return nullptr; }
			else
			{
				return m_data.emplace_back(value);
			}
		}

		auto pop_overlay(layer * value) noexcept -> iterator
		{
			if (!value) { return end(); }
			else if (auto const it{ std::find(begin(), end(), value) }
			; it == end()) { return it; }
			else
			{
				return m_data.erase(it);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto operator[](size_t const i) noexcept -> reference { return m_data[i]; }

		ML_NODISCARD auto operator[](size_t const i) const noexcept -> const_reference { return m_data[i]; }

		ML_NODISCARD auto at(size_t const i) -> reference { return m_data.at(i); }

		ML_NODISCARD auto at(size_t const i) const -> const_reference { return m_data.at(i); }

		ML_NODISCARD auto back() noexcept -> reference { return m_data.back(); }

		ML_NODISCARD auto back() const noexcept -> const_reference { return m_data.back(); }

		ML_NODISCARD auto capacity() const noexcept -> size_t { return m_data.capacity(); }

		ML_NODISCARD auto data() noexcept -> pointer { return m_data.data(); }

		ML_NODISCARD auto data() const noexcept -> const_pointer { return m_data.data(); }

		ML_NODISCARD bool empty() const noexcept { return m_data.empty(); }

		ML_NODISCARD auto front() noexcept -> reference { return m_data.front(); }

		ML_NODISCARD auto front() const noexcept -> const_reference { return m_data.front(); }
		
		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type { return {}; }

		ML_NODISCARD auto max_size() const noexcept -> size_t { return m_data.max_size(); }

		ML_NODISCARD auto size() const noexcept -> size_t { return m_data.size(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto begin() noexcept -> iterator { return m_data.begin(); }

		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_data.begin(); }

		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return m_data.cbegin(); }

		ML_NODISCARD auto end() noexcept -> iterator { return m_data.end(); }

		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_data.end(); }

		ML_NODISCARD auto cend() const noexcept -> const_iterator { return m_data.cend(); }

		ML_NODISCARD auto rbegin() noexcept -> reverse_iterator { return m_data.rbegin(); }

		ML_NODISCARD auto rbegin() const noexcept -> const_reverse_iterator { return m_data.rbegin(); }

		ML_NODISCARD auto crbegin() const noexcept -> const_reverse_iterator { return m_data.crbegin(); }

		ML_NODISCARD auto rend() noexcept -> reverse_iterator { return m_data.rend(); }

		ML_NODISCARD auto rend() const noexcept -> const_reverse_iterator { return m_data.rend(); }

		ML_NODISCARD auto crend() const noexcept -> const_reverse_iterator { return m_data.crend(); }

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
				static_assert(std::is_same_v<U, storage_type>);
				return ML_compare(m_data, value);
			}
		}

		template <class U = self_type
		> ML_NODISCARD bool operator==(U const & value) const noexcept { return compare(value) == 0; }

		template <class U = self_type
		> ML_NODISCARD bool operator!=(U const & value) const noexcept { return compare(value) != 0; }

		template <class U = self_type
		> ML_NODISCARD bool operator<(U const & value) const noexcept { return compare(value) < 0; }

		template <class U = self_type
		> ML_NODISCARD bool operator>(U const & value) const noexcept { return compare(value) > 0; }

		template <class U = self_type
		> ML_NODISCARD bool operator<=(U const & value) const noexcept { return compare(value) <= 0; }

		template <class U = self_type
		> ML_NODISCARD bool operator>=(U const & value) const noexcept { return compare(value) >= 0; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::list<layer *>	m_data	; // 
		size_t				m_index	; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_LAYERS_HPP_