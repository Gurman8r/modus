#ifndef _ML_NATIVE_LIBRARY_HPP_
#define _ML_NATIVE_LIBRARY_HPP_

#include <modus_core/system/Platform.hpp>
#include <modus_core/detail/Hashmap.hpp>
#include <modus_core/detail/Method.hpp>

namespace ml
{
	// native library
	struct ML_CORE_API native_library final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;
		
		using proc_table = typename hash_map<string, void *>;

		template <class Ret> using result_type = typename std::conditional_t
		<
			std::is_same_v<Ret, void>, void, std::optional<Ret>
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		native_library(allocator_type alloc = {}) noexcept
			: m_hash	{}
			, m_path	{}
			, m_handle	{}
			, m_procs	{ alloc }
		{
		}

		native_library(fs::path const & path, allocator_type alloc = {}) noexcept
			: m_hash	{ hashof(path.string()) }
			, m_path	{ path }
			, m_handle	{}
			, m_procs	{ alloc }
		{
			this->open(path);
		}

		native_library(native_library && other, allocator_type alloc = {}) noexcept
			: m_hash	{}
			, m_path	{}
			, m_handle	{}
			, m_procs	{ alloc }
		{
			this->swap(std::move(other));
		}

		~native_library() noexcept
		{
			this->close();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		native_library & operator=(native_library && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(native_library & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_hash, other.m_hash);
				m_path.swap(other.m_path);
				std::swap(m_handle, other.m_handle);
				m_procs.swap(other.m_procs);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool open(fs::path const & path)
		{
			if (m_handle || path.empty()) { return false; }
			else
			{
				m_path = path;
				m_hash = hashof(path.string());
				m_handle = platform::load_library(path);
				return m_handle;
			}
		}

		bool close()
		{
			if (!m_handle) { return false; }
			else
			{
				m_path.clear();
				m_procs.clear();
				m_hash = 0;
				return platform::free_library(m_handle);
			}
		}

		void * proc(string const & method_name)
		{
			if (!m_handle) { return nullptr; }
			else if (auto const it{ m_procs.find(method_name) }; it != m_procs.end())
			{
				return it->second;
			}
			else
			{
				return m_procs.insert({
					method_name,
					platform::get_proc_address(m_handle, method_name)
				}).first->second;
			}
		}

		template <class Ret, class ... Args, class Str
		> auto proc(Str && method_name)
		{
			return reinterpret_cast<Ret(*)(Args...)>
			(
				this->proc(ML_forward(method_name))
			);
		}

		template <class Ret, class ... Args, class Str
		> auto eval(Str && method_name, Args && ... args) -> result_type<Ret>
		{
			if (auto const fn{ this->proc<Ret, Args...>(ML_forward(method_name)) })
			{
				return std::invoke(fn, ML_forward(args)...);
			}
			else if constexpr (!std::is_same_v<Ret, void>)
			{
				return std::nullopt;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD operator bool() const noexcept { return m_handle; }

		ML_NODISCARD bool good() const noexcept { return m_handle; }

		ML_NODISCARD auto hash_code() const noexcept -> hash_t { return m_hash; }

		ML_NODISCARD auto path() const noexcept -> fs::path const & { return m_path; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		hash_t				m_hash		; // hash code
		fs::path			m_path		; // file path
		library_handle		m_handle	; // handle
		proc_table			m_procs		; // procedures

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_NATIVE_LIBRARY_HPP_