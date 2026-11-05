#include "LanunQuestTriggerVolume.h"
#include "NusantaraWorldStateSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ALanunQuestTriggerVolume::ALanunQuestTriggerVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetBoxExtent(FVector(200.0f, 200.0f, 200.0f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = TriggerBox;
}

void ALanunQuestTriggerVolume::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALanunQuestTriggerVolume::OnTriggerBeginOverlap);
}

void ALanunQuestTriggerVolume::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasTriggered && bTriggerOnce)
	{
		return;
	}

	// Cek apakah yang overlap adalah player
	if (!OtherActor || !OtherActor->IsA<APlayerController>())
	{
		return;
	}

	// Set lokasi terakhir player
	if (APlayerController* PC = Cast<APlayerController>(OtherActor))
	{
		if (UNusantaraWorldStateSubsystem* WorldState = PC->GetWorld()->GetSubsystem<UNusantaraWorldStateSubsystem>())
		{
			WorldState->SetLastLocation(LocationID);
		}
	}

	// Start quest jika QuestID ditentukan
	if (!QuestIDToStart.IsNone())
	{
		if (UNusantaraWorldStateSubsystem* WorldState = GetWorld()->GetSubsystem<UNusantaraWorldStateSubsystem>())
		{
			WorldState->AddQuestToJournal(QuestIDToStart, QuestTitle);
			WorldState->UpdateQuestInJournal(QuestIDToStart, EQuestState::InProgress, 0);
			WorldState->OnFirstQuestStarted.Broadcast(QuestIDToStart);
		}
	}

	bHasTriggered = true;
}
