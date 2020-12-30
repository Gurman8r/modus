#ifndef _ML_LIBRARY_HPP_
#define _ML_LIBRARY_HPP_

// WIP

#include <modus_core/detail/Hashmap.hpp>
#include <modus_core/detail/Memory.hpp>
#include <modus_core/detail/Method.hpp>

namespace ml
{
	ML_decl_handle(library_id); // library id

	ML_decl_handle(library_handle); // library handle

	// library context
	struct ML_NODISCARD library_context final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ds::method<library_handle(fs::path const &)> load; // load library
		
		ds::method<bool(library_handle)> free; // free library

		ds::method<void * (library_handle, ds::string const &)> proc; // get procedure

		void swap(library_context & other) noexcept
		{
			if (this != std::addressof(other))
			{
				this->load.swap(other.load);
				this->free.swap(other.free);
				this->proc.swap(other.proc);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	// library
	struct ML_CORE_API library final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;
		
		using proc_table = typename ds::hashmap<ds::string, void *>;

		template <class Ret> using result_type = typename std::conditional_t
		<
			std::is_same_v<Ret, void>, void, std::optional<Ret>
		>;

		static library_context const native_context; // native library bindings

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		library(allocator_type alloc = {}) noexcept
			: m_hash	{}
			, m_path	{}
			, m_context	{ native_context }
			, m_handle	{}
			, m_procs	{ alloc }
		{
		}

		library(library_context const & context, allocator_type alloc = {})
			: m_hash	{}
			, m_path	{}
			, m_context	{ context }
			, m_handle	{}
			, m_procs	{ alloc }
		{
		}

		library(library_context && context, allocator_type alloc = {}) noexcept
			: m_hash	{}
			, m_path	{}
			, m_context	{ std::move(context) }
			, m_handle	{}
			, m_procs	{ alloc }
		{
		}

		library(fs::path const & path, allocator_type alloc = {}) noexcept
			: m_hash	{ hashof(path.string()) }
			, m_path	{ path }
			, m_context	{ native_context }
			, m_handle	{}
			, m_procs	{ alloc }
		{
			this->open(path);
		}

		library(library && other, allocator_type alloc = {}) noexcept
			: m_hash	{}
			, m_path	{}
			, m_context	{}
			, m_handle	{}
			, m_procs	{ alloc }
		{
			this->swap(std::move(other));
		}

		~library() noexcept
		{
			this->close();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		library & operator=(library && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(library & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_hash, other.m_hash);
				m_path.swap(other.m_path);
				m_context.swap(other.m_context);
				std::swap(m_handle, other.m_handle);
				m_procs.swap(other.m_procs);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_library_context() const noexcept -> library_context const & { return m_context; }

		template <class Value = library_context
		> void set_library_context(Value && value) noexcept { m_context = ML_forward(value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool open(fs::path const & path)
		{
			if (m_handle || path.empty()) { return false; }
			else
			{
				m_path = path;
				m_hash = hashof(path.string());
				m_handle = m_context.load(path);
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
				return m_context.free(m_handle);
			}
		}

		void * proc(ds::string const & method_name)
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
					m_context.proc(m_handle, method_name)
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
		library_context		m_context	; // context
		library_handle		m_handle	; // handle
		proc_table			m_procs		; // procedures

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_LIBRARY_HPP_