// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSEnemyFSMComponent.generated.h"

/**
 * EEnemyState
 * 
 * 적 AI의 상태 정의 열거형입니다.
 */
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	None,
	Idle,
	Move,
	Attack,
	Hit,
	Dead
};

/**
 * UTPSEnemyFSMComponent
 * 
 * 적 캐릭터의 상태 기계(FSM) 및 플레이어 추적 로직을 처리하는 액터 컴포넌트입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AITPSPROJECT_API UTPSEnemyFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// 생성자 선언
	UTPSEnemyFSMComponent();

protected:
	// 게임 시작 시 호출됩니다.
	virtual void BeginPlay() override;

public:	
	// 매 프레임 호출됩니다.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * 상태 변경 함수입니다.
	 * @param NewState 전환하고자 하는 새로운 AI 상태
	 */
	void SetState(EEnemyState NewState);

	// 현재 AI 상태를 반환하는 Getter 함수
	EEnemyState GetCurrentState() const { return CurrentState; }

	// 피격 당했을 때 호출되어 피격 상태로 전환하는 함수 (한글 주석)
	void NotifyHit();

	// 사망했을 때 호출되어 사망 상태로 전환하고 FSM 틱을 끄는 함수 (한글 주석)
	void NotifyDead();

private:
	/**
	 * 상태에 따른 한글 디버그 문자열을 얻기 위한 헬퍼 함수
	 * @param State 상태 값
	 * @return 상태 이름 문자열
	 */
	FString GetStateName(EEnemyState State) const;

	// 대기(Idle) 상태 프레임 업데이트 처리
	void UpdateIdle();

	// 이동(Move) 상태 프레임 업데이트 처리
	void UpdateMove();

	// 공격(Attack) 상태 프레임 업데이트 처리
	void UpdateAttack();

	// 피격(Hit) 상태 프레임 업데이트 처리 (한글 주석)
	void UpdateHit(float DeltaTime);

private:
	// 현재 적 캐릭터의 AI 상태
	UPROPERTY(VisibleAnywhere, Category = "AI State")
	EEnemyState CurrentState;

	// 플레이어를 감지하는 시야 범위 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings", meta = (AllowPrivateAccess = "true"))
	float DetectionRange = 1000.0f;

	// 추적 시 적용할 적 캐릭터의 이동 속도 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings", meta = (AllowPrivateAccess = "true"))
	float MovementSpeed = 300.0f;

	// 공격 가능한 유효 범위 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings", meta = (AllowPrivateAccess = "true"))
	float AttackRange = 150.0f;

	// 공격 판정 감지 반경 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings", meta = (AllowPrivateAccess = "true"))
	float AttackHitRadius = 100.0f;

	// 공격 시 전달할 데미지 수치 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings", meta = (AllowPrivateAccess = "true"))
	float AttackDamage = 20.0f;

	// 공격 후 대기할 쿨다운 시간 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings", meta = (AllowPrivateAccess = "true"))
	float AttackCooldown = 2.0f;

	// 현재 남은 공격 쿨다운 타이머
	float AttackCooldownTimer = 0.0f;

	// 피격 시 경직 시간 (에디터 노출, 한글 주석)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings", meta = (AllowPrivateAccess = "true"))
	float HitReactDuration = 1.0f;

	// 현재 남은 경직 시간 타이머
	float HitReactTimer = 0.0f;

	// 컴포넌트를 소유하고 있는 적 캐릭터 액터 참조 포인터
	UPROPERTY()
	class ACharacter* OwnerCharacter;

	// 추적 타겟이 되는 기본 플레이어 캐릭터 포인터
	UPROPERTY()
	class ACharacter* TargetPlayer;
};
