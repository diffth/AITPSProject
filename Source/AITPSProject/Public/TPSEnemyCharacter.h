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
	// 적 캐릭터의 임시 외형을 보여주기 위한 스태틱 메시 컴포넌트 (에디터에서 직접 에셋을 지정할 수 있도록 노출)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;

	// 적 캐릭터의 AI 행동 상태를 관리하는 FSM 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UTPSEnemyFSMComponent* EnemyFSM;

	// 사망 시 메시의 상대 회전값 (에디터 노출, 한글 주석)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FRotator DeathRotation = FRotator(-90.f, 0.f, 0.f);

	// 사망 시 메시의 상대 위치값 오프셋 (에디터 노출, 한글 주석)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FVector DeathOffset = FVector(0.f, 0.f, -80.f);

	// 사망 시 메시의 상대 스케일값 (에디터 노출, 한글 주석)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FVector DeathScale = FVector(1.f, 1.f, 1.f);

	// 사망 후 액터 제거까지의 대기 시간 (에디터 노출, 한글 주석)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float DestroyDelay = 2.0f;
};
