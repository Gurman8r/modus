#ifndef _ML_TEXT_LOG_HPP_
#define _ML_TEXT_LOG_HPP_

#include <modus_core/gui/ImGui.hpp>
#include <modus_core/detail/Method.hpp>

namespace ml::ImGuiExt
{
	// TEXT LOG
	struct ML_NODISCARD TextLog final : non_copyable, trackable
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using PrinterSignature = typename void(list<string> const &, size_t);

		// printer
		struct ML_NODISCARD Printer final : method< PrinterSignature >
		{
			using method< PrinterSignature >::method;

			Printer() noexcept : Printer{ [
			](list<pmr::string> const & lines, size_t i) noexcept
			{
				auto check_prefix = [&lines, i](cstring str) noexcept -> bool
				{
					return !std::strncmp(lines[i].c_str(), str, std::strlen(str));
				};

				color c;
				if (check_prefix("# ")) {
					c = { 1.0f, 0.8f, 0.6f, 1.0f };
				}
				else if (check_prefix(ML_DEBUG_MSG_SUCCESS)) {
					c = colors::lime;
				}
				else if (check_prefix(ML_DEBUG_MSG_FAILURE)) {
					c = colors::red;
				}
				else if (check_prefix(ML_DEBUG_MSG_WARNING)) {
					c = colors::yellow;
				}
				else {
					c = colors::white;
				}

				ImGui::PushStyleColor(ImGuiCol_Text, c);
				ImGui::TextUnformatted(lines[i].c_str());
				ImGui::PopStyleColor();
			} }
			{
			}
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ImGuiTextFilter			Filter			; // filter
		list<string>	Lines			; // lines
		bool					AutoScroll		; // auto scroll
		bool					ScrollToBottom	; // scroll to bottom

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		TextLog(cstring default_filter = "", bool auto_scroll = true, allocator_type alloc = {}) noexcept
			: Filter		{ default_filter }
			, Lines			{ alloc }
			, AutoScroll	{ auto_scroll }
			, ScrollToBottom{}
		{
		}

		explicit TextLog(allocator_type alloc) noexcept : TextLog{ "", true, alloc }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		TextLog & Write(char value) noexcept {
			switch (value)
			{
			default: { // default
				if (Lines.empty()) { Lines.push_back({}); }
				Lines.back().push_back(value);
			} break;

			case '\n': { // new line
				Lines.push_back({});
			} break;
			}
			return (*this);
		}

		TextLog & Print(cstring value = "\n") noexcept {
			std::for_each_n(value, std::strlen(value), [&](char c) noexcept {
				this->Write(c);
			});
			return (*this);
		}

		TextLog & Print(string const & value = "\n") noexcept {
			for (char c : value) {
				this->Write(c);
			}
			return (*this);
		}

		template <class ... Args
		> TextLog & Printf(cstring str, Args && ... args) noexcept {
			char buf[256]{};
			std::sprintf(buf, str, ML_forward(args)...);
			return this->Print(buf);
		}

		template <class ... Args
		> TextLog & Printf(string const & str, Args && ... args) noexcept {
			return this->Printf(str.c_str(), ML_forward(args)...);
		}

		TextLog & operator<<(char value) noexcept { return this->Write(value); }

		TextLog & operator<<(cstring value) noexcept { return this->Print(value); }

		TextLog & operator<<(string const & value) noexcept { return this->Print(value); }

		template <class T
		> TextLog & operator<<(T && value) noexcept
		{
			stringstream ss{};
			ss << ML_forward(value);
			return this->Dump(ss);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void Draw(Printer const & printer = {})
		{
			// print lines
			ImGui::BeginGroup();
			for (size_t i = 0, imax = Lines.size(); i < imax; ++i)
			{
				if (Filter.PassFilter(Lines[i].c_str()))
				{
					printer(Lines, i);
				}
			}
			ImGui::EndGroup();

			// scroll to bottom
			if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			{
				ImGui::SetScrollHereY(1.0f);
			}
			ScrollToBottom = false;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_TEXT_LOG_HPP_