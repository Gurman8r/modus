#ifndef _ML_CORE_APPLICATION_HPP_
#define _ML_CORE_APPLICATION_HPP_

#include <modus_core/detail/Timer.hpp>
#include <modus_core/embed/PluginManager.hpp>

namespace ml
{
	// core application
	struct ML_CORE_API core_application : non_copyable, trackable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		using library_storage = typename batch_vector<library_id, ref<native_library>>;

		core_application(int32 argc, char * argv[], json const & argj = {}, allocator_type alloc = {});

		virtual ~core_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual int32 exec() { return m_exit_code; }

		virtual void exit(int32 value = EXIT_SUCCESS) { m_exit_code = value; }

		void quit() noexcept { this->exit(EXIT_SUCCESS); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_app_data_path() const noexcept -> fs::path const & { return m_app_data_path; }

		ML_NODISCARD auto get_app_file_name() const noexcept -> fs::path const & { return m_app_file_name; }

		ML_NODISCARD auto get_app_file_path() const noexcept -> fs::path const & { return m_app_file_path; }

		ML_NODISCARD auto get_app_name() noexcept -> string const & { return m_app_name; }

		ML_NODISCARD auto get_app_version() noexcept -> string const & { return m_app_version; }

		ML_NODISCARD auto get_arguments() noexcept -> list<string> & { return m_arguments; }

		ML_NODISCARD auto get_arguments() const noexcept -> list<string> const & { return m_arguments; }

		ML_NODISCARD auto get_arguments(size_t i) noexcept -> string & { return m_arguments[i]; }

		ML_NODISCARD auto get_arguments(size_t i) const noexcept -> string const & { return m_arguments[i]; }

		ML_NODISCARD auto get_attr() noexcept -> json & { return m_attributes; }

		ML_NODISCARD auto get_attr() const noexcept -> json const & { return m_attributes; }

		template <class I> ML_NODISCARD auto get_attr(I && i) noexcept -> json & { return m_attributes[ML_forward(i)]; }

		template <class I> ML_NODISCARD auto get_attr(I && i) const noexcept -> json const & { return m_attributes[ML_forward(i)]; }

		template <class I> ML_NODISCARD bool has_attr(I && i) const noexcept { return m_attributes.contains(ML_forward(i)); }

		ML_NODISCARD auto get_library_paths() noexcept -> list<fs::path> & { return m_library_paths; }

		ML_NODISCARD auto get_library_paths() const noexcept -> list<fs::path> const & { return m_library_paths; }

		ML_NODISCARD auto get_library_paths(size_t i) noexcept -> fs::path & { return m_library_paths[i]; }

		ML_NODISCARD auto get_library_paths(size_t i) const noexcept -> fs::path const & { return m_library_paths[i]; }

		ML_NODISCARD auto get_path_to(fs::path const & value) const noexcept -> fs::path { return m_app_data_path.native() + value.native(); }

		ML_NODISCARD auto get_temp_buffer() noexcept -> list<byte> & { return m_temp_buffer; }

		ML_NODISCARD auto get_temp_buffer() const noexcept -> list<byte> const & { return m_temp_buffer; }

		ML_NODISCARD auto get_time() const noexcept -> duration { return m_main_timer.elapsed(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_library(library_id id) -> ref<native_library>
		{
			if (size_t const i{ m_libraries.lookup<library_id>(id) }; i == m_libraries.npos) { return nullptr; }
			else { return m_libraries.get<ref<native_library>>(i); }
		}

		ML_NODISCARD auto get_library(library_id id) const -> ref<native_library>
		{
			if (size_t const i{ m_libraries.lookup<library_id>(id) }; i == m_libraries.npos) { return nullptr; }
			else { return m_libraries.get<ref<native_library>>(i); }
		}

		ML_NODISCARD auto get_library(fs::path const & path) noexcept -> ref<native_library> { return this->get_library((library_id)hashof(path.string())); }

		ML_NODISCARD auto get_library(fs::path const & path) const noexcept -> ref<native_library> { return this->get_library((library_id)hashof(path.string())); }

		ML_NODISCARD bool has_library(library_id id) const noexcept { return m_libraries.contains<library_id>(id); }

		ML_NODISCARD bool has_library(fs::path const & path) const noexcept { return this->has_library((library_id)hashof(path.string())); }

		ML_NODISCARD bool has_library(ref<native_library> const & value) const noexcept { return value && this->has_library(value->get_uuid()); }

		auto load_library(fs::path const & path, allocator_type alloc = {}) noexcept -> ref<native_library>
		{
			if (ref<native_library> lib{ this->get_library(path) }) { return lib; }
			else if (!(lib = alloc_ref<native_library>(alloc, path)) || !*lib) { return nullptr; }
			else { return std::get<1>(m_libraries.push_back(lib->get_uuid(), std::move(lib))); }
		}

		bool add_library(ref<native_library> const & value) noexcept
		{
			if (!value || !*value || m_libraries.contains<library_id>(value->get_uuid())) { return false; }
			else { m_libraries.push_back(value->get_uuid(), value); return true; }
		}

		bool free_library(library_id id) noexcept
		{
			if (size_t const i{ m_libraries.lookup<library_id>(id) }; i == m_libraries.npos) { return false; }
			else { uninstall_plugin((plugin_id)id); m_libraries.erase(i); return true; }
		}

		bool free_library(fs::path const & path) noexcept { return free_library((library_id)hashof(path.string())); }

		bool free_library(ref<native_library> const & lib) noexcept { return lib && free_library(lib->get_uuid()); }

		void free_all_libraries() { uninstall_all_plugins(); while (!m_libraries.empty()) { m_libraries.pop_back(); } }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		template <class ID> ML_NODISCARD bool has_plugin(ID && id) const noexcept { return m_plugins.contains(ML_forward(id)); }

		auto install_plugin(ref<native_library> const & lib, void * userptr = nullptr) noexcept -> plugin_id { return m_plugins.install_plugin(lib, userptr); }

		auto install_plugin(fs::path const & path, void * userptr = nullptr) noexcept -> plugin_id { return install_plugin(load_library(path), userptr); }

		bool uninstall_plugin(plugin_id id) noexcept { return m_plugins.uninstall_plugin(id); }

		bool uninstall_plugin(fs::path const & path) noexcept { return uninstall_plugin((plugin_id)hashof(path.string())); }

		bool uninstall_plugin(ref<native_library> const & lib) noexcept { return lib && uninstall_plugin((plugin_id)lib->get_uuid()); }

		void uninstall_all_plugins() { m_plugins.uninstall_all_plugins(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		timer			m_main_timer	; // main timer
		int32			m_exit_code		; // exit code
		fs::path		m_app_data_path	; // app data path
		fs::path		m_app_file_name	; // app file name
		fs::path		m_app_file_path	; // app file path
		string			m_app_name		; // app name
		string			m_app_version	; // app version
		list<string>	m_arguments		; // arguments
		list<fs::path>	m_library_paths	; // native_library paths
		json			m_attributes	; // attributes
		list<byte>		m_temp_buffer	; // temp buffer
		
		event_bus		m_event_bus	; // event bus
		library_storage	m_libraries	; // native_library storage
		plugin_manager	m_plugins	; // plugin manager

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global core_application
namespace ml::globals
{
	ML_decl_global(core_application) get_global();

	ML_decl_global(core_application) set_global(core_application *);
}

#endif // !_ML_CORE_APPLICATION_HPP_