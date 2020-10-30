#ifndef _ML_PLUGIN_MANAGER_HPP_
#define _ML_PLUGIN_MANAGER_HPP_

#include <modus_core/runtime/Plugin.hpp>
#include <modus_core/system/SharedLibrary.hpp>

namespace ml
{
	// plugin details
	struct ML_NODISCARD plugin_details final
	{
		ds::string name, path, extension;

		hash_t id;
	};

	// plugin interface
	struct ML_NODISCARD plugin_iface final
	{
		plugin * (*do_install)(plugin_manager *, void *);

		void (*do_uninstall)(plugin_manager *, plugin *);
	};

	// plugin manager
	struct ML_CORE_API plugin_manager final : runtime_object<plugin_manager>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using plugin_storage = typename ds::batch_vector
		<
			plugin_id		,	// id
			shared_library	,	// library
			plugin_details	,	// details
			plugin_iface	,	// interface
			ds::manual<plugin>	// instance
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit plugin_manager(application * app);

		~plugin_manager() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		plugin_id install(fs::path const & path, void * userptr = nullptr);

		bool uninstall(plugin_id value);

		void uninstall_all() noexcept
		{
			auto & ids{ m_data.get<plugin_id>() };

			while (!ids.empty()) { uninstall(ids.back()); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_application() const noexcept -> application * { return m_app; }

		ML_NODISCARD auto get_data()  noexcept -> plugin_storage & { return m_data; }

		ML_NODISCARD auto get_data() const noexcept -> plugin_storage const & { return m_data; }

		ML_NODISCARD bool has_plugin(fs::path const & path) const noexcept
		{
			return m_data.contains<plugin_id>
			(
				(plugin_id)hashof(shared_library::format_path(path).string())
			);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		application * const	m_app	; // application
		plugin_storage		m_data	; // plugin data

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_MANAGER_HPP_