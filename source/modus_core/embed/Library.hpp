#ifndef _ML_LIBRARY_HPP_
#define _ML_LIBRARY_HPP_

#include <modus_core/system/Memory.hpp>
#include <modus_core/detail/FileUtility.hpp>
#include <modus_core/detail/Hashmap.hpp>
#include <modus_core/detail/Method.hpp>

namespace ml
{
	// library handle
	ML_decl_handle(library_handle);

	// base library
	struct ML_CORE_API library : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		template <class Ret
		> using result_type = typename std::conditional_t
		<
			std::is_same_v<Ret, void>, void, std::optional<Ret>
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual ~library() noexcept override = default;

		virtual bool open(fs::path const & path) = 0;

		virtual bool close() = 0;

		ML_NODISCARD virtual hash_t get_hash_code() const noexcept = 0;

		ML_NODISCARD virtual file_info_struct const & get_file_info() const noexcept = 0;

		ML_NODISCARD virtual library_handle get_handle() const noexcept = 0;

		ML_NODISCARD virtual void * get_proc_address(string const & method_name) = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD operator bool() const noexcept
		{
			return (bool)get_handle();
		}

		ML_NODISCARD bool good() const noexcept
		{
			return (bool)get_handle();
		}

		template <class Ret, class ... Args, class Name
		> ML_NODISCARD auto target(Name && method_name) -> Ret(*)(Args...)
		{
			return reinterpret_cast<Ret(*)(Args...)>(get_proc_address(ML_forward(method_name)));
		}

		template <class Ret, class ... Args, class Name
		> auto call(Name && method_name, Args && ... args) -> result_type<Ret>
		{
			if (auto const fn{ get_method<Ret, Args...>(ML_forward(method_name)) })
			{
				return std::invoke(fn, ML_forward(args)...);
			}
			else if constexpr (!std::is_same_v<Ret, void>)
			{
				return std::nullopt;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_LIBRARY_HPP_