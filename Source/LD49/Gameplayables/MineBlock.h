#pragma once
/*!
* \class   AMineBlock
* \author  Ruggero Riccobene
* \date    02/10/2021

* \brief
digraph Block
{
    Solid
    Supported
    Unsupported
    Collapsed
    
    Solid -> Unsupported [label="you use a tool"]
    Supported -> Unsupported [label="there is beam within 7x7"];
    Unsupported -> Supported [label="no beam within 7x7"]
    Unsupported -> Collapsed [label="timer expires"]
    Collapsed -> Unsupported [label="you use a tool"]
}
*/

#include "LD49/Interaction/InteractableActor.h"
#include "LD49/utils/fsm.h"
#include "MineBlock.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MineBlockLog, Log, All);

UCLASS()
class LD49_API AMineBlock : public AInteractableActor
{
    GENERATED_BODY()
    
public:
    AMineBlock();
    
    UFUNCTION(BlueprintCallable)
    void AddSupport();
    UFUNCTION(BlueprintCallable)
    void RemoveSupport();
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnDug();
private:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable", meta = (AllowPrivateAccess = "true"))
    USceneComponent* SolidProp;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable", meta = (AllowPrivateAccess = "true"))
    USceneComponent* UnsupportedProp;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable", meta = (AllowPrivateAccess = "true"))
    USceneComponent* SupportedProp;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable", meta = (AllowPrivateAccess = "true"))
    USceneComponent* CollapsedProp;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable", meta = (AllowPrivateAccess = "true"))
    float DiggingTime{3.f};
    
    uint32_t m_supported{0};

    void Tick(float deltaTime) override;
    void BeginPlay() override;
    
    bool ShouldTickIfViewportsOnly() const override;
    
    void StartInteraction() override;
    void EndInteraction() override;
    
    using MineBlocker = std::reference_wrapper<AMineBlock>;
    enum class State {Idle, Solid, Unsupported, Supported, Collapsed };
    State m_currentStatus{ State::Idle };
    
    // //=====\\
    // || FSM ||
    // \\=====//
    struct IdleState final
    {
        IdleState() {};
        void ProcessFSM(const float deltaTime, AMineBlock& master) {};
    };
    
    /*!
     * Initial state, can be interacted and if the interaction continues for the duration, it moves to Unsupported State
     */
    struct SolidState
    {
        SolidState(MineBlocker master);
        void ProcessFSM(const float deltaTime, AMineBlock& master);
        void StartInteraction();
        void EndInteraction();
        ~SolidState();
    private:
        MineBlocker m_master;
        bool m_interacting{false};
        float m_timer{0.f};
    };
    
    /*!
     * Unsupported, cannot be interacted. If this block is not set as supported a timer starts and if that expires we'll collapse.
     */
    struct UnsupportedState final
    {
        UnsupportedState(MineBlocker master);
        void ProcessFSM(const float deltaTime, AMineBlock& master);
    private:
        float m_timer{0.f};
    };
    
    /*!
     * Supported: it will stay in this state until this block is supported.
     */
    struct SupportedState final
    {
        SupportedState(MineBlocker master);
        void ProcessFSM(const float deltaTime, AMineBlock& master);
    };
    
    /*!
     * Collapsed: similar to Solid but the prop shown might be different.
     */
    struct CollapsedState final
    {
        CollapsedState(AMineBlock& master);
        ~CollapsedState();
        void ProcessFSM(const float deltaTime, AMineBlock& master);
        void StartInteraction();
        void EndInteraction();
        
    private:
        MineBlocker m_master;
        bool m_interacting{false};
        float m_timer{2.f};
    };
    
    using fsm_t = static_fsm<IdleState, SolidState, UnsupportedState, SupportedState, CollapsedState>;
    
    fsm_t m_fsm;
};
