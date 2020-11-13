#include <modus_core/engine/PluginManager.hpp>
#include <modus_core/engine/Application.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin_manager::plugin_manager(application * app, allocator_type alloc) noexcept
		: m_app	{ ML_check(app) }
		, m_mem	{ ML_check(get_global<memory_manager>()) }
		, m_data{ alloc }
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin_id plugin_manager::install(fs::path const & path, void * userptr)
	{
		auto load_library = [&]() -> plugin_id
		{
			if (this->contains(path))
			{
				return nullptr;
			}
			else if (shared_library lib{ path }; lib && !this->contains(lib))
			{
				return std::get<0>(m_data.push_back
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
				return nullptr;
			}
		};

		if (!load_library())
		{
			return nullptr;
		}
		else if (plugin * const p{ m_data.get<plugin_installer>().back().create(this, userptr) })
		{
			m_data.get<plugin_instance>().back().reset(p);

			return m_data.get<plugin_id>().back();
		}
		else
		{
			return nullptr;
		}
	}

	bool plugin_manager::uninstall(plugin_id value)
	{
		if (!value) { return false; }
		else if (auto const i{ m_data.lookup<plugin_id>(value) }
		; i == m_data.npos) { return false; }
		else
		{
			auto const p{ m_data.at<plugin_instance>(i).release() };
			m_data.at<plugin_installer>(i).destroy(this, p);
			m_data.erase(i);
			return true;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}