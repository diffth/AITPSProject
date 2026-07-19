// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSEnemyFSMComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "TPSEnemyAnimInstance.h"
#include "Components/CapsuleComponent.h"

// 생성자: 기본값 설정
UTPSEnemyFSMComponent::UTPSEnemyFSMComponent()
{
	// 이 컴포넌트가 매 프레임 TickComponent()를 호출하도록 설정합니다.
	PrimaryComponentTick.bCanEverTick = true;

	// 초기 상태는 대기(Idle)로 시작합니다.
	CurrentState = EEnemyState::Idle;

	// 공격 관련 기본 설정값 초기화 (한글 주석)
	AttackRange = 150.0f;
	AttackHitRadius = 100.0f;
	AttackDamage = 20.0f;
	AttackCooldown = 2.0f;
	AttackCooldownTimer = 0.0f;

	// 피격 경직 관련 기본 설정값 초기화 (한글 주석)
	HitReactDuration = 1.0f;
	HitReactTimer = 0.0f;
}

// 게임 시작 시 호출됩니다.
void UTPSEnemyFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	// 컴포넌트를 들고 있는 액터를 Character 타입으로 가져옵니다.
	OwnerCharacter = Cast<ACharacter>(GetOwner());

	// 월드에 존재하는 0번째 로컬 플레이어 캐릭터를 탐색해 타겟으로 지정합니다.
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	// 캐릭터의 최대 이동 속도를 에디터에서 설정한 값으로 매핑시킵니다.
	if (OwnerCharacter && OwnerCharacter->GetCharacterMovement())
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	}

	// 게임 시작 시 소유한 캐릭터 메시의 애니메이션 인스턴스를 Anim에 캐싱 및 초기화
	if (OwnerCharacter && OwnerCharacter->GetMesh())
	{
		Anim = Cast<UTPSEnemyAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
		if (Anim)
		{
			Anim->AnimState = EEnemyState::Idle;
		}
	}
}

// 매 프레임 호출됩니다.
void UTPSEnemyFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 필수 객체 참조가 유효하지 않으면 연산을 수행하지 않고 반환
	if (!OwnerCharacter || !TargetPlayer) return;

	// 사망(Dead) 상태인 경우 매 프레임 업데이트를 즉시 건너뜁니다. (한글 주석)
	if (CurrentState == EEnemyState::Dead)
	{
		return;
	}

	// 피격(Hit) 상태 매 프레임 처리 맨 앞 확인 (한글 주석)
	if (CurrentState == EEnemyState::Hit)
	{
		UpdateHit(DeltaTime);
		return; // 경직 상태일 때는 아래의 이동/공격 로직으로 분기하지 않음
	}

	// 공격 쿨다운 타이머 감소 처리 (매 프레임마다 줄어듦)
	if (AttackCooldownTimer > 0.0f)
	{
		AttackCooldownTimer -= DeltaTime;
		if (AttackCooldownTimer < 0.0f)
		{
			AttackCooldownTimer = 0.0f;
		}
	}

	// 상태 기계 동작 흐름 제어
	switch (CurrentState)
	{
		case EEnemyState::Idle:
			UpdateIdle();
			break;
		case EEnemyState::Move:
			UpdateMove();
			break;
		case EEnemyState::Attack:
			UpdateAttack();
			break;
		default:
			break;
	}
}

// AI 상태 변경 및 로그 처리
void UTPSEnemyFSMComponent::SetState(EEnemyState NewState)
{
	if (CurrentState == NewState) return;

	EEnemyState OldState = CurrentState;
	CurrentState = NewState;

	// FSM 상태 변경 시 애니메이션 인스턴스의 AnimState도 동기화
	if (Anim)
	{
		Anim->AnimState = NewState;
	}

	// 상태가 변경될 때마다 노란색 경고 로그(Warning)로 적 이름과 전후 상태를 출력합니다.
	UE_LOG(LogTemp, Warning, TEXT("[적 FSM] '%s'의 상태 변경: %s -> %s"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("알 수 없음"),
		*GetStateName(OldState),
		*GetStateName(NewState));

	// 새로운 상태 진입 시 수행해야 할 1회성 로직 (한글 주석)
	if (CurrentState == EEnemyState::Attack || CurrentState == EEnemyState::Hit)
	{
		// 진입 즉시 이동을 멈춥니다.
		if (OwnerCharacter && OwnerCharacter->GetCharacterMovement())
		{
			OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
		}
	}
}

// 상태 이름을 문자열로 얻는 함수
FString UTPSEnemyFSMComponent::GetStateName(EEnemyState State) const
{
	switch (State)
	{
		case EEnemyState::None:   return TEXT("None");
		case EEnemyState::Idle:   return TEXT("Idle");
		case EEnemyState::Move:   return TEXT("Move");
		case EEnemyState::Attack: return TEXT("Attack");
		case EEnemyState::Hit:    return TEXT("Hit");
		case EEnemyState::Dead:   return TEXT("Dead");
		default:                  return TEXT("Unknown");
	}
}

// 대기(Idle) 상태 업데이트 로직
void UTPSEnemyFSMComponent::UpdateIdle()
{
	if (!OwnerCharacter || !TargetPlayer) return;

	// 적 캐릭터와 플레이어 캐릭터 사이의 거리 계산
	float Distance = FVector::Dist(OwnerCharacter->GetActorLocation(), TargetPlayer->GetActorLocation());

	// 감지 범위 내에 들어오면 즉시 이동(Move) 상태로 전환합니다.
	if (Distance <= DetectionRange)
	{
		SetState(EEnemyState::Move);
	}
}

// 이동(Move) 상태 업데이트 로직
void UTPSEnemyFSMComponent::UpdateMove()
{
	if (!OwnerCharacter || !TargetPlayer) return;

	FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	FVector TargetLocation = TargetPlayer->GetActorLocation();

	// 매 프레임 거리를 체크하여 범위 판단
	float Distance = FVector::Dist(OwnerLocation, TargetLocation);

	// 플레이어가 감지 범위를 벗어나면 대기(Idle) 상태로 복귀합니다.
	if (Distance > DetectionRange)
	{
		SetState(EEnemyState::Idle);
		return;
	}

	// 플레이어와의 거리가 공격 범위 이내이고 쿨다운이 없을 때 공격 상태로 전환합니다. (한글 주석)
	if (Distance <= AttackRange && AttackCooldownTimer <= 0.0f)
	{
		SetState(EEnemyState::Attack);
		if (Anim)
		{
			Anim->bAttackPlay = true;
		}
		return;
	}

	// 에디터 튜닝 대응: 에디터에서 이동 속도 변수를 변경할 수 있으므로, 매 프레임 MaxWalkSpeed를 맞춰줍니다.
	if (OwnerCharacter->GetCharacterMovement())
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	}

	// 위아래 방향(Z축)은 무시하고 수평 평면상에서의 방향 벡터 계산
	FVector Direction = FVector(TargetLocation.X - OwnerLocation.X, TargetLocation.Y - OwnerLocation.Y, 0.0f);
	Direction.Normalize();

	// 플레이어를 향해 직선 수평 이동 입력 전달
	OwnerCharacter->AddMovementInput(Direction, 1.0f);
}

// 공격(Attack) 상태 업데이트 로직 (한글 주석)
void UTPSEnemyFSMComponent::UpdateAttack()
{
	if (!OwnerCharacter || !TargetPlayer) return;

	// 적 캐릭터와 플레이어 캐릭터 사이의 거리 계산
	float Distance = FVector::Dist(OwnerCharacter->GetActorLocation(), TargetPlayer->GetActorLocation());

	// 플레이어가 공격 범위를 벗어나면 bAttackPlay=false로 설정하고 이동 상태로 복귀
	if (Distance > AttackRange)
	{
		if (Anim)
		{
			Anim->bAttackPlay = false;
		}
		SetState(EEnemyState::Move);
		return;
	}

	// 공격 상태에서는 일정 시간(AttackCooldown)마다 근접 공격을 실행하고 bAttackPlay=true로 설정
	if (AttackCooldownTimer <= 0.0f)
	{
		// 공격 위치 설정 (적 캐릭터의 현재 위치)
		FVector AttackLocation = OwnerCharacter->GetActorLocation();
		
		// 오버랩 결과 및 쿼리 파라미터 설정
		TArray<FOverlapResult> OverlapResults;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwnerCharacter); // 공격자 자신은 제외

		// AttackHitRadius 반경의 구체를 생성하여 감지
		FCollisionShape SphereShape = FCollisionShape::MakeSphere(AttackHitRadius);
		
		// ECC_Pawn 채널을 사용해 구체 내 Pawn 대상 오버랩 수행
		bool bHit = GetWorld()->OverlapMultiByChannel(
			OverlapResults,
			AttackLocation,
			FQuat::Identity,
			ECC_Pawn,
			SphereShape,
			QueryParams
		);

		// 감지된 캐릭터들을 담을 배열
		TArray<ACharacter*> DetectedCharacters;
		if (bHit)
		{
			for (const FOverlapResult& Result : OverlapResults)
			{
				ACharacter* DetectedChar = Cast<ACharacter>(Result.GetActor());
				// 감지된 액터가 유효한 ACharacter이고 자신이 아니라면 목록에 추가
				if (DetectedChar && DetectedChar != OwnerCharacter)
				{
					DetectedCharacters.Add(DetectedChar);
				}
			}
		}

		// 감지된 대상이 있을 경우 데미지를 입힘 (제한: 근접 공격 내부의 데미지 처리 코드는 수정하지 않는다.)
		if (DetectedCharacters.Num() > 0)
		{
			for (ACharacter* DamagedChar : DetectedCharacters)
			{
				// 감지된 대상에게 AttackDamage 만큼 데미지 적용
				UGameplayStatics::ApplyDamage(
					DamagedChar,
					AttackDamage,
					OwnerCharacter->GetController(),
					OwnerCharacter,
					UDamageType::StaticClass()
				);

				// 데미지 전달 여부를 노란색 경고 로그(Warning)로 출력
				UE_LOG(LogTemp, Warning, TEXT("[적 공격] '%s'가 반경 %.1f 내의 '%s'에게 %.1f 만큼의 데미지를 전달했습니다."),
					*OwnerCharacter->GetName(),
					AttackHitRadius,
					*DamagedChar->GetName(),
					AttackDamage);
			}
		}
		else
		{
			// 감지된 대상이 없더라도 근접 공격 시도는 수행되었으므로 로그를 남김
			UE_LOG(LogTemp, Warning, TEXT("[적 공격] '%s' 주변에 감지된 캐릭터가 없어 데미지를 입히지 못했습니다."),
				*OwnerCharacter->GetName());
		}

		// 공격 성공/시도 후 쿨다운 타이머 설정 및 bAttackPlay=true 설정
		AttackCooldownTimer = AttackCooldown;
		if (Anim)
		{
			Anim->bAttackPlay = true;
		}
	}
}

// 사망했을 때 호출되어 사망 상태로 전환하고 FSM 틱을 끄는 함수 (한글 주석)
void UTPSEnemyFSMComponent::NotifyDead()
{
	// 중복으로 사망 처리되지 않도록 막습니다.
	if (CurrentState == EEnemyState::Dead) return;

	// 사망(Dead) 상태로 전환합니다.
	SetState(EEnemyState::Dead);

	// 사망 상태 진입 시 매 프레임 업데이트(Tick)를 완전히 비활성화합니다.
	SetComponentTickEnabled(false);

	if (OwnerCharacter != nullptr)
	{
		// 이동 즉시 멈추고 비활성화 (요구사항 반영)
		if (OwnerCharacter->GetCharacterMovement())
		{
			OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
			OwnerCharacter->GetCharacterMovement()->DisableMovement();
		}

		// 몸통(캡슐)과 메시 충돌을 모두 끕니다. (요구사항 반영)
		if (OwnerCharacter->GetCapsuleComponent())
		{
			OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		if (OwnerCharacter->GetMesh())
		{
			OwnerCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// PlayDeadAnim() 호출 (요구사항 반영)
		if (UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance())
		{
			if (UTPSEnemyAnimInstance* EnemyAnimInst = Cast<UTPSEnemyAnimInstance>(AnimInst))
			{
				EnemyAnimInst->PlayDeadAnim();
			}
		}
	}
}

// 피격 당했을 때 외부에서 호출하여 피격 상태로 전환시키는 함수 (한글 주석)
void UTPSEnemyFSMComponent::NotifyHit()
{
	// 즉시 피격(Hit) 상태로 전환 (이동 정지는 SetState 진입 로직에서 처리됨)
	SetState(EEnemyState::Hit);

	// 경직 타이머를 설정값으로 지정
	HitReactTimer = HitReactDuration;
}

// 피격(Hit) 상태 업데이트 로직 (한글 주석)
void UTPSEnemyFSMComponent::UpdateHit(float DeltaTime)
{
	if (HitReactTimer > 0.0f)
	{
		// 경직 타이머 매 프레임 감소
		HitReactTimer -= DeltaTime;
		if (HitReactTimer <= 0.0f)
		{
			HitReactTimer = 0.0f;

			// 경직이 끝났을 때 타겟 플레이어의 유효성 및 범위 판단
			if (TargetPlayer)
			{
				float Distance = FVector::Dist(OwnerCharacter->GetActorLocation(), TargetPlayer->GetActorLocation());
				if (Distance <= DetectionRange)
				{
					// 플레이어가 감지 범위 내에 있으면 추적(Move) 상태로 전환
					SetState(EEnemyState::Move);
				}
				else
				{
					// 감지 범위를 벗어나 있으면 대기(Idle) 상태로 전환
					SetState(EEnemyState::Idle);
				}
			}
			else
			{
				// 플레이어가 없으면 대기(Idle) 상태로 전환
				SetState(EEnemyState::Idle);
			}
		}
	}
}
