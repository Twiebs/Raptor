#ifndef RAPTOR_METAFUCK_HPP
#define RAPTOR_METAFUCK_HPP

template <template <typename...> class TNewName, typename T>
struct RenameHelper;

template <template <typename...> class TNewName,
		  template <typename...> class TOldName,
		  typename... TArgs>

		struct RenameHelper<TNewName, TOldName<TArgs...>> {
			using type = TNewName<TArgs...>;
		};

template <template <typename...> class TNewName, typename T>
using Rename = typename RenameHelper<TNewName, T>::type;

template <typename TTypeList>
using Tuple = Rename<std::tuple, TTypeList>;

template <typename... TArgs>
struct TypeList { static constexpr std::size_t Size () { return sizeof...(TArgs); } };

template<typename, typename>
struct IndexOf;

template<typename T, typename... Ts>
struct IndexOf<T, TypeList<T, Ts...>> : std::integral_constant<std::size_t, 0> { };

template<typename T, typename TOther, typename... Ts>
struct IndexOf<T, TypeList<TOther, Ts...>>
		: std::integral_constant<std::size_t, 1 + IndexOf<T, TypeList<Ts...>>{}> { };

template <std::size_t TIndex, typename TTypeList>
using AtIndex = std::tuple_element_t<TIndex, Tuple<TTypeList>>;

#endif //RAPTOR_METAFUCK_HPP
