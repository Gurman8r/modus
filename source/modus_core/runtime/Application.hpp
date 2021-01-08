#ifndef _ML_APPLICATION_HPP_
#define _ML_APPLICATION_HPP_

#include <modus_core/runtime/GuiApplication.hpp>

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
		using gui_application::exec;

		using gui_application::exit;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
	protected:
		using gui_application::on_enter;
		
		using gui_application::on_exit;
		
		using gui_application::on_idle;

		virtual void on_event(event const & value) override;

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