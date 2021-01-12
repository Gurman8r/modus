#ifndef _ML_CORE_APPLICATION_HPP_
#define _ML_CORE_APPLICATION_HPP_

#include <modus_core/detail/Timer.hpp>
#include <modus_core/embed/AddonManager.hpp>

namespace ml
{
	// core application
	struct ML_CORE_API core_application : non_copyable, trackable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		using library_storage = typename batch_vector<hash_t, ref<native_library>>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		core_application(int32 argc, char * argv[], json const & argj = {}, allocator_type alloc = {});

		virtual ~core_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual int32 run() { return m_exit_code; }

		virtual void exit(int32 value = EXIT_SUCCESS) { m_exit_code = value; }

		void quit() noexcept { this->exit(EXIT_SUCCESS); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type { return {}; }

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

		ML_NODISCARD auto get_user_pointer() const noexcept -> void * { return m_userptr; }

		void set_user_pointer(void * value) noexcept { m_userptr = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_library(hash_t id) -> ref<native_library>
		{
			if (size_t const i{ m_libs.lookup<hash_t>(id) }; i == m_libs.npos) { return nullptr; }
			else { return m_libs.get<ref<native_library>>(i); }
		}

		ML_NODISCARD auto get_library(hash_t id) const -> ref<native_library>
		{
			if (size_t const i{ m_libs.lookup<hash_t>(id) }; i == m_libs.npos) { return nullptr; }
			else { return m_libs.get<ref<native_library>>(i); }
		}

		ML_NODISCARD auto get_library(fs::path const & path) noexcept -> ref<native_library>
		{
			return this->get_library(hashof(path.filename().string()));
		}

		ML_NODISCARD auto get_library(fs::path const & path) const noexcept -> ref<native_library>
		{
			return this->get_library(hashof(path.filename().string()));
		}

		ML_NODISCARD bool has_library(hash_t id) const noexcept
		{
			return m_libs.contains<hash_t>(id);
		}

		ML_NODISCARD bool has_library(fs::path const & path) const noexcept
		{
			return this->has_library(hashof(path.filename().string()));
		}

		ML_NODISCARD bool has_library(ref<native_library> const & value) const noexcept
		{
			return value && this->has_library(value->get_hash_code());
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto load_library(fs::path const & path, allocator_type alloc = {}) noexcept -> ref<native_library>
		{
			if (ref<native_library> lib{ this->get_library(path) }) { return lib; }
			else if (!*(lib = alloc_ref<native_library>(alloc, path))) { return nullptr; }
			else { return std::get<1>(m_libs.push_back(lib->get_hash_code(), std::move(lib))); }
		}

		bool add_library(ref<native_library> const & value) noexcept
		{
			if (!value || !*value || m_libs.contains<hash_t>(value->get_hash_code())) { return false; }
			else { m_libs.push_back(value->get_hash_code(), value); return true; }
		}

		bool free_library(hash_t id) noexcept
		{
			if (size_t const i{ m_libs.lookup<hash_t>(id) }; i == m_libs.npos) { return false; }
			else { m_libs.erase(i); return true; }
		}

		bool free_library(fs::path const & path) noexcept
		{
			return free_library(hashof(path.filename().string()));
		}

		bool free_library(ref<native_library> const & lib) noexcept
		{
			return lib && free_library(lib->get_hash_code());
		}

		void free_all_libraries() noexcept
		{
			uninstall_all_addons();

			while (!m_libs.empty()) { m_libs.pop_back(); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		template <class I> ML_NODISCARD bool has_addon(I && i) const noexcept { return m_addons.contains(ML_forward(i)); }

		auto install_addon(ref<native_library> const & lib, void * userptr = nullptr) noexcept -> hash_t { return m_addons.install_addon(lib, userptr); }

		auto install_addon(fs::path const & path, void * userptr = nullptr) noexcept -> hash_t { return install_addon(load_library(path), userptr); }

		bool uninstall_addon(hash_t id) noexcept { return m_addons.uninstall_addon(id); }

		bool uninstall_addon(fs::path const & path) noexcept { return uninstall_addon(hashof(path.filename().string())); }

		bool uninstall_addon(ref<native_library> const & lib) noexcept { return lib && uninstall_addon(lib->get_hash_code()); }

		void uninstall_all_addons() { m_addons.uninstall_all_addons(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		int32			m_exit_code		; // exit code
		fs::path		m_app_data_path	; // app data path
		fs::path		m_app_file_name	; // app file name
		fs::path		m_app_file_path	; // app file path
		string			m_app_name		; // app name
		string			m_app_version	; // app version
		list<string>	m_arguments		; // arguments
		json			m_attributes	; // attributes
		list<fs::path>	m_library_paths	; // library paths
		list<byte>		m_temp_buffer	; // temp buffer
		void *			m_userptr		; // user pointer

		timer			m_main_timer	; // main timer
		event_bus		m_event_bus		; // event bus
		library_storage	m_libs			; // library manager
		addon_manager	m_addons		; // addon manager

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