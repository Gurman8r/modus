#ifndef _ML_SCENE_MANAGER_HPP_
#define _ML_SCENE_MANAGER_HPP_

#include <modus_core/scene/SceneTree.hpp>
#include <modus_core/system/Events.hpp>
#include <modus_core/graphics/RenderWindow.hpp>

namespace ml
{
	struct ML_CORE_API scene_manager final : event_listener
	{
		scene_manager(memory_manager * mem, event_bus * bus, render_window * win);

		void on_event(event && value) override;
	};
}

#endif // !_ML_SCENE_MANAGER_HPP_