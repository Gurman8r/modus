#ifndef _ML_CORE_APPLICATION_HPP_
#define _ML_CORE_APPLICATION_HPP_

#include <modus_core/detail/Layers.hpp>
#include <modus_core/detail/LoopSystem.hpp>

namespace ml
{
	// core application
	struct ML_CORE_API core_application : non_copyable, trackable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		explicit core_application(int32 argc, char * argv[], allocator_type alloc = {});

		virtual ~core_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		int32 exec() noexcept
		{
			ML_check(m_main_loop)->process();

			return m_exit_code;
		}

		void exit(int32 value) noexcept
		{
			ML_check(m_main_loop)->halt();

			m_exit_code = value;
		}

		void quit() noexcept
		{
			this->exit(EXIT_SUCCESS);
		}

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
			return m_attributes;
		}

		ML_NODISCARD auto attr() const noexcept -> json const &
		{
			return m_attributes;
		}

		template <class I> ML_NODISCARD auto attr(I && i) noexcept -> json &
		{
			return m_attributes[ML_forward(i)];
		}

		template <class I> ML_NODISCARD auto attr(I && i) const noexcept -> json const &
		{
			return m_attributes[ML_forward(i)];
		}

		ML_NODISCARD auto library_paths() const noexcept -> ds::list<fs::path> const &
		{
			return m_library_paths;
		}

		ML_NODISCARD auto library_paths(size_t i) const noexcept -> fs::path const &
		{
			return m_library_paths[i];
		}

		ML_NODISCARD auto path_to(fs::path const & value) const noexcept -> fs::path
		{
			return m_library_paths.empty()
				? value
				: m_library_paths.front().native() + value.native();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto set_app_name(ds::string const & value) noexcept -> ds::string &
		{
			return m_app_name = value;
		}

		auto set_app_version(ds::string const & value) noexcept -> ds::string &
		{
			return m_app_version = value;
		}

		auto set_attributes(json const & value) noexcept -> json &
		{
			return m_attributes = value;
		}

		auto set_library_paths(ds::list<fs::path> const & value) noexcept -> ds::list<fs::path> &
		{
			return m_library_paths = value;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_main_loop() const noexcept -> ds::ref<loop_system> const &
		{
			return m_main_loop;
		}

		auto set_main_loop(ds::ref<loop_system> const & value) noexcept -> ds::ref<loop_system> &
		{
			ML_assert(!m_main_loop || !m_main_loop->running());
			return m_main_loop = value;
		}

		template <class Loop = loop_system, class ... Args
		> auto new_main_loop(Args && ... args) noexcept -> ds::ref<Loop>
		{
			static_assert(std::is_base_of_v<loop_system, Loop>);

			return std::static_pointer_cast<Loop>
			(
				set_main_loop(_ML make_ref<Loop>(ML_forward(args)...))
			);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		ML_NODISCARD bool initialize_interpreter();

		void finalize_interpreter();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		fs::path				m_app_file_name	; // 
		fs::path				m_app_file_path	; // 
		ds::string				m_app_name		; // 
		ds::string				m_app_version	; // 
		ds::list<ds::string>	m_arguments		; // 
		json					m_attributes	; // 
		ds::list<fs::path>		m_library_paths	; // 

		int32					m_exit_code		; // 
		event_bus				m_dispatcher	; // 
		ds::ref<loop_system>	m_main_loop		; // 
		

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