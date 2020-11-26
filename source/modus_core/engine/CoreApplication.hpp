#ifndef _ML_CORE_APPLICATION_HPP_
#define _ML_CORE_APPLICATION_HPP_

#include <modus_core/detail/Memory.hpp>

namespace ml
{
	// core application
	struct ML_CORE_API core_application : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		explicit core_application(int32 argc, char * argv[], allocator_type alloc = {});

		virtual ~core_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual int32 exec()
		{
			return m_exit_code;
		}

		virtual void exit(int32 value)
		{
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

		void set_app_name(ds::string const & value) noexcept
		{
			if (m_app_name != value) {
				m_app_name = value;
			}
		}

		void set_app_version(ds::string const & value) noexcept
		{
			if (m_app_version != value) {
				m_app_version = value;
			}
		}

		void set_attributes(json const & value) noexcept
		{
			if (m_attributes != value) {
				m_attributes = value;
			}
		}

		void set_library_paths(ds::list<fs::path> const & value) noexcept
		{
			if (m_library_paths != value) {
				m_library_paths = value;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		ML_NODISCARD bool initialize_interpreter();

		void finalize_interpreter();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		int32					m_exit_code		; // 
		fs::path				m_app_file_name	; // 
		fs::path				m_app_file_path	; // 
		ds::string				m_app_name		; // 
		ds::string				m_app_version	; // 
		ds::list<ds::string>	m_arguments		; // 
		json					m_attributes	; // 
		ds::list<fs::path>		m_library_paths	; // 
		
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