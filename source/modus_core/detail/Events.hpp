#ifndef _ML_EVENTS_HPP_
#define _ML_EVENTS_HPP_

#include <modus_core/detail/Debug.hpp>
#include <modus_core/detail/Map.hpp>
#include <modus_core/detail/NonCopyable.hpp>

// event declarator helper
#define ML_decl_event(Type) \
	struct Type final : _ML impl::event_helper<Type>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// TYPES
	struct event			; // event base
	struct event_listener	; // event listener
	struct event_bus		; // event bus

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	// EVENT BASE
	struct ML_NODISCARD event : non_copyable
	{
		ML_NODISCARD constexpr operator hash_t() const noexcept { return m_id; }

	protected:
		constexpr explicit event(hash_t id) noexcept : m_id{ id }
		{
		}

		hash_t const m_id; // event id
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// EVENT HELPER
	namespace impl
	{
		template <class Derived> struct event_helper : event
		{
			enum : hash_t { ID = hashof_v<Derived> };

			constexpr event_helper() noexcept : event{ ID } {}
		};
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// EVENT LISTENER
	struct ML_CORE_API event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		explicit event_listener(event_bus * bus) noexcept : m_bus{ bus }
		{
			ML_assert_msg(m_bus, "INVALID EVENT BUS");
		}

		explicit event_listener(event_listener const & other) noexcept : event_listener{ other.m_bus }
		{
		}

		virtual ~event_listener() noexcept { this->unsubscribe(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void on_event(event &&) = 0; // handle event

		ML_NODISCARD event_bus * get_bus() const noexcept
		{
			return m_bus;
		}

		template <class Ev> bool subscribe() noexcept
		{
			return ML_check(m_bus)->add_listener<Ev>(this);
		}

		template <class ... Ev> void unsubscribe() noexcept
		{
			if constexpr (0 == sizeof...(Ev))
			{
				ML_check(m_bus)->remove_listener(this);
			}
			else
			{
				ML_check(m_bus)->remove_listener<Ev...>(this);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		event_bus * const m_bus; // event bus

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// EVENT BUS
	struct event_bus : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type	= typename pmr::polymorphic_allocator<byte_t>;
		using category_type		= typename ds::set<event_listener *>;
		using categories_type	= typename ds::map<hash_t, category_type>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Ev> static constexpr bool is_valid_event
		{
			std::is_base_of_v<event, Ev> && !std::is_same_v<event, Ev>
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		event_bus(allocator_type alloc = {}) noexcept : m_categories{ alloc } {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Ev, class ... Args
		> void fire(Args && ... args) noexcept
		{
			static_assert(is_valid_event<Ev>, "invalid event type");

			if (auto const cat{ m_categories.find(Ev::ID) })
			{
				for (auto const it : (*cat->second))
				{
					it->on_event(Ev{ ML_forward(args)... });
				}
			}
		}

		template <class Ev
		> bool add_listener(event_listener * value) noexcept
		{
			static_assert(is_valid_event<Ev>, "invalid event type");

			return value
				&& this == value->get_bus()
				&& m_categories[Ev::ID].insert(value).second;
		}

		template <class Ev
		> void remove_listener(event_listener * value) noexcept
		{
			static_assert(is_valid_event<Ev>, "invalid event type");

			if (!value || (this != value->get_bus())) { return; }
			else if (auto const cat{ m_categories.find(Ev::ID) })
			{
				if (auto const it{ cat->second->find(value) }
				; it != cat->second->end())
				{
					cat->second->erase(it);
				}
			}
		}

		void remove_listener(event_listener * value) noexcept
		{
			if (!value) { return; }

			m_categories.for_each([&](hash_t, category_type & cat) noexcept
			{
				if (auto const it{ cat.find(value) }; it != cat.end())
				{
					cat.erase(it);
				}
			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		categories_type m_categories{}; // categories

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_EVENTS_HPP_