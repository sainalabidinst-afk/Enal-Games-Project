#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"
#include "ShipMovementComponent.h"
#include "ShipPawnBase.generated.h"

class ALanunCharacterHeroBase;

UENUM(BlueprintType)
enum class EShipBoardState : uint8
{
    None,
    Grappling,
    Boarded,
    Disembarking
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBoardingStarted, FName, TargetShipID, AShipPawnBase*, TargetShip);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBoardingEnded, FName, ShipID, bool, bSuccess);

UCLASS()
class TIDES_OF_NUSANTARA_API AShipPawnBase : public APawn
{
	GENERATED_BODY()

public:
	AShipPawnBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Components")
	UStaticMeshComponent* ShipMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Components")
	UShipMovementComponent* ShipMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Components")
	USceneComponent* RudderInteractiveComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Components")
	USceneComponent* DisembarkLocationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Components")
	USceneComponent* BoardingGrappleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boarding")
	FName ShipID = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boarding")
	EShipBoardState BoardState = EShipBoardState::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boarding")
	AShipPawnBase* GrapplingTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boarding", meta = (ClampMin = "0.0"))
	float MaxGrappleDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boarding", meta = (ClampMin = "0.0"))
	float GrappleSpeed = 800.0f;

	UFUNCTION(BlueprintCallable, Category = "Ship Control")
	void InputRudder(float Value);

	UFUNCTION(BlueprintCallable, Category = "Ship Control")
	void InputRaiseSails();

	UFUNCTION(BlueprintCallable, Category = "Ship Control")
	void InputLowerSails();

	UFUNCTION(BlueprintCallable, Category = "Ship Control")
	void InputAdjustSailAngle(float Value);

	UFUNCTION(BlueprintCallable, Category = "Ship Interaction")
	void DisembarkShip(ALanunCharacterHeroBase* HeroCharacter);

	UFUNCTION(BlueprintCallable, Category = "Boarding")
	bool GrappleShip(AShipPawnBase* TargetShip);

	UFUNCTION(BlueprintCallable, Category = "Boarding")
	bool BoardShip();

	UFUNCTION(BlueprintCallable, Category = "Boarding")
	void DisembarkFromBoarding(ALanunCharacterHeroBase* HeroCharacter);

	UFUNCTION(BlueprintCallable, Category = "Ship Components")
	USceneComponent* GetRudderInteractiveComponent() const { return RudderInteractiveComponent; }

	UFUNCTION(BlueprintCallable, Category = "Ship Components")
	USceneComponent* GetDisembarkLocationComponent() const { return DisembarkLocationComponent; }

	UFUNCTION(BlueprintCallable, Category = "Boarding")
	USceneComponent* GetBoardingGrappleComponent() const { return BoardingGrappleComponent; }

	UFUNCTION(BlueprintPure, Category = "Ship|Components")
	UShipMovementComponent* GetShipMovementComponent() const { return ShipMovementComponent; }

	UFUNCTION(BlueprintPure, Category = "Ship|State")
	EShipBoardState GetBoardState() const { return BoardState; }

	UPROPERTY(BlueprintAssignable, Category = "Boarding|Events")
	FOnBoardingStarted OnBoardingStarted;

	UPROPERTY(BlueprintAssignable, Category = "Boarding|Events")
	FOnBoardingEnded OnBoardingEnded;
};
