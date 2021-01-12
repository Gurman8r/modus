#ifndef _ML_EVENT_SYSTEM_HPP_
#define _ML_EVENT_SYSTEM_HPP_

#include <modus_core/detail/FlatMap.hpp>
#include <modus_core/system/Memory.hpp>
#include <modus_core/detail/Method.hpp>

// event helper
#define ML_event(Ev) struct Ev : _ML event_helper<Ev>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct event;

	template <class> struct event_helper;
	
	struct event_listener;
	
	struct dummy_listener;

	template <class> struct event_delegate;
	
	struct event_bus;

	ML_alias event_callback = method<void(event const &)>;

	template <class Ev> constexpr bool is_event_v{ std::is_base_of_v<event, Ev> && !std::is_same_v<event, Ev> };

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	// base event
	struct ML_NODISCARD event
	{
		ML_NODISCARD constexpr hash_t event_id() const noexcept { return m_id; }

		ML_NODISCARD constexpr operator hash_t() const noexcept { return m_id; }

		ML_NODISCARD constexpr bool operator==(hash_t value) const noexcept { return m_id == value; }

		ML_NODISCARD constexpr bool operator!=(hash_t value) const noexcept { return m_id != value; }

	protected:
		constexpr explicit event(hash_t id) noexcept : m_id{ id } {}

		constexpr event(event const &) = default;
		constexpr event(event &&) noexcept = default;
		constexpr event & operator=(event const &) = default;
		constexpr event & operator=(event &&) noexcept = default;

	private:
		hash_t m_id; // ID
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// event helper
	template <class Derived
	> struct event_helper : event
	{
		enum : hash_t { ID = hashof_v<Derived> };

		constexpr event_helper() noexcept : event{ ID } {}

		constexpr event_helper(event_helper const &) = default;
		constexpr event_helper(event_helper &&) noexcept = default;
		constexpr event_helper & operator=(event_helper const &) = default;
		constexpr event_helper & operator=(event_helper &&) noexcept = default;
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// event listener
	struct ML_CORE_API event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~event_listener() noexcept { this->unsubscribe(); }

		ML_NODISCARD auto get_bus() const noexcept -> event_bus * { return m_bus; }
		
		ML_NODISCARD auto get_bus_index() const noexcept -> int64 { return m_index; }

		ML_NODISCARD auto get_bus_order(event_listener const & other) const noexcept -> int32
		{
			return (this == std::addressof(other)) ? 0 : ML_compare(m_index, other.m_index);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		friend event_bus;

		explicit event_listener(event_bus * bus, int64 index) noexcept
			: m_bus{ bus }
			, m_index{ index }
		{
		}

		template <class Bus = event_bus> event_listener(Bus * bus) noexcept
			: event_listener{ bus, bus->next_id() }
		{
		}

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
		event_bus *	const	m_bus	; // event bus
		int64 const			m_index	; // bus index

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// dummy listener
	struct ML_CORE_API dummy_listener final : non_copyable, trackable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		~dummy_listener() noexcept final { m_callback = nullptr; }

		template <class ... Args
		> dummy_listener(event_bus * bus, Args && ... args) noexcept : event_listener{ bus }
		{
			if constexpr (0 < sizeof...(args)) { this->set_callback(ML_forward(args)...); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using event_listener::subscribe;

		using event_listener::unsubscribe;

		void on_event(event const & value) noexcept final { ML_check(m_callback)(value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_callback() const noexcept -> event_callback const & { return m_callback; }

		template <class Fn, class ... Args
		> auto set_callback(Fn && fn, Args && ... args) noexcept -> event_callback
		{
			return m_callback = std::bind(ML_forward(fn), std::placeholders::_1, ML_forward(args)...)
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		event_callback m_callback;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	// base delegate
	template <> struct event_delegate<void> : event_listener
	{
	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		virtual ~event_delegate() noexcept override = default;

		explicit event_delegate(event_bus * bus) noexcept : event_listener{ bus } {}

	protected:
		virtual void on_event(event const &) override = 0;
	};

	// event delegate
	template <class Ev> struct event_delegate final : event_delegate<void>
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using event_type				= typename Ev;
		using base_type					= typename event_delegate<void>;
		using self_type					= typename event_delegate<event_type>;
		using method_type				= typename method<void(event_type const &)>;
		using storage_type				= typename list<method_type>;
		using iterator					= typename storage_type::iterator;
		using const_iterator			= typename storage_type::const_iterator;
		using reverse_iterator			= typename storage_type::reverse_iterator;
		using const_reverse_iterator	= typename storage_type::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		~event_delegate() noexcept final { this->clear(); }

		event_delegate(event_bus * bus, allocator_type alloc = {}) noexcept
			: base_type{ bus }
			, m_data{ alloc }
		{
			this->subscribe<event_type>();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class event_type> void dispatch(event_type && value) noexcept { this->on_event(ML_forward(value)); }

		template <class event_type> void operator()(event_type && value) noexcept { this->on_event(ML_forward(value)); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void clear() noexcept { while (!m_data.empty()) { m_data.pop_back(); } }

		void erase(size_t i, size_t n = 1) noexcept { m_data.erase(begin() + i, begin() + i + n); }

		void reserve(size_t count) noexcept { m_data.reserve(count); }

		ML_NODISCARD auto at(size_t i) noexcept -> method_type & { return m_data[i]; }

		ML_NODISCARD auto at(size_t i) const noexcept -> method_type const & { return m_data[i]; }

		ML_NODISCARD auto operator[](size_t i) noexcept -> method_type & { return m_data[i]; }

		ML_NODISCARD auto operator[](size_t i) const noexcept -> method_type const & { return m_data[i]; }

		template <class ... Args
		> auto insert(size_t i, Args && ... args) noexcept -> method_type & { return m_data.emplace(begin() + i, ML_forward(args)...); }

		template <class ... Args
		> auto add(Args && ... args) noexcept -> method_type & { return m_data.emplace_back(ML_forward(args)...); }

		template <class Fn
		> auto operator+=(Fn && fn) noexcept -> self_type & { this->add(ML_forward(fn)); return (*this); }

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

	private:
		void on_event(event const & value) final
		{
			for (method_type const & fn : m_data)
			{
				ML_check(fn)((event_type const &)value);
			}
		}

		storage_type m_data;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// EVENT BUS
	struct ML_CORE_API event_bus : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		struct comparator final
		{
			comparator() noexcept = default;

			ML_NODISCARD bool operator()(event_listener * a, event_listener * b) const noexcept
			{
				return ML_check(a)->get_bus_order(*ML_check(b)) < 0;
			}
		};

		using allocator_type	= typename pmr::polymorphic_allocator<byte>;
		using listener_set		= typename flat_set<event_listener *, comparator>;
		using listener_map		= typename flat_map<hash_t, listener_set>;
		using delegate_map		= typename flat_map<hash_t, event_delegate<void> *>;
		using dummy_ref			= typename ref<dummy_listener>;
		using dummy_list		= typename list<dummy_ref>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		~event_bus() noexcept { this->remove_delegates(); }

		event_bus(allocator_type alloc = {}) noexcept
			: m_next_id		{}
			, m_listeners	{ alloc }
			, m_delegates	{ alloc }
			, m_dummies		{ alloc }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		template <class Ev = event
		> void broadcast(Ev && value) noexcept
		{
			if (auto const cat{ m_listeners.find(value) })
			{
				for (event_listener * const listener : (*cat->second))
				{
					ML_check(listener)->on_event(ML_forward(value));
				}
			}
		}

		template <class Ev, class ... Args
		> void broadcast(Args && ... args) noexcept
		{
			static_assert(_ML is_event_v<Ev>, "invalid event type");

			this->broadcast(Ev{ ML_forward(args)... });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto next_id() noexcept -> int64 { return ++m_next_id; }

		template <class Ev
		> bool add_listener(event_listener * value) noexcept
		{
			static_assert(_ML is_event_v<Ev>, "invalid event type");

			return value
				&& this == value->get_bus()
				&& m_listeners[Ev::ID].insert(value).second;
		}

		template <class Ev
		> void remove_listener(event_listener * value) noexcept
		{
			static_assert(_ML is_event_v<Ev>, "invalid event type");

			if (!value || (this != value->get_bus())) { return; }
			
			if (auto const cat{ m_listeners.find(Ev::ID) })
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

			m_listeners.for_each([&](auto, listener_set & cat) noexcept
			{
				if (auto const listener{ cat.find(value) }; listener != cat.end())
				{
					cat.erase(listener);
				}
			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		template <class Ev
		> ML_NODISCARD auto get_delegate(allocator_type alloc = {}) noexcept -> event_delegate<Ev> &
		{
			static_assert(_ML is_event_v<Ev>, "invalid event type");

			return *(event_delegate<Ev> *)(m_delegates.find_or_add_fn(hashof_v<Ev>, [&]() noexcept
			{
				return ML_new(event_delegate<Ev>, this, alloc);
			}));
		}

		template <class ... Evs
		> void remove_delegates() noexcept
		{
			if constexpr (0 < sizeof...(Evs))
			{
				meta::for_types<Evs...>([&](auto tag) noexcept
				{
					using T = typename decltype(tag)::type;

					static_assert(std::is_base_of_v<event, T>, "invalid event type");

					if (auto const it{ m_delegates.find(hashof_v<T>) })
					{
						ML_delete((event_delegate<T> *)(*it->second));

						m_delegates.erase(it->first);
					}
				});
			}
			else
			{
				m_delegates.for_each([&](auto, event_delegate<void> * value) noexcept
				{
					ML_delete(value);
				});
				m_delegates.clear();
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_dummies() const noexcept -> dummy_list const & { return m_dummies; }

		template <class ... Evs, class ... Args
		> auto new_dummy(Args && ... args) noexcept -> dummy_ref &
		{
			auto temp{ _ML make_ref<dummy_listener>(this, ML_forward(args)...) };

			if constexpr (0 < sizeof...(Evs)) { temp->subscribe<Evs...>(); }

			return m_dummies.emplace_back(std::move(temp));
		}

		auto remove_dummy(dummy_ref const & value) noexcept
		{
			if (auto it{ std::find(m_dummies.begin(), m_dummies.end(), value) }
			; it == m_dummies.end()) { return it; }
			else { return m_dummies.erase(it); }
		}

		void remove_all_dummies() noexcept { while (!m_dummies.empty()) { m_dummies.pop_back(); } }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		int64			m_next_id	; // counter
		listener_map	m_listeners	; // listeners
		delegate_map	m_delegates	; // delegates
		dummy_list		m_dummies	; // dummies

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_EVENT_SYSTEM_HPP_