#include "MineBlock.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"

DEFINE_LOG_CATEGORY(MineBlockLog);

namespace MineBlockInternal
{
    enum class CollisionHandling { None, Disable, Enable};
    void SetVisible(USceneComponent* component, const bool value, const CollisionHandling collision)
    {
        if (component)
        {
            component->SetHiddenInGame(!value, true);
            
            if (collision != CollisionHandling::None)
            {
                const bool enable = collision == CollisionHandling::Enable;
                
                auto& children = component->GetAttachChildren();
                
                for(USceneComponent* child : children)
                {
                    UPrimitiveComponent* primitive = Cast<UPrimitiveComponent>(child);
                    
                    if (primitive == nullptr)
                    {
                        continue;
                    }
                    else
                    {
                        const ECollisionEnabled::Type collisionType = enable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
                        primitive->SetCollisionEnabled(collisionType);
                    }
                }
            }
        }
    }
    
    //TODO: move them to a blueprintable configuration, but not in AMineBlock.
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
    
    SetActorTickEnabled(true);
    PrimaryActorTick.bCanEverTick = true;
}

void AMineBlock::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
    m_fsm.Process(deltaTime, *this);
}

void AMineBlock::BeginPlay()
{
    Super::BeginPlay();
    MineBlocker masterbuild(*this);
    m_fsm.SetState<SolidState>(masterbuild);
}

void AMineBlock::AddSupport()
{
    ++m_supported;
}

void AMineBlock::RemoveSupport()
{
    if (m_supported > 0)
    {
        --m_supported;
    }
}



bool AMineBlock::ShouldTickIfViewportsOnly() const
{
    return true;
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
        default:
        {
            UE_LOG(MineBlockLog, Log, TEXT("Entering unkown state (%d)"), m_fsm.GetStatusID());
        }
    }
}

void AMineBlock::EndInteraction()
{
    AInteractableActor::EndInteraction();
    
    // This because due to the FSM the destructors of SolidState and CollapsedState will interrupt the interaction and will call EndInteraction again
    // It's harmless because the object is not destroyed yet but I'll prevent to be safe.
    if (m_canBeInteracted == false)
    {
        return;
    }

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
        default: UE_LOG(MineBlockLog, Log, TEXT("Ending Unknown interaction."));
    }
}

// //=====================\\
// || BEGIN OF SolidState ||
// \\=====================//

AMineBlock::SolidState::SolidState(MineBlocker masterclass)
    : m_master(masterclass)
    , m_timer(MineBlockInternal::DigTime)
{
    AMineBlock& master = masterclass;
    master.m_currentStatus = State::Solid;
    master.m_canBeInteracted = true;
    MineBlockInternal::SetVisible(master.SolidProp, true, MineBlockInternal::CollisionHandling::Enable);
    MineBlockInternal::SetVisible(master.SupportedProp, false, MineBlockInternal::CollisionHandling::None);
    MineBlockInternal::SetVisible(master.UnsupportedProp, false, MineBlockInternal::CollisionHandling::None);
    MineBlockInternal::SetVisible(master.CollapsedProp, false, MineBlockInternal::CollisionHandling::Disable);
}

AMineBlock::SolidState::~SolidState()
{
    AMineBlock& master = m_master;
    master.m_canBeInteracted = false;
    master.Execute_SetInteractionDone(&master);
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
    : m_timer(MineBlockInternal::UnsupportTime)
{
    AMineBlock& master = masterclass;
    master.m_canBeInteracted = false;
    master.m_currentStatus = State::Unsupported;
    MineBlockInternal::SetVisible(master.SolidProp, false, MineBlockInternal::CollisionHandling::Disable);
    MineBlockInternal::SetVisible(master.SupportedProp, false, MineBlockInternal::CollisionHandling::None);
    MineBlockInternal::SetVisible(master.UnsupportedProp, true, MineBlockInternal::CollisionHandling::None);
    MineBlockInternal::SetVisible(master.CollapsedProp, false, MineBlockInternal::CollisionHandling::Disable);
}

void AMineBlock::UnsupportedState::ProcessFSM(const float deltaTime, AMineBlock& master)
{
    if (master.m_supported > 0)
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
    master.m_currentStatus = State::Supported;
    master.m_canBeInteracted = true;
    MineBlockInternal::SetVisible(master.SolidProp, false, MineBlockInternal::CollisionHandling::Disable);
    MineBlockInternal::SetVisible(master.SupportedProp, true, MineBlockInternal::CollisionHandling::None);
    MineBlockInternal::SetVisible(master.UnsupportedProp, false, MineBlockInternal::CollisionHandling::None);
    MineBlockInternal::SetVisible(master.CollapsedProp, false, MineBlockInternal::CollisionHandling::Disable);
}

void AMineBlock::SupportedState::ProcessFSM(const float deltaTime, AMineBlock& master)
{
    if (master.m_supported == 0)
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
    : m_master(master)
{
    master.m_currentStatus = State::Collapsed;
    master.m_canBeInteracted = true;
    MineBlockInternal::SetVisible(master.SolidProp, false, MineBlockInternal::CollisionHandling::Disable);
    MineBlockInternal::SetVisible(master.SupportedProp, false, MineBlockInternal::CollisionHandling::None);
    MineBlockInternal::SetVisible(master.UnsupportedProp, false, MineBlockInternal::CollisionHandling::None);
    MineBlockInternal::SetVisible(master.CollapsedProp, true, MineBlockInternal::CollisionHandling::Enable);
}

AMineBlock::CollapsedState::~CollapsedState()
{
    AMineBlock& master = m_master;
    master.m_canBeInteracted = false;
    master.Execute_SetInteractionDone(&master);
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

