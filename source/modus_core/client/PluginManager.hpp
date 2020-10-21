#ifndef _ML_PLUGIN_MANAGER_HPP_
#define _ML_PLUGIN_MANAGER_HPP_

#include <modus_core/client/Plugin.hpp>
#include <modus_core/system/SharedLibrary.hpp>

namespace ml
{
	// plugin details
	struct ML_NODISCARD plugin_details final
	{
		pmr::string name, path, extension;

		hash_t id;
	};

	// plugin interface
	struct ML_NODISCARD plugin_iface final
	{
		plugin * (*do_install)(plugin_manager *, void *);

		void (*do_uninstall)(plugin_manager *, plugin *);
	};

	// plugin manager
	struct ML_CORE_API plugin_manager final : client_object<plugin_manager>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using plugin_storage = typename ds::batch_vector
		<
			plugin_id		,	// id
			shared_library	,	// library
			plugin_details	,	// details
			plugin_iface	,	// interface
			manual<plugin>		// instance
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit plugin_manager(client_context * context);

		~plugin_manager() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		plugin_id install(fs::path const & path, void * user = nullptr);

		bool uninstall(plugin_id value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD plugin_storage const & get_storage() const noexcept
		{
			return m_data;
		}

		ML_NODISCARD bool is_installed(fs::path const & path) const noexcept
		{
			return m_data.contains<plugin_id>
			(
				(plugin_id)hashof(shared_library::format_path(path).string())
			);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		plugin_storage m_data; // plugins

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_MANAGER_HPP_