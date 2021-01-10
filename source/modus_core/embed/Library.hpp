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

	// library id
	ML_decl_handle(library_id);

	// library
	struct ML_CORE_API library : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		using procedure_map = typename hash_map<string, void *>;

		template <class Ret
		> using result_type = typename std::conditional_t<std::is_same_v<Ret, void>, void, std::optional<Ret>>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual bool open(fs::path path) = 0;

		virtual bool close() = 0;

		ML_NODISCARD virtual void * get_proc_address(string const & method_name) = 0;

		ML_NODISCARD virtual library_handle get_handle() const noexcept = 0;

		ML_NODISCARD virtual file_info const & get_file_info() const noexcept = 0;

		ML_NODISCARD virtual library_id get_uuid() const noexcept = 0;

		ML_NODISCARD inline operator bool() const noexcept { return (bool)get_handle(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Ret, class ... Args, class Name
		> ML_NODISCARD auto get_method(Name && method_name)
		{
			return reinterpret_cast<Ret(*)(Args...)>(get_proc_address(ML_forward(method_name)));
		}

		template <class Ret, class ... Args, class Name
		> auto call_method(Name && method_name, Args && ... args) -> result_type<Ret>
		{
			if (auto const fn{ this->get_method<Ret, Args...>(ML_forward(method_name)) })
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

namespace ml
{
	struct ML_CORE_API native_library final : library
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr cwstring default_extension
		{
#ifdef ML_os_windows
			L".dll"
#else
			L".so"
#endif
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		~native_library() noexcept final;

		native_library(allocator_type alloc = {}) noexcept
			: m_uuid	{}
			, m_info	{}
			, m_handle	{}
			, m_procs	{ alloc }
		{
		}

		native_library(fs::path const & path, allocator_type alloc = {}) noexcept
			: m_uuid	{}
			, m_info	{}
			, m_handle	{}
			, m_procs	{ alloc }
		{
			this->open(path);
		}

		native_library(native_library && other, allocator_type alloc = {}) noexcept
			: m_uuid	{}
			, m_info	{}
			, m_handle	{}
			, m_procs	{ alloc }
		{
			this->swap(std::move(other));
		}

		native_library & operator=(native_library && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(native_library & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_uuid, other.m_uuid);
				std::swap(m_info, other.m_info);
				std::swap(m_handle, other.m_handle);
				std::swap(m_procs, other.m_procs);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool open(fs::path path) final;

		bool close() final;

		void * get_proc_address(string const & method_name) final;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD library_handle get_handle() const noexcept final { return m_handle; }

		ML_NODISCARD file_info const & get_file_info() const noexcept final { return m_info; }

		ML_NODISCARD library_id get_uuid() const noexcept final { return m_uuid; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		library_id		m_uuid		; // id
		file_info		m_info		; // file info
		library_handle	m_handle	; // handle
		procedure_map	m_procs		; // procedure map

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_LIBRARY_HPP_