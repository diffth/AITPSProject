// Copyright Epic Games, Inc. All Rights Reserved.


#include "TPSGameModeBase.h"

ATPSGameModeBase::ATPSGameModeBase()
{
	// 기본 게임 모드 설정
	// TODO: 플레이어 캐릭터 클래스가 완성되면 여기서 DefaultPawnClass를 설정합니다.
}

void ATPSGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 게임 모드 시작 로그 (노란색 - 일반 정보)
	UE_LOG(LogTemp, Log, TEXT("TPSGameModeBase가 시작되었습니다!"));
}
