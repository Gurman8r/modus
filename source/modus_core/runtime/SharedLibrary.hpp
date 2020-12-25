#ifndef _ML_SHARED_LIBRARY_HPP_
#define _ML_SHARED_LIBRARY_HPP_

#include <modus_core/detail/Hashmap.hpp>
#include <modus_core/detail/Memory.hpp>

namespace ml
{
	// library handle
	ML_decl_handle(library_handle);

	// shared library
	struct shared_library final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD static library_handle load_library(fs::path const & path);

		static bool free_library(library_handle instance);

		ML_NODISCARD static void * get_proc_address(library_handle instance, ds::string const & method_name);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;
		
		using proc_table = typename ds::hashmap<ds::string, void *>;

		template <class Ret> using result_type = typename std::conditional_t
		<
			std::is_same_v<Ret, void>, void, std::optional<Ret>
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr auto default_extension // native file type
		{
#ifdef ML_os_windows
			ML_wide(".dll")
#else
			ML_wide(".so")
#endif
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		shared_library(allocator_type alloc = {}) noexcept
			: m_handle	{}
			, m_hash	{}
			, m_path	{}
			, m_procs	{ alloc }
		{
		}

		explicit shared_library(fs::path const & path, allocator_type alloc = {}) noexcept
			: shared_library{ alloc }
		{
			this->open(path);
		}

		explicit shared_library(shared_library && value, allocator_type alloc = {}) noexcept
			: shared_library{ alloc }
		{
			this->swap(std::move(value));
		}

		~shared_library() noexcept override
		{
			this->close();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		shared_library & operator=(shared_library && value) noexcept
		{
			this->swap(std::move(value));
			return (*this);
		}

		void swap(shared_library & value) noexcept
		{
			if (this != std::addressof(value))
			{
				std::swap(m_handle, value.m_handle);
				std::swap(m_hash, value.m_hash);
				m_path.swap(value.m_path);
				m_procs.swap(value.m_procs);
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
				m_handle = shared_library::load_library(path);
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
				return shared_library::free_library(m_handle);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void * proc(ds::string const & method_name)
		{
			if (!m_handle) { return nullptr; }
			else if (auto const it{ m_procs.find(method_name) }
			; it != m_procs.end()) { return it->second; }
			else
			{
				return m_procs.insert({
					method_name,
					shared_library::get_proc_address(m_handle, method_name)
				}).first->second;
			}
		}

		template <class Ret, class ... Args, class ID
		> auto proc(ID && method_name) noexcept
		{
			return reinterpret_cast<Ret(*)(Args...)>
			(
				this->proc(ML_forward(method_name))
			);
		}

		template <class Ret, class ... Args, class ID
		> auto eval(ID && method_name, Args && ... args) noexcept -> result_type<Ret>
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
		library_handle	m_handle; // handle
		hash_t			m_hash	; // hash
		fs::path		m_path	; // path
		proc_table		m_procs	; // procedures

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SHARED_LIBRARY_HPP_