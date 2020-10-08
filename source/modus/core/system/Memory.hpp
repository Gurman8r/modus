#ifndef _ML_MEMORY_HPP_
#define _ML_MEMORY_HPP_

#include <core/detail/Debug.hpp>
#include <core/detail/Singleton.hpp>
#include <core/detail/BatchVector.hpp>

// passthrough
namespace ml
{
	// proxy for testing an upstream resource
	struct passthrough_resource final : public pmr::memory_resource, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using pointer			= typename byte_t *;
		using const_pointer		= typename byte_t const *;
		using reference			= typename byte_t &;
		using const_reference	= typename byte_t const &;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		explicit passthrough_resource(pmr::memory_resource * u, pointer const b, size_t c) noexcept
			: m_upstream{ u }, m_buffer{ b }, m_capacity{ c }
		{
			ML_assert("invalid passthrough_resource parameters" && u && b && c);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto base() const noexcept -> size_t { return (size_t)m_buffer; }

		ML_NODISCARD auto capacity() const noexcept -> size_t { return m_capacity; }

		ML_NODISCARD auto count() const noexcept -> size_t { return m_count; }
		
		ML_NODISCARD auto data() const noexcept -> pointer const { return m_buffer; }

		ML_NODISCARD auto free() const noexcept -> size_t { return m_capacity - m_used; }

		ML_NODISCARD auto resource() const noexcept -> pmr::memory_resource * const { return m_upstream; }

		ML_NODISCARD auto used() const noexcept -> size_t { return m_used; }
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto fraction() const noexcept -> float_t { return (float_t)m_used / (float_t)m_capacity; }

		ML_NODISCARD auto percentage() const noexcept -> float_t { return fraction() * 100.f; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto front() & noexcept -> reference { return m_buffer[0]; }

		ML_NODISCARD auto front() const & noexcept -> const_reference { return m_buffer[0]; }

		ML_NODISCARD auto back() & noexcept -> reference { return m_buffer[m_capacity - 1]; }

		ML_NODISCARD auto back() const & noexcept -> const_reference { return m_buffer[m_capacity - 1]; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto begin() noexcept -> pointer { return m_buffer; }

		ML_NODISCARD auto begin() const noexcept -> const_pointer { return m_buffer; }

		ML_NODISCARD auto cbegin() const noexcept -> const_pointer { return begin(); }

		ML_NODISCARD auto end() noexcept -> pointer { return m_buffer + m_capacity; }

		ML_NODISCARD auto end() const noexcept -> const_pointer { return m_buffer + m_capacity; }

		ML_NODISCARD auto cend() const noexcept -> const_pointer { return end(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void * do_allocate(size_t bytes, size_t align) override
		{
			++m_count;
			m_used += bytes;
			return m_upstream->allocate(bytes, align);
		}

		void do_deallocate(void * ptr, size_t bytes, size_t align) override
		{
			--m_count;
			m_used -= bytes;
			return m_upstream->deallocate(ptr, bytes, align);
		}

		bool do_is_equal(pmr::memory_resource const & value) const noexcept override
		{
			return m_upstream->is_equal(value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		pmr::memory_resource * m_upstream;
		pointer const m_buffer;
		size_t const m_capacity;

		size_t m_count{};
		size_t m_used{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// types
namespace ml
{
	// trackable
	struct trackable;

	// no delete
	struct no_delete final
	{
		template <class T
		> void operator()(T *) const noexcept {}
	};

	// default delete
	template <class ...> struct default_delete;

	// scoped pointer ( std::unique_ptr<T, Dx> )
	template <class T, class Dx = default_delete<T>
	> ML_alias unique = typename std::unique_ptr<T, Dx>;

	// manual pointer ( std::unique_ptr<T, no_delete> )
	template <class T
	> ML_alias manual = typename unique<T, no_delete>;

	// shared pointer ( std::shared_ptr<T> )
	template <class T
	> ML_alias shared = typename std::shared_ptr<T>;

	// unowned pointer ( std::weak_ptr<T> )
	template <class T
	> ML_alias unown = typename std::weak_ptr<T>;
}

// record
namespace ml
{
	// memory record
	struct ML_NODISCARD memory_record final
	{
		byte_t * addr; size_t index; size_t count; size_t size;

		constexpr operator bool() const noexcept
		{
			return addr && index && count && size;
		}
	};

	static void to_json(json & j, memory_record const & v)
	{
		j["index"	] = v.index;
		j["addr"	] = (intptr_t)v.addr;
		j["count"	] = v.count;
		j["size"	] = v.size;
	}

	static void from_json(json const & j, memory_record & v)
	{
		j["index"	].get_to(v.index);
		j["addr"	].get_to(*(intptr_t *)v.addr);
		j["count"	].get_to(v.count);
		j["size"	].get_to(v.size);
	}
}

// memory
namespace ml
{
	// memory manager
	struct ML_CORE_API memory final : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		enum : size_t { id_addr, id_index, id_count, id_size };

		using record_manager = typename ds::batch_vector
		<
			byte_t *	,	// address
			size_t		,	// index
			size_t		,	// count
			size_t			// size
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit memory(passthrough_resource & res);

		explicit memory(pmr::memory_resource * res) : memory{
			*reinterpret_cast<passthrough_resource *>(res)
		}
		{
		}

		~memory() noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// malloc
		ML_NODISCARD static void * allocate(size_t size) noexcept
		{
			return ML_check(g_mem)->do_allocate(1, size);
		}

		// calloc
		ML_NODISCARD static void * allocate(size_t count, size_t size) noexcept
		{
			return ML_check(g_mem)->do_allocate(count, size);
		}

		// free
		static void deallocate(void * addr) noexcept
		{
			ML_check(g_mem)->do_deallocate(addr);
		}

		// realloc
		static void * reallocate(void * addr, size_t size) noexcept
		{
			return memory::reallocate(addr, size, size);
		}

		// realloc (sized)
		static void * reallocate(void * addr, size_t oldsz, size_t newsz) noexcept
		{
			if (newsz == 0)				{ memory::deallocate(addr); return nullptr; }
			else if (addr == nullptr)	{ return memory::allocate(newsz); }
			else if (newsz <= oldsz)	{ return addr; }
			else
			{
				auto const temp{ memory::allocate(newsz) };
				if (temp)
				{
					std::memcpy(temp, addr, oldsz);
					memory::deallocate(addr);
				}
				return temp;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// allocate object
		template <class T
		> ML_NODISCARD static T * allocate_object(size_t count = 1) noexcept
		{
			return (T *)memory::allocate(count, sizeof(T));
		}

		// deallocate object
		template <class T
		> static void deallocate_object(T * addr) noexcept
		{
			memory::deallocate(addr);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// new object
		template <class T, class ... Args
		> ML_NODISCARD static T * new_object(Args && ... args) noexcept
		{
			return util::construct(memory::allocate_object<T>(), ML_forward(args)...);
		}

		// delete object
		template <class T
		> static void delete_object(T * addr) noexcept
		{
			util::destruct(addr);
			memory::deallocate_object(addr);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// allocate shared
		template <class T, class ... Args
		> ML_NODISCARD static shared<T> alloc_ref(allocator_type alloc, Args && ... args) noexcept
		{
			return std::allocate_shared<T>(alloc, ML_forward(args)...);
		}

		// make shared
		template <class T, class ... Args
		> ML_NODISCARD static shared<T> make_ref(Args && ... args) noexcept
		{
			return memory::alloc_ref<T>(memory::get_allocator(), ML_forward(args)...);
		}

		// make unique
		template <class T, class Dx = default_delete<T>, class ... Args
		> ML_NODISCARD static unique<T, Dx> make_scope(Args && ... args) noexcept
		{
			return { memory::new_object<T>(ML_forward(args)...), Dx{} };
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// get allocator
		ML_NODISCARD static auto get_allocator() noexcept -> allocator_type { return ML_check(g_mem)->m_alloc; }

		// get counter
		ML_NODISCARD static auto get_counter() noexcept -> size_t { return ML_check(g_mem)->m_counter; }

		// get records
		ML_NODISCARD static auto get_records() noexcept -> record_manager const & { return ML_check(g_mem)->m_records; }

		// get resource
		ML_NODISCARD static auto get_resource() noexcept -> passthrough_resource * { return ML_check(g_mem)->m_resource; }

		// get singleton
		ML_NODISCARD static auto get_singleton() noexcept -> memory * const { return g_mem; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// get record
		ML_NODISCARD static auto get_record(size_t i) noexcept -> memory_record
		{
			ML_assert(g_mem);
			return {
				g_mem->m_records.at<id_addr>(i),
				g_mem->m_records.at<id_index>(i),
				g_mem->m_records.at<id_index>(i),
				g_mem->m_records.at<id_index>(i)
			};
		}

		// get record address
		ML_NODISCARD static auto get_record_addr(size_t i) noexcept -> byte_t *
		{
			return ML_check(g_mem)->m_records.at<id_addr>(i);
		}

		// get record count
		ML_NODISCARD static auto get_record_count(size_t i) noexcept -> size_t
		{
			return ML_check(g_mem)->m_records.at<id_count>(i);
		}

		// get record index
		ML_NODISCARD static auto get_record_index(size_t i) noexcept -> size_t
		{
			return ML_check(g_mem)->m_records.at<id_index>(i);
		}

		// get record size
		ML_NODISCARD static auto get_record_size(size_t i) noexcept -> size_t
		{
			return ML_check(g_mem)->m_records.at<id_size>(i);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void * do_allocate(size_t count, size_t size) noexcept
		{
			auto const addr{ m_alloc.allocate(count * size) };

			return std::get<id_addr>(m_records.push_back
			(
				addr, ++m_counter, count, size
			));
		}

		void do_deallocate(void * addr) noexcept
		{
			if (auto const i{ m_records.lookup<id_addr>(addr) }; i != m_records.npos)
			{
				m_alloc.deallocate(
					(byte_t *)addr,
					m_records.at<id_count>(i) * m_records.at<id_size>(i));

				m_records.erase(i);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		static memory *					g_mem		; // singleton
		passthrough_resource * const	m_resource	; // resource
		allocator_type					m_alloc		; // allocator
		record_manager					m_records	; // records
		size_t							m_counter	; // counter

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// trackable
namespace ml
{
	// trackable base
	struct ML_CORE_API trackable
	{
		virtual ~trackable() noexcept = default;

		ML_NODISCARD void * operator new(size_t size) noexcept { return memory::allocate(size); }

		ML_NODISCARD void * operator new[](size_t size) noexcept { return memory::allocate(size); }

		void operator delete(void * addr) noexcept { memory::deallocate(addr); }

		void operator delete[](void * addr) noexcept { memory::deallocate(addr); }
	};
}

// deleters
namespace ml
{
	template <> struct default_delete<> final
	{
		void operator()(void * addr) const noexcept
		{
			memory::deallocate(addr);
		}
	};

	template <> struct default_delete<void> final
	{
		void operator()(void * addr) const noexcept
		{
			memory::deallocate(addr);
		}
	};

	template <class T> struct default_delete<T> final
	{
		void operator()(T * addr) const noexcept
		{
			memory::delete_object<T>(addr);
		}
	};
}

#endif // !_ML_MEMORY_HPP_