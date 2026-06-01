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
		PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &ATPSPlayer::InputJump);
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
	
	//컨트롤러의 현재 회전 값들 (Yaw, Pitch, Roll)값을 가져옴
	FRotator controlRot = GetControlRotation();
	//Pitch자체를 0으로 설정 -> 기우는 현상 차단 -> 일정속도 이동
	controlRot.Pitch = 0.0f;
	controlRot.Roll = 0.0f;
	//원래도 0이긴함 -> 카메라 움직임에 따라에도 움직일 가능성 존재하여 사전 차단
    
	//Yaw 좌우 측값만 남은 벡터에 입력 -> 카메라의 회전과 관계엾이 수평면 위에서만 이동
	direction = FTransform(GetControlRotation()).TransformFVector4(direction);
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void ATPSPlayer::Move(const FInputActionValue& Value)
{
	FVector2D value = Value.Get<FVector2D>();

	direction.X = value.X;
	direction.Y = value.Y;
}

void ATPSPlayer::InputJump(const FInputActionValue& inputValue)
{
	Jump(); // ACharacter 클래스가 제공하는 기본 점프 함수 호출
}