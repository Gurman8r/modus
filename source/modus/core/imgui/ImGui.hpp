#ifndef _ML_IMGUI_HPP_
#define _ML_IMGUI_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <core/system/Memory.hpp>
#include <core/detail/Color.hpp>

#ifndef IM_VEC2_CLASS_EXTRA
#define IM_VEC2_CLASS_EXTRA								\
	constexpr ImVec2(_ML vec2 const & v) noexcept		\
		: x{ v[0] }, y{ v[1] }							\
	{}													\
	constexpr operator _ML vec2() const noexcept {		\
		return { x, y };								\
	}
#endif

#ifndef IM_VEC4_CLASS_EXTRA
#define IM_VEC4_CLASS_EXTRA								\
	constexpr ImVec4(_ML vec4 const & v) noexcept		\
		: x{ v[0] }, y{ v[1] }, z{ v[2] }, w{ v[3] }	\
	{}													\
	constexpr ImVec4(_ML color const & c) noexcept		\
		: x{ c[0] }, y{ c[1] }, z{ c[2] }, w{ c[3] }	\
	{}													\
	constexpr operator _ML vec4() const noexcept {		\
		return { x, y, z, w };							\
	}
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <ImGuizmo/ImGuizmo.h>
#include <ImGuizmo/ImSequencer.h>
#include <ImGuizmo/ImCurveEdit.h>
#include <ImGuiColorTextEdit/TextEditor.h>
#include <imgui_club/imgui_memory_editor/imgui_memory_editor.h>
#include <imgui-node-editor/NodeEditor/Include/imgui_node_editor.h>

namespace ml
{
	template <> struct default_delete<ax::NodeEditor::EditorContext>
	{
		void operator()(ax::NodeEditor::EditorContext * ptr)
		{
			ax::NodeEditor::DestroyEditor(ptr);
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// ImGui::PushID(...); ML_defer() { ImGui::PopID(); };
#define ML_ImGui_ScopeID(...) \
	auto ML_anon = _ML impl::imgui_scope_id{ ##__VA_ARGS__ }

namespace ml::impl
{
	struct ML_NODISCARD imgui_scope_id final
	{
		template <class ... Args
		> imgui_scope_id(Args && ... args) noexcept { ImGui::PushID(ML_forward(args)...); }

		~imgui_scope_id() noexcept { ImGui::PopID(); }
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml::ImGuiExt
{
	// tooltip ex
	template <class ... Args
	> void TooltipEx(Args && ... args) noexcept
	{
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			std::invoke(ML_forward(args)...);
			ImGui::EndTooltip();
		}
	}

	// tooltip
	inline void Tooltip(cstring first, cstring last = nullptr) noexcept
	{
		TooltipEx([first, last]() noexcept
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(first, last);
			ImGui::PopTextWrapPos();
		});
	}

	// tooltip fmt
	inline void TooltipFmt(cstring fmt, ...) noexcept
	{
		char buf[1024] = "";
		va_list args;
		va_start(args, fmt);
		std::vsnprintf(buf, ML_arraysize(buf), fmt, args);
		va_end(args);
		Tooltip(buf);
	}

	// help marker ex
	template <class ... Args
	> void HelpMarkerEx(Args && ... args) noexcept
	{
		ImGui::TextDisabled("(?)");
		TooltipEx(ML_forward(args)...);
	}

	// help marker
	inline void HelpMarker(cstring first, cstring last = nullptr) noexcept
	{
		ImGui::TextDisabled("(?)");
		Tooltip(first, last);
	}

	// help marker
	inline void HelpMarkerFmt(cstring fmt, ...) noexcept
	{
		char buf[1024] = "";
		va_list args;
		va_start(args, fmt);
		std::vsnprintf(buf, ML_arraysize(buf), fmt, args);
		va_end(args);
		Tooltip(buf);
	}
}

namespace ml
{
	// form
	struct ML_NODISCARD gui_form final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		cstring		title	{ "New Form" };
		bool		open	{ true };
		int32_t		flags	{ ImGuiWindowFlags_None };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn, class ... Args
		> void render(Fn && fn, Args && ... args) noexcept
		{
			if (!open) { return; }
			ML_ImGui_ScopeID(ImGui::GetID(title));
			ML_defer() { ImGui::End(); };
			if (ImGui::Begin(title, &open, flags))
			{
				std::invoke(ML_forward(fn), ML_forward(args)...);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		gui_form & focus() noexcept
		{
			open = true;
			ImGui::SetWindowFocus(title);
			return (*this);
		}

		uint32_t getid() const noexcept
		{
			return ImGui::GetID(title);
		}

		bool menu_item(cstring shortcut = "") noexcept
		{
			return ImGui::MenuItem(title, shortcut, &open);
		}

		bool selectable(int32_t f = ImGuiSelectableFlags_None, vec2 const & size = {}) noexcept
		{
			return ImGui::Selectable(title, &open, f, size);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// plot
	struct ML_NODISCARD gui_plot final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		enum : int32_t { lines, histogram };

		using buffer_t	= typename pmr::vector<float_t>;
		using overtxt_t	= typename ds::array<char, 32>;
		using get_str_t = typename std::function<cstring()>;
		using get_fn_t	= typename std::function<float_t()>;

		buffer_t	buffer		{};
		int32_t		mode		{};
		cstring		label		{};
		get_str_t	get_fmt		{ []() noexcept { return "%f"; } };
		get_fn_t	get_value	{ []() noexcept { return 0.f; } };
		vec2		size		{};
		vec2		scale		{ FLT_MAX, FLT_MAX };
		int32_t		offset		{};
		char		overtxt[32]	{};
		bool		animate		{ true };
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// make gui_plot
		template <class ... Args
		> static auto create(size_t const cap, Args && ... args) noexcept
		{
			return gui_plot{ buffer_t{ cap, buffer_t::allocator_type{} }, ML_forward(args)... };
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void update() noexcept
		{
			ML_assert(get_value);
			update(std::invoke(get_value));
		}

		template <class Delta = float_t
		> void update(Delta const v) noexcept
		{
			static_assert(std::is_floating_point_v<Delta>);
			if (!animate || buffer.empty()) { return; }
			std::sprintf(overtxt, get_fmt(), v);
			buffer[offset] = static_cast<float_t>(v);
			offset = (offset + 1) % buffer.size();
		}

		void render() const noexcept
		{
			ML_ImGui_ScopeID(this);

			// expand to available width
			float_t width{ size[0] };
			if ((width == 0.f) && (label && label[0] == '#' && label[1] == '#'))
			{
				width = ImGui::GetContentRegionAvailWidth();
			}

			// draw
			switch (mode)
			{
			case lines:
				return ImGui::PlotLines(label
					, buffer.data(), (int32_t)buffer.size(), offset
					, overtxt
					, scale[0], scale[1], { width, size[1] }
					, sizeof(float_t)
				);
			case histogram:
				return ImGui::PlotHistogram(label
					, buffer.data(), (int32_t)buffer.size(), offset
					, overtxt
					, scale[0], scale[1], { width, size[1] }
					, sizeof(float_t)
				);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// plot controller
	struct ML_NODISCARD gui_plot_controller final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		pmr::vector<gui_plot> plots{};
		
		float_t ref_time{};

		gui_plot_controller(std::initializer_list<gui_plot> init) noexcept
			: plots{ init.begin(), init.end() }, ref_time{}
		{
		}

		template <class Delta = float_t
		> void update(Delta const tt, Delta const dt = (Delta)(1.f / 60.f)) noexcept
		{
			if (ref_time == 0.f)
			{
				ref_time = static_cast<float_t>(tt);
				return;
			}
			while (ref_time < static_cast<float_t>(tt))
			{
				for (auto & p : plots) { p.update(); }

				ref_time += static_cast<float_t>(dt);
			}
		}

		auto begin() noexcept { return plots.begin(); }
		auto begin() const noexcept { return plots.begin(); }
		auto cbegin() const noexcept { return plots.cbegin(); }
		auto end() noexcept { return plots.end(); }
		auto end() const noexcept { return plots.end(); }
		auto cend() const noexcept { return plots.cend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// TEXTURE PREVIEW
namespace ml
{
	struct ML_NODISCARD gui_texture_preview final
	{
		void *	tex_addr	{ nullptr };
		vec2i	tex_size	{};
		vec2	img_size	{};
		float_t	reg_zoom	{ 4.f };
		float_t	reg_size	{ 32.f };

		void render() noexcept
		{
			if (!tex_addr) { return; }

			auto const & io		{ ImGui::GetIO() };
			auto const reg_avail{ ImGui::GetContentRegionAvail() };
			auto const scr_pos	{ ImGui::GetCursorScreenPos() };
			auto const scr_size	{ util::scale_to_fit((vec2)tex_size,
			{
				img_size[0] == 0.f ? reg_avail[0] : img_size[0],
				img_size[1] == 0.f ? reg_avail[1] : img_size[1]
			}) };

			ImGui::Image(
				tex_addr,
				scr_size,
				{ 0, 1 },
				{ 1, 0 },
				colors::white,
				colors::gray
			);

			// zoom tooltip region
			if ((0.f < reg_size) && (0.f < reg_zoom) && ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip(); ML_defer() { ImGui::EndTooltip(); };
				
				float_t rx{ io.MousePos[0] - scr_pos[0] - reg_size * .5f };
				if (rx < 0.f) { rx = 0.f; }
				else if (rx > scr_size[0] - reg_size) { rx = (scr_size[0] - reg_size); }

				float_t ry{ io.MousePos[1] - scr_pos[1] - reg_size * .5f };
				if (ry < 0.f) { ry = 0.f; }
				else if (ry > scr_size[1] - reg_size) { ry = (scr_size[1] - reg_size); }

				ImGui::Text("%u: %dx%d (%.0fx%.0f)",
					(uint32_t)(intptr_t)tex_addr,
					tex_size[0], tex_size[1],
					scr_size[0], scr_size[1]
				);
				ImGui::Text("Min: (%.2f, %.2f)", rx, ry);
				ImGui::Text("Max: (%.2f, %.2f)", rx + reg_size, ry + reg_size);
				ImGui::Image(
					tex_addr,
					{ reg_size * reg_zoom, reg_size * reg_zoom },
					{
						(rx / scr_size[0]),
						(1.f - (ry / scr_size[1]))
					},
					{
						((rx + reg_size) / scr_size[0]),
						(1.f - ((ry + reg_size) / scr_size[1]))
					},
					colors::white,
					colors::gray
				);
			}
		}
	};
}

// CONSOLE
namespace ml
{
	struct ML_NODISCARD gui_console final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type			= typename _ML gui_console;
		using allocator_type	= typename pmr::polymorphic_allocator<byte_t>;
		using command_name		= typename pmr::string;
		using command_args		= typename pmr::vector<pmr::string>;
		using command_clbk		= typename std::function<void(command_args)>;
		using command_info		= typename pmr::vector<pmr::string>;

		struct ML_NODISCARD command_data final
		{
			command_name name; // name
			command_clbk clbk; // callback
			command_info info; // information

			bool operator==(command_data const & other) const noexcept
			{
				return (this == std::addressof(other)) || (name == other.name);
			}

			bool operator<(command_data const & other) const noexcept
			{
				return (this != std::addressof(other)) && (name < other.name);
			}
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		gui_console(allocator_type alloc = {}) noexcept
			: commands	{ alloc }
			, history	{ alloc }
			, lines		{ alloc }
		{
		}

		pmr::vector<command_data>	commands		{}		; // commands
		pmr::vector<pmr::string>	history			{}		; // history buffer
		pmr::vector<pmr::string>	lines			{}		; // text buffer

		ds::array<char, 256>		input			{}		; // input buffer
		int32_t						history_pos		{ -1 }	; // history index
		ImGuiTextFilter				filter			{}		; // text filter
		bool						auto_scroll		{ true }; // auto-scroll
		bool						scroll_to_bot	{}		; // scroll-to-bottom
		cstring						cmd_lock		{}		; // forced command prefix

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool lock(cstring value) noexcept
		{
			return value && !cmd_lock && (cmd_lock = value);
		}

		bool unlock(cstring value) noexcept
		{
			return value && cmd_lock && (0 == std::strcmp(cmd_lock, value));
		}

		self_type & clear() noexcept
		{
			lines.clear();
			return (*this);
		}

		self_type & write(char const value) noexcept
		{
			switch (value)
			{
			default:
				if (lines.empty()) { lines.push_back({}); }
				lines.back().push_back(value);
				break;
			case '\n':
				lines.push_back({});
				break;
			}
			return (*this);
		}

		self_type & dump(pmr::stringstream & ss)
		{
			for (char c : ss.str())
			{
				this->write(c);
			}
			ss.str({});
			return (*this);
		}

		self_type & print(pmr::string const & value) noexcept
		{
			for (char c : value)
			{
				this->write(c);
			}
			return (*this);
		}

		self_type & printl(pmr::string const & value = {}) noexcept
		{
			return this->print(value).write('\n');
		}

		self_type & printf(cstring fmt, ...) noexcept
		{
			ds::array<char, 1024> buf{};
			va_list args{};
			va_start(args, fmt);
			std::vsnprintf(buf.data(), buf.size(), fmt, args);
			buf.back() = 0;
			va_end(args);
			return this->print(buf.data());
		}

		self_type & execute(cstring value) noexcept
		{
			this->printf("# %s\n", value);

			// update history
			history_pos = -1;
			if (auto const it{ std::find(history.begin(), history.end(), value) }
			; it != history.end())
			{
				history.erase(it);
			}
			history.push_back(value);

			// process command
			if (auto toks{ util::tokenize(value, " ") }; !toks.empty())
			{
				if (auto const it{ std::find_if(commands.begin(), commands.end(), [&
				](auto & cmd) noexcept
				{
					return cmd.name == (cmd_lock ? cmd_lock : toks.front());
				}) }
				; it != commands.end())
				{
					if (!cmd_lock) { toks.erase(toks.begin()); }

					std::invoke(it->clbk, std::move(toks));
				}
				else
				{
					this->printf("unknown command: \'%s\'\n", toks.front().c_str());
				}
			}

			scroll_to_bot = true;
			return (*this);
		}

		self_type & render()
		{
			ML_ImGui_ScopeID(this);

			// HEADER
			filter.Draw("filter", 180); ImGui::SameLine();
			ImGui::Checkbox("auto-scroll", &auto_scroll); ImGui::SameLine();
			if (ImGui::Button("clear")) clear(); ImGui::SameLine();
			ImGui::Text("lock: %s", cmd_lock ? cmd_lock : "-"); //ImGui::SameLine();
			ImGui::Separator();

			float_t const footer_height{ ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() };

			// CONTENT
			ImGui::BeginChild("console content area",
				{ 0, -footer_height }, false, ImGuiWindowFlags_HorizontalScrollbar
			);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 1 });
			for (auto const & line : lines)
			{
				auto const str{ line.c_str() };
				if (!filter.PassFilter(str)) continue;

				bool pop_color{};
				if (0 == std::strncmp(str, "# ", 2))
				{
					// # (orange)
					ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.8f, 0.6f, 1.0f }); pop_color = true;
				}
				else if (std::strstr(str, ML_IMPL_DEBUG_INFO))
				{
					// [ info ] (green)
					ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f, 1.0f, 0.0f, 1.0f }); pop_color = true;
				}
				else if (std::strstr(str, ML_IMPL_DEBUG_ERROR))
				{
					// [ error ] (red)
					ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.2f, 0.4f, 1.0f }); pop_color = true;
				}
				else if (std::strstr(str, ML_IMPL_DEBUG_WARNING))
				{
					// [ warning ] (yellow)
					ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.75f, 0.0f, 1.0f }); pop_color = true;
				}
				ImGui::TextUnformatted(str);
				if (pop_color) { ImGui::PopStyleColor(); }
			}
			if (scroll_to_bot || (auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			{
				ImGui::SetScrollHereY(1.0f);
			}
			scroll_to_bot = false;
			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::Separator();

			// COMMAND LINE
			bool reclaim_focus{};
			ImGui::TextDisabled("%s:~$", cmd_lock ? cmd_lock : ""); ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
			if (ImGui::InputText("##input", &input[0], ML_arraysize(input),
				ImGuiInputTextFlags_EnterReturnsTrue |
				ImGuiInputTextFlags_CallbackCompletion |
				ImGuiInputTextFlags_CallbackHistory,
				[](auto * u) { return ((gui_console *)u->UserData)->text_edit_callback(u); },
				this
			))
			{
				if (auto const s{ util::trim((pmr::string)input.data()) }; !s.empty())
				{
					execute(s.c_str());
				}
				std::strcpy(input.data(), "");
				reclaim_focus = true;
			}
			ImGui::PopItemWidth();

			// Auto-focus on window apparition
			ImGui::SetItemDefaultFocus();
			if (reclaim_focus)
			{
				ImGui::SetKeyboardFocusHere(-1); // Auto focus previous window
			}

			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		int32_t text_edit_callback(ImGuiInputTextCallbackData * data)
		{
			switch (data->EventFlag)
			{
			case ImGuiInputTextFlags_CallbackCompletion:
			{
				// TEXT COMPLETION

				// Locate beginning of current word
				cstring word_end = data->Buf + data->CursorPos;
				cstring word_start = word_end;
				while (word_start > data->Buf)
				{
					char const c = word_start[-1];
					if (c == ' ' || c == '\t' || c == ',' || c == ';')
						break;
					word_start--;
				}

				// Build a list of candidates
				pmr::vector<cstring> candidates;
				for (size_t i = 0; i < commands.size(); i++)
					if (std::strncmp(commands[i].name.c_str(), word_start, (size_t)(word_end - word_start)) == 0)
						candidates.push_back(commands[i].name.c_str());

				if (candidates.size() == 0)
				{
					// No match
					this->printf("No match for \'%.*s\'!\n", (size_t)(word_end - word_start), word_start);
				}
				else if (candidates.size() == 1)
				{
					// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
					data->DeleteChars((int32_t)(word_start - data->Buf), (int32_t)(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0]);
					data->InsertChars(data->CursorPos, " ");
				}
				else
				{
					// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
					size_t match_len = (size_t)(word_end - word_start);
					for (;;)
					{
						size_t c = 0;
						bool all_candidates_matches = true;
						for (size_t i = 0; i < candidates.size() && all_candidates_matches; i++)
							if (i == 0)
								c = toupper(candidates[i][match_len]);
							else if (c == 0 || c != toupper(candidates[i][match_len]))
								all_candidates_matches = false;
						if (!all_candidates_matches)
							break;
						match_len++;
					}

					if (match_len > 0)
					{
						data->DeleteChars((int32_t)(word_start - data->Buf), (int32_t)(word_end - word_start));
						data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
					}

					// List matches
					this->printf("Possible matches:\n");
					for (size_t i = 0; i < candidates.size(); i++)
						this->printf("- %s\n", candidates[i]);
				}

				break;
			}
			case ImGuiInputTextFlags_CallbackHistory:
			{
				// HISTORY
				const size_t prev_history_pos = history_pos;
				if (data->EventKey == ImGuiKey_UpArrow)
				{
					if (history_pos == -1)
						history_pos = (int32_t)history.size() - 1;
					else if (history_pos > 0)
						history_pos--;
				}
				else if (data->EventKey == ImGuiKey_DownArrow)
				{
					if (history_pos != -1)
						if (++history_pos >= (int32_t)history.size())
							history_pos = -1;
				}

				if (prev_history_pos != history_pos)
				{
					cstring history_str = (history_pos >= 0) ? history[history_pos].c_str() : "";
					data->DeleteChars(0, data->BufTextLen);
					data->InsertChars(0, history_str);
				}
			}
			}
			return 0;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_IMGUI_HPP_