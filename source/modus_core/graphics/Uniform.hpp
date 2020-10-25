#ifndef _ML_UNIFORM_HPP_
#define _ML_UNIFORM_HPP_

// FIXME: not great, needs some TLC <3

#include <modus_core/graphics/RenderAPI.hpp>

namespace ml
{
	// uniform
	struct uniform final : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using sampler = typename shared<gfx::texture>;

		template <class T> static constexpr bool is_sampler_ish
		{
			!std::is_same_v<T, sampler> &&
			std::is_convertible_v<std::add_pointer_t<T>, std::add_pointer_t<sampler::element_type>>
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allowed_types = meta::list<
			bool, int32_t, float32_t,
			vec2, vec3, vec4, color,
			mat2, mat3, mat4,
			sampler
		>;

		using variable_type = typename meta::rename<std::variant, allowed_types>;

		using function_type = typename ds::method<variable_type()>;

		enum : size_t { id_variable, id_function };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using info_type		= typename typeof<>;
		using name_type		= typename ds::string;
		using data_type		= typename std::variant<variable_type, function_type>;
		using storage_type	= typename std::tuple<info_type, name_type, data_type>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		uniform() noexcept
			: m_storage{}
		{
		}

		explicit uniform(storage_type const & value)
			: m_storage{ value }
		{
		}

		explicit uniform(storage_type && value) noexcept
			: m_storage{ std::move(value) }
		{
		}

		uniform(uniform const & other)
			: m_storage{ other.m_storage }
		{
		}

		uniform(uniform && other) noexcept
			: m_storage{ std::move(other.m_storage) }
		{
		}

		template <class I, class N, class D
		> uniform(I && type, N && name, D && data)
			: m_storage{ ML_forward(type), ML_forward(name), ML_forward(data) }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		uniform & operator=(uniform const & other)
		{
			uniform temp{ other };
			this->swap(temp);
			return (*this);
		}

		uniform & operator=(uniform && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(uniform & other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_storage.swap(other.m_storage);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto const & type() const & noexcept { return std::get<info_type>(m_storage); }

		ML_NODISCARD auto const & name() const & noexcept { return std::get<name_type>(m_storage); }

		ML_NODISCARD auto const & data() const & noexcept { return std::get<data_type>(m_storage); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD variable_type var() const noexcept
		{
			switch (auto const & d{ std::get<data_type>(m_storage) }; d.index())
			{
			default			: return variable_type{};
			case id_variable: return std::get<variable_type>(d);
			case id_function: return std::invoke(std::get<function_type>(d));
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> ML_NODISCARD bool holds() const noexcept
		{
			if constexpr (is_sampler_ish<T>)
			{
				return this->holds<sampler>();
			}
			else
			{
				return std::holds_alternative<T>(this->var());
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> ML_NODISCARD auto get() const noexcept
		{
			if constexpr (is_sampler_ish<T>)
			{
				return this->get<sampler>();
			}
			else if (auto const v{ this->var() }; std::holds_alternative<T>(v))
			{
				return std::make_optional<T>(std::get<T>(v));
			}
			else
			{
				return (std::optional<T>)std::nullopt;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T, class ... Args
		> bool set(Args && ... args) noexcept
		{
			if constexpr (is_sampler_ish<T>)
			{
				return this->set<sampler>(ML_forward(args)...);
			}
			else if (this->holds<T>())
			{
				std::get<data_type>(m_storage) = data_type{ ML_forward(args)... };
				return true;
			}
			else
			{
				return false;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto compare(uniform const & value) const noexcept
		{
			if (type() != value.type())
			{
				return util::compare((hash_t)type(), (hash_t)value.type());
			}
			else
			{
				return name().compare(value.name());
			}
		}

		ML_NODISCARD bool operator==(uniform const & value) const noexcept { return compare(value) == 0; }

		ML_NODISCARD bool operator!=(uniform const & value) const noexcept { return compare(value) != 0; }

		ML_NODISCARD bool operator<(uniform const & value) const noexcept { return compare(value) < 0; }

		ML_NODISCARD bool operator>(uniform const & value) const noexcept { return compare(value) > 0; }

		ML_NODISCARD bool operator<=(uniform const & value) const noexcept { return compare(value) <= 0; }

		ML_NODISCARD bool operator>=(uniform const & value) const noexcept { return compare(value) >= 0; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		storage_type m_storage;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Type, class Name, class ... Args
	> ML_NODISCARD auto make_uniform(Name && name, Args && ... args) noexcept
	{
		return uniform{ typeof_v<Type>, ML_forward(name), uniform::data_type{ ML_forward(args)... } };
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

namespace ml
{
	// uniform buffer
	struct uniform_buffer final : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type			= typename pmr::polymorphic_allocator<byte_t>;
		using storage_type				= typename _ML ds::set<uniform>;
		using iterator					= typename storage_type::iterator;
		using const_iterator			= typename storage_type::const_iterator;
		using reverse_iterator			= typename storage_type::reverse_iterator;
		using const_reverse_iterator	= typename storage_type::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		uniform_buffer(allocator_type alloc = {})
			: m_storage{ alloc }
		{
		}

		explicit uniform_buffer(storage_type const & data, allocator_type alloc = {})
			: m_storage{ data, alloc }
		{
		}

		explicit uniform_buffer(storage_type && data, allocator_type alloc = {}) noexcept
			: m_storage{ std::move(data), alloc }
		{
		}

		uniform_buffer(std::initializer_list<uniform> init, allocator_type alloc = {})
			: m_storage{ init, alloc }
		{
		}

		template <class It
		> uniform_buffer(It first, It last, allocator_type alloc = {})
			: m_storage{ first, last, alloc }
		{
		}

		uniform_buffer(uniform_buffer const & value, allocator_type alloc = {})
			: m_storage{ value.m_storage, alloc }
		{
		}

		uniform_buffer(uniform_buffer && value, allocator_type alloc = {}) noexcept
			: m_storage{ alloc }
		{
			swap(std::move(value));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		uniform_buffer & operator=(uniform_buffer const & value)
		{
			uniform_buffer temp{ value };
			swap(temp);
			return (*this);
		}

		uniform_buffer & operator=(uniform_buffer && value) noexcept
		{
			swap(std::move(value));
			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void clear() noexcept
		{
			m_storage.clear();
		}

		void swap(uniform_buffer & value) noexcept
		{
			if (this != std::addressof(value))
			{
				std::swap(m_storage, value.m_storage);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD iterator find(ds::string const & name)
		{
			if (name.empty()) return end();

			return std::find_if(begin(), end(), [&name](auto const & u)
			{
				return u.name() == name;
			});
		}

		ML_NODISCARD const_iterator find(ds::string const & name) const
		{
			if (name.empty()) return cend();
			
			return std::find_if(cbegin(), cend(), [&name](auto const & u)
			{
				return u.name() == name;
			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> ML_NODISCARD iterator find(ds::string const & name)
		{
			if (auto const it{ find(name) }; it != end() && it->holds<T>())
			{
				return it;
			}
			else
			{
				return end();
			}
		}

		template <class T
		> ML_NODISCARD const_iterator find(ds::string const & name) const
		{
			if (auto const it{ find(name) }; it != cend() && it->holds<T>())
			{
				return it;
			}
			else
			{
				return cend();
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD uniform * get(ds::string const & name)
		{
			if (auto const it{ find(name) }; it != end())
			{
				return &(*it);
			}
			else
			{
				return nullptr;
			}
		}

		ML_NODISCARD uniform const * get(ds::string const & name) const
		{
			if (auto const it{ find(name) }; it != end())
			{
				return &(*it);
			}
			else
			{
				return nullptr;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> ML_NODISCARD uniform * get(ds::string const & name)
		{
			if (auto const it{ find<T>(name) }; it != end())
			{
				return &(*it);
			}
			else
			{
				return nullptr;
			}
		}

		template <class T
		> ML_NODISCARD uniform const * get(ds::string const & name) const
		{
			if (auto const it{ find<T>(name) }; it != cend())
			{
				return &(*it);
			}
			else
			{
				return nullptr;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T, class Data
		> uniform_buffer & set(ds::string const & name, Data const & data)
		{
			if (uniform * u{ get(name) })
			{
				u->set<T>(data);
			}
			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool insert(uniform const & value)
		{
			return !value.name().empty() && m_storage.insert(value).second;
		}

		bool insert(uniform && value)
		{
			return !value.name().empty() && m_storage.insert(std::move(value)).second;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		iterator erase(iterator it)
		{
			return m_storage.erase(it);
		}

		iterator erase(iterator first, iterator last)
		{
			return m_storage.erase(first, last);
		}

		iterator erase(ds::string const & name)
		{
			return m_storage.erase(this->find(name));
		}

		template <class T
		> iterator erase(ds::string const & name)
		{
			return m_storage.erase(this->find<T>(name));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		uniform_buffer & concat(uniform_buffer const & value)
		{
			for (auto const & u : value)
			{
				this->insert(u);
			}
			return (*this);
		}

		uniform_buffer & concat(uniform_buffer && value)
		{
			for (auto && u : value)
			{
				this->insert(std::move(u));
			}
			return (*this);
		}

		uniform_buffer concat(uniform_buffer const & value) const
		{
			uniform_buffer temp{ *this };
			return temp.concat(value);
		}

		uniform_buffer concat(uniform_buffer && value) const
		{
			uniform_buffer temp{ *this };
			return temp.concat(std::move(value));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		uniform_buffer & operator+=(uniform_buffer const & value)
		{
			return this->concat(value);
		}

		uniform_buffer & operator+=(uniform_buffer && value)
		{
			return this->concat(std::move(value));
		}

		uniform_buffer operator+(uniform_buffer const & value) const
		{
			return this->concat(value);
		}

		uniform_buffer operator+(uniform_buffer && value) const
		{
			return this->concat(std::move(value));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto operator[](size_t const i) noexcept -> uniform & { return m_storage[i]; }

		ML_NODISCARD auto operator[](size_t const i) const noexcept -> uniform const & { return m_storage[i]; }

		ML_NODISCARD auto at(size_t const i) -> uniform & { return m_storage[i]; }

		ML_NODISCARD auto at(size_t const i) const -> uniform const & { return m_storage[i]; }

		ML_NODISCARD auto begin() noexcept -> iterator { return m_storage.begin(); }
		
		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_storage.begin(); }

		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return m_storage.cbegin(); }
		
		ML_NODISCARD auto cend() const noexcept -> const_iterator { return m_storage.cend(); }
		
		ML_NODISCARD auto crbegin() const noexcept -> const_reverse_iterator { return m_storage.crbegin(); }
		
		ML_NODISCARD auto crend() const noexcept -> const_reverse_iterator { return m_storage.crend(); }
		
		ML_NODISCARD auto end() noexcept -> iterator { return m_storage.end(); }
		
		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_storage.end(); }

		ML_NODISCARD auto rbegin() noexcept -> reverse_iterator { return m_storage.rbegin(); }
		
		ML_NODISCARD auto rbegin() const noexcept -> const_reverse_iterator { return m_storage.rbegin(); }

		ML_NODISCARD auto rend() noexcept -> reverse_iterator { return m_storage.rend(); }
		
		ML_NODISCARD auto rend() const noexcept -> const_reverse_iterator { return m_storage.rend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		storage_type m_storage;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_UNIFORM_HPP_