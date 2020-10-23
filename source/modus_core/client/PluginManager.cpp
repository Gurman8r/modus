#include <modus_core/client/PluginManager.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin_manager::plugin_manager(client_context * ctx)
		: client_object{ ctx }
		, m_data		{ ctx->mem->get_allocator() }
	{
	}

	plugin_manager::~plugin_manager() noexcept
	{
		// uninstall plugins
		while (!m_data.get<plugin_id>().empty())
		{
			uninstall(m_data.get<plugin_id>().back());
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin_id plugin_manager::install(fs::path const & path, void * user)
	{
		// check exists
		if (this->is_installed(path))
		{
			return nullptr;
		}
		// load library
		else if (auto const id{ std::invoke([&, &lib = shared_library{ path }]() -> plugin_id
		{
			return (!lib || m_data.contains<shared_library>(lib)) ? nullptr
				: std::get<plugin_id &>(m_data.push_back
				(
					(plugin_id)lib.hash_code(),
					std::move(lib),
					plugin_details
					{
						(pmr::string)lib.path().stem().string(),
						(pmr::string)lib.path().string(),
						(pmr::string)lib.path().extension().string(),
						lib.hash_code()
					},
					plugin_iface
					{
						lib.proc<plugin *, plugin_manager *, void *>("ml_plugin_attach"),
						lib.proc<void, plugin_manager *, plugin *>("ml_plugin_detach"),
					},
					nullptr
				));
		}) })
		// load plugin
		{
			if (auto const p{ m_data.back<plugin_iface>().do_install(this, user) })
			{
				m_data.back<manual<plugin>>().reset(p);

				return id;
			}
		}
		// failed
		return nullptr;
	}

	bool plugin_manager::uninstall(plugin_id value)
	{
		if (!value) { return false; }
		else if (auto const i{ m_data.lookup<plugin_id>(value) }
		; i == m_data.npos) { return false; }
		else
		{
			auto const p{ m_data.at<manual<plugin>>(i).release() };
			m_data.at<plugin_iface>(i).do_uninstall(this, p);
			m_data.erase(i);
			return true;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}