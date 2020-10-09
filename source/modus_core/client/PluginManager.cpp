#include <client/PluginManager.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin_manager::plugin_manager(client_context * context)
		: client_object	{ context }
		, m_data		{ get_memory()->get_allocator() }
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

	void plugin_manager::on_event(event && value)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin_id plugin_manager::install(fs::path const & path, void * user)
	{
		// check exists
		if (this->has_plugin(path))
		{
			return nullptr;
		}
		// load library
		else if (auto const id{ std::invoke([&, &lib = shared_library{ path }]() -> plugin_id
		{
			return (!lib || m_data.contains<shared_library>(lib)) ? nullptr :
				std::get<plugin_id &>(m_data.push_back
				(
					ML_handle(plugin_id, lib.hash()), lib.path(), std::move(lib), nullptr,
					plugin_iface
					{
						lib.proc<plugin *, plugin_manager *, void *>("ml_plugin_attach"),
						lib.proc<void, plugin_manager *, plugin *>("ml_plugin_detach"),
					}
				));
		}) })
		// load plugin
		{
			if (auto const p{ m_data.back<plugin_iface>().attach(this, user) })
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
		else if (auto const it{ m_data.find<plugin_id>(value) }
		; it == m_data.end<plugin_id>()) { return false; }
		else
		{
			auto const i{ m_data.index_of<plugin_id>(it) };

			auto const p{ m_data.at<manual<plugin>>(i).release() };

			m_data.at<plugin_iface>(i).detach(this, p);

			m_data.erase(i);

			return true;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}