// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TPSPlayerAnimInstance.generated.h"

/**
 * UTPSPlayerAnimInstance
 * 
 * 플레이어 캐릭터의 애니메이션 블루프린트(ABP_TPSPlayer)에서 부모 클래스로 사용할 C++ 클래스입니다.
 * 캐릭터의 실시간 속도, 방향, 공중 여부 데이터를 제공합니다.
 */
UCLASS()
class AITPSPROJECT_API UTPSPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 매 프레임 애니메이션 값을 업데이트하는 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 애니메이션 인스턴스가 초기화될 때 호출되는 함수
	virtual void NativeInitializeAnimation() override;

	// 공격 애니메이션(몽타주) 재생 함수
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayAttackAnim();

private:
	// 애니메이션을 소유한 플레이어 캐릭터 캐싱
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class ATPSPlayerCharacter* PlayerCharacter;

	// 사격 시 재생할 공격 애님 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* AttackMontage;

	// 앞뒤 이동 속도 (내적 결과값: 전방 이동 시 양수, 후방 이동 시 음수)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;

	// 좌우 이동 속도 (내적 결과값: 우측 이동 시 양수, 좌측 이동 시 음수)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Direction;

	// 공중에 떠 있는지 여부 (점프/낙하 상태)
	// 에픽게임즈 C++ 명명 규칙에 따라 접두사 'b'를 붙여 bIsInAir로 선언하며, 에디터 및 블루프린트에는 'IsInAir'로 노출됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;
};
