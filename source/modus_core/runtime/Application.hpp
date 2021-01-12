#ifndef _ML_APPLICATION_HPP_
#define _ML_APPLICATION_HPP_

#include <modus_core/runtime/GuiApplication.hpp>
#include <modus_core/scene/SceneTree.hpp>

namespace ml
{
	// application
	struct ML_CORE_API application : gui_application
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using gui_application::allocator_type;

		application(int32 argc, char * argv[], json const & argj = {}, allocator_type alloc = {});

		virtual ~application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_active_scene() const noexcept -> ref<scene_tree> const & { return m_active_scene; }

		void set_active_scene(ref<scene_tree> const & value) noexcept { m_active_scene = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ref<scene_tree> m_active_scene;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global application
namespace ml::globals
{
	ML_decl_global(application) get_global();

	ML_decl_global(application) set_global(application *);
}

#endif // !_ML_APPLICATION_HPP_