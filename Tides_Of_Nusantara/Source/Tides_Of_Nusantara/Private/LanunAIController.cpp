#include "LanunAIController.h"

#include "LanunCharacterHeroBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ALanunAIController::ALanunAIController()
{
    LanunPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("LanunPerceptionComponent"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    if (SightConfig && LanunPerceptionComponent)
    {
        SightConfig->SightRadius = 1500.0f;
        SightConfig->LoseSightRadius = 2000.0f;
        SightConfig->PeripheralVisionAngleDegrees = 65.0f;
        SightConfig->SetMaxAge(5.0f);
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

        LanunPerceptionComponent->ConfigureSense(*SightConfig);
        LanunPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    }
}

void ALanunAIController::BeginPlay()
{
    Super::BeginPlay();

    if (LanunPerceptionComponent)
    {
        LanunPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ALanunAIController::HandleTargetPerceived);
    }
}

void ALanunAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    const ALanunAICharacterBase* AICharacter = Cast<ALanunAICharacterBase>(InPawn);
    if (AICharacter)
    {
        MyFaction = AICharacter->Faction;
    }
}

bool ALanunAIController::IsHostileTowards(ELanunFaction OtherFaction) const
{
    switch (MyFaction)
    {
    case ELanunFaction::KongsiDagangBarat:
        return OtherFaction == ELanunFaction::PelautBebas || OtherFaction == ELanunFaction::Kesultanan;
    case ELanunFaction::PelautBebas:
        return OtherFaction == ELanunFaction::KongsiDagangBarat;
    case ELanunFaction::Kesultanan:
        return OtherFaction == ELanunFaction::KongsiDagangBarat;
    default:
        return false;
    }
}

void ALanunAIController::HandleTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor)
    {
        return;
    }

    if (!Stimulus.WasSuccessfullySensed())
    {
        if (CurrentTargetActor == Actor)
        {
            CurrentTargetActor = nullptr;
        }
        return;
    }

    ELanunFaction TargetFaction = ELanunFaction::Masyarakat;
    if (Actor->IsA(ALanunCharacterHeroBase::StaticClass()))
    {
        TargetFaction = ELanunFaction::PelautBebas;
    }
    else
    {
        const ALanunAICharacterBase* OtherAI = Cast<ALanunAICharacterBase>(Actor);
        if (OtherAI)
        {
            if (!OtherAI->IsAlive())
            {
                return;
            }
            TargetFaction = OtherAI->Faction;
        }
    }

    if (IsHostileTowards(TargetFaction))
    {
        CurrentTargetActor = Actor;
    }
}
