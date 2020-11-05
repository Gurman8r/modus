#ifndef _ML_COLOR_HPP_
#define _ML_COLOR_HPP_

#include <modus_core/detail/Matrix.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	namespace util
	{
		template <class To, class From
		> ML_NODISCARD constexpr tvec4<To> color_cast(const tvec4<From> & value) noexcept
		{
			return (tvec4<To>)value;
		}

		ML_NODISCARD constexpr vec4b color_cast(vec4f const & value) noexcept
		{
			return (vec4b)(value * 255.f);
		}

		ML_NODISCARD constexpr vec4f color_cast(vec4b const & value) noexcept
		{
			return ((vec4f)value) / 255.f;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	template <class _T> struct ML_NODISCARD basic_color final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type					= typename basic_color<_T>;
		using value_type				= typename _T;
		using rgb_type					= typename tvec3<value_type>;
		using rgba_type					= typename tvec4<value_type>;
		using size_type					= typename rgba_type::size_type;
		using difference_type			= typename rgba_type::difference_type;
		using pointer					= typename rgba_type::pointer;
		using reference					= typename rgba_type::reference;
		using const_pointer				= typename rgba_type::const_pointer;
		using const_reference			= typename rgba_type::const_reference;
		using rvalue					= typename rgba_type::rvalue;
		using iterator					= typename rgba_type::iterator;
		using const_iterator			= typename rgba_type::const_iterator;
		using reverse_iterator			= typename rgba_type::reverse_iterator;
		using const_reverse_iterator	= typename rgba_type::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr basic_color(rgba_type const & value)
			: m_data{ value }
		{
		}

		constexpr basic_color(rgb_type const & rgb, value_type a)
			: self_type{ rgb[0], rgb[1], rgb[2], a }
		{
		}

		constexpr basic_color(value_type rgba)
			: m_data{ rgba, rgba, rgba, rgba }
		{
		}

		template <class R, class G, class B, class A
		> constexpr basic_color(R r, G g, B b, A a) : m_data{
			static_cast<value_type>(r),
			static_cast<value_type>(g),
			static_cast<value_type>(b),
			static_cast<value_type>(a)
		}
		{
		}

		template <class R, class G, class B
		> constexpr basic_color(R r, G g, B b) : self_type{ r, g, b, 1 }
		{
		}

		template <class U> constexpr basic_color(tvec4<U> const & value)
			: m_data{ util::color_cast(value) }
		{
		}

		template <class U> constexpr basic_color(basic_color<U> const & value)
			: m_data{ util::color_cast(value.rgba()) }
		{
		}

		constexpr basic_color()
			: m_data{ 0 }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		operator rgba_type & () & noexcept { return m_data; }

		constexpr operator rgba_type const & () const & noexcept { return m_data; }

		operator rgba_type && () && noexcept { return std::move(m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr operator pointer() noexcept { return m_data; }

		constexpr operator const_pointer() const noexcept { return m_data; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr auto r() const  noexcept-> value_type const & { return m_data[0]; }
		
		constexpr auto g() const  noexcept-> value_type const & { return m_data[1]; }
		
		constexpr auto b() const  noexcept-> value_type const & { return m_data[2]; }
		
		constexpr auto a() const  noexcept-> value_type const & { return m_data[3]; }
		
		constexpr auto rgb() const  noexcept-> rgb_type { return (rgb_type)m_data; }

		constexpr auto rgba() & noexcept-> rgba_type & { return m_data; }
		
		constexpr auto rgba() const & noexcept-> rgba_type const & { return m_data; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		constexpr auto begin() noexcept -> iterator { return m_data.begin(); }

		constexpr auto begin() const noexcept -> const_iterator { return m_data.begin(); }

		constexpr auto cbegin() const noexcept -> const_iterator { return m_data.cbegin(); }

		constexpr auto cend() const noexcept -> const_iterator { return m_data.cend(); }

		constexpr auto crbegin() const noexcept -> const_reverse_iterator { return m_data.crbegin(); }

		constexpr auto crend() const noexcept -> const_reverse_iterator { return m_data.crend(); }

		constexpr auto end() noexcept -> iterator { return m_data.end(); }

		constexpr auto end() const noexcept -> const_iterator { return m_data.end(); }

		constexpr auto rbegin() noexcept -> reverse_iterator { return m_data.rbegin(); }

		constexpr auto rbegin() const noexcept -> const_reverse_iterator { return m_data.rbegin(); }

		constexpr auto rend() noexcept -> reverse_iterator { return m_data.rend(); }

		constexpr auto rend() const noexcept -> const_reverse_iterator { return m_data.rend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		rgba_type m_data;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias color = typename basic_color<float_t>;
	
	ML_alias color32 = typename basic_color<byte_t>;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T
	> std::ostream & operator<<(std::ostream & out, basic_color<T> const & value)
	{
		return out << value.rgba();
	}

	template <class T
	> std::istream & operator>>(std::istream & in, basic_color<T> & value)
	{
		return in >> value.rgba();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	class colors final
	{
	public:
		static constexpr color clear	{ 0.0f, 0.0f, 0.0f, 0.0f };
		static constexpr color white	{ 1.0f, 1.0f, 1.0f, 1.0f };
		static constexpr color gray		{ 0.5f, 0.5f, 0.5f, 1.0f };
		static constexpr color black	{ 0.0f, 0.0f, 0.0f, 1.0f };

		static constexpr color red		{ 1.0f, 0.0f, 0.0f, 1.0f };
		static constexpr color green	{ 0.0f, 1.0f, 0.0f, 1.0f };
		static constexpr color blue		{ 0.0f, 0.0f, 1.0f, 1.0f };
		static constexpr color cyan		{ 0.0f, 1.0f, 1.0f, 1.0f };
		static constexpr color yellow	{ 1.0f, 1.0f, 0.0f, 1.0f };
		static constexpr color magenta	{ 1.0f, 0.0f, 1.0f, 1.0f };
		static constexpr color violet	{ 0.5f, 0.0f, 1.0f, 1.0f };
		static constexpr color lime		{ 0.5f, 1.0f, 0.0f, 1.0f };
		static constexpr color orange	{ 1.0f, 0.5f, 0.0f, 1.0f };
		static constexpr color fuchsia	{ 1.0f, 0.0f, 0.5f, 1.0f };
		static constexpr color aqua		{ 0.0f, 1.0f, 0.5f, 1.0f };
		static constexpr color azure	{ 0.0f, 0.5f, 1.0f, 1.0f };
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_COLOR_HPP_