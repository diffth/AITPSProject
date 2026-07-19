// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSEnemyManager.generated.h"

/**
 * ATPSEnemyManager
 * 
 * 맵 상에 적 캐릭터를 주기적으로 자동 스폰하고 생존 한도 수를 조율하는 매니저 액터 클래스입니다.
 */
UCLASS()
class AITPSPROJECT_API ATPSEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// 생성자 선언
	ATPSEnemyManager();

protected:
	// 게임 시작 시 또는 스폰 시 호출됩니다.
	virtual void BeginPlay() override;

private:
	// 타이머에 의해 주기적으로 호출되어 적을 스폰하는 함수 (한글 주석)
	void SpawnEnemy();

private:
	// 생성할 적 클래스 타입 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class APawn> EnemyClass;

	// 적 생성 주기 (초 단위, 에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings", meta = (AllowPrivateAccess = "true"))
	float SpawnInterval;

	// 최대 동시 존재 수 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings", meta = (AllowPrivateAccess = "true"))
	int32 MaxEnemyCount;

	// 스폰 위치를 찾기 위한 태그 이름 (에디터 노출)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings", meta = (AllowPrivateAccess = "true"))
	FName SpawnSpotTag;

	// 태그를 가진 스폰 위치 액터들을 캐싱하는 배열
	UPROPERTY()
	TArray<class AActor*> SpawnSpotActors;

	// 스폰용 타이머 핸들
	FTimerHandle SpawnTimerHandle;
};
