// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSEnemyCharacter.generated.h"

/**
 * ATPSEnemyCharacter
 * 
 * 플레이어와 대적하는 기본 적 캐릭터 클래스입니다.
 */
UCLASS()
class AITPSPROJECT_API ATPSEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// 생성자 선언
	ATPSEnemyCharacter();

protected:
	// 게임 시작 시 또는 스폰 시 호출됩니다.
	virtual void BeginPlay() override;

	// 에디터에서 속성 변경 시 실시간 반영을 위한 OnConstruction 오버라이드
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// 매 프레임 호출됩니다.
	virtual void Tick(float DeltaTime) override;

	// 데미지를 입었을 때 엔진 시스템에 의해 호출되는 가상 함수 오버라이드
	virtual float TakeDamage(
		float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		class AActor* DamageCauser
	) override;

	// 사망 처리를 담당하는 함수 (한글 주석)
	void HandleDead();

private:
	// 적 캐릭터의 AI 행동 상태를 관리하는 FSM 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UTPSEnemyFSMComponent* EnemyFSM;

	// 에디터에서 직접 조정할 수 있는 스켈레탈 메시의 위치 보정 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	FVector MeshOffset;

	// 에디터에서 직접 조정할 수 있는 스켈레탈 메시의 회전 보정 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	FRotator MeshRotation;

	// 에디터에서 직접 조정할 수 있는 적 캐릭터의 회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FRotator RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	// 에디터에서 직접 조정할 수 있는 적 캐릭터의 최대 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed = 300.0f;

	// 에디터에서 직접 조정 가능한 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	int32 hp = 3;

	// 에디터에서 직접 조정 가능한 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	int32 maxHp = 3;

	// 사망 후 가라앉기 시작할 때까지 대기 시간 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float sinkDelayTime = 3.0f;

	// 땅속으로 가라앉는 속도 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float sinkSpeed = 50.0f;

	// 이 값 이하로 내려가면 캐릭터를 제거합니다 (사망 시점 Z 대비 오프셋)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float removeZ = -200.0f;

private:
	// 사망 후 가라앉는 연출 진행 여부
	bool bShouldSink = false;

	// 가라앉기 대기용 누적 타이머
	float sinkTimer = 0.0f;

	// 최종 제거 기준이 되는 목표 Z 높이
	float targetSinkZ = 0.0f;
};
