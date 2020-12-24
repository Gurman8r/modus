#ifndef _ML_CORE_APPLICATION_HPP_
#define _ML_CORE_APPLICATION_HPP_

#include <modus_core/embed/Python.hpp>
#include <modus_core/system/SharedLibrary.hpp>

namespace ml
{
	// core application
	struct ML_CORE_API core_application : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		core_application(int32 argc, char * argv[], json const & j = {}, allocator_type alloc = {});

		virtual ~core_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual int32 exec() { return m_exit_code; }

		virtual void exit(int32 value) { m_exit_code = value; }

		void quit() noexcept { this->exit(EXIT_SUCCESS); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto app_data_path() const noexcept -> fs::path const &
		{
			return m_app_data_path;
		}

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

		ML_NODISCARD auto library_paths() const noexcept -> ds::list<fs::path> const &
		{
			return m_library_paths;
		}

		ML_NODISCARD auto library_paths(size_t i) const noexcept -> fs::path const &
		{
			ML_assert(i < m_library_paths.size());

			return m_library_paths[i];
		}

		ML_NODISCARD auto path_to(fs::path const & value) const noexcept -> fs::path
		{
			return m_app_data_path.native() + value.native();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto attr() noexcept -> json &
		{
			return m_attributes;
		}

		ML_NODISCARD auto attr() const noexcept -> json const &
		{
			return m_attributes;
		}

		template <class Arg0, class ... Args
		> ML_NODISCARD auto attr(Arg0 && arg0, Args && ... args) noexcept -> json &
		{
			return *ML_check(util::json_ptr(&m_attributes, ML_forward(arg0), ML_forward(args)...));
		}

		template <class Arg0, class ... Args
		> ML_NODISCARD auto attr(Arg0 && arg0, Args && ... args) const noexcept -> json const &
		{
			return *ML_check(util::json_ptr(&m_attributes, ML_forward(arg0), ML_forward(args)...));
		}

		template <class Arg0, class ... Args
		> ML_NODISCARD bool has_attr(Arg0 && arg0, Args && ... args) const noexcept
		{
			return util::has_json(m_attributes, ML_forward(arg0), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_interpreter() const noexcept -> python_interpreter *
		{
			return const_cast<python_interpreter *>(&m_interpreter);
		}

		ML_NODISCARD bool has_interpreter() const noexcept
		{
			return m_interpreter.is_initialized();
		}

		bool initialize_interpreter() noexcept
		{
			if (m_library_paths.empty()) { return debug::failure("library paths are empty"); }

			return m_interpreter.initialize(m_app_file_name, m_library_paths[0]);
		}

		bool finalize_interpreter() noexcept
		{
			return m_interpreter.finalize();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		int32					m_exit_code		; // exit code
		fs::path				m_app_data_path	; // app data path
		fs::path				m_app_file_name	; // app file name
		fs::path				m_app_file_path	; // app file path
		ds::string				m_app_name		; // app name
		ds::string				m_app_version	; // app version
		ds::list<ds::string>	m_arguments		; // arguments
		ds::list<fs::path>		m_library_paths	; // library paths
		json					m_attributes	; // attributes
		python_interpreter		m_interpreter	; // python interpreter
		
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