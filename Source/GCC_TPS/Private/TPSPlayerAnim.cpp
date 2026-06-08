// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerAnim.h"

#include "TPSPlayer.h"

//매 프레임 자동 호출 - Tick 직접 구성안해도 UE가 자체 호출
void UTPSPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//이 Animation Instance를 소유한 폰(TPSPlayer) 가져오기
	APawn* pawn = TryGetPawnOwner();
	if (pawn == nullptr) return;
    
	//APSPlayer 폰의 속도 측정
	ATPSPlayer* player = Cast<ATPSPlayer>(pawn);
	if (player)
	{
		speed = player->GetVelocity().Size();
	}
    
	//Debug - 프레임마다 편하는 speed 값 출력용
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,0.f,FColor::Green,
			FString::Printf(TEXT("Anim Speed: %f"),speed));
	}
}