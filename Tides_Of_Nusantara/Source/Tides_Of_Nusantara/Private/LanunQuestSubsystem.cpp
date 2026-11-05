#include "LanunQuestSubsystem.h"
#include "LanunAttributeSetBase.h"
#include "LanunHeroProgressionSubsystem.h"
#include "RivalNemesisSubsystem.h"
#include "Kismet/GameplayStatics.h"

void ULanunQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void ULanunQuestSubsystem::Deinitialize()
{
	QuestDefinitions.Empty();
	QuestSaveData.Empty();
	Super::Deinitialize();
}

void ULanunQuestSubsystem::RegisterQuestDefinition(const FLanunQuestData& QuestData)
{
	if (!QuestData.QuestID.IsNone())
	{
		QuestDefinitions.Add(QuestData.QuestID, QuestData);
	}
}

bool ULanunQuestSubsystem::GetQuestDefinition(FName QuestID, FLanunQuestData& OutQuestData) const
{
	const FLanunQuestData* Found = QuestDefinitions.Find(QuestID);
	if (Found)
	{
		OutQuestData = *Found;
		return true;
	}
	return false;
}

bool ULanunQuestSubsystem::CanStartQuest(FName QuestID) const
{
	const FLanunQuestData* QuestDef = QuestDefinitions.Find(QuestID);
	if (!QuestDef)
	{
		return false;
	}

	const FLanunQuestSaveData* SaveData = QuestSaveData.Find(QuestID);
	if (SaveData && SaveData->State != EQuestState::NotStarted)
	{
		return false;
	}

	// Check prerequisites
	for (const FName& PrereqID : QuestDef->PrerequisiteQuestIDs)
	{
		const FLanunQuestSaveData* PrereqSave = QuestSaveData.Find(PrereqID);
		if (!PrereqSave || PrereqSave->State != EQuestState::Completed)
		{
			return false;
		}
	}

	// Check level requirement via HeroProgressionSubsystem
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		if (ULanunHeroProgressionSubsystem* Prog = GI->GetSubsystem<ULanunHeroProgressionSubsystem>())
		{
			if (Prog->GetProgressionState().Level < QuestDef->MinLevel)
			{
				return false;
			}
		}
	}

	return true;
}

bool ULanunQuestSubsystem::StartQuest(FName QuestID, int32 CurrentDay)
{
	if (!CanStartQuest(QuestID))
	{
		return false;
	}

	FLanunQuestSaveData& SaveData = QuestSaveData.FindOrAdd(QuestID);
	SaveData.QuestID = QuestID;
	SaveData.State = EQuestState::InProgress;
	SaveData.CurrentObjectiveIndex = 0;
	SaveData.AcceptedDay = CurrentDay;

	// Initialize objective progress
	SaveData.ObjectiveProgress.Reset();
	SaveData.ObjectiveCompleted.Reset();

	const FLanunQuestData* QuestDef = QuestDefinitions.Find(QuestID);
	if (QuestDef)
	{
		for (const FQuestObjective& Obj : QuestDef->Objectives)
		{
			SaveData.ObjectiveProgress.Add(0);
			SaveData.ObjectiveCompleted.Add(false);
		}
	}

	OnQuestStateChanged.Broadcast(QuestID);
	return true;
}

bool ULanunQuestSubsystem::UpdateObjective(FName QuestID, EQuestObjectiveType ObjectiveType, FName TargetID, int32 Amount)
{
	FLanunQuestSaveData* SaveData = QuestSaveData.Find(QuestID);
	if (!SaveData || SaveData->State != EQuestState::InProgress)
	{
		return false;
	}

	const FLanunQuestData* QuestDef = QuestDefinitions.Find(QuestID);
	if (!QuestDef)
	{
		return false;
	}

	int32 CurrentObjIdx = SaveData->CurrentObjectiveIndex;
	if (CurrentObjIdx >= QuestDef->Objectives.Num())
	{
		return false;
	}

	FQuestObjective& CurrentObj = const_cast<FQuestObjective&>(QuestDef->Objectives[CurrentObjIdx]);

	// Check if this objective matches
	if (CurrentObj.ObjectiveType != ObjectiveType)
	{
		return false;
	}

	// Check target ID matches
	if (!CurrentObj.TargetNPCID.IsNone() && CurrentObj.TargetNPCID != TargetID)
	{
		return false;
	}
	if (!CurrentObj.TargetLocationID.IsNone() && CurrentObj.TargetLocationID != TargetID)
	{
		return false;
	}
	if (!CurrentObj.TargetItemID.IsNone() && CurrentObj.TargetItemID != TargetID)
	{
		return false;
	}

	// Update progress
	SaveData->ObjectiveProgress[CurrentObjIdx] += Amount;
	if (SaveData->ObjectiveProgress[CurrentObjIdx] >= CurrentObj.RequiredAmount)
	{
		SaveData->ObjectiveCompleted[CurrentObjIdx] = true;
		CurrentObj.bIsCompleted = true;
	}

	OnObjectiveUpdated.Broadcast(QuestID, CurrentObjIdx);

	CheckQuestCompletion(QuestID);
	return true;
}

void ULanunQuestSubsystem::CheckQuestCompletion(FName QuestID)
{
	FLanunQuestSaveData* SaveData = QuestSaveData.Find(QuestID);
	if (!SaveData || SaveData->State != EQuestState::InProgress)
	{
		return;
	}

	const FLanunQuestData* QuestDef = QuestDefinitions.Find(QuestID);
	if (!QuestDef)
	{
		return;
	}

	// Check if all objectives completed
	bool bAllCompleted = true;
	for (bool bCompleted : SaveData->ObjectiveCompleted)
	{
		if (!bCompleted)
		{
			bAllCompleted = false;
			break;
		}
	}

	if (bAllCompleted)
	{
		CompleteQuest(QuestID);
	}
}

bool ULanunQuestSubsystem::CompleteQuest(FName QuestID)
{
	FLanunQuestSaveData* SaveData = QuestSaveData.Find(QuestID);
	if (!SaveData || SaveData->State != EQuestState::InProgress)
	{
		return false;
	}

	SaveData->State = EQuestState::Completed;

	const FLanunQuestData* QuestDef = QuestDefinitions.Find(QuestID);
	if (QuestDef)
	{
		ApplyQuestRewards(*QuestDef);
	}

	OnQuestCompleted.Broadcast(QuestID);
	OnQuestStateChanged.Broadcast(QuestID);
	return true;
}

void ULanunQuestSubsystem::ApplyQuestRewards(const FLanunQuestData& QuestData)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return;
	}

	// Apply Rupiah
	if (QuestData.Rewards.RupiahReward > 0)
	{
		if (ULanunHeroProgressionSubsystem* Prog = GI->GetSubsystem<ULanunHeroProgressionSubsystem>())
		{
			// We need to add Rupiah via AttributeSet
			// For now, stored for UI binding
		}
	}

	// Apply XP
	if (QuestData.Rewards.XPReward > 0)
	{
		if (ULanunHeroProgressionSubsystem* Prog = GI->GetSubsystem<ULanunHeroProgressionSubsystem>())
		{
			Prog->AddXP(QuestData.Rewards.XPReward);
		}
	}

	// Apply Attribute Points
	if (QuestData.Rewards.AttributePointReward > 0)
	{
		if (ULanunHeroProgressionSubsystem* Prog = GI->GetSubsystem<ULanunHeroProgressionSubsystem>())
		{
			const FHeroProgressionState State = Prog->GetProgressionState();
			int32 NewAP = State.UnallocatedAttributePoints + QuestData.Rewards.AttributePointReward;
			// Note: Need setter on HeroProgressionSubsystem for production use
		}
	}

	// Apply Skill Points
	if (QuestData.Rewards.SkillPointReward > 0)
	{
		if (ULanunHeroProgressionSubsystem* Prog = GI->GetSubsystem<ULanunHeroProgressionSubsystem>())
		{
			// Skill point reward logic
		}
	}

	// Apply Reputation changes
	if (QuestData.Rewards.FactionReputationChange != 0 ||
		QuestData.Rewards.TerritoryPopularityChange != 0 ||
		QuestData.Rewards.CrewLoyaltyChange != 0)
	{
		if (ULanunAttributeSetBase* AttrSet = Cast<ULanunAttributeSetBase>(
			GI->GetWorld()->GetFirstPlayerController()->GetPawn()->GetComponentByClass(ULanunAttributeSetBase::StaticClass())))
		{
			if (QuestData.Rewards.FactionReputationChange != 0)
			{
				float NewRep = AttrSet->GetFactionReputation() + QuestData.Rewards.FactionReputationChange;
				AttrSet->SetFactionReputation(FMath::Clamp(NewRep, -100.0f, 100.0f));
			}
			if (QuestData.Rewards.TerritoryPopularityChange != 0)
			{
				float NewPop = AttrSet->GetTerritoryPopularity() + QuestData.Rewards.TerritoryPopularityChange;
				AttrSet->SetTerritoryPopularity(FMath::Clamp(NewPop, -100.0f, 100.0f));
			}
			if (QuestData.Rewards.CrewLoyaltyChange != 0)
			{
				float NewLoyalty = AttrSet->GetCrewLoyalty() + QuestData.Rewards.CrewLoyaltyChange;
				AttrSet->SetCrewLoyalty(FMath::Clamp(NewLoyalty, -100.0f, 100.0f));
			}
		}
	}

	// Unlock skill
	if (!QuestData.Rewards.UnlockSkillID.IsNone())
	{
		// Grant skill unlock via gameplay tag or quest completion tag
	}
}

EQuestState ULanunQuestSubsystem::GetQuestState(FName QuestID) const
{
	const FLanunQuestSaveData* SaveData = QuestSaveData.Find(QuestID);
	if (SaveData)
	{
		return SaveData->State;
	}

	// Not started yet - check if available
	const FLanunQuestData* QuestDef = QuestDefinitions.Find(QuestID);
	if (QuestDef && CanStartQuest(QuestID))
	{
		return EQuestState::Available;
	}

	return EQuestState::NotStarted;
}

TArray<FLanunQuestData> ULanunQuestSubsystem::GetAvailableQuests() const
{
	TArray<FLanunQuestData> Result;
	for (const TPair<FName, FLanunQuestData>& Pair : QuestDefinitions)
	{
		if (CanStartQuest(Pair.Key))
		{
			Result.Add(Pair.Value);
		}
	}
	return Result;
}

TArray<FLanunQuestData> ULanunQuestSubsystem::GetActiveQuests() const
{
	TArray<FLanunQuestData> Result;
	for (const TPair<FName, FLanunQuestSaveData>& Pair : QuestSaveData)
	{
		if (Pair.Value.State == EQuestState::InProgress)
		{
			if (const FLanunQuestData* Def = QuestDefinitions.Find(Pair.Key))
			{
				Result.Add(*Def);
			}
		}
	}
	return Result;
}

TArray<FLanunQuestData> ULanunQuestSubsystem::GetCompletedQuests() const
{
	TArray<FLanunQuestData> Result;
	for (const TPair<FName, FLanunQuestSaveData>& Pair : QuestSaveData)
	{
		if (Pair.Value.State == EQuestState::Completed)
		{
			if (const FLanunQuestData* Def = QuestDefinitions.Find(Pair.Key))
			{
				Result.Add(*Def);
			}
		}
	}
	return Result;
}

void ULanunQuestSubsystem::SaveQuestState(TArray<FLanunQuestSaveData>& OutSaveData) const
{
	OutSaveData.Reset();
	for (const TPair<FName, FLanunQuestSaveData>& Pair : QuestSaveData)
	{
		OutSaveData.Add(Pair.Value);
	}
}

void ULanunQuestSubsystem::LoadQuestState(const TArray<FLanunQuestSaveData>& SaveData)
{
	QuestSaveData.Empty();
	for (const FLanunQuestSaveData& Data : SaveData)
	{
		QuestSaveData.Add(Data.QuestID, Data);
	}
}

bool ULanunQuestSubsystem::IsAtQuestStartLocation(FName QuestID, FName CurrentLocationID) const
{
	const FLanunQuestData* QuestDef = QuestDefinitions.Find(QuestID);
	if (QuestDef)
	{
		return QuestDef->StartLocationID == CurrentLocationID;
	}
	return false;
}
