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

		bool use() noexcept { return !m_used && (m_used = true); }

		bool used() const noexcept { return m_used; }

	protected:
		constexpr explicit event(hash_t id) noexcept : m_id{ id }, m_used{}
		{
		}

		hash_t const m_id; // event id
		bool m_used; // used
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// EVENT HELPER
	namespace impl
	{
		template <class Derived> struct event_helper : event
		{
			enum : hash_t { ID = hashof_v<Derived> };

			constexpr event_helper() noexcept : event{ ID }
			{
			}
		};
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// EVENT LISTENER
	struct ML_CORE_API event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit event_listener(event_bus * bus) noexcept : m_bus{ bus }, m_listening{ true }
		{
			ML_assert_msg(bus, "INVALID EVENT BUS");
		}

		virtual ~event_listener() noexcept; // EOF

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void on_event(event &&) = 0; // handle event

		ML_NODISCARD event_bus * get_bus() const noexcept { return m_bus; }

		ML_NODISCARD bool listening_enabled() const noexcept { return m_listening; }

		bool enable_listening(bool value) noexcept { return m_listening = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		template <class Ev
		> bool subscribe() noexcept
		{
			return ML_check(m_bus)->add_listener<Ev>(this);
		}

		template <class ... Args
		> bool subscribe(Args && ... args) noexcept
		{
			return ML_check(m_bus)->add_listener(ML_forward(args)..., this);
		}

		template <class Ev
		> void unsubscribe() noexcept
		{
			return ML_check(m_bus)->remove_listener<Ev>(this);
		}

		template <class ... Args
		> void unsubscribe(Args && ... args) noexcept
		{
			return ML_check(m_bus)->remove_listener(ML_forward(args)..., this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		event_bus * const m_bus; // event bus

	private:
		bool m_listening; // listening enabled

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

		event_bus(allocator_type alloc = {}) noexcept : m_categories{ alloc }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void fire(event && ev) noexcept
		{
			if (ev.used()) { return; }
			else if (auto const cat{ m_categories.find(ev) })
			{
				for (auto const listener : (*cat->second))
				{
					if (ev.used()) { return; }
					else if (listener->listening_enabled())
					{
						listener->on_event(ML_forward(ev));
					}
				}
			}
		}

		template <class Ev, class ... Args
		> void fire(Args && ... args) noexcept
		{
			static_assert(std::is_base_of_v<event, Ev>, "invalid event type");

			this->fire(Ev{ ML_forward(args)... });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		bool add_listener(hash_t event_id, event_listener * value) noexcept
		{
			return value
				&& this == value->get_bus()
				&& m_categories[event_id].insert(value).second;
		}

		template <class Ev
		> bool add_listener(event_listener * value) noexcept
		{
			static_assert(std::is_base_of_v<event, Ev>, "invalid event type");

			return this->add_listener(Ev::ID, value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void remove_listener(hash_t event_id, event_listener * value) noexcept
		{
			if (!value || (this != value->get_bus())) { return; }
			else if (auto const cat{ m_categories.find(event_id) })
			{
				if (auto const listener{ cat->second->find(value) }; listener != cat->second->end())
				{
					cat->second->erase(listener);
				}
			}
		}

		template <class Ev
		> void remove_listener(event_listener * value) noexcept
		{
			static_assert(std::is_base_of_v<event, Ev>, "invalid event type");

			this->remove_listener(Ev::ID, value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void remove_listener(event_listener * value) noexcept
		{
			if (!value) { return; }

			m_categories.for_each([&](hash_t, category_type & cat) noexcept
			{
				if (auto const listener{ cat.find(value) }; listener != cat.end())
				{
					cat.erase(listener);
				}
			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		categories_type m_categories{}; // categories

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// ~event_listener
	inline event_listener::~event_listener() noexcept
	{
		unsubscribe(); // remove listener from all events
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_EVENTS_HPP_