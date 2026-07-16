// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSEnemyCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "TPSEnemyFSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// 생성자: 기본값 설정
ATPSEnemyCharacter::ATPSEnemyCharacter()
{
 	// 이 캐릭터가 매 프레임 Tick()을 호출하도록 설정합니다. 
	// GEMINI.md 규칙에 따라 기본적으로는 비활성화하며, 필요 시 true로 변경합니다.
	PrimaryActorTick.bCanEverTick = false;

	// 임시 외형 메시 컴포넌트 생성 및 설정
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	// 사격 판정(Visibility 채널 등)이 메시 영역에 정상 적중되도록 콜리전 프로파일 설정
	StaticMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 적 AI FSM 컴포넌트 생성 및 등록 (요구사항 반영)
	EnemyFSM = CreateDefaultSubobject<UTPSEnemyFSMComponent>(TEXT("EnemyFSM"));

	// 마우스 시점 회전에 따라 몸이 돌아가는 기능은 비활성화 (요구사항 반영)
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 이동하는 방향을 바라보도록 자동 회전을 활성화 (요구사항 반영)
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// 캐릭터가 회전하는 속도 설정 (초당 회전 각도)
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	}

	// 사망 설정 기본값 매핑 (한글 주석)
	DeathRotation = FRotator(-90.f, 0.f, 0.f);
	DeathOffset = FVector(0.f, 0.f, -80.f);
	DeathScale = FVector(1.f, 1.f, 1.f);
	DestroyDelay = 2.0f;
}

// 게임 시작 시 또는 스폰 시 호출됩니다.
void ATPSEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 적 캐릭터 생성 확인 로그 (노란색 - 일반 정보로 경고 레벨 Warning 사용)
	UE_LOG(LogTemp, Warning, TEXT("TPSEnemyCharacter가 생성되어 준비되었습니다!"));
}

// 매 프레임 호출됩니다.
void ATPSEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ATPSEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 부모 클래스의 TakeDamage를 호출하여 최종 적용 데미지 값을 반환받습니다.
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 받은 데미지 수치를 "경고 로그(노란색 - Warning)"로 출력합니다. (요구사항 반영)
	UE_LOG(LogTemp, Warning, TEXT("[데미지 수신] 적(%s)이 %.1f의 데미지를 받았습니다! (원인: %s)"), 
		*GetName(), ActualDamage, DamageCauser ? *DamageCauser->GetName() : TEXT("알 수 없음"));

	// 데미지를 받으면 즉시 사망 처리합니다. (요구사항 반영, 한글 주석)
	HandleDead();

	return ActualDamage;
}

// 사망 처리를 담당하는 함수 (요구사항 반영 순서 준수, 한글 주석)
void ATPSEnemyCharacter::HandleDead()
{
	// 1. FSM 컴포넌트의 NotifyDead()를 호출합니다.
	if (EnemyFSM)
	{
		EnemyFSM->NotifyDead();
	}

	// 2. 캐릭터 이동을 비활성화합니다.
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}

	// 3. 몸통(캡슐)과 외형 메시의 충돌을 모두 끕니다.
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (StaticMesh)
	{
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 4. 외형 메시에 사망 연출을 적용합니다. (에디터 노출 변수 사용)
	if (StaticMesh)
	{
		StaticMesh->SetRelativeRotation(DeathRotation);
		StaticMesh->SetRelativeLocation(DeathOffset);
		StaticMesh->SetRelativeScale3D(DeathScale);
	}

	// 5. 일정 시간 후 자동으로 제거되도록 설정합니다. (에디터 노출 변수 사용)
	SetLifeSpan(DestroyDelay);

	UE_LOG(LogTemp, Warning, TEXT("[적 사망] '%s' 캐릭터의 사망 처리가 완료되었습니다. %.1f초 후 자동 제거됩니다."),
		*GetName(), DestroyDelay);
}
