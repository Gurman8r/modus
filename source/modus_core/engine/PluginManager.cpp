#include <modus_core/engine/PluginManager.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin_manager::plugin_manager(event_bus * bus, allocator_type alloc)
		: core_object{ bus }
		, m_storage{ alloc }
	{
	}

	plugin_manager::~plugin_manager() noexcept
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin_id plugin_manager::install(fs::path const & path, void * userptr)
	{
		auto load_library = [&]()
		{
			if (contains(path))
			{
				return plugin::null;
			}
			else if (shared_library lib{ path }; lib && !contains(lib))
			{
				return std::get<0>(storage().push_back
				(
					(plugin_id)lib.hash(),
					plugin_details{ lib },
					plugin_installer{ lib },
					std::move(lib),
					nullptr
				));
			}
			else
			{
				return plugin::null;
			}
		};

		if (!load_library())
		{
			return plugin::null;
		}
		else if (plugin * const p{ get<plugin_installer>().back().create(this, userptr) })
		{
			get<plugin_instance>().back().reset(p);

			return get<plugin_id>().back();
		}
		else
		{
			return plugin::null;
		}
		
	}

	bool plugin_manager::uninstall(plugin_id value)
	{
		if (!value) { return false; }
		else if (auto const i{ m_storage.lookup<plugin_id>(value) }
		; i == m_storage.npos) { return false; }
		else
		{
			auto const p{ m_storage.at<plugin_instance>(i).release() };
			m_storage.at<plugin_installer>(i).destroy(this, p);
			m_storage.erase(i);
			return true;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}