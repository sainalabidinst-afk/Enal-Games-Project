#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "LanunAICharacterBase.h"
#include "LanunAIController.generated.h"

UCLASS()
class TIDES_OF_NUSANTARA_API ALanunAIController : public AAIController
{
    GENERATED_BODY()

public:
    ALanunAIController();

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
    UAIPerceptionComponent* LanunPerceptionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
    UAISenseConfig_Sight* SightConfig;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI State")
    AActor* CurrentTargetActor = nullptr;

    UFUNCTION(BlueprintPure, Category = "AI Faction")
    bool IsHostileTowards(ELanunFaction OtherFaction) const;

protected:
    UFUNCTION()
    void HandleTargetPerceived(AActor* Actor, FAIStimulus Stimulus);

    ELanunFaction MyFaction = ELanunFaction::KongsiDagangBarat;
};
