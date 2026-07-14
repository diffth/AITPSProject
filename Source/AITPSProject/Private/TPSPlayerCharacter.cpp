// Copyright Epic Games, Inc. All Rights Reserved.


#include "TPSPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"


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

	// 카메라가 회전해도 캐릭터의 Yaw 회전이 강제로 동기화되지 않도록 설정 (이동 방향 회전을 위해 false 지정)
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 캐릭터가 이동하는 방향을 바라보도록 자동 회전을 활성화
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 캐릭터가 회전하는 속도 설정 (초당 회전 각도)
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
}

// 게임 시작 시 또는 스폰 시 호출됩니다.
void ATPSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임 시작 시 DefaultMappingContext를 로컬 플레이어 입력 시스템에 등록합니다.
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
				UE_LOG(LogTemp, Warning, TEXT("입력 매핑 컨텍스트(DefaultMappingContext)가 성공적으로 등록되었습니다."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext가 지정되지 않았습니다! 에디터에서 에셋을 할당해 주세요."));
			}
		}
	}

	// 캐릭터 생성 확인 로그 (노란색 - 일반 정보로 경고 레벨 Warning 사용)
	UE_LOG(LogTemp, Warning, TEXT("TPSPlayerCharacter가 월드에 준비되었습니다!"));
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

	// Enhanced Input Component로 캐스팅하여 액션을 바인딩합니다.
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 이동 입력 액션 바인딩
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATPSPlayerCharacter::Move);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MoveAction이 지정되지 않았습니다! 에디터에서 에셋을 할당해 주세요."));
		}

		// 시점 회전 입력 액션 바인딩
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATPSPlayerCharacter::Look);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("LookAction이 지정되지 않았습니다! 에디터에서 에셋을 할당해 주세요."));
		}


	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Enhanced Input Component를 찾을 수 없습니다."));
	}
}

void ATPSPlayerCharacter::Move(const FInputActionValue& Value)
{
	// 2D 벡터 형태의 입력값을 받습니다.
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 카메라가 바라보는 방향(Yaw 회전 값)을 구합니다.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 카메라 방향 기준 앞뒤(Forward) 및 좌우(Right) 벡터를 계산합니다.
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 요구사항: 입력값 X는 앞뒤(ForwardDirection), 입력값 Y는 좌우(RightDirection)로 매핑
		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}

void ATPSPlayerCharacter::Look(const FInputActionValue& Value)
{
	// 2D 벡터 형태의 마우스 입력값을 받습니다.
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 마우스 좌우 입력은 시점 좌우 회전(Yaw), 마우스 상하 입력은 시점 상하 회전(Pitch)에 적용합니다.
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


