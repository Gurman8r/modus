#ifndef _ML_TERMINAL_HPP_
#define _ML_TERMINAL_HPP_

#include <modus_core/gui/TextLog.hpp>

namespace ml::ImGuiExt
{
	// TERMINAL
	struct ML_NODISCARD Terminal final : non_copyable, trackable
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		struct ML_NODISCARD CommandData final
		{
			ds::string name;
			
			ds::list<ds::string> info;
			
			ds::method<void(ds::string &&)> proc;
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ds::string UserName, HostName, PathName, ModeName; // info

		ds::array<char, 256> Input; // input

		TextLog Output; // output

		ds::list<CommandData> Cmd; // commands

		ds::list<ds::string> History; // history

		int32 HistoryPos; // history pos

		// colors
		struct ML_NODISCARD {
			color
				Delimeter	{ colors::white },
				UserName	{ colors::aqua },
				HostName	{ colors::magenta },
				PathName	{ colors::cyan },
				ModeName	{ colors::fuchsia };
		} Colors;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		Terminal(allocator_type alloc = {}) noexcept
			: UserName		{ alloc }
			, HostName		{ alloc }
			, PathName		{ alloc }
			, ModeName		{ alloc }
			, Input			{}
			, Output		{ alloc }
			, Cmd			{ alloc }
			, History		{ alloc }
			, HistoryPos	{ -1 }
			, Colors		{}
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void DrawPrefixOptions()
		{
			// USER
			char username[32]{}; std::strcpy(username, UserName.c_str());
			ImGui::TextDisabled("user"); ImGui::SameLine();
			if (ImGui::InputText("##username", username, ML_arraysize(username), ImGuiInputTextFlags_EnterReturnsTrue)) {
				UserName = username;
			} ImGui::SameLine();
			ImGui::ColorEdit4("##usercolor", Colors.UserName, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

			// HOST
			char hostname[32]{}; std::strcpy(hostname, HostName.c_str());
			ImGui::TextDisabled("host"); ImGui::SameLine();
			if (ImGui::InputText("##hostname", hostname, ML_arraysize(hostname), ImGuiInputTextFlags_EnterReturnsTrue)) {
				HostName = hostname;
			} ImGui::SameLine();
			ImGui::ColorEdit4("##hostcolor", Colors.HostName, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

			// PATH
			char pathname[32]{}; std::strcpy(pathname, PathName.c_str());
			ImGui::TextDisabled("path"); ImGui::SameLine();
			if (ImGui::InputText("##pathname", pathname, ML_arraysize(pathname), ImGuiInputTextFlags_EnterReturnsTrue)) {
				PathName = pathname;
			} ImGui::SameLine();
			ImGui::ColorEdit4("##pathcolor", Colors.PathName, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

			// MODE
			char modename[32]{}; std::strcpy(modename, ModeName.c_str());
			ImGui::TextDisabled("mode"); ImGui::SameLine();
			if (ImGui::InputText("##modename", modename, ML_arraysize(modename), ImGuiInputTextFlags_EnterReturnsTrue)) {
				ModeName = modename;
			} ImGui::SameLine();
			ImGui::ColorEdit4("##modecolor", Colors.ModeName, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		}

		void DrawPrefix()
		{
			// PREFIX user@host:path$ /mode
			ImGui::BeginGroup();
			ImGui::AlignTextToFramePadding();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
			ImGui::PushStyleColor(ImGuiCol_Text, Colors.Delimeter);
			if (!UserName.empty()) {
				ImGui::PushStyleColor(ImGuiCol_Text, Colors.UserName);
				ImGui::Text("%.*s", UserName.size(), UserName.data());
				ImGui::PopStyleColor(); ImGui::SameLine();
			}
			ImGui::Text("@"); ImGui::SameLine();
			if (!HostName.empty()) {
				ImGui::PushStyleColor(ImGuiCol_Text, Colors.HostName);
				ImGui::Text("%.*s", HostName.size(), HostName.data());
				ImGui::PopStyleColor(); ImGui::SameLine();
			}
			ImGui::Text(":"); ImGui::SameLine();
			if (!PathName.empty()) {
				ImGui::PushStyleColor(ImGuiCol_Text, Colors.PathName);
				ImGui::Text("%.*s", PathName.size(), PathName.data());
				ImGui::PopStyleColor(); ImGui::SameLine();
			}
			ImGui::Text("$"); ImGui::SameLine();
			if (!ModeName.empty()) {
				ImGui::Text(" /"); ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, Colors.ModeName);
				ImGui::Text("%.*s", ModeName.size(), ModeName.data());
				ImGui::PopStyleColor(); ImGui::SameLine();
			}
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			ImGui::EndGroup();
		}

		void DrawInput()
		{
			ImGui::BeginGroup(); ML_defer(&) { ImGui::EndGroup(); };
			ImGui::AlignTextToFramePadding();

			bool reclaim_focus{};
			ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
			if (ImGui::InputText
			(
				"##input_text",
				Input.data(),
				Input.max_size(),
				ImGuiInputTextFlags_EnterReturnsTrue |
				ImGuiInputTextFlags_CallbackCompletion |
				ImGuiInputTextFlags_CallbackHistory,
				[](auto u) { return ((Terminal *)u->UserData)->InputTextCallbackStub(u); },
				this
			))
			{
				if (Input) {
					Execute(Input.data());
					Output.ScrollToBottom = true;
				}
				std::strcpy(Input, "");
				reclaim_focus = true;
			}
			ImGui::PopItemWidth();
			ImGui::SetItemDefaultFocus(); // focus on window apparition
			if (reclaim_focus) { ImGui::SetKeyboardFocusHere(-1); } // focus previous widget
		}

		void DrawOutput(TextLog::Printer const & printer = {})
		{
			Output.Draw(printer);
		}

		int32 Execute(ds::string line)
		{
			// empty check
			if (util::trim(line).empty()) { return debug::failure(); }
		
			// append line
			Output.Printf("# %s\n", line.c_str());
			HistoryPos = -1;
			if (auto const it{ std::find(History.begin(), History.end(), line) }
			; it != History.end()) {
				History.erase(it);
			} History.push_back(line);

			// validate line
			if ((line.front() != '/' && ModeName.empty()) || util::trim_front(line, [
			](char c) { return c == '/' || util::is_whitespace(c); }).empty()) {
				return debug::failure();
			}

			// process command
			ds::string name;
			if (!ModeName.empty()) {
				name = ModeName;
			}
			else if (size_t const i{ line.find_first_of(' ') }; i != line.npos) {
				name = line.substr(0, i);
				line = line.substr(i + 1);
			}
			else {
				name = line;
				line.clear();
			}

			// execute procedure
			if (auto const it{ std::find_if(Cmd.begin(), Cmd.end(), [&](CommandData const & e) {
				return name == e.name;
			}) }; it != Cmd.end())
			{
				std::invoke(it->proc, std::move(line));

				return true;
			}
			else
			{
				return debug::failure("unknown command: {0} {1}", name, line);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		int32 InputTextCallbackStub(ImGuiInputTextCallbackData * data)
		{
			switch (data->EventFlag)
			{
			// COMPLETION
			case ImGuiInputTextFlags_CallbackCompletion: {

				// locate beginning of current word
				cstring last{ data->Buf + data->CursorPos }, first{ last };
				while (first > data->Buf)
				{
					if (char const c{ first[-1] }
					; util::is_whitespace(c) || c == ',' || c == ';' || c == '/')
					{
						break;
					}
					first--;
				}

				// build list of candidates
				ds::list<cstring> candidates{};
				for (auto const & e : this->Cmd)
				{
					cstring name{ e.name.c_str() };

					if (!std::strncmp(name, first, (size_t)(last - first)))
					{
						candidates.push_back(name);
					}
				}
				// no matches
				if (candidates.empty())
				{
					Output.Printf("# %.*s\n", (size_t)(last - first), first);
				}
				// single match
				else if (candidates.size() == 1)
				{
					// entirely replace beginning of word
					data->DeleteChars((int32)(first - data->Buf), (int32)(last - first));
					data->InsertChars(data->CursorPos, candidates[0]);
					data->InsertChars(data->CursorPos, " ");
				}
				// multiple matches
				else
				{
					// complete as much as we can
					auto match_len{ (size_t)(last - first) };
					for (;;)
					{
						size_t c{};
						bool all_match{ true };
						for (size_t i = 0; i < candidates.size() && all_match; ++i)
						{
							if (!i) { c = std::toupper(candidates[i][match_len]); }
							else if (c == 0 || c != std::toupper(candidates[i][match_len])) {
								all_match = false;
							}
						}
						if (!all_match) {
							break;
						}
						match_len++;
					}
					if (match_len > 0) {
						data->DeleteChars((int32)(first - data->Buf), (int32)(last - first));
						data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
					}

					// display suggestions
					Output.Printf("suggestions:\n");
					for (size_t i = 0; i < candidates.size(); ++i)
					{
						Output.Printf("- %s\n", candidates[i]);
					}
				}
			} break;

			// HISTORY
			case ImGuiInputTextFlags_CallbackHistory: {
				int32 const prev_pos{ HistoryPos };
				if (data->EventKey == ImGuiKey_UpArrow)
				{
					if (HistoryPos == -1)
					{
						HistoryPos = (int32)History.size() - 1;
					}
					else if (HistoryPos > 0)
					{
						HistoryPos--;
					}
				}
				else if (data->EventKey == ImGuiKey_DownArrow)
				{
					if (HistoryPos != -1 && (++HistoryPos >= (int32)History.size()))
					{
						HistoryPos = -1;
					}
				}

				if (prev_pos != HistoryPos)
				{
					auto history_str{ (HistoryPos >= 0) ? History[HistoryPos].c_str() : "" };
					data->DeleteChars(0, data->BufTextLen);
					data->InsertChars(0, history_str);
				}
			} break;
			}
			return 0;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_TERMINAL_HPP_