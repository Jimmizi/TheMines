#pragma once

// //==========================\\
// || BEGIN OF static_fsm_impl ||
// \\==========================//

template<typename Executor, typename... Classes>
template<typename... FirstCtorArgs>
static_fsm_impl<Executor, Classes...>::static_fsm_impl(FirstCtorArgs&&... args)
    : m_fsm(std::forward<FirstCtorArgs>(args)...)
{
    
}

template<typename Executor, typename... Classes>
template<typename StateClass, typename... CtorArgs>
bool static_fsm_impl<Executor, Classes...>::SetState(CtorArgs&&... args)
{
    if(m_iterationLock)
    {
        Continue<StateClass>(std::forward<CtorArgs>(args)...);
        return false;
    }
    m_fsm.template emplace<StateClass>(std::forward<CtorArgs>(args)...);
    return true;
}

template<typename Executor, typename... Classes>
template<typename... ExecArgs>
void static_fsm_impl<Executor, Classes...>::Process(ExecArgs&&... args)
{
    m_iterationLock = true;
    Executor::Execute(m_fsm, std::forward<ExecArgs>(args)...);
    
    if (m_nextClassNoCtor)
    {
        std::invoke(m_nextClassNoCtor, m_fsm);
        m_nextClassNoCtor = nullptr;
    }
    m_iterationLock = false;
}

template<typename Executor, typename... Classes>
template<typename NextClass, typename... NextCtorArgs>
void static_fsm_impl<Executor, Classes...>::Continue(NextCtorArgs&&... args)
{
    m_nextClassNoCtor = [=](fsm_t& fsm){ fsm.template emplace<NextClass>(args...);};
}

template<typename Executor, typename... Classes>
size_t static_fsm_impl<Executor, Classes...>::GetStatusID() const
{
    return m_fsm.index();
}

// //========================\\
// || END OF static_fsm_impl ||
// \\========================//

// //===================\\
// || BEGIN OF Executor ||
// \\===================//

template<typename... Classes>
template<typename... ExecArgs>
inline void Executor<Classes...>::Execute(std::variant<Classes...>& fsm, ExecArgs&&... args)
{
    std::visit([&](auto&& current){ current.ProcessFSM(std::forward<ExecArgs>(args)...);}, fsm);
}

// //=================\\
// || END OF Executor ||
// \\=================//

