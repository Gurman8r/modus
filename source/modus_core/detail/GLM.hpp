#ifndef _ML_GLM_HPP_
#define _ML_GLM_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtx/quaternion.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef ML_MATRIX_CLASS_EXTRA
#define ML_MATRIX_CLASS_EXTRA \
\
template <size_t L, class U, glm::qualifier Q = glm::defaultp							\
> ML_NODISCARD operator glm::vec<L, U, Q>() const noexcept								\
{																						\
	using Other = glm::vec<L, U, Q>;													\
	if constexpr (std::is_same_v<value_type, U> && (_Width == L) && (_Height == 1))		\
	{																					\
		return *((Other *)this);														\
	}																					\
	else																				\
	{																					\
		Other temp{};																	\
		for (size_t i = 0; i < L; ++i)													\
		{																				\
			if (i < (_Width * _Height))													\
			{																			\
				temp[i] = static_cast<U>(this->at(i));									\
			}																			\
		}																				\
		return temp;																	\
	}																					\
}																						\
																						\
template <size_t W, size_t H, class U, glm::qualifier Q = glm::defaultp					\
> ML_NODISCARD operator glm::mat<W, H, U, Q>() const noexcept							\
{																						\
	using Other = glm::mat<W, H, U, Q>;													\
	if constexpr (std::is_same_v<value_type, U> && (_Width == W) && (_Height == H))		\
	{																					\
		return *((Other *)this);														\
	}																					\
	else																				\
	{																					\
		Other temp{};																	\
		for (size_t i = 0; i < (W * H); ++i)											\
		{																				\
			if (size_t const x{ i % W }, y{ i / W }; (x < _Width) && (y < _Height))		\
			{																			\
				temp[x][y] = static_cast<U>(this->at(x, y));							\
			}																			\
		}																				\
		return temp;																	\
	}																					\
}																						\
																						\
template <class U, glm::qualifier Q = glm::defaultp										\
> ML_NODISCARD operator glm::qua<U, Q>() const noexcept									\
{																						\
	using Other = glm::qua<U, Q>;														\
	if constexpr (std::is_same_v<value_type, U> && (_Width == 4) && (_Height == 1))		\
	{																					\
		return *((Other *)this);														\
	}																					\
	else																				\
	{																					\
		Other temp{};																	\
		for (size_t i = 0; i < 4; ++i)													\
		{																				\
			if (i < (_Width * _Height))													\
			{																			\
				temp[i] = static_cast<U>(this->at(i));									\
			}																			\
		}																				\
		return temp;																	\
	}																					\
}
#endif // ML_MATRIX_CLASS_EXTRA

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_GLM_HPP_