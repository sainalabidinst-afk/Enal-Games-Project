#include "LanunSaveGameSubsystem.h"

#include "Misc/AutomationTest.h"
#include "GameplayTagContainer.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLanunSaveMigrationTest, "Lanun.SaveSystem.SchemaMigrationV1ToV2",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FLanunSaveMigrationTest::RunTest(const FString& Parameters)
{
	// 1. Arrange: Buat objek SaveGame tiruan format V1
	ULanunSaveGame* DummySave = NewObject<ULanunSaveGame>();
	DummySave->SaveSchemaVersion = 1;

	FOfficerSaveState LegacyOfficer;
	LegacyOfficer.AssignedShipID = FName("Daeng_Rannu");
	LegacyOfficer.CurrentRole = EOfficerRole::None;

	DummySave->SaveData.LegacyOfficers_DEPRECATED.Add(LegacyOfficer);

	// 2. Act: Jalankan migrasi
	ULanunSaveGameSubsystem* SaveSubsystem = NewObject<ULanunSaveGameSubsystem>();
	SaveSubsystem->MigrateSaveDataSchema(DummySave);

	// 3. Assert: Verifikasi hasil migrasi V2
	TestEqual(TEXT("Schema Version harus menjadi 2"), DummySave->SaveSchemaVersion, 2);
	TestEqual(TEXT("Array V1 harus dikosongkan"), DummySave->SaveData.LegacyOfficers_DEPRECATED.Num(), 0);
	TestEqual(TEXT("Array V2 harus terisi 1 perwira"), DummySave->SaveData.OfficerLegacyStates.Num(), 1);

	const FOfficerLegacyState& Migrated = DummySave->SaveData.OfficerLegacyStates[0];
	TestEqual(TEXT("Officer ID cocok"), Migrated.OfficerID, FName("Daeng_Rannu"));
	TestEqual(TEXT("GenerationIndex default 1"), Migrated.GenerationIndex, 1);
	TestEqual(TEXT("LegacyStatus default ActiveOnBoard"), (int32)Migrated.LegacyStatus, (int32)ECompanionLegacyStatus::ActiveOnBoard);
	TestEqual(TEXT("InheritedLoyaltyBonus default 0"), Migrated.InheritedLoyaltyBonus, 0);
	TestEqual(TEXT("LegacyBonusAppliedGeneration default 1"), Migrated.LegacyBonusAppliedGeneration, 1);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
