#ifndef _ML_FILE_UTILITY_HPP_
#define _ML_FILE_UTILITY_HPP_

#include <modus_core/detail/StringUtility.hpp>

namespace ml
{
	struct ML_NODISCARD file_info_struct final
	{
		char	type				{ ' ' };
		string	file_path			{};
		string	file_name			{};
		string	ext					{};
		size_t	file_size			{};
		string	formatted_file_size	{};
		string	file_modified_date	{};
	};
}

namespace ml::util
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	static std::optional<file_info_struct> get_file_info(fs::path const & path)
	{
		if (!fs::exists(path)) { return std::nullopt; }

		file_info_struct temp{};

		temp.file_path = (string)path.string();

		temp.file_name = (string)path.stem().string();

		temp.ext = (string)path.extension().string();

		temp.file_size = 0u;

		temp.formatted_file_size = "0B";

		temp.file_modified_date = "DD/MM/YYYY";

		return temp;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// load file contents into vector
	template <class Ch = char, class Buf = list<Ch>, class Al = Buf::allocator_type
	> ML_NODISCARD std::optional<Buf> get_file_contents(fs::path const & path, Al alloc = {})
	{
		std::basic_ifstream<Ch, std::char_traits<Ch>> file{ path, std::ios_base::binary };
		ML_defer(&){ file.close(); };
		if (!file) { return std::nullopt; }

		Buf temp{ alloc };
		file.seekg(0, std::ios_base::end);
		if (std::streamsize size{ file.tellg() }; size > 0)
		{
			file.seekg(0, std::ios_base::beg);
			
			temp.resize(static_cast<size_t>(size));
			
			file.read(&temp[0], size);
		}
		return std::make_optional(std::move(temp));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// load file contents into string
	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str get_file_string(fs::path const & path, Al alloc = {}) noexcept
	{
		if (auto const buf{ util::get_file_contents<Ch>(path) }; !buf) { return std::nullopt; }
		else { return Str{ buf->begin(), buf->end(), alloc }; }
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_FILE_UTILITY_HPP_