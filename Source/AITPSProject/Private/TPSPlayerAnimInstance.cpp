// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerAnimInstance.h"
#include "TPSPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// 초기화 시 호출되는 함수
void UTPSPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 애니메이션이 시작될 때 미리 캐릭터를 가져와 캐싱해 둡니다.
	APawn* OwningPawn = TryGetPawnOwner();
	if (OwningPawn)
	{
		PlayerCharacter = Cast<ATPSPlayerCharacter>(OwningPawn);
	}
}

// 매 프레임 업데이트 시 호출되는 함수
void UTPSPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 만약 플레이어 캐릭터 참조를 유실했거나 캐싱되지 않은 경우 재시도합니다.
	if (!PlayerCharacter)
	{
		APawn* OwningPawn = TryGetPawnOwner();
		if (OwningPawn)
		{
			PlayerCharacter = Cast<ATPSPlayerCharacter>(OwningPawn);
		}
	}

	// 플레이어 캐릭터가 유효할 때 값을 실시간 갱신합니다.
	if (PlayerCharacter)
	{
		// 1. 캐릭터의 현재 속도(Velocity) 벡터 획득
		FVector Velocity = PlayerCharacter->GetVelocity();

		// 2. 캐릭터의 앞(Forward) 및 오른쪽(Right) 방향 벡터 획득
		FVector ForwardVector = PlayerCharacter->GetActorForwardVector();
		FVector RightVector = PlayerCharacter->GetActorRightVector();

		// 3. 내적(Dot Product)을 통해 로컬 속도 성분 계산
		// 앞뒤 속도 성분: 전방 이동 시 양수, 후방 이동 시 음수
		Speed = FVector::DotProduct(Velocity, ForwardVector);

		// 좌우 속도 성분: 우측 이동 시 (Strafe Right) 양수, 좌측 이동 시 (Strafe Left) 음수
		Direction = FVector::DotProduct(Velocity, RightVector);

		// 4. 캐릭터 무브먼트 컴포넌트를 통해 공중 여부(IsFalling) 갱신
		UCharacterMovementComponent* MovementComp = PlayerCharacter->GetCharacterMovement();
		if (MovementComp)
		{
			bIsInAir = MovementComp->IsFalling();
		}
	}
}
