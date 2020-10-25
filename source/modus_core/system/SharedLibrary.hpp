#ifndef _ML_SHARED_LIBRARY_HPP_
#define _ML_SHARED_LIBRARY_HPP_

#include <modus_core/detail/Map.hpp>
#include <modus_core/system/Memory.hpp>

namespace ml
{
	ML_decl_handle(library_handle);

	struct ML_CORE_API shared_library final : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;
		
		using symbol_table = typename ds::map<hash_t, void *>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr auto default_extension // native library extension
		{
#if defined(ML_os_windows)
			L".dll"
#else
			L".so"
#endif
		};

		static fs::path format_path(fs::path const & path) noexcept
		{
			return !path.extension().empty() ? path : path.native() + default_extension;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		shared_library(allocator_type alloc = {}) noexcept
			: m_handle{}, m_path{}, m_hash{}, m_proc{ alloc }
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

		~shared_library() noexcept
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
				m_proc.swap(value.m_proc);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool open(fs::path const & path);

		bool close();

		void * get_proc_address(cstring name);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		void * get_proc_address(ds::string const & name) noexcept
		{
			return this->get_proc_address(name.c_str());
		}

		template <class Ret, class ... Args, class ID
		> auto proc(ID && name) noexcept
		{
			return reinterpret_cast<Ret(*)(Args...)>
			(
				this->get_proc_address(ML_forward(name))
			);
		}

		template <class Ret, class ... Args, class ID
		> auto call(ID && name, Args && ... args) noexcept -> std::conditional_t
		<
			!std::is_same_v<Ret, void>, std::optional<Ret>, void
		>
		{
			if (auto const fn{ this->proc<Ret, Args...>(ML_forward(name)) })
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

		ML_NODISCARD auto hash_code() const noexcept -> hash_t { return m_hash; }

		ML_NODISCARD auto path() const noexcept -> fs::path const & { return m_path; }

		ML_NODISCARD auto procedures() const noexcept -> symbol_table const & { return m_proc; }

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
				return util::compare(hash_code(), value);
			}
		}

		template <class U = shared_library
		> ML_NODISCARD bool operator==(U const & value) const noexcept
		{
			return compare(value) == 0;
		}

		template <class U = shared_library
		> ML_NODISCARD bool operator!=(U const & value) const noexcept
		{
			return compare(value) != 0;
		}

		template <class U = shared_library
		> ML_NODISCARD bool operator<(U const & value) const noexcept
		{
			return compare(value) < 0;
		}

		template <class U = shared_library
		> ML_NODISCARD bool operator>(U const & value) const noexcept
		{
			return compare(value) > 0;
		}

		template <class U = shared_library
		> ML_NODISCARD bool operator<=(U const & value) const noexcept
		{
			return compare(value) <= 0;
		}

		template <class U = shared_library
		> ML_NODISCARD bool operator>=(U const & value) const noexcept
		{
			return compare(value) >= 0;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		library_handle	m_handle; // handle
		fs::path		m_path	; // path
		hash_t			m_hash	; // hash
		symbol_table	m_proc	; // procedures

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SHARED_LIBRARY_HPP_