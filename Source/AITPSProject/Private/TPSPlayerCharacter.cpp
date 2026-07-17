// Copyright Epic Games, Inc. All Rights Reserved.


#include "TPSPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

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

	// 스켈레탈 메시 기본 오프셋 및 회전값 초기화
	MeshOffset = FVector(0.0f, 0.0f, -90.0f);
	MeshRotation = FRotator(0.0f, -90.0f, 0.0f);

	// SKM_Manny_Simple 스켈레탈 메시 로드 및 기본 메시 슬롯에 적용
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannyMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple"));
	if (MannyMesh.Succeeded() && GetMesh())
	{
		GetMesh()->SetSkeletalMesh(MannyMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(MeshOffset, MeshRotation);
	}

	// 애니메이션 블루프린트(ABP_TPSPlayer) 클래스 로드 및 적용
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(TEXT("/Game/Animation/ABP_TPSPlayer.ABP_TPSPlayer_C"));
	if (AnimBPClass.Succeeded() && GetMesh())
	{
		GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);
	}

	// 캡슐 컴포넌트 콜리전 설정: 플레이어 몸통(캡슐)은 사격 판정(ECC_Visibility)에서 제외
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn); // 오브젝트 타입은 Pawn으로 유지
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}

	// 메시 컴포넌트 콜리전 설정: 물리 충돌 없이 사격 판정(ECC_Visibility)만 받도록 설정
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 물리 충돌 방지 (쿼리 전용)
		GetMesh()->SetCollisionObjectType(ECC_Pawn); // 오브젝트 타입은 Pawn으로 유지
		GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore); // 다른 물리 충돌 채널은 전부 무시
		GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 사격 판정 채널만 블록
	}

	// 카메라가 회전하면 캐릭터의 Yaw 회전이 강제로 동기화되도록 설정 (카메라 방향 정면 유지)
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 캐릭터가 이동하는 방향을 바라보도록 자동 회전을 비활성화
	GetCharacterMovement()->bOrientRotationToMovement = false;
	// 캐릭터가 회전하는 속도 설정 (초당 회전 각도)
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
}

// 에디터에서 속성 수정 시 실시간으로 스켈레탈 메시의 보정값을 적용합니다.
void ATPSPlayerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (GetMesh())
	{
		GetMesh()->SetRelativeLocationAndRotation(MeshOffset, MeshRotation);
	}
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

		// 크로스헤어 UI 위젯 생성 및 화면 추가 (요구사항 반영)
		if (CrosshairWidgetClass)
		{
			CrosshairWidget = CreateWidget<UUserWidget>(PlayerController, CrosshairWidgetClass);
			if (CrosshairWidget)
			{
				CrosshairWidget->AddToViewport();
				UE_LOG(LogTemp, Warning, TEXT("크로스헤어 UI 위젯(WBP_Crosshair)이 화면에 성공적으로 추가되었습니다."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CrosshairWidgetClass가 지정되지 않았습니다! 에디터에서 WBP_Crosshair 에셋을 할당해 주세요."));
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

		// 점프 입력 액션 바인딩 (눌렀을 때 시작되도록 Started 이벤트에 바인딩)
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("JumpAction이 지정되지 않았습니다! 에디터에서 에셋을 할당해 주세요."));
		}

		// 사격 입력 액션 바인딩 (마우스 클릭 시 Triggered 이벤트로 바인딩)
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ATPSPlayerCharacter::Fire);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FireAction이 지정되지 않았습니다! 에디터에서 에셋을 할당해 주세요."));
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

void ATPSPlayerCharacter::Fire(const FInputActionValue& Value)
{
	if (Camera == nullptr) return;

	// 카메라 위치와 바라보는 방향 계산 (요구사항: 카메라의 위치와 바라보는 방향 기준)
	FVector Start = Camera->GetComponentLocation();
	FVector ForwardVector = Camera->GetForwardVector();
	FVector End = Start + (ForwardVector * FireRange);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // 자기 자신은 충돌에서 제외

	// ECC_Visibility 채널을 사용하여 단일 라인 트레이스 수행
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		CollisionParams
	);

	// 디버그 드로잉 지속 시간 (2초)
	float DrawTime = 2.0f;

	if (bHit)
	{
		// 무언가에 맞은 경우: 충돌 지점까지 초록색 선을 그리고, 그 자리에 빨간색 구형 점을 그린다
		DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Green, false, DrawTime, 0, 1.5f);
		DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 12.0f, FColor::Red, false, DrawTime);

		// 맞은 대상의 이름을 "경고 로그(노란색 - Warning)"로 출력하고, 데미지를 가합니다.
		if (AActor* HitActor = HitResult.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("[사격 적중] 맞은 대상: %s"), *HitActor->GetName());

			// 기본 데미지 전달 API 호출 (데미지 타입 기본값인 nullptr 전달)
			UGameplayStatics::ApplyDamage(
				HitActor,
				FireDamage,
				GetController(),
				this,
				nullptr
			);
		}

		// 맞은 표면의 법선 방향으로 회전값 계산 (로컬 X축이 법선을 바라보도록 회전)
		FRotator SpawnRotation = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();
		
		// 표면에 파묻히지 않도록 법선 방향으로 지정 오프셋만큼 띄운 스폰 위치 계산
		FVector SpawnLocation = HitResult.ImpactPoint + (HitResult.ImpactNormal * ImpactFXOffset);

		// 이펙트 에셋이 할당되어 있다면 스폰 (할당되지 않았다면 조용히 건너뜀)
		if (BulletImpactFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), 
				BulletImpactFX, 
				SpawnLocation, 
				SpawnRotation, 
				true
			);
		}
	}
	else
	{
		// 아무것도 맞지 않은 경우: 최대 사거리까지 빨간색 선을 그린다
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, DrawTime, 0, 1.5f);
		UE_LOG(LogTemp, Warning, TEXT("[사격] 대상을 맞추지 못했습니다."));
	}
}
