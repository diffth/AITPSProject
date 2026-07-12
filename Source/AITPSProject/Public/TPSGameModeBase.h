// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPSGameModeBase.generated.h"

/**
 * ATPSGameModeBase
 * 
 * 프로젝트의 기본 게임 규칙을 관리하는 게임 모드 클래스입니다.
 */
UCLASS()
class AITPSPROJECT_API ATPSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// 생성자 선언
	ATPSGameModeBase();

protected:
	// 게임 시작 시 호출되는 함수
	virtual void BeginPlay() override;

	/**
	 * [참고] 플레이어 캐릭터 연결 계획
	 * TODO: 나중에 제작될 플레이어 캐릭터(TPSCharacter 등)를 DefaultPawnClass에 연결할 예정입니다.
	 * 현재는 기본 폰을 사용합니다.
	 */
};
