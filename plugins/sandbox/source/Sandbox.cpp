/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/detail/StreamSniper.hpp>
#include <modus_core/client/ClientRuntime.hpp>
#include <modus_core/client/ClientEvents.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/graphics/Font.hpp>
#include <modus_core/graphics/Mesh.hpp>
#include <modus_core/graphics/Shader.hpp>
#include <modus_core/graphics/Renderer.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/client/ImGuiExt.hpp>
#include <modus_core/scene/SceneManager.hpp>
#include <modus_core/scene/Viewport.hpp>
#include <modus_core/window/WindowEvents.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	struct ML_PLUGIN_API sandbox final : plugin
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		vec2 m_resolution{ 1280, 720 };

		shared<gfx::framebuffer> m_fb{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		~sandbox() noexcept override {}

		sandbox(plugin_manager * manager, void * user) noexcept : plugin{ manager, user }
		{
			subscribe<client_enter_event>();
			subscribe<client_exit_event>();
			subscribe<client_idle_event>();
			subscribe<client_dockspace_event>();
			subscribe<client_menubar_event>();
			subscribe<client_gui_event>();
		}

		void on_event(event && ev) override
		{
			switch (ev)
			{
			case client_enter_event		::ID: return on_client_enter	((client_enter_event &&)ev);
			case client_exit_event		::ID: return on_client_exit		((client_exit_event &&)ev);
			case client_idle_event		::ID: return on_client_idle		((client_idle_event &&)ev);
			case client_dockspace_event	::ID: return on_client_dockspace((client_dockspace_event &&)ev);
			case client_menubar_event	::ID: return on_client_menubar	((client_menubar_event &&)ev);
			case client_gui_event		::ID: return on_client_gui		((client_gui_event &&)ev);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_client_enter(client_enter_event && ev)
		{
			if (bitmap const icon{ get_io()->path2("resource/icon.png") })
			{
				get_window()->set_icons(icon.width(), icon.height(), 1, icon.data());
			}

			m_fb = gfx::framebuffer::create({ "0", m_resolution });
		}

		void on_client_exit(client_exit_event && ev)
		{
		}

		void on_client_idle(client_idle_event && ev)
		{
			m_fb->resize(m_resolution);

			get_window()->execute(
				gfx::command::bind_framebuffer(m_fb),
				gfx::command::set_clear_color(colors::magenta),
				gfx::command::clear(gfx::clear_color | gfx::clear_depth),
				gfx::command([&](gfx::render_context * ctx) noexcept
				{
					// custom rendering...
				}),
				gfx::command::bind_framebuffer(nullptr));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_client_dockspace(client_dockspace_event && ev)
		{
			if (auto const root{ ev->begin_builder() })
			{
				ev->dock("viewport", root);
				ev->end_builder(root);
			}
		}

		void on_client_menubar(client_menubar_event && ev)
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

		void on_client_gui(client_gui_event && ev)
		{
			// viewport
			ImGui::SetNextWindowSize({ 540, 480 }, ImGuiCond_Once);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f, 0.f });
			ImGuiExt::DrawWindow("viewport", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar,
			[&]() noexcept {
				ImGui::PopStyleVar(1);
				draw_viewport_contents();
			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void draw_viewport_contents()
		{
			static auto const & modes{ video_mode::fullscreen_modes() };
			static size_t		index{};
			static bool			fixed{};
			video_mode const &	video{ modes[index] };

			if (ImGui::BeginMenuBar())
			{
				// resolution
				char res_label[128]{};
				constexpr auto
					fmt_fa{ "automatic" },
					fmt_vm{ "%i x %i @ %dhz" };
				if (!fixed) std::sprintf(res_label, fmt_fa);
				else std::sprintf(res_label, fmt_vm,
					video.resolution[0],
					video.resolution[1],
					video.refresh_rate);
				ImGui::SetNextItemWidth(160);
				if (ImGui::BeginCombo("##resolution", res_label))
				{
					if (ImGui::Selectable(fmt_fa, !fixed)) { fixed = false; }
					ImGui::Separator();
					for (size_t i = 0; i < modes.size(); ++i)
					{
						std::sprintf(res_label, fmt_vm,
							modes[i].resolution[0],
							modes[i].resolution[1],
							modes[i].refresh_rate);
						if (ImGui::Selectable(res_label, fixed && (i == index)))
						{
							index = i; fixed = true;
						}
					}
					ImGui::EndCombo();
				}
				ImGui::Separator();

				// fps
				auto const fps{ get_io()->fps };
				ImGui::TextDisabled("%.3f ms/frame ( %.1f fps )", 1000.f / fps, fps);
				ImGui::Separator();

				ImGui::EndMenuBar();
			}

			auto const tex{ m_fb->get_color_attachments().front().get() };
			vec2 const dst{ (vec2)ImGui::GetContentRegionAvail() };
			vec2 src{};
			if (!fixed) {
				src = m_resolution = dst;
			} else {
				src = tex->get_data().size;
				m_resolution = util::scale_to_fit((vec2)modes[index].resolution, dst);
			}
			ImGui::Image(tex->get_handle(), util::scale_to_fit(src, dst), { 0, 1 }, { 1, 0 });
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