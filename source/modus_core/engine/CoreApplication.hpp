#ifndef _ML_CORE_APPLICATION_HPP_
#define _ML_CORE_APPLICATION_HPP_

#include <modus_core/detail/LoopSystem.hpp>
#include <modus_core/engine/Object.hpp>
#include <modus_core/engine/PlatformAPI.hpp>

// CORE APP
namespace ml
{
	// core application
	struct ML_CORE_API core_application : non_copyable, trackable, core_object
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using subsystem = typename loop_system::subsystem;

		explicit core_application(int32_t argc, char * argv[], allocator_type alloc = {});

		virtual ~core_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto app_file_name() const noexcept -> fs::path const &
		{
			return m_app_file_name;
		}

		ML_NODISCARD auto app_file_path() const noexcept -> fs::path const &
		{
			return m_app_file_path;
		}

		ML_NODISCARD auto app_name() noexcept -> ds::string const &
		{
			return m_app_name;
		}

		ML_NODISCARD auto app_version() noexcept -> ds::string const &
		{
			return m_app_version;
		}

		ML_NODISCARD auto arguments() const noexcept -> ds::list<ds::string> const &
		{
			return m_arguments;
		}

		ML_NODISCARD auto arguments(size_t i) const noexcept -> ds::string const &
		{
			return m_arguments[i];
		}

		ML_NODISCARD auto attr() noexcept -> json &
		{
			return m_attribs;
		}

		ML_NODISCARD auto attr() const noexcept -> json const &
		{
			return m_attribs;
		}

		template <class I> ML_NODISCARD auto attr(I && i) noexcept -> json &
		{
			return m_attribs[ML_forward(i)];
		}

		template <class I> ML_NODISCARD auto attr(I && i) const noexcept -> json const &
		{
			return m_attribs[ML_forward(i)];
		}

		ML_NODISCARD auto library_paths() const noexcept -> ds::list<fs::path> const &
		{
			return m_lib_paths;
		}

		ML_NODISCARD auto library_paths(size_t i) const noexcept -> fs::path const &
		{
			return m_lib_paths[i];
		}

		ML_NODISCARD auto path_to(fs::path const & value = {}) const noexcept -> fs::path
		{
			return library_paths(0).native() + value.native();
		}

	public:
		auto set_app_name(ds::string const & value) noexcept -> ds::string const &
		{
			return m_app_name = value;
		}

		auto set_app_version(ds::string const & value) noexcept -> ds::string const &
		{
			return m_app_version = value;
		}

		auto set_attributes(json const & value) noexcept -> json const &
		{
			return m_attribs = value;
		}

		auto set_library_paths(ds::list<fs::path> const & value) noexcept -> ds::list<fs::path> const &
		{
			return m_lib_paths = value;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		int32_t exec()
		{
			m_loop->process();

			return m_exit_code;
		}

		void exit(int32_t exit_code)
		{
			m_exit_code = exit_code;

			m_loop->set_loop_condition(nullptr);
		}

		void quit() noexcept
		{
			this->exit(EXIT_SUCCESS);
		}

	public:
		ML_NODISCARD auto get_main_loop() const noexcept -> subsystem const &
		{
			return m_loop;
		}

		auto set_main_loop(subsystem const & value) noexcept -> subsystem const &
		{
			return m_loop = value;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD bool initialize_interpreter();

		void finalize_interpreter();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		fs::path				m_app_file_name	; // 
		fs::path				m_app_file_path	; // 
		ds::string				m_app_name		; // 
		ds::string				m_app_version	; // 
		ds::list<ds::string>	m_arguments		; // 
		json					m_attribs		; // 
		ds::list<fs::path>		m_lib_paths		; // 
		
		int32_t		m_exit_code		; // 
		event_bus	m_dispatcher	; // 
		subsystem	m_loop			; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global core_application
namespace ml::globals
{
	ML_decl_global(core_application) get() noexcept;

	ML_decl_global(core_application) set(core_application * value) noexcept;
}

// EVENTS
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(app_enter_event) {};

	ML_event(app_exit_event) {};

	ML_event(app_idle_event) {};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_CORE_APPLICATION_HPP_