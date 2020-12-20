#ifndef _ML_SHARED_LIBRARY_HPP_
#define _ML_SHARED_LIBRARY_HPP_

#include <modus_core/detail/Hashmap.hpp>
#include <modus_core/system/Platform.hpp>

namespace ml
{
	// shared library
	struct shared_library final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;
		
		using proc_table = typename ds::hashmap<ds::string, void *>;

		template <class Ret> using proc_result = typename std::conditional_t
		<
			std::is_same_v<Ret, void>, void, std::optional<Ret>
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr auto default_extension // native file type
		{
#ifdef ML_os_windows
			L".dll"
#else
			L".so"
#endif
		};

		ML_NODISCARD static fs::path format_path(fs::path const & path) noexcept
		{
			return (path.empty()
				? path
				: (!path.extension().empty()
					? path
					: path.native() + default_extension));
		}

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
				m_path = format_path(path);

				m_hash = hashof(m_path.string());

				return m_handle = platform::load_library(m_path);
			}
		}

		bool close()
		{
			if (!m_handle) { return false; }
			else
			{
				m_path.clear();
				m_procs.clear();
				m_hash = {};
				return platform::free_library(m_handle);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void * get_proc(ds::string const & name)
		{
			if (!m_handle) { return nullptr; }
			else if (auto const it{ m_procs.find(name) }
			; it != m_procs.end()) { return it->second; }
			else
			{
				return m_procs.insert({
					name,
					platform::get_proc_address(m_handle, name)
				}).first->second;
			}
		}

		template <class Ret, class ... Args, class Name
		> auto get_proc(Name && name) noexcept
		{
			return reinterpret_cast<Ret(*)(Args...)>
			(
				this->get_proc(ML_forward(name))
			);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Ret, class ... Args, class Name
		> auto run_proc(Name && name, Args && ... args) noexcept -> proc_result<Ret>
		{
			if (auto const fn{ this->get_proc<Ret, Args...>(ML_forward(name)) })
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

		ML_NODISCARD auto handle() const noexcept -> library_handle { return m_handle; }

		ML_NODISCARD auto hash() const noexcept -> hash_t { return m_hash; }

		ML_NODISCARD auto path() const noexcept -> fs::path const & { return m_path; }

		ML_NODISCARD auto procedures() const noexcept -> proc_table const & { return m_procs; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class U = shared_library
		> ML_NODISCARD auto compare(U const & value) const noexcept
		{
			if constexpr (std::is_same_v<U, shared_library>)
			{
				return (this != std::addressof(value)) ? compare(value.m_path) : 0;
			}
			else if constexpr (std::is_same_v<U, fs::path>)
			{
				return compare(hashof(value.string()));
			}
			else
			{
				static_assert(std::is_same_v<U, hash_t>);
				return util::compare(m_hash, value);
			}
		}

		template <class U = shared_library
		> ML_NODISCARD bool operator==(U const & value) const noexcept { return this->compare(value) == 0; }

		template <class U = shared_library
		> ML_NODISCARD bool operator!=(U const & value) const noexcept { return this->compare(value) != 0; }

		template <class U = shared_library
		> ML_NODISCARD bool operator<(U const & value) const noexcept { return this->compare(value) < 0; }

		template <class U = shared_library
		> ML_NODISCARD bool operator>(U const & value) const noexcept { return this->compare(value) > 0; }

		template <class U = shared_library
		> ML_NODISCARD bool operator<=(U const & value) const noexcept { return this->compare(value) <= 0; }

		template <class U = shared_library
		> ML_NODISCARD bool operator>=(U const & value) const noexcept { return this->compare(value) >= 0; }

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