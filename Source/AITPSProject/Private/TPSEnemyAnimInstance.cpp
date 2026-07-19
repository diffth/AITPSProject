// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSEnemyAnimInstance.h"

// 생성자: 기본값 초기화
UTPSEnemyAnimInstance::UTPSEnemyAnimInstance()
{
	// 기본 상태를 대기(Idle)로 설정합니다.
	AnimState = EEnemyState::Idle;
	bAttackPlay = false;

	// 피격 애니메이션 몽타주(AM_EnemyHit)를 코드에서 직접 로드하여 기본값으로 지정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> HitMontageObj(TEXT("/Game/_ART/Enemy/Animations/AM_EnemyHit"));
	if (HitMontageObj.Succeeded())
	{
		DamageMontage = HitMontageObj.Object;
	}
	else
	{
		DamageMontage = nullptr;
	}

	// 사망 애니메이션 몽타주(AM_EnemyDeath)를 코드에서 직접 로드하여 기본값으로 지정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeathMontageObj(TEXT("/Game/_ART/Enemy/Animations/AM_EnemyDeath"));
	if (DeathMontageObj.Succeeded())
	{
		DeadMontage = DeathMontageObj.Object;
	}
	else
	{
		DeadMontage = nullptr;
	}
}

void UTPSEnemyAnimInstance::PlayDamageAnim()
{
	if (DamageMontage != nullptr)
	{
		Montage_Play(DamageMontage);
	}
}

void UTPSEnemyAnimInstance::PlayDeadAnim()
{
	if (DeadMontage != nullptr)
	{
		Montage_Play(DeadMontage);
	}
}

void UTPSEnemyAnimInstance::AnimNotify_AttackEnd()
{
	bAttackPlay = false;
}
