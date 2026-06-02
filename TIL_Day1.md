# TIL — TPS 프로토타입 1일차 (2026-06-01)

## 오늘 한 것

언리얼 엔진 C++로 3인칭 슈터(TPS) 프로토타입의 기초 메커니즘을 구현했다.  
시각 에셋 없이 충돌체·임시 메시만으로 이동·회전·점프 로직을 검증하는 MVP 방식으로 진행.

---

## 1. ACharacter 상속 계층

```
UObject → AActor → APawn → ACharacter
```

`ACharacter`는 세 가지 컴포넌트를 내장한다.

| 컴포넌트 | 역할 |
|---|---|
| `CapsuleComponent` | 루트 충돌 판정 |
| `SkeletalMeshComponent` | 외관 · 애니메이션 |
| `CharacterMovementComponent` | 걷기 · 점프 · 낙하 물리 |

`AGameModeBase`에서 `DefaultPawnClass`를 지정하고 레벨에 `PlayerStart`를 배치하면 자동 스폰된다.

---

## 2. SpringArm + Camera 리그

```cpp
SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
SpringArm->SetupAttachment(RootComponent);
SpringArm->SetRelativeLocation(FVector(0.f, 70.f, 90.f));
SpringArm->TargetArmLength = 400.f;

FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
FollowCamera->SetupAttachment(SpringArm);
```

- `TargetArmLength`: 캐릭터↔카메라 거리 (보통 300~400 unit)
- `SocketOffset`: 어깨 오프셋 → TPS 특유의 비대칭 시점
- **Do Collision Test**: 벽 충돌 시 카메라 위치 자동 보정 → 메시 클리핑 방지
- **Camera Lag**: 위치/회전 보간으로 자연스러운 추적

---

## 3. Enhanced Input System

입력 키와 게임 동작을 분리하는 아키텍처. 키 변경 시 코드 수정 없이 에셋만 교체.

| 구성 요소 | 역할 |
|---|---|
| Input Action (IA) | 의미적 동작 정의 (IA_Move, IA_LookUp 등) |
| Input Mapping Context (IMC) | 키 ↔ IA 매핑 묶음 |
| BindAction | `SetupPlayerInputComponent`에서 IA → C++ 함수 연결 |

```cpp
// BeginPlay에서 IMC 등록
auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
subsystem->AddMappingContext(imc_TPS, 0);

// SetupPlayerInputComponent에서 바인딩
PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);
PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started,   this, &ATPSPlayer::InputJump);
```

---

## 4. 마우스 회전 구현

```cpp
void ATPSPlayer::Turn(const FInputActionValue& inputValue)
{
    AddControllerYawInput(inputValue.Get<float>());   // Z축 좌우 회전
}

void ATPSPlayer::LookUp(const FInputActionValue& inputValue)
{
    AddControllerPitchInput(inputValue.Get<float>()); // Y축 상하 회전
}
```

`bOrientRotationToMovement = true` + `RotationRate` 설정으로 이동 방향으로 몸이 부드럽게 회전.

---

## 5. 이동 로직 진화 (1차 → 2차 → 최종)

### 1차: 수동 위치 계산

```cpp
FVector P = GetActorLocation() + direction * walkSpeed * DeltaTime;
SetActorLocation(P);
```
직접 위치를 계산했지만, 충돌·물리가 무시된다.

### 2차: AddMovementInput 사용

```cpp
AddMovementInput(direction); // CharacterMovementComponent가 물리 처리
```
엔진 내장 이동 함수로 교체하면 충돌·가속도 등을 자동으로 처리한다.

### 최종 (버그 픽스 포함): Pitch 제거 후 벡터 변환

```cpp
void ATPSPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FRotator controlRot = GetControlRotation();
    controlRot.Pitch = 0.f;  // Pitch를 제거해야 수평 이동이 보장됨
    controlRot.Roll  = 0.f;

    direction = FTransform(controlRot).TransformFVector4(direction);
    AddMovementInput(direction);
    direction = FVector::ZeroVector;
}
```

---

## 6. Pitch 버그 — 핵심 교훈

**증상**: W/S 이동이 막히거나 현저히 느려짐. A/D는 정상. 카메라를 아래로 향할수록 심해짐.

**원인**: `GetControlRotation()`에는 Pitch(상하각)가 포함되어 있다.  
이 회전을 그대로 이동 벡터에 적용하면 Pitch가 이동 방향을 아래로 꺾어버린다.

**해결**: 벡터 변환 전에 Pitch, Roll을 0으로 리셋 → **Yaw(좌우 회전)만** 이동 방향에 반영.

---

## 7. 점프 구현

```cpp
// IA_Jump는 ETriggerEvent::Started — 누르는 순간 한 번만 발동
PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &ATPSPlayer::InputJump);

void ATPSPlayer::InputJump(const FInputActionValue& inputValue)
{
    Jump(); // ACharacter 내장 점프 함수 — 물리 처리 포함
}
```

이동/회전은 `Triggered`(매 프레임), 점프는 `Started`(키 누르는 순간) — 트리거 이벤트 구분이 중요.

---

## 핵심 정리

- 프로토타이핑은 시각 에셋 전에 코어 로직을 먼저 검증한다.
- `SetActorLocation` 직접 이동 < `AddMovementInput` 물리 이동.
- 컨트롤러 회전을 이동 벡터에 쓸 때는 반드시 **Pitch를 0으로** 만든다.
- Enhanced Input은 키 ↔ 동작을 분리해 유지보수성을 높인다.
- `ETriggerEvent::Started` vs `Triggered` 구분을 명확히 하자.
