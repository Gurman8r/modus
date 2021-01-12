#ifndef _ML_NATIVE_LIBRARY_HPP_
#define _ML_NATIVE_LIBRARY_HPP_

#include <modus_core/embed/Library.hpp>

namespace ml
{
	// native library
	struct ML_CORE_API native_library final : library
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
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
		~native_library() noexcept final
		{
			this->close();
		}

		native_library(allocator_type alloc = {}) noexcept
			: m_hash	{}
			, m_info	{}
			, m_handle	{}
			, m_lookup	{ alloc }
		{
		}

		native_library(fs::path const & path, allocator_type alloc = {}) noexcept
			: m_hash	{}
			, m_info	{}
			, m_handle	{}
			, m_lookup	{ alloc }
		{
			this->open(path);
		}

		native_library(native_library && other, allocator_type alloc = {}) noexcept
			: m_hash	{}
			, m_info	{}
			, m_handle	{}
			, m_lookup	{ alloc }
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
				std::swap(m_hash, other.m_hash);
				std::swap(m_handle, other.m_handle);
				std::swap(m_info, other.m_info);
				std::swap(m_lookup, other.m_lookup);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		bool open(fs::path const & path) final;

		bool close() final;

		ML_NODISCARD hash_t get_hash_code() const noexcept final { return m_hash; }

		ML_NODISCARD file_info_struct const & get_file_info() const noexcept final { return m_info; }
		
		ML_NODISCARD library_handle get_handle() const noexcept final { return m_handle; }

		ML_NODISCARD void * get_proc_address(string const & method_name) final;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		hash_t						m_hash		; // hash code
		library_handle				m_handle	; // native handle
		file_info_struct			m_info		; // file info
		hash_map<string, void *>	m_lookup	; // name cache

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}
#endif // !_ML_NATIVE_LIBRARY_HPP_