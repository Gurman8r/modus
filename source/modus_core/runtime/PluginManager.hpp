#ifndef _ML_PLUGIN_MANAGER_HPP_
#define _ML_PLUGIN_MANAGER_HPP_

#include <modus_core/runtime/Plugin.hpp>
#include <modus_core/system/SharedLibrary.hpp>

namespace ml
{
	// plugin details
	struct ML_NODISCARD plugin_details final
	{
		hash_t id;

		ds::string name, path, extension;
	};

	// plugin installer
	struct ML_NODISCARD plugin_installer final
	{
		plugin * (*install)(plugin_manager *, void *); // creates plugin

		void (*uninstall)(plugin_manager *, plugin *); // destroys plugin

		ML_NODISCARD constexpr operator bool() const noexcept // validity check
		{
			return install && uninstall;
		}
	};

	// plugin manager
	struct ML_CORE_API plugin_manager final : runtime_object<plugin_manager>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		friend application;

		explicit plugin_manager(application * const app) noexcept;

		~plugin_manager() noexcept override;

		void on_event(event &&) override {} // unused

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using plugin_storage = typename ds::batch_vector
		<
			plugin_id			,	// id
			plugin_details		,	// details
			plugin_installer	,	// installer
			shared_library		,	// library
			ds::manual<plugin>		// plugin
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		plugin_id install(fs::path const & path, void * userptr = nullptr);

		bool uninstall(plugin_id value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool has_plugin(fs::path const & path) const noexcept
		{
			return m_data.contains<plugin_id>
			(
				(plugin_id)hashof(shared_library::format_path(path).string())
			);
		}

		void uninstall_all() noexcept
		{
			auto & ids{ m_data.get<plugin_id>() };

			while (!ids.empty()) { uninstall(ids.back()); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_application() const noexcept -> application * { return m_app; }

		ML_NODISCARD auto get_data() noexcept -> plugin_storage & { return m_data; }

		ML_NODISCARD auto get_data() const noexcept -> plugin_storage const & { return m_data; }

		template <class T
		> ML_NODISCARD auto get_data() & noexcept -> auto & { return m_data.get<T>(); }

		template <class T
		> ML_NODISCARD auto get_data() const & noexcept -> auto const & { return m_data.get<T>(); }

		template <class T
		> ML_NODISCARD auto get_data(size_t i) & noexcept -> auto & { return m_data.at<T>(i); }

		template <class T
		> ML_NODISCARD auto get_data(size_t i) const & noexcept -> auto const & { return m_data.at<T>(i); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		application * const	m_app	; // application
		plugin_storage		m_data	; // plugin data

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_MANAGER_HPP_