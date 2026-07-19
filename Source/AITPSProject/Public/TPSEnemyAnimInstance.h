// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TPSEnemyFSMComponent.h"
#include "TPSEnemyAnimInstance.generated.h"

/**
 * UTPSEnemyAnimInstance
 * 
 * 적 캐릭터의 애니메이션 블루프린트(ABP_TPSEnemy)의 부모 클래스입니다.
 * FSM 상태 기계의 실시간 상태 값을 제공받아 애니메이션 전이를 처리합니다.
 */
UCLASS()
class AITPSPROJECT_API UTPSEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 생성자 선언
	UTPSEnemyAnimInstance();

	// FSM 상태 변수 (에디터 노출, 블루프린트 읽기 가능)
	// 기본값은 Idle(EEnemyState::Idle)로 생성자에서 초기화합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	EEnemyState AnimState;

	// 공격 중인지 여부 (기본값 false, 에디터 및 블루프린트 노출)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bAttackPlay;

	// 피격 시 재생할 애니메이션 몽타주 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* DamageMontage;

	// 피격 애니메이션 재생 함수 (블루프린트 호출 가능)
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayDamageAnim();

	// 사망 시 재생할 애니메이션 몽타주 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* DeadMontage;

	// 사망 애니메이션 재생 함수 (블루프린트 호출 가능)
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayDeadAnim();

	// 애니메이션 재생 중 AttackEnd 노티파이가 트리거되면 엔진에 의해 자동으로 호출되는 함수
	UFUNCTION()
	void AnimNotify_AttackEnd();
};
