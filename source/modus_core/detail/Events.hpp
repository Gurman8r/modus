#ifndef _ML_EVENTS_HPP_
#define _ML_EVENTS_HPP_

#include <modus_core/detail/Map.hpp>
#include <modus_core/detail/Memory.hpp>
#include <modus_core/detail/Method.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// EVENT SYSTEM
	struct event;
	struct event_listener;
	struct dummy_listener;
	struct event_bus;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	// EVENT BASE
	struct ML_NODISCARD event
	{
		bool consume() noexcept { return !m_used && (m_used = true); }

		ML_NODISCARD bool used() const noexcept { return m_used; }

		ML_NODISCARD constexpr hash_t get_id() const noexcept { return m_id; }

		ML_NODISCARD constexpr operator hash_t() const noexcept { return m_id; }

		ML_NODISCARD constexpr bool operator==(hash_t value) const noexcept { return m_id == value; }

		ML_NODISCARD constexpr bool operator!=(hash_t value) const noexcept { return m_id != value; }

	protected:
		constexpr explicit event(hash_t id) noexcept : m_id{ id }, m_used{} {}

		constexpr event(event const &) = default;
		constexpr event(event &&) noexcept = default;
		constexpr event & operator=(event const &) = default;
		constexpr event & operator=(event &&) noexcept = default;

	private:
		hash_t m_id; // ID
		bool m_used; // used
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// EVENT HELPER
	namespace impl
	{
		template <class Derived
		> struct event_helper : event
		{
			using helper_base = event_helper<Derived>;

			enum : hash_t { ID = hashof_v<Derived> };

			constexpr event_helper() noexcept : event{ ID } {}

			constexpr event_helper(helper_base const &) = default;
			constexpr event_helper(helper_base &&) noexcept = default;
			constexpr helper_base & operator=(helper_base const &) = default;
			constexpr helper_base & operator=(helper_base &&) noexcept = default;
		};
	}

	// event declarator helper
#define ML_event(Type) \
	struct Type : _ML impl::event_helper<Type>

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// EVENT LISTENER
	struct ML_CORE_API event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit event_listener(event_bus * bus) noexcept : m_bus{ ML_check(bus) }
		{
		}

		virtual ~event_listener() noexcept { this->unsubscribe(); }

		ML_NODISCARD auto get_bus() const noexcept -> event_bus * { return m_bus; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		friend event_bus;

		event_listener(event_listener const &) = default;
		event_listener(event_listener &&) noexcept = default;
		event_listener & operator=(event_listener const &) = default;
		event_listener & operator=(event_listener &&) noexcept = default;

		// on event
		virtual void on_event(event const &) = 0;

		// subscribe
		template <class ... Evs> void subscribe() noexcept
		{
			static_assert(0 < sizeof...(Evs), "must provide at least one event type");

			ML_assert(m_bus);

			meta::for_types<Evs...>([&](auto tag) noexcept
			{
				m_bus->add_listener<decltype(tag)::type>(this);
			});
		}

		// unsubscribe
		template <class ... Evs> void unsubscribe() noexcept
		{
			ML_assert(m_bus);

			if constexpr (0 == sizeof...(Evs))
			{
				m_bus->remove_listener(this); // remove from all events
			}
			else
			{
				meta::for_types<Evs...>([&](auto tag) noexcept
				{
					m_bus->remove_listener<decltype(tag)::type>(this);
				});
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		event_bus * m_bus; // event bus

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// DUMMY LISTENER
	struct ML_CORE_API dummy_listener final : trackable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using event_callback = typename ds::method< void(event const &) >;

		explicit dummy_listener(event_bus * bus) noexcept : event_listener{ bus }
		{
		}

		template <class Fn, class ... Args
		> dummy_listener(event_bus * bus, Fn && fn, Args && ... args) noexcept : dummy_listener{ bus }
		{
			this->set_event_callback(ML_forward(fn), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using event_listener::subscribe;

		using event_listener::unsubscribe;

		void on_event(event const & value) noexcept final
		{
			if (m_on_event) { m_on_event(value); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_event_callback() const noexcept -> event_callback const &
		{
			return m_on_event;
		}

		template <class Fn, class ... Args
		> auto set_event_callback(Fn && fn, Args && ... args) noexcept -> event_callback
		{
			return util::chain(m_on_event, ML_forward(fn), std::placeholders::_1, ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		event_callback m_on_event;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// EVENT BUS
	struct event_bus : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type	= typename pmr::polymorphic_allocator<byte>;
		using category_type		= typename ds::set<event_listener *>;
		using categories_type	= typename ds::map<hash_t, category_type>;
		using event_queue		= typename ds::list<ds::scope<event>>;
		using dummy_ref			= typename ds::ref<dummy_listener>;
		using dummy_list		= typename ds::list<dummy_ref>;

		template <class Ev> static constexpr bool is_valid_event
		{
			std::is_base_of_v<event, Ev> && !std::is_same_v<event, Ev>
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		event_bus(allocator_type alloc = {}) noexcept
			: m_cats	{ alloc }
			, m_queue	{ alloc }
			, m_dummies	{ alloc }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Ev = event
		> void fire(Ev && value) noexcept
		{
			if (auto const cat{ m_cats.find(value) })
			{
				for (auto const listener : (*cat->second))
				{
					ML_check(listener)->on_event(ML_forward(value));
				}
			}
		}

		template <class Ev, class ... Args
		> void fire(Args && ... args) noexcept
		{
			static_assert(is_valid_event<Ev>, "invalid event type");

			this->fire(Ev{ ML_forward(args)... });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_queue() const noexcept -> event_queue const &
		{
			return m_queue;
		}

		template <class Ev, class ... Args
		> void post(Args && ... args) noexcept
		{
			static_assert(is_valid_event<Ev>, "invalid event type");

			m_queue.emplace_back(new Ev{ ML_forward(args)... });
		}

		void process_events() noexcept
		{
			for (auto const & ev : m_queue)
			{
				this->fire(*ML_check(ev.get()));
			}
			m_queue.clear();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_dummies() const noexcept -> dummy_list const &
		{
			return m_dummies;
		}

		template <class ... Evs, class ... Args
		> auto new_dummy(Args && ... args) noexcept -> dummy_ref &
		{
			auto temp{ _ML make_ref<dummy_listener>(this, ML_forward(args)...) };

			if constexpr (0 < sizeof...(Evs)) { temp->subscribe<Evs...>(); }

			return m_dummies.emplace_back(std::move(temp));
		}

		auto delete_dummy(dummy_ref const & value) noexcept
		{
			if (auto it{ std::find(m_dummies.begin(), m_dummies.end(), value) }
			; it == m_dummies.end()) { return it; }
			else { return m_dummies.erase(it); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Ev
		> bool add_listener(event_listener * value) noexcept
		{
			static_assert(is_valid_event<Ev>, "invalid event type");

			return value
				&& this == value->get_bus()
				&& m_cats[Ev::ID].insert(value).second;
		}

		template <class Ev
		> void remove_listener(event_listener * value) noexcept
		{
			static_assert(is_valid_event<Ev>, "invalid event type");

			if (!value || (this != value->get_bus())) { return; }
			else if (auto const cat{ m_cats.find(Ev::ID) })
			{
				if (auto const listener{ cat->second->find(value) }
				; listener != cat->second->end())
				{
					cat->second->erase(listener);
				}
			}
		}

		void remove_listener(event_listener * value) noexcept
		{
			if (!value || (this != value->get_bus())) { return; }

			m_cats.for_each([&](hash_t, category_type & cat) noexcept
			{
				if (auto const listener{ cat.find(value) }; listener != cat.end())
				{
					cat.erase(listener);
				}
			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		categories_type	m_cats		; // listener storage
		event_queue		m_queue		; // event queue
		dummy_list		m_dummies	; // dummy listeners

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_EVENTS_HPP_