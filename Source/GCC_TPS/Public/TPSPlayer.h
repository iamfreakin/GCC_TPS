#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class GCC_TPS_API ATPSPlayer : public ACharacter
{
    GENERATED_BODY()

public:
    ATPSPlayer();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

    // ==================== 컴포넌트 ====================

    UPROPERTY(VisibleAnywhere, Category = Camera)
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, Category = Camera)
    class UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, Category = GunMesh)
    class USkeletalMeshComponent* gunMeshComp;
    
    UPROPERTY(VisibleAnywhere, Category = GunMesh)
    class UStaticMeshComponent* sniperGunComp;

    // ==================== 입력 (IMC / IA) ====================

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputMappingContext* imc_TPS;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* ia_Move;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* ia_Jump;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* ia_LookUp;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* ia_Turn;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* ia_Fire;
    
    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* ia_GrenadeGun;
    
    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* ia_SniperGun;

    // ==================== 플레이어 설정 ====================

    UPROPERTY(EditDefaultsOnly, Category = PlayerSetting)
    float walkSpeed = 600.f;

    FVector direction;
    bool bUsingGrenadeGun = true;

    // ==================== 팩토리 ====================

    UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
    TSubclassOf<class ABullet> bulletFactory;

    // ==================== 입력 핸들러 ====================

    void Move(const struct FInputActionValue& inputValue);
    void InputJump(const struct FInputActionValue& inputValue);
    void LookUp(const struct FInputActionValue& inputValue);
    void Turn(const struct FInputActionValue& inputValue);
    void InputFire(const struct FInputActionValue& inputValue);
    void ChangeToGrenadeGun(const struct FInputActionValue& inputValue);
    void ChangeToSniperGun(const struct FInputActionValue& inputValue);
};  