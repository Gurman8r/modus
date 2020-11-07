#ifndef _ML_PLUGIN_MANAGER_HPP_
#define _ML_PLUGIN_MANAGER_HPP_

#include <modus_core/runtime/Plugin.hpp>
#include <modus_core/detail/SharedLibrary.hpp>

// DETAILS
namespace ml
{
	// plugin details
	struct ML_NODISCARD plugin_details final : trackable
	{
	public:
		hash_t hash; // hash code

		ds::string name, path, ext; // file info

		plugin_details(plugin_details const &) = default;
		plugin_details(plugin_details &&) noexcept = default;
		plugin_details & operator=(plugin_details const &) = default;
		plugin_details & operator=(plugin_details &&) noexcept = default;

	private:
		friend plugin_manager;

		explicit plugin_details(shared_library const & lib) noexcept
			: hash	{ lib.hash() }
			, name	{ lib.path().stem().string() }
			, path	{ lib.path().string() }
			, ext	{ lib.path().extension().string() }
		{
		}
	};
}

// INSTALLER
namespace ml
{
	// plugin installer
	struct ML_NODISCARD plugin_installer final : trackable
	{
	public:
		// create plugin
		ds::method< plugin * (plugin_manager *, void *)
		> create;

		// destroy plugin
		ds::method< void (plugin_manager *, plugin *)
		> destroy;

		plugin_installer(plugin_installer const &) = default;
		plugin_installer(plugin_installer &&) noexcept = default;
		plugin_installer & operator=(plugin_installer const &) = default;
		plugin_installer & operator=(plugin_installer &&) noexcept = default;
		
	private:
		friend plugin_manager;

		explicit plugin_installer(shared_library & lib) noexcept
			: create{ lib.get_proc<plugin *, plugin_manager *, void *>("ml_plugin_install") }
			, destroy{ lib.get_proc<void, plugin_manager *, plugin *>("ml_plugin_uninstall") }
		{
		}
	};
}

// MANAGER
namespace ml
{
	// plugin manager
	struct ML_CORE_API plugin_manager final : runtime_object<plugin_manager>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using runtime_base = runtime_object<plugin_manager>;

		ML_NODISCARD auto get_app() const noexcept { return m_app; }

		using runtime_base::get_bus;

		using runtime_base::get_context;

		using runtime_base::get_db;

		using runtime_base::get_io;

		using runtime_base::get_loopsys;

		using runtime_base::get_memory;

		using runtime_base::get_window;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static auto make_id(fs::path const & path) noexcept
		{
			return (plugin_id)hashof(shared_library::format_path(path).string());
		}

		using plugin_storage = typename ds::batch_vector
		<
			plugin_id			,	// id
			plugin_details		,	// details
			plugin_installer	,	// installer
			shared_library		,	// library
			ds::manual<plugin>		// plugin
		>;

		plugin_id install(fs::path const & path, void * userptr = nullptr) noexcept;

		bool uninstall(plugin_id value) noexcept;

		void uninstall_all() noexcept
		{
			auto & ids{ get_data<plugin_id>() };
			
			while (!ids.empty()) { uninstall(ids.back()); }
		}

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
			using P = ds::manual<plugin>;
			return m_data.end<P>() != m_data.find_if<P>([value](P const & e) noexcept
			{
				return value == e.get();
			});
		}

		ML_NODISCARD bool contains(ds::manual<plugin> const & value) const noexcept
		{
			return m_data.contains<ds::manual<plugin>>(value);
		}

		ML_NODISCARD auto get_data() noexcept -> plugin_storage & { return m_data; }

		ML_NODISCARD auto get_data() const noexcept -> plugin_storage const & { return m_data; }

		template <class T
		> ML_NODISCARD auto get_data() & noexcept -> ds::list<T> & { return m_data.get<T>(); }

		template <class T
		> ML_NODISCARD auto get_data() const & noexcept -> ds::list<T> const & { return m_data.get<T>(); }

		template <class T
		> ML_NODISCARD auto get_data(size_t i) & noexcept -> T & { return m_data.at<T>(i); }

		template <class T
		> ML_NODISCARD auto get_data(size_t i) const & noexcept -> T const & { return m_data.at<T>(i); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		friend application;

		explicit plugin_manager(application * const app) noexcept;

		~plugin_manager() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		application * const	m_app	; // application
		plugin_storage		m_data	; // plugin data

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_MANAGER_HPP_