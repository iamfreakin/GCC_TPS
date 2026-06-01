// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class GCC_TPS_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// 스프링 암 컴포넌트 생성
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* SpringArm;
	//카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;
	
	// IMC 선택 필드 선언
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputMappingContext* imc_TPS;

	// 상하 회전 IA 필드 선언
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* ia_LookUp;

	// 좌우 회전 IA 필드 선언
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* ia_Turn;

	// 상하 회전 입력 함수 선언
	void LookUp(const struct FInputActionValue& inputValue);

	// 좌우 회전 입력 함수 선언
	void Turn(const struct FInputActionValue& inputValue);
	
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_Move;
    
	//이동 속도
	UPROPERTY(EditDefaultsOnly, Category = PlayerSetting)
	float walkSpeed = 600.f;
    
	//이동 방향
	FVector direction;
	void Move(const struct FInputActionValue& inputValue);
};
