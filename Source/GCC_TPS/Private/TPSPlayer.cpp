// Fill out your copyright notice in the Description page of Project Settings.


#include "GCC_TPS/Public/TPSPlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ATPSPlayer::ATPSPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 마우스를 돌려도 캐릭터 몸통이 즉시 회전하지 않도록 방지 (TPS 기본)
	// bUseControllerRotationPitch = false;
	// bUseControllerRotationYaw = true;
	// bUseControllerRotationRoll = false;

	// 움직이는 방향으로 캐릭터 몸이 부드럽게 회전하도록 설정
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // 회전 속도
	
	// TPS 카메라를 SpringArm 컴포넌트에 부착
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.0f, 70.0f, 90.0f));
	SpringArm->TargetArmLength = 400.0f;
	
	//카메라 컴포넌트
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	FollowCamera->SetupAttachment(SpringArm);
	
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}
}

void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);
		PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);
	}
}

// 좌우 회전 입력에 따른 콜백 함수
void ATPSPlayer::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerYawInput(value); // YAW(Z 축) 회전
}

// 상하 회전 입력에 따른 콜백 함수
void ATPSPlayer::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput(value); // PITCH(Y 축) 회전
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//플레이어 이동 처리
	//P(결과위치) = P0(초기위치) + v(속도) * t(시간)
	direction = FTransform(GetControlRotation()).TransformFVector4(direction);
	
	// FVector P0 = GetActorLocation();
	// FVector vt = direction * walkSpeed * DeltaTime;
	// FVector P = P0 + vt;
	// SetActorLocation(P);
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void ATPSPlayer::Move(const FInputActionValue& Value)
{
	FVector2D value = Value.Get<FVector2D>();

	direction.X = value.X;
	direction.Y = value.Y;
	// FVector2D MovementVector = Value.Get<FVector2D>();
	//
	// if (Controller != nullptr)
	// {
	// 	// 1. 컨트롤러(카메라)의 회전값을 가져옵니다.
	// 	const FRotator Rotation = Controller->GetControlRotation();
	// 	// 2. 바닥 평면 이동을 위해 Pitch(상하)와 Roll을 제외하고 Yaw(좌우) 값만 땁니다.
	// 	const FRotator YawRotation(0, Rotation.Yaw, 0);
	//
	// 	// 3. 카메라 Yaw 기준으로 진짜 '앞(Forward)'과 '오른쪽(Right)'이 어디인지 수학적으로 계산합니다.
	// 	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// 	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	//
	// 	// 4. 계산된 월드 방향 벡터에 입력 축 값을 곱해 이동시킵니다.
	// 	AddMovementInput(ForwardDirection, MovementVector.Y); // W, S 입력
	// 	AddMovementInput(RightDirection, MovementVector.X);   // A, D 입력
	// }
}