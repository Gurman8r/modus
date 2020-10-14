#include <modus_core/scene/SceneManager.hpp>

namespace ml
{
	scene_manager::scene_manager(memory_manager * mem, event_bus * bus, render_window * win, imgui_runtime * gui)
		: event_listener{ bus }
	{
	}

	void scene_manager::on_event(event && value)
	{
	}
}