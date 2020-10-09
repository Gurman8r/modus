/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <core/client/Blackboard.hpp>
#include <core/detail/StreamSniper.hpp>
#include <core/client/PluginManager.hpp>
#include <core/client/ClientEvents.hpp>
#include <core/embed/Python.hpp>
#include <core/graphics/Font.hpp>
#include <core/graphics/Mesh.hpp>
#include <core/graphics/Shader.hpp>
#include <core/graphics/Renderer.hpp>
#include <core/graphics/RenderWindow.hpp>
#include <core/imgui/ImGuiContext.hpp>
#include <core/imgui/ImGuiEvents.hpp>
#include <core/imgui/ImGui.hpp>
#include <core/scene/SceneManager.hpp>
#include <core/window/WindowEvents.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	struct ML_PLUGIN_API sandbox final : plugin
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		blackboard::var< ds::map<pmr::string, shared<font>>			> m_fonts		; // 
		blackboard::var< ds::map<pmr::string, shared<bitmap>>		> m_images		; // 
		blackboard::var< ds::map<pmr::string, shared<mesh>>			> m_meshes		; // 
		blackboard::var< ds::map<pmr::string, shared<gfx::shader>>	> m_shaders		; // 
		blackboard::var< ds::map<pmr::string, shared<gfx::texture>>	> m_textures	; // 

		gui_form m_viewport{ "viewport", true };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		~sandbox() noexcept override {}

		sandbox(plugin_manager * manager, void * user) noexcept : plugin{ manager, user }
			, m_fonts	{ get_vars(), "fonts" }
			, m_images	{ get_vars(), "images" }
			, m_meshes	{ get_vars(), "meshes" }
			, m_shaders	{ get_vars(), "shaders" }
			, m_textures{ get_vars(), "textures" }
		{
			subscribe<client_enter_event>();
			subscribe<client_exit_event>();
			subscribe<client_update_event>();

			subscribe<imgui_dockspace_event>();
			subscribe<imgui_menubar_event>();
			subscribe<imgui_render_event>();

			subscribe<window_key_event>();
			subscribe<window_mouse_event>();
			subscribe<window_cursor_position_event>();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_event(event && value) override
		{
			switch (value)
			{
			case client_enter_event				::ID: return on_client_enter((client_enter_event &&)value);
			case client_exit_event				::ID: return on_client_exit((client_exit_event &&)value);
			case client_update_event			::ID: return on_client_update((client_update_event &&)value);
			
			case imgui_dockspace_event			::ID: return on_imgui_dockspace((imgui_dockspace_event &&)value);
			case imgui_menubar_event			::ID: return on_imgui_menubar((imgui_menubar_event &&)value);
			case imgui_render_event				::ID: return on_imgui_render((imgui_render_event &&)value);
			
			case window_key_event				::ID: return on_window_key((window_key_event &&)value);
			case window_mouse_event				::ID: return on_window_mouse((window_mouse_event &&)value);
			case window_cursor_position_event	::ID: return on_window_cursor_position((window_cursor_position_event &&)value);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_client_enter(client_enter_event && ev)
		{
			// set icon
			if (auto & i = m_images["icon"] = get_memory()->make_ref<bitmap>
			(
				get_io()->path2("resource/icon.png")
			))
			{
				get_window()->set_icons(i->width(), i->height(), 1, i->data());
			}
		}

		void on_client_exit(client_exit_event && ev)
		{
		}

		void on_client_update(client_update_event && ev)
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_imgui_dockspace(imgui_dockspace_event && ev)
		{
		}

		void on_imgui_menubar(imgui_menubar_event && ev)
		{
			if (ImGui::BeginMenu("file"))
			{
				if (ImGui::MenuItem("quit", "alt+f4"))
				{
					get_window()->set_should_close(true);
				}
				ImGui::EndMenu();
			}
		}

		void on_imgui_render(imgui_render_event && ev)
		{
			m_viewport.render([&]()
			{

			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_window_key(window_key_event && ev)
		{
			get_io()->keyboard[ev.key] = ev.action;
		}

		void on_window_mouse(window_mouse_event && ev)
		{
		}

		void on_window_cursor_position(window_cursor_position_event && ev)
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern "C"
{
	ML_PLUGIN_API ml::plugin * ml_plugin_attach(ml::plugin_manager * manager, void * user)
	{
		return manager->get_memory()->new_object<ml::sandbox>(manager, user);
	}

	ML_PLUGIN_API void ml_plugin_detach(ml::plugin_manager * manager, ml::plugin * ptr)
	{
		manager->get_memory()->delete_object((ml::sandbox *)ptr);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */