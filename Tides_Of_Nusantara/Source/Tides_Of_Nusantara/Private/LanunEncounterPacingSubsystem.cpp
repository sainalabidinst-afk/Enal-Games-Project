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

	if (TimeSinceLastEncounter >= EncounterCooldownSeconds && ActiveEncounterCount < MaxSimultaneousEncounters)
	{
		OnEncounterReady.Broadcast(EEncounterTriggerReason::TimeElapsed, TimeSinceLastEncounter);
		TimeSinceLastEncounter = 0.0f;
	}
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
