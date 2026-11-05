#include "RivalNemesisSubsystem.h"

void URivalNemesisSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void URivalNemesisSubsystem::RecordCombatAction(FName CaptainID, FName ActionType)
{
	FRivalProfile& Profile = RivalRegistry.FindOrAdd(CaptainID);
	if (Profile.CaptainID.IsNone())
	{
		Profile.CaptainID = CaptainID;
		Profile.State = ERivalRelationshipState::Neutral;
		Profile.GrudgeScore = 0;
	}

	Profile.TacticalMemory.RecordAction(ActionType);
}

void URivalNemesisSubsystem::ModifyGrudgeScore(FName CaptainID, int32 Delta)
{
	FRivalProfile& Profile = RivalRegistry.FindOrAdd(CaptainID);
	if (Profile.CaptainID.IsNone())
	{
		Profile.CaptainID = CaptainID;
		Profile.State = ERivalRelationshipState::Neutral;
		Profile.GrudgeScore = 0;
	}

	Profile.GrudgeScore = FMath::Clamp(Profile.GrudgeScore + Delta, -100, 100);
	Profile.State = GetRelationshipStateFromGrudge(Profile.GrudgeScore);
}

FRivalProfile URivalNemesisSubsystem::GetRivalProfile(FName CaptainID) const
{
	const FRivalProfile* Profile = RivalRegistry.Find(CaptainID);
	if (Profile)
	{
		return *Profile;
	}
	return FRivalProfile();
}

ERivalRelationshipState URivalNemesisSubsystem::GetRelationshipStateFromGrudge(int32 GrudgeScore)
{
	if (GrudgeScore < -60)
	{
		return ERivalRelationshipState::Nemesis;
	}
	if (GrudgeScore > 50)
	{
		return ERivalRelationshipState::Ally;
	}
	if (GrudgeScore < 0)
	{
		return ERivalRelationshipState::Rival;
	}
	if (GrudgeScore > 0)
	{
		return ERivalRelationshipState::Neutral;
	}
	return ERivalRelationshipState::Unmet;
}

TMap<FName, int32> URivalNemesisSubsystem::GetAllRivalGrudgeStates() const
{
	TMap<FName, int32> GrudgeStates;
	for (const TPair<FName, FRivalProfile>& Pair : RivalRegistry)
	{
		GrudgeStates.Add(Pair.Key, Pair.Value.GrudgeScore);
	}
	return GrudgeStates;
}

void URivalNemesisSubsystem::LoadRivalGrudgeStates(const TMap<FName, int32>& GrudgeStates)
{
	RivalRegistry.Empty();
	for (const TPair<FName, int32>& Pair : GrudgeStates)
	{
		FRivalProfile& Profile = RivalRegistry.Add(Pair.Key);
		Profile.CaptainID = Pair.Key;
		Profile.State = GetRelationshipStateFromGrudge(Pair.Value);
		Profile.GrudgeScore = Pair.Value;
	}
}
