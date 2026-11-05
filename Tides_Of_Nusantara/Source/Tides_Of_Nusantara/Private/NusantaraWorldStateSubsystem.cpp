#include "NusantaraWorldStateSubsystem.h"
#include "LanunProgressionUtility.h"

void UNusantaraWorldStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Inisialisasi wilayah default
	SetRegionState(TEXT("Teluk_Spermonde"), ERegionState::Discovered, 10, 0);
	SetRegionState(TEXT("Bandar_Somba_Opu"), ERegionState::Unlocked, 0, 0);
	SetRegionState(TEXT("Teluk_Tersembunyi"), ERegionState::Unlocked, 0, 0);
	SetRegionState(TEXT("Selat_Makassar"), ERegionState::Unlocked, 0, 0);
	SetRegionState(TEXT("Celah_Karang_Salisipan"), ERegionState::Unlocked, 0, 0);
	SetRegionState(TEXT("Rawa_Papua"), ERegionState::Unlocked, 0, 0);
	SetRegionState(TEXT("Bandar_Utama"), ERegionState::Unlocked, 0, 0);
	SetRegionState(TEXT("Pantai_Pesisir_Hancur"), ERegionState::Discovered, 5, 0);
}

void UNusantaraWorldStateSubsystem::Deinitialize()
{
	RegionStates.Empty();
	QuestJournal.Empty();
	Super::Deinitialize();
}

void UNusantaraWorldStateSubsystem::SetRegionState(FName RegionID, ERegionState NewState, int32 DiscoveryLevel, int32 ControlPercentage)
{
	FRegionData& Data = RegionStates.FindOrAdd(RegionID);
	Data.RegionID = RegionID;
	Data.State = NewState;
	Data.DiscoveryLevel = DiscoveryLevel;
	Data.ControlPercentage = ControlPercentage;

	OnRegionStateChanged.Broadcast(RegionID, NewState);
}

ERegionState UNusantaraWorldStateSubsystem::GetRegionState(FName RegionID) const
{
	const FRegionData* Data = RegionStates.Find(RegionID);
	return Data ? Data->State : ERegionState::Unlocked;
}

void UNusantaraWorldStateSubsystem::AddQuestToJournal(FName QuestID, FText QuestTitle)
{
	if (QuestJournal.Contains(QuestID))
	{
		return;
	}

	FLanunQuestJournalEntry Entry;
	Entry.QuestID = QuestID;
	Entry.QuestTitle = QuestTitle;
	Entry.State = EQuestState::NotStarted;
	Entry.CurrentObjectiveIndex = 0;
	Entry.LastUpdatedLocation = LastLocationID.IsNone() ? TEXT("Unknown") : LastLocationID.ToString();

	QuestJournal.Add(QuestID, Entry);
}

void UNusantaraWorldStateSubsystem::UpdateQuestInJournal(FName QuestID, EQuestState NewState, int32 ObjectiveIndex)
{
	FLanunQuestJournalEntry* Entry = QuestJournal.Find(QuestID);
	if (!Entry)
	{
		return;
	}

	Entry->State = NewState;
	Entry->CurrentObjectiveIndex = ObjectiveIndex;
	Entry->LastUpdatedLocation = LastLocationID.IsNone() ? TEXT("Unknown") : LastLocationID.ToString();

	OnQuestJournalUpdated.Broadcast(QuestID, NewState);
}

TArray<FLanunQuestJournalEntry> UNusantaraWorldStateSubsystem::GetQuestJournal() const
{
	TArray<FLanunQuestJournalEntry> Result;
	for (const TPair<FName, FLanunQuestJournalEntry>& Pair : QuestJournal)
	{
		Result.Add(Pair.Value);
	}
	return Result;
}

bool UNusantaraWorldStateSubsystem::IsQuestCompleted(FName QuestID) const
{
	const FLanunQuestJournalEntry* Entry = QuestJournal.Find(QuestID);
	return Entry && Entry->State == EQuestState::Completed;
}

bool UNusantaraWorldStateSubsystem::IsQuestInProgress(FName QuestID) const
{
	const FLanunQuestJournalEntry* Entry = QuestJournal.Find(QuestID);
	return Entry && Entry->State == EQuestState::InProgress;
}

void UNusantaraWorldStateSubsystem::SetLastLocation(FName LocationID)
{
	LastLocationID = LocationID;
}

FName UNusantaraWorldStateSubsystem::GetLastLocation() const
{
	return LastLocationID;
}

bool UNusantaraWorldStateSubsystem::IsAtLocation(FName LocationID) const
{
	return LastLocationID == LocationID;
}

void UNusantaraWorldStateSubsystem::StartQuest1_SumpahSiri()
{
	AddQuestToJournal(TEXT("Q001_HujanApiPelarian"), LOCTEXT("Quest1_Title", "Hujan Api & Pelarian dari Pesisir"));
	UpdateQuestInJournal(TEXT("Q001_HujanApiPelarian"), EQuestState::InProgress, 0);

	OnFirstQuestStarted.Broadcast(TEXT("Q001_HujanApiPelarian"));
}

bool UNusantaraWorldStateSubsystem::IsQuest1Completed() const
{
	return IsQuestCompleted(TEXT("Q001_HujanApiPelarian"));
}

void UNusantaraWorldStateSubsystem::CompleteQuest1_SumpahSiri()
{
	UpdateQuestInJournal(TEXT("Q001_HujanApiPelarian"), EQuestState::Completed, 0);
}
