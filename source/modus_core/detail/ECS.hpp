#ifndef _ML_ECS_HPP_
#define _ML_ECS_HPP_

// Data Oriented ECS - Vittorio Romeo
// https://github.com/SuperV1234/cppcon2015
// https://www.youtube.com/watch?v=NTWSeQtHZ9M

#include <modus_core/detail/BatchVector.hpp>
#include <modus_core/detail/BitSet.hpp>
#include <modus_core/detail/Debug.hpp>

// system declarator helper
#define ML_decl_ecs_sx(S, X, ...)									\
	using S = typename _ML meta::list<##__VA_ARGS__>;				\
	template <class> struct X final : _ML ecs::detail::x_base<S>	\

// UTILITY
namespace ml::ecs::detail
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// base system helper
	template <class Signature
	> struct x_base
	{
		using signature_type = typename Signature;
	};

	// for storing "template template" systems in type lists
	template <template <class> class System
	> struct x_wrapper final
	{
		template <class Traits
		> using type = typename System<Traits>;
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// (T) TAGS
namespace ml::ecs::detail
{
	// tags
	template <class ... Tags
	> struct ML_NODISCARD tags final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type = typename tags<Tags...>;

		using type_list = typename meta::list<Tags...>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr size_t count() noexcept
		{
			return meta::size<type_list>();
		}

		template <class T
		> static constexpr bool contains() noexcept
		{
			return meta::contains<T, type_list>();
		}

		template <class T
		> static constexpr size_t index() noexcept
		{
			static_assert(self_type::contains<T>(), "tag not found");
			return meta::index_of<T, type_list>();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> using impl_filter = std::bool_constant<self_type::contains<T>()>;

		template <class T
		> using filter = typename meta::filter<impl_filter, T>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// (C) COMPONENTS
namespace ml::ecs::detail
{
	// components
	template <class ... Components
	> struct ML_NODISCARD components final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type = typename components<Components...>;

		using type_list = typename meta::list<Components...>;

		using storage_type = typename batch_vector<Components...>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr size_t count() noexcept
		{
			return meta::size<type_list>();
		}

		template <class T
		> static constexpr bool contains() noexcept
		{
			return meta::contains<T, type_list>();
		}

		template <class T
		> static constexpr size_t index() noexcept
		{
			static_assert(self_type::contains<T>(), "component not found");
			return meta::index_of<T, type_list>();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> using impl_filter = std::bool_constant<self_type::contains<T>()>;

		template <class T
		> using filter = typename meta::filter<impl_filter, T>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// (S) SIGNATURES
namespace ml::ecs::detail
{
	// signatures
	template <class ... Signatures
	> struct ML_NODISCARD signatures final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type = typename signatures<Signatures...>;

		using type_list = typename meta::list<Signatures...>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr size_t count() noexcept
		{
			return meta::size<type_list>();
		}

		template <class T
		> static constexpr bool contains() noexcept
		{
			return meta::contains<T, type_list>();
		}

		template <class T
		> static constexpr size_t index() noexcept
		{
			static_assert(self_type::contains<T>(), "signature not found");
			return meta::index_of<T, type_list>();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// (X) SYSTEMS
namespace ml::ecs::detail
{
	// systems
	template <template <class> class ... Systems
	> struct ML_NODISCARD systems final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type = typename systems<Systems...>;

		using type_list = typename meta::list<x_wrapper<Systems>...>;

		template <class Traits
		> using storage_type = typename meta::tuple<meta::list<Systems<Traits>...>>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr size_t count() noexcept
		{
			return meta::size<type_list>();
		}

		template <template <class> class X
		> static constexpr bool contains() noexcept
		{
			return meta::contains<x_wrapper<X>, type_list>();
		}

		template <template <class> class X
		> static constexpr size_t index() noexcept
		{
			static_assert(self_type::contains<X>(), "system not found");
			return meta::index_of<x_wrapper<X>, type_list>();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// (O) OPTIONS
namespace ml::ecs::detail
{
	// options
	template <
		size_t	GrowBase = 5,
		class	GrowMult = std::ratio<2, 1>
	> struct ML_NODISCARD options final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// base growth amount
		static constexpr size_t grow_base{ GrowBase };
		static_assert(0 < grow_base, "growth base negative or zero");

		// growth multiplier
		static constexpr float32 grow_mult{ util::ratio_cast<GrowMult>() };
		static_assert(1.f <= grow_mult, "expression would result in negative growth");

		// growth calculator
		static constexpr size_t calc_growth(size_t const cap) noexcept
		{
			return (size_t)((float32)(cap + grow_base) * grow_mult);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// (U) TRAITS
namespace ml::ecs::detail
{
	template <
		class T = tags			<>,	// tags
		class C = components	<>,	// components
		class S = signatures	<>,	// signatures
		class X = systems		<>,	// systems
		class O = options		<>	// options
	> struct ML_NODISCARD traits final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		using self_type			= typename traits<T, C, S, X, O>;
		using tags_type			= typename T;
		using components_type	= typename C;
		using signatures_type	= typename S;
		using systems_type		= typename X;
		using options_type		= typename O;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr size_t tag_count		{ tags_type::count() };
		static constexpr size_t component_count	{ components_type::count() };
		static constexpr size_t signature_count	{ signatures_type::count() };
		static constexpr size_t system_count	{ systems_type::count() };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using component_list	= typename components_type::type_list;
		using tag_list			= typename tags_type::type_list;
		using signature_list	= typename signatures_type::type_list;
		using system_list		= typename systems_type::type_list;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using component_storage = typename components_type::storage_type;
		using system_storage	= typename systems_type::template storage_type<self_type>;
		using signature_type	= typename ds::bitset<component_count + tag_count>;
		using signature_storage	= typename meta::array<signature_type, signature_count>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class C
		> static constexpr bool has_component() noexcept
		{
			return components_type::template contains<C>();
		}

		template <class C
		> static constexpr size_t component_id() noexcept
		{
			return components_type::template index<C>();
		}

		template <class C
		> static constexpr size_t component_bit() noexcept
		{
			return self_type::component_id<C>();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> static constexpr bool has_tag() noexcept
		{
			return tags_type::template contains<T>();
		}

		template <class T
		> static constexpr size_t tag_id() noexcept
		{
			return tags_type::template index<T>();
		}

		template <class T
		> static constexpr size_t tag_bit() noexcept
		{
			return component_count + self_type::tag_id<T>();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class S
		> static constexpr bool has_signature() noexcept
		{
			return signatures_type::template contains<S>();
		}

		template <class S
		> static constexpr size_t signature_id() noexcept
		{
			return signatures_type::template index<S>();
		}

		template <class S
		> static constexpr signature_type const & signature_bitset() noexcept
		{
			return std::get<self_type::signature_id<S>()>(m_signature_bitsets);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <template <class> class X
		> static constexpr bool has_system() noexcept
		{
			return systems_type::template contains<X>();
		}

		template <template <class> class X
		> static constexpr size_t system_id() noexcept
		{
			return systems_type::template index<X>();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		static constexpr auto m_signature_bitsets{ ([&]() noexcept -> signature_storage
		{
			// generate bitsets for each signature_type
			signature_storage temp{};
			meta::for_type_list<typename signatures_type::type_list
			>([&temp](auto s)
			{
				// get the signature_type's bitset
				auto & b{ std::get<self_type::signature_id<decltype(s)::type>()>(temp) };

				// enable component bits
				meta::for_type_list<components_type::template filter<decltype(s)::type>
				>([&b](auto c)
				{
					b.set(self_type::component_bit<decltype(c)::type>());
				});

				// enable tag bits
				meta::for_type_list<tags_type::template filter<decltype(s)::type>
				>([&b](auto t)
				{
					b.set(self_type::tag_bit<decltype(t)::type>());
				});
			});
			return temp;
		})() };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// (M) MANAGER
namespace ml::ecs
{
	template <class U = detail::traits<>
	> struct manager final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type	= typename pmr::polymorphic_allocator<byte>;
		using traits			= typename U;
		using self_type			= typename manager<traits>;
		using tags				= typename traits::tags_type;
		using tag_list			= typename traits::tag_list;
		using components		= typename traits::components_type;
		using component_list	= typename traits::component_list;
		using component_storage	= typename traits::component_storage;
		using signatures		= typename traits::signatures_type;
		using signature			= typename traits::signature_type;
		using signature_list	= typename traits::signature_list;
		using systems			= typename traits::systems_type;
		using system_list		= typename traits::system_list;
		using system_storage	= typename traits::system_storage;
		using options			= typename traits::options_type;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		struct handle;

		using handle_storage = typename list<handle>;

		enum : size_t { id_alive, id_index, id_handle, id_bitset };

		using entity_storage = typename batch_vector
		<
			bool,		// state of entity ( alive / dead )
			size_t,		// component index
			size_t,		// handle index
			signature	// component signature
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// object oriented abstraction over entity/manager interface
		struct handle final
		{
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

			ML_NODISCARD operator bool() const noexcept
			{
				ML_assert(m_manager);
				return m_manager->is_valid_handle(*this);
			}

			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

			ML_NODISCARD bool is_alive() const noexcept
			{
				ML_assert(m_manager);
				return m_manager->is_alive(*this);
			}

			handle & kill() noexcept
			{
				ML_assert(m_manager);
				m_manager->kill(*this);
				return (*this);
			}

			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

			template <class T
			> handle & add_tag() noexcept
			{
				ML_assert(m_manager);
				m_manager->template add_tag<T>(*this);
				return (*this);
			}

			template <class T
			> handle & del_tag() noexcept
			{
				ML_assert(m_manager);
				m_manager->template del_tag<T>(*this);
				return (*this);
			}

			template <class T
			> bool has_tag() noexcept
			{
				ML_assert(m_manager);
				return m_manager->template has_tag<T>(*this);
			}

			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

			template <class C, class ... Args
			> auto & add_component(Args && ... args) noexcept
			{
				ML_assert(m_manager);
				return m_manager->template add_component<C>(*this, ML_forward(args)...);
			}

			template <class C
			> auto & add_component() noexcept
			{
				ML_assert(m_manager);
				return m_manager->template add_component<C>(*this);
			}

			template <class C
			> handle & del_component() noexcept
			{
				ML_assert(m_manager);
				m_manager->template del_component<C>(*this);
				return (*this);
			}

			template <class C
			> ML_NODISCARD auto & get_component() noexcept
			{
				ML_assert(m_manager);
				return m_manager->template get_component<C>(*this);
			}

			template <class C
			> ML_NODISCARD auto const & get_component() const noexcept
			{
				ML_assert(m_manager);
				return m_manager->template get_component<C>(*this);
			}

			template <class C
			> ML_NODISCARD bool has_component() const noexcept
			{
				ML_assert(m_manager);
				return m_manager->template has_component<C>(*this);
			}

			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

			template <class S
			> ML_NODISCARD bool matches_signature() const noexcept
			{
				ML_assert(m_manager);
				return m_manager->template matches_signature<S>(*this);
			}

			template <template <class> class X
			> ML_NODISCARD bool matches_system() const noexcept
			{
				ML_assert(m_manager);
				return m_manager->template matches_system<X>();
			}

			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		private:
			friend self_type;

			self_type *	m_manager	; // owning manager
			size_t		m_entity	; // entity index
			size_t		m_self		; // handle index
			int32		m_counter	; // reference counter

			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		};
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		manager(allocator_type alloc = {}) noexcept
			: m_capacity	{}
			, m_size		{}
			, m_size_next	{}
			, m_entities	{ alloc }
			, m_components	{ alloc }
			, m_handles		{ alloc }
			, m_systems		{}
		{
		}

		manager(self_type const & value, allocator_type alloc = {})
			: self_type{ alloc }
		{
			this->assign(value);
		}
		
		manager(self_type && value, allocator_type alloc = {}) noexcept
			: self_type{ alloc }
		{
			this->swap(std::move(value));
		}

		manager(size_t const cap, allocator_type alloc = {})
			: self_type{ alloc }
		{
			this->grow_to(cap);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		self_type & operator=(self_type const & value)
		{
			this->assign(value);
			return (*this);
		}

		self_type & operator=(self_type && value) noexcept
		{
			this->swap(std::move(value));
			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void assign(self_type const & value)
		{
			if (this != std::addressof(value))
			{
				m_capacity	= value.m_capacity;
				m_size		= value.m_size;
				m_size_next = value.m_size_next;
				m_components= value.m_components;
				m_entities	= value.m_entities;
				m_handles	= value.m_handles;
				m_systems	= value.m_systems;
			}
		}

		void swap(self_type & value) noexcept
		{
			if (this != std::addressof(value))
			{
				std::swap(m_capacity,	value.m_capacity);
				std::swap(m_size,		value.m_size);
				std::swap(m_size_next,	value.m_size_next);

				m_components.swap(value.m_components);
				m_entities	.swap(value.m_entities);
				m_handles	.swap(value.m_handles);
				m_systems	.swap(value.m_systems);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto get_allocator() const noexcept -> allocator_type { return allocator_type{}; }

		auto get_capacity() const noexcept -> size_t { return m_capacity; }

		auto get_components() const noexcept -> component_storage const & { return m_components; }

		auto get_entities() const noexcept -> entity_storage const & { return m_entities; }

		auto get_handles() const noexcept -> handle_storage const & { return m_handles; }

		auto get_systems() const noexcept -> system_storage const & { return m_systems; }

		auto get_size() const noexcept -> size_t { return m_size; }

		auto get_size_next() const noexcept -> size_t { return m_size_next; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void apply_changes() noexcept
		{
			if (m_size_next == 0)
			{
				m_size = 0;
			}
			else m_size = m_size_next = std::invoke([&]() noexcept
			{
				// arrange all alive entities towards the left
				size_t dead{}, alive{ m_size_next - 1 };
				while (true)
				{
					// find dead entity from the left
					for (; true; ++dead)
					{
						if (dead > alive) { return dead; }
						if (!m_entities.at<id_alive>(dead)) { break; }
					}

					// find alive entity from the right
					for (; true; --alive)
					{
						if (m_entities.at<id_alive>(alive)) { break; }
						if (alive <= dead) { return dead; }
					}

					// found two entities that need to be swapped
					ML_assert(m_entities.at<id_alive>(alive));
					ML_assert(!m_entities.at<id_alive>(dead));
					
					// swap the entities
					m_entities.swap(alive, dead);

					// refresh alive entity
					auto & a{ m_handles[alive] };
					a.m_entity = alive;

					// invalidate and refresh dead entity
					auto & d{ m_handles[dead] };
					++d.m_counter;
					d.m_entity = dead;

					// move both iterator indices
					++dead; --alive;
				}
				return dead;
			});
		}

		void clear() noexcept
		{
			for (size_t i = 0; i < m_capacity; ++i)
			{
				m_entities.expand_all(i, [i](auto & a, auto & e, auto & h, auto & b)
				{
					a = false;	// alive
					e = i;		// index
					h = i;		// handle
					b = {};		// bitset
				});
				
				auto & h{ m_handles[i] };
				h.m_entity = i;
				h.m_counter = 0;
			}
			m_size = m_size_next = 0;
		}

		void grow_to(size_t const cap)
		{
			if (cap <= m_capacity) { return; }

			m_components.resize(cap);
			m_handles.resize(cap);
			m_entities.reserve(cap);

			for (size_t i = m_capacity; i < cap; ++i)
			{
				m_entities.push_back(false, i, i, signature{});

				auto & h{ m_handles[i] };
				h.m_manager = this;
				h.m_entity = i;
				h.m_counter = 0;
			}
			m_capacity = cap;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD size_t new_entity()
		{
			// grow if needed
			if (m_capacity <= m_size_next)
			{
				this->grow_to(options::calc_growth(m_capacity));
			}

			size_t const i{ m_size_next++ };
			m_entities.at<id_alive>(i) = true;
			m_entities.at<id_bitset>(i) = {};
			return i;
		}

		ML_NODISCARD bool is_alive(size_t const i) const
		{
			return m_entities.at<id_alive>(i);
		}

		ML_NODISCARD bool is_alive(handle const & h) const
		{
			return this->is_alive(h.m_entity);
		}

		self_type & kill(size_t const i)
		{
			m_entities.at<id_alive>(i) = false;
			return (*this);
		}

		self_type & kill(handle const & h)
		{
			return this->kill(h.m_entity);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD handle create_handle()
		{
			size_t const i{ this->new_entity() };
			size_t const e{ m_entities.at<id_handle>(i) };
			auto & h{ m_handles[e] };

			handle temp{};
			temp.m_manager	= h.m_manager;
			temp.m_entity	= h.m_entity = i;
			temp.m_self		= e;
			temp.m_counter	= h.m_counter;
			ML_assert(temp);
			return temp;
		}

		ML_NODISCARD bool is_valid_handle(handle const & h) const noexcept
		{
			return (h.m_counter == this->get_handle(h).m_counter);
		}

		ML_NODISCARD handle & get_handle(size_t const i)
		{
			return m_handles[i];
		}

		ML_NODISCARD handle const & get_handle(size_t const i) const
		{
			return m_handles[i];
		}

		ML_NODISCARD handle & get_handle(handle const & h)
		{
			return this->get_handle(h.m_self);
		}

		ML_NODISCARD handle const & get_handle(handle const & h) const
		{
			return this->get_handle(h.m_self);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> self_type & add_tag(size_t const i) noexcept
		{
			m_entities.at<id_bitset>(i).set(traits::template tag_bit<T>());
			return (*this);
		}

		template <class T
		> self_type & add_tag(handle const & h) noexcept
		{
			return this->add_tag<T>(h.m_entity);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> self_type & del_tag(size_t const i) noexcept
		{
			m_entities.at<id_bitset>(i).clear(traits::template tag_bit<T>());
			return (*this);
		}

		template <class T
		> self_type & del_tag(handle const & h) noexcept
		{
			return this->del_tag<T>(h.m_entity);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> ML_NODISCARD bool has_tag(size_t const i) const noexcept
		{
			return m_entities.at<id_bitset>(i).read(traits::template tag_bit<T>());
		}

		template <class T
		> ML_NODISCARD bool has_tag(handle const & h) const noexcept
		{
			return this->has_tag(h.m_entity);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class C, class ... Args
		> auto & add_component(size_t const i, Args && ... args) noexcept
		{
			m_entities.at<id_bitset>(i).set(traits::template component_bit<C>());

			auto & c{ m_components.at<C>(m_entities.at<id_index>(i)) };
			c = C{ ML_forward(args)... };
			return c;
		}

		template <class C
		> auto & add_component(size_t const i) noexcept
		{
			return this->add_component<C>(i, C{});
		}

		template <class C, class ... Args
		> auto & add_component(handle const & h, Args && ... args) noexcept
		{
			return this->add_component<C>(h.m_entity, ML_forward(args)...);
		}

		template <class C
		> auto & add_component(handle const & h) noexcept
		{
			return this->add_component<C>(h.m_entity);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class C
		> self_type & del_component(size_t const i) noexcept
		{
			m_entities.at<id_bitset>(i).clear(traits::template component_bit<C>());
			return (*this);
		}

		template <class C
		> self_type & del_component(handle const & h) noexcept
		{
			return this->del_component<C>(h.m_entity);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class C
		> ML_NODISCARD auto & get_component(size_t const i) noexcept
		{
			return m_components.at<C>(m_entities.at<id_index>(i));
		}

		template <class C
		> ML_NODISCARD auto const & get_component(size_t const i) const noexcept
		{
			return m_components.at<C>(m_entities.at<id_index>(i));
		}

		template <class C
		> ML_NODISCARD auto & get_component(handle const & h) noexcept
		{
			return this->get_component<C>(h.m_entity);
		}

		template <class C
		> ML_NODISCARD auto const & get_component(handle const & h) const noexcept
		{
			return this->get_component<C>(h.m_entity);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class C
		> ML_NODISCARD bool has_component(size_t const i) const noexcept
		{
			return this->get_signature(i).read(traits::template component_bit<C>());
		}

		template <class C
		> ML_NODISCARD bool has_component(handle const & h) const noexcept
		{
			return this->has_component<C>(h.m_entity);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD signature const & get_signature(size_t const i) const noexcept
		{
			return m_entities.at<id_bitset>(i);
		}

		ML_NODISCARD signature const & get_signature(handle const & h) const noexcept
		{
			return this->get_signature(h.m_entity);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool matches_signature(size_t const i, signature const & s) const noexcept
		{
			return (s & this->get_signature(i)) == s;
		}

		template <class S
		> ML_NODISCARD bool matches_signature(size_t const i) const noexcept
		{
			return this->matches_signature(i, traits::template signature_bitset<S>());
		}

		template <class S
		> ML_NODISCARD bool matches_signature(handle const & h) const noexcept
		{
			return this->matches_signature<S>(h.m_entity);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <template <class> class X
		> ML_NODISCARD bool matches_system(size_t const i) const noexcept
		{
			return this->matches_signature<typename X<traits>::signature_type>(i);
		}

		template <template <class> class X
		> ML_NODISCARD bool matches_system(handle const & h) const noexcept
		{
			return this->matches_system<X>(h.m_entity);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// invoke function on every alive entity
		template <class Fn
		> self_type & for_entities(Fn && fn) noexcept
		{
			for (size_t i = 0; i < m_size; ++i)
			{
				std::invoke(ML_forward(fn), i);
			}
			return (*this);
		}

		// invoke function on each of an entity's components
		template <class Fn
		> self_type & for_components(size_t const i, Fn && fn) noexcept
		{
			meta::for_type_list<typename components::type_list>([&](auto c) noexcept
			{
				using C = typename decltype(c)::type;
				if (this->has_component<C>(i))
				{
					std::invoke(ML_forward(fn), this->get_component<C>(i));
				}
			});
			return (*this);
		}

		// invoke function on all alive entities matching a signature
		template <class S, class Fn
		> self_type & for_matching(Fn && fn) noexcept
		{
			return this->for_entities([&](size_t const i) noexcept
			{
				if (this->matches_signature<S>(i))
				{
					this->expand_call<S>(i, ML_forward(fn));
				}
			});
		}

		// invoke function on all alive entities matching a system
		template <template <class> class X, class Fn
		> self_type & for_system(Fn && fn) noexcept
		{
			return this->for_matching<typename X<traits>::signature_type
			>([&, &x = std::get<traits::template system_id<X>()>(m_systems)
			](size_t, auto && ... req_comp) noexcept
			{
				std::invoke(ML_forward(fn), x, ML_forward(req_comp)...);
			});
		}

		// invoke system on all alive entities
		template <template <class> class X, class ... Extra
		> self_type & update(Extra && ... extra) noexcept
		{
			return this->for_system<X>([&](auto & x, auto && ... req_comp) noexcept
			{
				std::invoke(x, ML_forward(req_comp)..., ML_forward(extra)...);
			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		template <class ... Ts
		> struct expand_call_helper;

		template <class S, class Fn
		> void expand_call(size_t const i, Fn && fn) noexcept
		{
			using req_comp = components::template filter<S>;

			using helper = meta::rename<expand_call_helper, req_comp>;

			helper::call(i, *this, ML_forward(fn));
		}

		template <class ... Ts
		> struct expand_call_helper final
		{
			template <class Fn
			> static void call(size_t const i, self_type & self, Fn && fn) noexcept
			{
				self.m_components.expand<Ts...>(self.m_entities.at<id_index>(i), [&
				](auto && ... req_comp) noexcept
				{
					std::invoke(ML_forward(fn), i, ML_forward(req_comp)...);
				});
			}
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		size_t m_capacity	; // total allocated slots
		size_t m_size		; // size excluding newly created entities
		size_t m_size_next	; // size including newly created entities
		
		entity_storage		m_entities	; // entity data
		handle_storage		m_handles	; // handle data
		component_storage	m_components; // component data
		system_storage		m_systems	; // system data

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// TESTS
namespace ml::ecs::tests
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// tags
	struct T0 {};
	struct T1 {};
	struct T2 {};

	// components
	struct C0 {};
	struct C1 {};
	struct C2 {};
	struct C3 {};
	struct C4 {};

	// signatures
	using S0 = meta::list<>;				// 00000000
	using S1 = meta::list<C0, C1>;			// 11000000
	using S2 = meta::list<C0, C4, T0>;		// 10001100
	using S3 = meta::list<C1, T0, C3, T2>;	// 01010101

	// traits
	using U = detail::traits<
		detail::tags		<T0, T1, T2>,
		detail::components	<C0, C1, C2, C3, C4>,
		detail::signatures	<S0, S1, S2, S3>,
		detail::systems		<>,
		detail::options		<>
	>;

	// tests
	static_assert(U::tag_count				== 3);
	static_assert(U::component_count		== 5);
	static_assert(U::signature_count		== 4);
	static_assert(U::system_count			== 0);

	static_assert(U::component_id<C0>()		== 0);
	static_assert(U::component_id<C1>()		== 1);
	static_assert(U::component_id<C2>()		== 2);
	static_assert(U::component_id<C3>()		== 3);
	static_assert(U::component_id<C4>()		== 4);
	static_assert(U::tag_id<T0>()			== 0);
	static_assert(U::tag_id<T1>()			== 1);
	static_assert(U::tag_id<T2>()			== 2);
	static_assert(U::signature_id<S0>()		== 0);
	static_assert(U::signature_id<S1>()		== 1);
	static_assert(U::signature_id<S2>()		== 2);
	static_assert(U::signature_id<S3>()		== 3);

	static_assert(U::component_bit<C0>()	== 0);
	static_assert(U::component_bit<C1>()	== 1);
	static_assert(U::component_bit<C2>()	== 2);
	static_assert(U::component_bit<C3>()	== 3);
	static_assert(U::component_bit<C4>()	== 4);
	static_assert(U::tag_bit<T0>()			== 5);
	static_assert(U::tag_bit<T1>()			== 6);
	static_assert(U::tag_bit<T2>()			== 7);

	static_assert(U::signature_bitset<S0>() == 0b00000000);
	static_assert(U::signature_bitset<S1>() == 0b00000011);
	static_assert(U::signature_bitset<S2>() == 0b00110001);
	static_assert(U::signature_bitset<S3>() == 0b10101010);

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_ECS_HPP_