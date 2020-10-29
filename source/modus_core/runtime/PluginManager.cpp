#include <modus_core/runtime/PluginManager.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin_manager::plugin_manager(runtime_api * api)
		: runtime_object{ api }
		, m_data		{ api->mem->get_allocator() }
	{
		if (!get_global<plugin_manager>())
		{
			set_global<plugin_manager>(this);
		}
	}

	plugin_manager::~plugin_manager() noexcept
	{
		uninstall_all();

		if (this == get_global<plugin_manager>())
		{
			set_global<plugin_manager>(nullptr);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin_id plugin_manager::install(fs::path const & path, void * userptr)
	{
		// check exists
		if (this->has_plugin(path))
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
						(ds::string)lib.path().stem().string(),
						(ds::string)lib.path().string(),
						(ds::string)lib.path().extension().string(),
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
			if (auto const p{ m_data.back<plugin_iface>().do_install(this, userptr) })
			{
				m_data.back<ds::manual_ptr<plugin>>().reset(p);

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
			auto const p{ m_data.at<ds::manual_ptr<plugin>>(i).release() };
			m_data.at<plugin_iface>(i).do_uninstall(this, p);
			m_data.erase(i);
			return true;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global plugin manager
namespace ml::globals
{
	static plugin_manager * g_plugin_manager{};

	template <> plugin_manager * get() noexcept {
		return g_plugin_manager;
	}

	template <> plugin_manager * set(plugin_manager * value) noexcept {
		return g_plugin_manager = value;
	}
}