// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSEnemyCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "TPSEnemyFSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TPSEnemyAnimInstance.h"

// 생성자: 기본값 설정
ATPSEnemyCharacter::ATPSEnemyCharacter()
{
 	// 이 캐릭터가 매 프레임 Tick()을 호출하도록 설정합니다. 
	// GEMINI.md 규칙에 따라 기본적으로는 비활성화하며, 필요 시 true로 변경합니다.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // 기본 상태에서는 틱 오버헤드를 막기 위해 비활성화합니다.

	// 적 AI FSM 컴포넌트 생성 및 등록 (요구사항 반영)
	EnemyFSM = CreateDefaultSubobject<UTPSEnemyFSMComponent>(TEXT("EnemyFSM"));

	// 기본 외형 오프셋 및 회전값 초기화
	MeshOffset = FVector(0.0f, 0.0f, -90.0f);
	MeshRotation = FRotator(0.0f, -90.0f, 0.0f);

	// vampire_a_lusth 스켈레탈 메시 로드 및 기본 메시 슬롯에 적용
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> EnemyMesh(TEXT("/Game/Enemy/Model/vampire_a_lusth"));
	if (EnemyMesh.Succeeded() && GetMesh())
	{
		GetMesh()->SetSkeletalMesh(EnemyMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(MeshOffset, MeshRotation);
	}

	// 애니메이션 블루프린트(ABP_TPSEnemy) 클래스 로드 및 적용
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(TEXT("/Game/_ART/Animation/ABP_TPSEnemy.ABP_TPSEnemy_C"));
	if (AnimBPClass.Succeeded() && GetMesh())
	{
		GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);
	}

	// 캡슐 컴포넌트 콜리전 설정: 몸통(캡슐)은 사격 판정(ECC_Visibility)에서 제외
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn); // 오브젝트 타입은 Pawn으로 유지
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}

	// 메시 컴포넌트 콜리전 설정: 물리 충돌 없이 사격 판정(ECC_Visibility)만 받도록 설정
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 물리 충돌 방지 (쿼리 전용)
		GetMesh()->SetCollisionObjectType(ECC_Pawn);                  // 오브젝트 타입은 Pawn으로 유지
		GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);       // 다른 물리 충돌 채널은 전부 무시
		GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 사격 판정 채널만 블록
	}

	// 마우스 시점 회전에 따라 몸이 돌아가는 기능은 비활성화 (요구사항 반영)
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 이동하는 방향을 바라보도록 자동 회전을 활성화 (요구사항 반영)
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// 에디터 노출 변수로 회전 속도 및 최대 이동 속도 설정
		GetCharacterMovement()->RotationRate = RotationRate;
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	}

	// HP 시스템 변수 초기화
	hp = 3;
	maxHp = 3;

	// 가라앉기 시스템 변수 초기화
	sinkDelayTime = 3.0f;
	sinkSpeed = 50.0f;
	removeZ = -200.0f;
	bShouldSink = false;
	sinkTimer = 0.0f;
	targetSinkZ = 0.0f;
}

// 에디터에서 속성 수정 시 실시간으로 스켈레탈 메시의 보정값을 적용합니다.
void ATPSEnemyCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (GetMesh())
	{
		GetMesh()->SetRelativeLocationAndRotation(MeshOffset, MeshRotation);
	}
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

	// 사망 후 가라앉기 연출 처리 (요구사항 반영)
	if (bShouldSink)
	{
		sinkTimer += DeltaTime;
		if (sinkTimer >= sinkDelayTime)
		{
			FVector CurrentLoc = GetActorLocation();
			CurrentLoc.Z -= sinkSpeed * DeltaTime;
			SetActorLocation(CurrentLoc);

			// removeZ 이하 도달 시 오브젝트 제거
			if (CurrentLoc.Z <= targetSinkZ)
			{
				Destroy();
			}
		}
	}
}

float ATPSEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 부모 클래스의 TakeDamage를 호출하여 최종 적용 데미지 값을 반환받습니다.
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 데미지를 받으면 hp를 1 감소시킵니다.
	hp = FMath::Max(0, hp - 1);

	// 받은 데미지와 남은 hp 수치를 "경고 로그(노란색 - Warning)"로 출력합니다. (요구사항 반영)
	UE_LOG(LogTemp, Warning, TEXT("[데미지 수신] 적(%s)이 %.1f의 데미지를 받았습니다! 현재 HP: %d/%d (원인: %s)"), 
		*GetName(), ActualDamage, hp, maxHp, DamageCauser ? *DamageCauser->GetName() : TEXT("알 수 없음"));

	// hp 상태에 따른 분기 처리
	if (hp > 0)
	{
		// hp가 남아 있으면 피격(Hit) 상태로 전환
		if (EnemyFSM)
		{
			EnemyFSM->NotifyHit();
		}

		// 피격 애니메이션 재생
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
			{
				if (UTPSEnemyAnimInstance* EnemyAnimInst = Cast<UTPSEnemyAnimInstance>(AnimInst))
				{
					EnemyAnimInst->PlayDamageAnim();
				}
			}
		}
	}
	else
	{
		// hp가 0 이하이면 사망(Dead) 상태로 전환
		HandleDead();
	}

	return ActualDamage;
}

// 사망 처리를 담당하는 함수 (요구사항 반영 순서 준수, 한글 주석)
void ATPSEnemyCharacter::HandleDead()
{
	// FSM 컴포넌트의 NotifyDead()를 호출하여 이동 및 충돌 비활성화, 사망 애니메이션 재생을 처리합니다.
	if (EnemyFSM)
	{
		EnemyFSM->NotifyDead();
	}

	// 가라앉기 연출 상태 활성화 및 대상 Z 위치 설정
	bShouldSink = true;
	sinkTimer = 0.0f;
	targetSinkZ = GetActorLocation().Z + removeZ;

	// 사망 후 매 프레임 가라앉기를 계산하기 위해 액터 틱을 강제로 켭니다.
	SetActorTickEnabled(true);

	UE_LOG(LogTemp, Warning, TEXT("[적 사망] '%s' 캐릭터의 사망 처리가 시작되었습니다. %.1f초 후 땅속으로 가라앉습니다."),
		*GetName(), sinkDelayTime);
}
