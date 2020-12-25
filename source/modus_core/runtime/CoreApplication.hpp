#ifndef _ML_CORE_APPLICATION_HPP_
#define _ML_CORE_APPLICATION_HPP_

#include <modus_core/embed/Python.hpp>
#include <modus_core/runtime/LibraryManager.hpp>
#include <modus_core/runtime/PluginManager.hpp>

namespace ml
{
	// core application
	struct ML_CORE_API core_application : non_copyable, trackable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		core_application(int32 argc, char * argv[], json const & attrs = {}, allocator_type alloc = {});

		virtual ~core_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual int32 exec() { return m_exit_code; }

		virtual void exit(int32 value) { m_exit_code = value; }

		void quit() noexcept { exit(EXIT_SUCCESS); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto app_data_path() const noexcept -> fs::path const & { return m_app_data_path; }

		ML_NODISCARD auto app_file_name() const noexcept -> fs::path const & { return m_app_file_name; }

		ML_NODISCARD auto app_file_path() const noexcept -> fs::path const & { return m_app_file_path; }

		ML_NODISCARD auto app_name() noexcept -> ds::string const & { return m_app_name; }

		ML_NODISCARD auto app_version() noexcept -> ds::string const & { return m_app_version; }

		ML_NODISCARD auto arguments() const noexcept -> ds::list<ds::string> const & { return m_arguments; }

		ML_NODISCARD auto arguments(size_t i) const noexcept -> ds::string const & { return m_arguments[i]; }

		ML_NODISCARD auto library_paths() const noexcept -> ds::list<fs::path> const & { return m_library_paths; }

		ML_NODISCARD auto library_paths(size_t i) const noexcept -> fs::path const & { return m_library_paths[i]; }

		ML_NODISCARD auto path_to(fs::path const & value) const noexcept -> fs::path { return m_app_data_path.native() + value.native(); }

		ML_NODISCARD auto uptime() const noexcept -> duration { return m_main_timer.elapsed(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto attr() noexcept -> json * { return std::addressof(m_attributes); }

		ML_NODISCARD auto attr() const noexcept -> json const * { return std::addressof(m_attributes); }

		template <class Arg0, class ... Args
		> ML_NODISCARD auto attr(Arg0 && arg0, Args && ... args) noexcept -> json *
		{
			return util::json_find(attr(), ML_forward(arg0), ML_forward(args)...);
		}

		template <class Arg0, class ... Args
		> ML_NODISCARD auto attr(Arg0 && arg0, Args && ... args) const noexcept -> json const *
		{
			return util::json_find(attr(), ML_forward(arg0), ML_forward(args)...);
		}

		template <class Arg0, class ... Args
		> ML_NODISCARD bool has_attr(Arg0 && arg0, Args && ... args) const noexcept
		{
			return util::json_contains(m_attributes, ML_forward(arg0), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_library_manager() const noexcept { return std::addressof(m_libraries); }

		template <class ID> ML_NODISCARD bool has_library(ID && id) const noexcept { return m_libraries.contains(ML_forward(id)); }

		auto load_library(fs::path const & path) noexcept -> ds::ref<shared_library> { return m_libraries.load_library(path); }

		bool free_library(library_id id) noexcept { free_plugin((plugin_id)id); return m_libraries.free_library(id); }

		bool free_library(fs::path const & path) noexcept { return free_library((library_id)hashof(path.string())); }

		bool free_library(ds::ref<shared_library> const & lib) noexcept { return lib && free_library((library_id)lib->hash_code()); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_plugin_manager() const noexcept { return std::addressof(m_plugins); }

		template <class ID> ML_NODISCARD bool has_plugin(ID && id) const noexcept { return m_plugins.contains(ML_forward(id)); }

		auto load_plugin(ds::ref<shared_library> const & lib, void * userptr = nullptr) noexcept -> plugin_id { return m_plugins.load_plugin(lib, userptr); }

		auto load_plugin(fs::path const & path, void * userptr = nullptr) noexcept -> plugin_id { return load_plugin(load_library(path), userptr); }

		bool free_plugin(plugin_id id) noexcept { return m_plugins.free_plugin(id); }

		bool free_plugin(fs::path const & path) noexcept { return free_plugin((plugin_id)hashof(path.string())); }

		bool free_plugin(ds::ref<shared_library> const & lib) noexcept { return lib && free_plugin((plugin_id)lib->hash_code()); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		timer					m_main_timer	; // main timer
		int32					m_exit_code		; // exit code
		fs::path				m_app_data_path	; // app data path
		fs::path				m_app_file_name	; // app file name
		fs::path				m_app_file_path	; // app file path
		ds::string				m_app_name		; // app name
		ds::string				m_app_version	; // app version
		ds::list<ds::string>	m_arguments		; // arguments
		ds::list<fs::path>		m_library_paths	; // library paths

		json					m_attributes	; // attrs
		event_bus				m_dispatcher	; // event bus
		library_manager			m_libraries		; // library manager
		plugin_manager			m_plugins		; // plugin manager

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global core_application
namespace ml::globals
{
	ML_decl_global(core_application) get() noexcept;

	ML_decl_global(core_application) set(core_application * value) noexcept;
}

#endif // !_ML_CORE_APPLICATION_HPP_