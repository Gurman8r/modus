#ifndef _ML_CORE_APPLICATION_HPP_
#define _ML_CORE_APPLICATION_HPP_

// WIP

#include <modus_core/engine/Object.hpp>
#include <modus_core/detail/LoopSystem.hpp>

// CORE APP
namespace ml
{
	// core application
	struct ML_CORE_API core_application : core_object, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		explicit core_application(int32_t argc, char * argv[], allocator_type alloc = {});

		virtual ~core_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
	public:
		ML_NODISCARD auto get_app_filename() const noexcept -> fs::path const & { return m_app_file_name; }

		ML_NODISCARD auto get_app_file_path() const noexcept -> fs::path const & { return m_app_file_path; }

		ML_NODISCARD auto get_app_name() noexcept -> ds::string const & { return m_app_name; }

		ML_NODISCARD auto get_app_version() noexcept -> ds::string const & { return m_app_version; }

		ML_NODISCARD auto get_arguments() const noexcept -> ds::list<ds::string> const & { return m_arguments; }

		ML_NODISCARD auto get_library_paths() const noexcept -> ds::list<fs::path> const & { return m_lib_paths; }

		void set_app_name(ds::string const & value) noexcept { m_app_name = value; }

		void set_app_version(ds::string const & value) noexcept { m_app_version = value; }

		void set_library_paths(ds::list<fs::path> const & value) noexcept { m_lib_paths = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD int32_t exec();

		void exit(int32_t exit_code);

		void quit();

		ML_NODISCARD auto get_main_loop() const noexcept -> ds::ref<loop_system> const & { return m_loop; }

		ML_NODISCARD auto get_time() const noexcept -> duration { return m_uptimer.elapsed(); }

		void set_main_loop(ds::ref<loop_system> const & value) noexcept { m_loop = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using core_object::get_bus;

		template <class Ev, class ... Args
		> void fire_event(Args && ... args) noexcept
		{
			m_dispatcher.fire<Ev>(ML_forward(args)...);
		}
		
		template <class Ev, class ... Args
		> void post_event(Args && ... args) noexcept
		{
			m_dispatcher.post<Ev>(ML_forward(args)...);
		}

		void process_events() noexcept
		{
			m_dispatcher.process_events();
		}

	protected:
		using core_object::subscribe;

		using core_object::unsubscribe;

		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		fs::path				m_app_file_name	; // 
		fs::path				m_app_file_path	; // 
		ds::string				m_app_name		; // 
		ds::string				m_app_version	; // 
		ds::list<ds::string>	m_arguments		; // 
		ds::list<fs::path>		m_lib_paths		; // 
		
		timer					m_uptimer		; // 
		int32_t					m_exit_code		; // 
		ds::ref<loop_system>	m_loop			; // 
		event_bus				m_dispatcher	; // 

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