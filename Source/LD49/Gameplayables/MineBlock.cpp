#include "MineBlock.h"
#include "Components/SceneComponent.h"

namespace MineBlockInternal
{
    void SetVisible(USceneComponent* component, const bool value)
    {
        if (component)
        {
            component->SetVisibility(value, true);
        }
    }
    
    //TODO: move them to a blueprintable configuration, but not in these blocks.
    constexpr float DigTime{3.f};
    constexpr float UnsupportTime{2.f};
}

AMineBlock::AMineBlock()
{
    if (SceneRoot)
    {
        SolidProp = CreateDefaultSubobject<USceneComponent>(TEXT("SolidProp"));
        SolidProp->SetupAttachment(SceneRoot);
        
        UnsupportedProp = CreateDefaultSubobject<USceneComponent>(TEXT("UnsupportedProp"));
        UnsupportedProp->SetupAttachment(SceneRoot);
        
        SupportedProp = CreateDefaultSubobject<USceneComponent>(TEXT("SupportedProp"));
        SupportedProp->SetupAttachment(SceneRoot);
        
        CollapsedProp = CreateDefaultSubobject<USceneComponent>(TEXT("CollapsedProp"));
        CollapsedProp->SetupAttachment(SceneRoot);
    }
    
    PrimaryActorTick.bCanEverTick = true;
}

void AMineBlock::Tick(const float deltaTime)
{
    Super::Tick(deltaTime);
    m_fsm.Process(deltaTime, *this);
}

void AMineBlock::StartInteraction()
{
    AInteractableActor::StartInteraction();

    auto CallStart = [](auto* type)
    {
        if (type)
        {
            type->StartInteraction();
        }
    };
    
    switch(m_currentStatus)
    {
        case (State::Collapsed):
        {
            CallStart(m_fsm.GetCurrentState<CollapsedState>());
            break;
        }
        case (State::Solid):
        {
            CallStart(m_fsm.GetCurrentState<SolidState>());
            break;
        }
    }
}

void AMineBlock::EndInteraction()
{
    AInteractableActor::StartInteraction();

    auto CallEnd = [](auto* type)
    {
        if (type)
        {
            type->EndInteraction();
        }
    };
    
    switch(m_currentStatus)
    {
        case (State::Collapsed):
        {
            CallEnd(m_fsm.GetCurrentState<CollapsedState>());
            break;
        }
        case (State::Solid):
        {
            CallEnd(m_fsm.GetCurrentState<SolidState>());
            break;
        }
    }
}

// //=====================\\
// || BEGIN OF SolidState ||
// \\=====================//

AMineBlock::SolidState::SolidState(MineBlocker masterclass)
{
    AMineBlock& master = masterclass;
    MineBlockInternal::SetVisible(master.SolidProp, true);
    MineBlockInternal::SetVisible(master.SupportedProp, false);
    MineBlockInternal::SetVisible(master.UnsupportedProp, false);
    MineBlockInternal::SetVisible(master.CollapsedProp, false);
}

void AMineBlock::SolidState::ProcessFSM(const float deltaTime, AMineBlock& master)
{
    if (m_interacting)
    {
        m_timer -= deltaTime;
        if (m_timer < 0.f)
        {
            MineBlocker masterclass(master);
            master.m_fsm.Continue<UnsupportedState>(masterclass);
        }
    }
}

void AMineBlock::SolidState::StartInteraction()
{
    m_interacting = true;
    m_timer = MineBlockInternal::DigTime;
}

void AMineBlock::SolidState::EndInteraction()
{
    m_interacting = false;
}


// //===================\\
// || END OF SolidState ||
// \\===================//

// //===========================\\
// || BEGIN OF UnsupportedState ||
// \\===========================//

AMineBlock::UnsupportedState::UnsupportedState(MineBlocker masterclass)
{
    AMineBlock& master = masterclass;
    MineBlockInternal::SetVisible(master.SolidProp, false);
    MineBlockInternal::SetVisible(master.SupportedProp, false);
    MineBlockInternal::SetVisible(master.UnsupportedProp, true);
    MineBlockInternal::SetVisible(master.CollapsedProp, false);
}

void AMineBlock::UnsupportedState::ProcessFSM(const float deltaTime, AMineBlock& master)
{
    if (master.m_supported)
    {
        m_timer = MineBlockInternal::UnsupportTime;
    }
    else
    {
        m_timer -= deltaTime;
        
        if (m_timer < 0.f)
        {
            MineBlocker mastercard(master);
            master.m_fsm.Continue<CollapsedState>(mastercard);
        }
    }
}

// //=========================\\
// || END OF UnsupportedState ||
// \\=========================//

// //=========================\\
// || BEGIN OF SupportedState ||
// \\=========================//

AMineBlock::SupportedState::SupportedState(MineBlocker masterclass)
{
    AMineBlock& master = masterclass;
    MineBlockInternal::SetVisible(master.SolidProp, false);
    MineBlockInternal::SetVisible(master.SupportedProp, true);
    MineBlockInternal::SetVisible(master.UnsupportedProp, false);
    MineBlockInternal::SetVisible(master.CollapsedProp, false);
}

void AMineBlock::SupportedState::ProcessFSM(const float deltaTime, AMineBlock& master)
{
    if (master.m_supported == false)
    {
        MineBlocker mastermind(master);
        master.m_fsm.Continue<UnsupportedState>(mastermind);
    }
}

// //=======================\\
// || END OF SupportedState ||
// \\=======================//

// //=========================\\
// || BEGIN OF CollapsedState ||
// \\=========================//

AMineBlock::CollapsedState::CollapsedState(AMineBlock& master)
{
    MineBlockInternal::SetVisible(master.SolidProp, false);
    MineBlockInternal::SetVisible(master.SupportedProp, false);
    MineBlockInternal::SetVisible(master.UnsupportedProp, false);
    MineBlockInternal::SetVisible(master.CollapsedProp, true);
}

void AMineBlock::CollapsedState::ProcessFSM(const float deltaTime, AMineBlock& master)
{
    if (m_interacting)
    {
        m_timer -= deltaTime;
        if (m_timer < 0.f)
        {
            MineBlocker masterclass(master);
            master.m_fsm.Continue<UnsupportedState>(masterclass);
        }
    }
}

void AMineBlock::CollapsedState::StartInteraction()
{
    m_interacting = true;
    m_timer = MineBlockInternal::DigTime;
}

void AMineBlock::CollapsedState::EndInteraction()
{
    m_interacting = false;
}
// //=======================\\
// || END OF CollapsedState ||
// \\=======================//

