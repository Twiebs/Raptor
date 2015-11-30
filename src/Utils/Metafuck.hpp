#ifndef RAPTOR_METAFUCK_HPP
#define RAPTOR_METAFUCK_HPP

#include <tuple>
#include <type_traits>

template <typename...TArgs>
struct TypeList { 
	template<size_t n>
	using type = typename std::tuple_element<n, std::tuple<TArgs...>>::type;

	static constexpr std::size_t Size() { 
		return sizeof...(TArgs); 
	}
};

template<typename, typename>
struct IndexOfType;
template<typename TType, typename... TTypeList>
struct IndexOfType<TType, TypeList<TType, TTypeList...>>
	: std::integral_constant<size_t, 0> { };
template<typename TType, typename TOther, typename... TTypes>
struct IndexOfType<TType, TypeList<TOther, TTypes...>>
	: std::integral_constant<size_t, 1 + IndexOfType<TType, TypeList<TTypes...>>::value> { };


#endif //RAPTOR_METAFUCK_HPP
