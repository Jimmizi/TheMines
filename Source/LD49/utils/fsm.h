/*!
 * \class   fsm
 * \author  Ruggero Riccobene
 * \date    01/10/2020
 * 
 * \brief   Static-time Finished State Machine
 */
#pragma once
#include <variant>
#include <functional>

template<typename... Classes>
struct Executor
{
    template<typename... ExecArgs>
    static inline void Execute(std::variant<Classes...>& fsm, ExecArgs&&... args);
};

template<typename Executor, typename... Classes>
struct static_fsm_impl
{
    template<typename... FirstCtorArgs>
    static_fsm_impl(FirstCtorArgs&&...);
        
    template<typename StateClass, typename... CtorArgs>
    bool SetState(CtorArgs&&... args);
    
    template<typename... ExecArgs>
    void Process(ExecArgs&&... args);
    
    template<typename NextClass, typename... NextCtorArgs>
    void Continue(NextCtorArgs&&... args);
    
    size_t GetStatusID() const;
private:
    using fsm_t = std::variant<Classes...>;
    
    std::function<void(fsm_t&)> m_nextClassNoCtor;
    fsm_t m_fsm;
    bool m_iterationLock{false};
};


template<typename... Args>
using static_fsm = static_fsm_impl<Executor<Args...>, Args...>;

template<template<typename...> class ExecutorClass, typename... Args>
using static_fsm_ex = static_fsm_impl<ExecutorClass<Args...>, Args...>;

#include "fsm.inl"
