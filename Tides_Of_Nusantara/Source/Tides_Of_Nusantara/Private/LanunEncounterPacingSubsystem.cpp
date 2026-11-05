#include "LanunEncounterPacingSubsystem.h"

void ULanunEncounterPacingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TimeSinceLastEncounter = 0.0f;
	ActiveEncounterCount = 0;
	LastEncounterTime = -9999.0f;
}

void ULanunEncounterPacingSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void ULanunEncounterPacingSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DeltaTime <= 0.0f)
	{
		return;
	}

	TimeSinceLastEncounter += DeltaTime;

	const bool bTimeReady = TimeSinceLastEncounter >= EncounterCooldownSeconds;
	const bool bDistanceReady = bHasEncounterPosition &&
		(GetPlayerShipPosition() - LastEncounterPosition).Size() >= MinDistanceSinceLastEncounter;

	if ((bTimeReady || bDistanceReady) && ActiveEncounterCount < MaxSimultaneousEncounters)
	{
		const EEncounterTriggerReason Reason = bDistanceReady ? EEncounterTriggerReason::DistanceTraveled : EEncounterTriggerReason::TimeElapsed;
		OnEncounterReady.Broadcast(Reason, TimeSinceLastEncounter);
		TimeSinceLastEncounter = 0.0f;
		bHasEncounterPosition = false;
	}
}

FVector ULanunEncounterPacingSubsystem::GetPlayerShipPosition() const
{
	if (!GetWorld())
	{
		return FVector::ZeroVector;
	}

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			return Pawn->GetActorLocation();
		}
	}

	return FVector::ZeroVector;
}

bool ULanunEncounterPacingSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

bool ULanunEncounterPacingSubsystem::IsEncounterOnCooldown() const
{
	return TimeSinceLastEncounter < EncounterCooldownSeconds;
}

bool ULanunEncounterPacingSubsystem::CanTriggerEncounter() const
{
	return !IsEncounterOnCooldown() && ActiveEncounterCount < MaxSimultaneousEncounters;
}

float ULanunEncounterPacingSubsystem::GetTimeSinceLastEncounter() const
{
	return TimeSinceLastEncounter;
}

void ULanunEncounterPacingSubsystem::OnEncounterStarted()
{
	TimeSinceLastEncounter = 0.0f;
	LastEncounterTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	LastEncounterPosition = GetPlayerShipPosition();
	bHasEncounterPosition = true;
	++ActiveEncounterCount;
}

void ULanunEncounterPacingSubsystem::OnEncounterEnded()
{
	if (ActiveEncounterCount > 0)
	{
		--ActiveEncounterCount;
	}
}

int32 ULanunEncounterPacingSubsystem::GetActiveEncounterCount() const
{
	return ActiveEncounterCount;
}
