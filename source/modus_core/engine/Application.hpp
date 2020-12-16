#ifndef _ML_APPLICATION_HPP_
#define _ML_APPLICATION_HPP_

#include <modus_core/embed/Python.hpp>
#include <modus_core/engine/GuiApplication.hpp>
#include <modus_core/engine/PluginManager.hpp>
#include <modus_core/scene/Scene.hpp>

namespace ml
{
	// application
	struct ML_CORE_API application : gui_application
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using gui_application::allocator_type;

		application(int32 argc, char * argv[], json const & attributes = {}, allocator_type alloc = {});

		virtual ~application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using gui_application::exec;

		using gui_application::exit;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_plugin_manager() const noexcept -> plugin_manager *
		{
			return const_cast<plugin_manager *>(&m_plugin_manager);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_active_scene() const noexcept -> ds::ref<scene> const &
		{
			return m_active_scene;
		}

		void set_active_scene(ds::ref<scene> const & value) noexcept
		{
			if (m_active_scene != value)
			{
				m_active_scene = value;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
			ML_assert(!app_file_name().empty() && !library_paths().empty());
			return m_interpreter.initialize(app_file_name(), library_paths(0));
		}

		bool finalize_interpreter() noexcept
		{
			return m_interpreter.finalize();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		plugin_manager		m_plugin_manager; // plugin manager
		python_interpreter	m_interpreter	; // python interpreter
		ds::ref<scene>		m_active_scene	; // active scene

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global application
namespace ml::globals
{
	ML_decl_global(application) get() noexcept;

	ML_decl_global(application) set(application * value) noexcept;
}

#endif // !_ML_APPLICATION_HPP_