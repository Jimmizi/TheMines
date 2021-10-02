#pragma once
#include <type_traits>
#include <tuple>

template<typename Type>
struct template_trait;

template<template <typename...> class Class, typename... Args>
struct template_trait<Class<Args...>>
{
    static constexpr const std::size_t ArgsCount = sizeof...(Args);
    
    template<std::size_t Index>
    using param_t = typename std::tuple_element<Index, std::tuple<Args...>>::type;
};

template<template <typename...> class Class, typename... Args>
struct template_trait<const Class<Args...>>
{
    static constexpr const std::size_t ArgsCount = sizeof...(Args);
    
    template<std::size_t Index>
    using param_t = typename std::tuple_element<Index, std::tuple<Args...>>::type;
};
