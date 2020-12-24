#ifndef _ML_PLUGIN_MANAGER_HPP_
#define _ML_PLUGIN_MANAGER_HPP_

#include <modus_core/detail/BatchVector.hpp>
#include <modus_core/engine/Plugin.hpp>
#include <modus_core/system/SharedLibrary.hpp>

// plugin instance
namespace ml { ML_alias plugin_instance = typename ds::scary<plugin>; }

// details
namespace ml
{
	// plugin details
	struct ML_NODISCARD plugin_details final : trackable
	{
		hash_t hash; // hash code

		ds::string name, path, ext; // file info

		explicit plugin_details(shared_library & lib) noexcept
			: hash	{ lib.hash() }
			, name	{ lib.path().stem().string() }
			, path	{ lib.path().string() }
			, ext	{ lib.path().extension().string() }
		{
		}

		plugin_details(plugin_details const &) = default;
		plugin_details(plugin_details &&) noexcept = default;
		plugin_details & operator=(plugin_details const &) = default;
		plugin_details & operator=(plugin_details &&) noexcept = default;
	};
}

// installer
namespace ml
{
	// plugin installer
	struct ML_NODISCARD plugin_installer final : trackable
	{
		plugin * (*create)(plugin_manager *, void *); // create plugin
		
		void (*destroy)(plugin_manager *, plugin *); // destroy plugin
		
		explicit plugin_installer(shared_library & lib) noexcept
			: create{ lib.get_proc<plugin *, plugin_manager *, void *>("ml_plugin_create") }
			, destroy{ lib.get_proc<void, plugin_manager *, plugin *>("ml_plugin_destroy") }
		{
			ML_assert(create && destroy);
		}

		plugin_installer(plugin_installer const &) = default;
		plugin_installer(plugin_installer &&) noexcept = default;
		plugin_installer & operator=(plugin_installer const &) = default;
		plugin_installer & operator=(plugin_installer &&) noexcept = default;
	};
}

// manager
namespace ml
{
	// plugin manager
	struct ML_CORE_API plugin_manager final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		using plugin_storage = typename ds::batch_vector
		<
			plugin_id			,	// id
			plugin_details		,	// details
			plugin_installer	,	// installer
			shared_library		,	// library
			plugin_instance			// plugin
		>;

		static auto make_id(fs::path const & path) noexcept
		{
			return (plugin_id)hashof(shared_library::format_path(path).string());
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		plugin_manager(application * app, allocator_type alloc = {});

		~plugin_manager() noexcept final { this->uninstall_all(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		plugin_id install(fs::path const & path, void * userptr = nullptr);

		bool uninstall(plugin_id value);

		void uninstall_all() noexcept
		{
			auto & ids{ m_data.get<plugin_id>() };

			while (!ids.empty()) { this->uninstall(ids.back()); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Derived, class ... Args
		> auto allocate_plugin(Args && ... args) -> Derived *
		{
			static_assert(std::is_base_of_v<plugin, Derived>);
			
			auto ptr{ (Derived *)m_alloc.allocate(sizeof(Derived)) };
			
			util::construct(ptr, this, ML_forward(args)...);
			
			return ptr;
		}

		template <class Derived
		> void deallocate_plugin(plugin * value)
		{
			static_assert(std::is_base_of_v<plugin, Derived>);

			util::destruct((Derived *)value);

			m_alloc.deallocate((byte *)value, sizeof(Derived));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Ev
		> void fire(Ev && value) noexcept
		{
			for (auto & p : m_data.get<plugin_instance>())
			{
				p->on_event(ML_forward(value));
			}
		}

		template <class Ev, class ... Args
		> void fire(Args && ... args) noexcept
		{
			this->fire(Ev{ ML_forward(args)... });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Ev
		> void send(size_t i, Ev && value) noexcept
		{
			m_data.at<plugin_instance>(i)->on_event(ML_forward(value));
		}

		template <class Ev, class ... Args
		> void send(size_t i, Args && ... args) noexcept
		{
			m_data.at<plugin_instance>(i)->on_event(Ev{ ML_forward(args)... });
		}

		template <class Ev
		> void send(plugin_id id, Ev && value) noexcept
		{
			if (size_t const i{ m_data.lookup<plugin_id>(id) }; i != m_data.npos)
			{
				this->send(i, ML_forward(value));
			}
		}

		template <class Ev, class ... Args
		> void send(plugin_id id, Args && ... args) noexcept
		{
			if (size_t const i{ m_data.lookup<plugin_id>(id) }; i != m_data.npos)
			{
				this->send<Ev>(i, ML_forward(args)...);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type { return m_alloc; }

		ML_NODISCARD auto get_app() const noexcept -> application * { return m_app; }

		ML_NODISCARD auto get_bus() const noexcept -> event_bus * { return m_bus; }

		ML_NODISCARD auto get_data() const noexcept -> plugin_storage const & { return m_data; }

		template <class T
		> ML_NODISCARD auto get_data() const & noexcept -> ds::list<T> const & { return m_data.get<T>(); }

		template <class T
		> ML_NODISCARD auto get_data(size_t i) const & noexcept -> T const & { return m_data.at<T>(i); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool contains(plugin_id id) const noexcept
		{
			return m_data.contains<plugin_id>(id);
		}

		ML_NODISCARD bool contains(fs::path const & path) const noexcept
		{
			return m_data.contains<plugin_id>(make_id(path));
		}

		ML_NODISCARD bool contains(shared_library const & value) const noexcept
		{
			return m_data.contains<shared_library>(value);
		}

		ML_NODISCARD bool contains(plugin const * value) const noexcept
		{
			return m_data.end<plugin_instance>() != m_data.find_if<plugin_instance>([&
			](plugin_instance const & e) noexcept
			{
				return value == e.get();
			});
		}

		ML_NODISCARD bool contains(plugin_instance const & value) const noexcept
		{
			return m_data.contains<plugin_instance>(value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		allocator_type		m_alloc	; // allocator
		application * const	m_app	; // application
		event_bus * const	m_bus	; // event bus
		plugin_storage		m_data	; // plugin data

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_MANAGER_HPP_