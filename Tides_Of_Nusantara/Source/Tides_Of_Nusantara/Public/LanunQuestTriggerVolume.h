#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "NusantaraWorldStateSubsystem.h"
#include "LanunQuestTriggerVolume.generated.h"

UCLASS()
class TIDES_OF_NUSANTARA_API ALanunQuestTriggerVolume : public AActor
{
	GENERATED_BODY()

public:
	ALanunQuestTriggerVolume();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TriggerBox;

private:
	UPROPERTY(EditAnywhere, Category = "Quest")
	FName QuestIDToStart;

	UPROPERTY(EditAnywhere, Category = "Quest")
	FText QuestTitle;

	UPROPERTY(EditAnywhere, Category = "Quest")
	FName LocationID;

	UPROPERTY(EditAnywhere, Category = "Quest")
	bool bTriggerOnce = true;

	bool bHasTriggered = false;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
