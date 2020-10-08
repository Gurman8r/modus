#ifndef _ML_PLUGIN_MANAGER_HPP_
#define _ML_PLUGIN_MANAGER_HPP_

#include <core/system/SharedLibrary.hpp>
#include <core/client/Plugin.hpp>

namespace ml
{
	struct ML_CORE_API plugin_manager final : client_object<plugin_manager>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		struct ML_NODISCARD plugin_iface final
		{
			plugin * (*attach)(plugin_manager *, void *);

			void (*detach)(plugin_manager *, plugin *);
		};

		using plugin_storage = typename ds::batch_vector
		<
			plugin_id		,	// id
			fs::path		,	// path
			shared_library	,	// library
			manual<plugin>	,	// plugin
			plugin_iface		// interface
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit plugin_manager(client_context * context);

		~plugin_manager() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		plugin_id install(fs::path const & path, void * user = nullptr);

		bool uninstall(plugin_id value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool has_plugin(fs::path const & path) const noexcept
		{
			return m_data.contains<plugin_id>
			(
				ML_handle(plugin_id, util::hash(shared_library::format_path(path).string()))
			);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		plugin_storage m_data; // plugins

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_MANAGER_HPP_