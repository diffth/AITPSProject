// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSEnemyManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// 생성자: 기본값 설정
ATPSEnemyManager::ATPSEnemyManager()
{
 	// 이 액터는 매 프레임 업데이트(Tick)를 사용하지 않습니다. (요구사항 반영)
	PrimaryActorTick.bCanEverTick = false;

	// 에디터 조정 변수 기본값 설정
	SpawnInterval = 5.0f;
	MaxEnemyCount = 5;
	SpawnSpotTag = FName(TEXT("SpawnSpot"));
}

// 게임 시작 시 또는 스폰 시 호출됩니다.
void ATPSEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	
	// 1. 월드 내에서 지정된 태그(SpawnSpotTag)를 가진 스폰 위치 액터들을 수집합니다.
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), SpawnSpotTag, SpawnSpotActors);

	UE_LOG(LogTemp, Warning, TEXT("[적 매니저] 게임 시작 시 태그 '%s'를 가진 스폰 위치 액터 %d개를 찾았습니다."), 
		*SpawnSpotTag.ToString(), SpawnSpotActors.Num());

	// 2. 적 생성 조건(클래스 지정 및 스폰 지점 유효성)이 충족되면 타이머를 가동합니다.
	if (EnemyClass != nullptr && SpawnSpotActors.Num() > 0)
	{
		// 지정된 생성 주기마다 SpawnEnemy()를 반복 호출하도록 타이머 설정
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ATPSEnemyManager::SpawnEnemy, SpawnInterval, true);
		
		UE_LOG(LogTemp, Warning, TEXT("[적 매니저] 조건 충족! %.1f초 주기마다 자동 스폰 타이머를 작동합니다."), SpawnInterval);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[적 매니저] 스폰 조건 불충족: 에너미 클래스가 지정되지 않았거나 스폰 위치 액터가 없습니다!"));
	}
}

// 타이머마다 적 생성 처리 함수
void ATPSEnemyManager::SpawnEnemy()
{
	if (EnemyClass == nullptr || SpawnSpotActors.Num() == 0) return;

	// 1. 현재 맵에 존재하는 해당 에너미 클래스의 개수를 파악합니다.
	TArray<AActor*> CurrentEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyClass, CurrentEnemies);

	int32 CurrentEnemyCount = CurrentEnemies.Num();

	// 2. 현재 적 수가 최대 동시 존재 수 미만일 때만 스폰을 실행합니다.
	if (CurrentEnemyCount < MaxEnemyCount)
	{
		// 캐싱된 스폰 위치 액터 목록 중 임의의 인덱스를 선정합니다.
		int32 RandomIndex = FMath::RandRange(0, SpawnSpotActors.Num() - 1);
		AActor* SelectedSpot = SpawnSpotActors[RandomIndex];

		if (SelectedSpot != nullptr)
		{
			FVector SpawnLocation = SelectedSpot->GetActorLocation();
			FRotator SpawnRotation = SelectedSpot->GetActorRotation();

			// 적 스폰 파라미터 구성 (충돌 스폰 규칙 등)
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// 적 스폰
			AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);

			if (SpawnedEnemy != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("[적 매니저] 적 스폰 성공! (현재 적 개수: %d/%d, 스폰 위치: %s)"), 
					CurrentEnemyCount + 1, MaxEnemyCount, *SpawnLocation.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[적 매니저] 적 스폰에 실패했습니다."));
			}
		}
	}
	else
	{
		// 최대 적 수를 넘어가면 스폰하지 않고 디버그 로그 출력 (선택 사항)
		UE_LOG(LogTemp, Verbose, TEXT("[적 매니저] 현재 적 수가 최대 수(%d/%d)에 도달하여 스폰을 건너뜁니다."), 
			CurrentEnemyCount, MaxEnemyCount);
	}
}
