#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameCamera.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USceneComponent;
class UGameCameraShake;

UCLASS(Blueprintable)
class WALLDEFENSE_API AGameCamera : public AActor
{
	GENERATED_BODY()

public:
	AGameCamera();

	UFUNCTION(BlueprintCallable, Category = "GameCamera|Shake")
	void PlayCameraShake(TSubclassOf<UGameCameraShake> ShakeClass, float Scale = 1.f);

	UFUNCTION(BlueprintCallable, Category = "GameCamera|Shake")
	void StopAllCameraShakes(bool bImmediately = true);

	UFUNCTION(BlueprintCallable, Category = "GameCamera")
	void ActivateAsViewTarget(float BlendTime = 0.f);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	void ApplyCameraSettings();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameCamera|Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameCamera|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameCamera|Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameCamera|TopDown", meta = (ClampMin = "0"))
	float CameraDistance = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameCamera|TopDown", meta = (ClampMin = "-90", ClampMax = "0"))
	float CameraPitch = -75.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameCamera|TopDown")
	float CameraYaw = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameCamera|TopDown", meta = (ClampMin = "5", ClampMax = "170"))
	float FieldOfView = 70.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameCamera|TopDown")
	bool bUseOrthographic = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameCamera|TopDown", meta = (EditCondition = "bUseOrthographic", ClampMin = "1"))
	float OrthoWidth = 2048.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameCamera|TopDown")
	bool bAutoActivateOnBeginPlay = true;
};
