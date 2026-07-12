// Copyright Epic Games, Inc. All Rights Reserved.


#include "TPSPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"

// 생성자: 기본값 설정
ATPSPlayerCharacter::ATPSPlayerCharacter()
{
 	// 이 캐릭터가 매 프레임 Tick()을 호출하도록 설정합니다. 
	// GEMINI.md 규칙에 따라 기본적으로는 비활성화하며, 필요 시 true로 변경합니다.
	PrimaryActorTick.bCanEverTick = false;

	// 스프링 암 컴포넌트 생성 및 설정
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = CameraDistance; // 카메라 거리 적용
	SpringArm->bUsePawnControlRotation = true;    // 마우스 시점 방향을 미리 회전 (컨트롤러 회전값 사용)

	// 카메라 컴포넌트 생성 및 스프링 암 소켓에 부착
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;     // 카메라 자체는 컨트롤 회전으로 별도 회전하지 않음

	// 임시 외형 메시 컴포넌트 생성 및 설정
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	// 캐릭터 몸통이 카메라가 바라보는 좌우 방향(Yaw)을 따라 회전하도록 설정
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
}

// 게임 시작 시 또는 스폰 시 호출됩니다.
void ATPSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 캐릭터 생성 확인 로그 (노란색 - 일반 정보)
	UE_LOG(LogTemp, Log, TEXT("TPSPlayerCharacter가 월드에 준비되었습니다!"));
}

// 매 프레임 호출됩니다.
void ATPSPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 입력을 바인딩하기 위해 호출됩니다.
void ATPSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// TODO: Enhanced Input 시스템을 사용한 이동 및 회전 액션 바인딩을 여기서 수행할 예정입니다.
}
