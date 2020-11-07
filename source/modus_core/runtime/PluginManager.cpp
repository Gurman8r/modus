#include <modus_core/runtime/PluginManager.hpp>
#include <modus_core/runtime/Application.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin_manager::plugin_manager(application * const app) noexcept
		: runtime_base	{ app->get_context() }
		, m_app			{ app }
		, m_data		{ get_memory()->get_allocator() }
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin_id plugin_manager::install(fs::path const & path, void * userptr) noexcept
	{
		// check exists
		if (this->contains(path))
		{
			return nullptr;
		}
		// load library
		else if (auto const id{ std::invoke([&, &lib = shared_library{ path }]() noexcept
		{
			return (!lib || this->contains(lib)) ? nullptr
				: std::get<plugin_id &>(m_data.push_back
				(
					(plugin_id)lib.hash(),
					plugin_details{ lib },
					plugin_installer{ lib },
					std::move(lib),
					nullptr
				));
		}) })
		{
			// load plugin
			if (auto const p{ m_data.back<plugin_installer>().create(this, userptr) })
			{
				m_data.back<plugin_instance>().reset(p);

				return id;
			}
		}
		// failed
		return nullptr;
	}

	bool plugin_manager::uninstall(plugin_id value) noexcept
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