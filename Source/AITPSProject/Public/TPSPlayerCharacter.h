// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
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

	// 에디터에서 속성 변경 시 실시간 반영을 위한 OnConstruction 오버라이드
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// 매 프레임 호출됩니다.
	virtual void Tick(float DeltaTime) override;

	// 입력을 바인딩하기 위해 호출됩니다.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/**
	 * 이동(Move) 입력을 처리하는 함수입니다.
	 * @param Value 입력 액션으로부터 넘어온 2D 벡터값 (X: 앞뒤, Y: 좌우)
	 */
	void Move(const FInputActionValue& Value);

	/**
	 * 시점 회전(Look) 입력을 처리하는 함수입니다.
	 * @param Value 마우스 상하/좌우 움직임에 따른 2D 벡터값 (X: Yaw, Y: Pitch)
	 */
	void Look(const FInputActionValue& Value);

	/**
	 * 사격(Fire) 입력을 처리하는 함수입니다.
	 */
	void Fire(const FInputActionValue& Value);

private:
	// 3인칭 카메라 거리를 유지하기 위한 스프링암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	// 플레이어 시점을 제공하는 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	// 에디터에서 직접 조정할 수 있는 스켈레탈 메시의 위치 보정 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	FVector MeshOffset;

	// 에디터에서 직접 조정할 수 있는 스켈레탈 메시의 회전 보정 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	FRotator MeshRotation;

	// 에디터에서 직접 조정할 수 있는 카메라의 기본 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraDistance = 400.0f;

	// ==========================================
	// 입력 시스템 (Enhanced Input) 에셋 설정
	// ==========================================

	// 기본 입력 매핑 컨텍스트 (IMC_PlayerDefault)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	// 이동 입력 액션 (IA_PlayerMove)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	// 시점 회전 입력 액션 (IA_PlayerLook)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	// 점프 입력 액션 (IA_PlayerJump)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	// 사격 입력 액션 (IA_PlayerFire)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	// 사격의 최대 사거리 (디폴트 10,000.0f)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float FireRange = 10000.0f;

	// 사격 시 가할 기본 데미지 수치 (에디터에서 자유롭게 조정 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float FireDamage = 20.0f;

	// 오른손에 부착될 총기(라이플) 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* RifleMeshComp;

	// 사격 시 재생할 총소리 사운드 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundBase* BulletSound;

	// ==========================================
	// UI 및 사용자 인터페이스 설정
	// ==========================================

	// 화면에 표시할 크로스헤어 UI 위젯 클래스 (에디터에서 WBP_Crosshair를 할당할 수 있도록 노출)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> CrosshairWidgetClass;

	// 생성 및 화면에 추가된 크로스헤어 UI 위젯의 런타임 인스턴스
	UPROPERTY(Transient)
	class UUserWidget* CrosshairWidget;

	// ==========================================
	// 이펙트 및 파티클 설정
	// ==========================================

	// 사격 적중 시 생성할 파티클 이펙트 (에디터에서 P_BulletEffect 에셋을 할당 가능)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* BulletImpactFX;

	// 이펙트가 표면에 파묻히지 않도록 표면 법선 방향으로 띄울 오프셋 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	float ImpactFXOffset = 2.0f;
};
