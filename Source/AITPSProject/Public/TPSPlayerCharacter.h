// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayerCharacter.generated.h"

/**
 * ATPSPlayerCharacter
 * 
 * 플레이어가 직접 조종하는 기본 캐릭터 클래스입니다.
 */
UCLASS()
class AITPSPROJECT_API ATPSPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// 생성자 선언
	ATPSPlayerCharacter();

protected:
	// 게임 시작 시 또는 스폰 시 호출됩니다.
	virtual void BeginPlay() override;

public:	
	// 매 프레임 호출됩니다.
	virtual void Tick(float DeltaTime) override;

	// 입력을 바인딩하기 위해 호출됩니다.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	 * [참고] 카메라 및 입력 기능 추가 계획
	 * TODO: 이동(Move) 및 회전(Look) 입력을 위한 Enhanced Input 설정을 추가할 예정입니다.
	 */

private:
	// 3인칭 카메라 거리를 유지하기 위한 스프링암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	// 플레이어 시점을 제공하는 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	// 캐릭터의 임시 외형을 보여주기 위한 스태틱 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;

	// 에디터에서 직접 조정할 수 있는 카메라의 기본 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraDistance = 400.0f;
};
