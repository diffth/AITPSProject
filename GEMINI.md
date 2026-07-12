# 🎮 AITPSProject - GEMINI AI 가이드라인

이 프로젝트는 **언리얼 엔진 5.8(Unreal Engine 5.8)**을 기반으로 개발 중인 3인칭 슈터(TPS) AI 프로젝트인 **AITPSProject**입니다. 
이 문서는 프로젝트의 구조를 이해하고, 개발 환경을 원활하게 빌드 및 실행하며, 일관성 있는 C++ 및 에셋 작업 컨벤션을 유지할 수 있도록 돕는 종합 안내서입니다. 😊

---

## 🚀 1. 프로젝트 개요 (Project Overview)

- **엔진 버전:** Unreal Engine 5.8
- **프로젝트 이름:** AITPSProject (초기 이름: `TP_Blank`에서 마이그레이션됨)
- **주요 기술 및 렌더링 스택:**
  - **C++:** 최첨단 언리얼 빌드 시스템(UBT) 연동 및 솔루션 관리
  - **Enhanced Input System:** 정교하고 유연한 캐릭터 컨트롤을 위한 향상된 입력 플러그인 탑재
  - **Substrate 머티리얼 시스템:** 차세대 모듈형 물리 기반 머티리얼 컴포지션 프레임워크 활성화 (`r.Substrate=True`)
  - **Ray Tracing & Lumen:** 사실적인 전역 조명 및 그림자 처리가 적용된 데스크톱 하드웨어 타겟팅 설정

---

## 📂 2. 주요 폴더 구조 (Directory Structure)

```text
E:\UNREAL\UNRealProjects\AITPSProject\
├── Config\                  # 엔진, 입력, 프로젝트 기본 설정 파일 (.ini)
├── Content\                 # 블루프린트, 머티리얼, 맵 등 게임 리소스 에셋
│   └── _ART\
│       └── Maps\
│           └── TPSMap.umap  # 프로젝트 기본 시작 레벨
├── Source\                  # C++ 소스 코드 소스 루트
│   ├── AITPSProject.Target.cs
│   ├── AITPSProjectEditor.Target.cs
│   └── AITPSProject\
│       ├── AITPSProject.Build.cs # 프로젝트 모듈 종속성 설정
│       ├── AITPSProject.h        # 주 게임 모듈 헤더
│       └── AITPSProject.cpp      # 주 게임 모듈 구현 파일
├── AITPSProject.uproject    # 언리얼 프로젝트 설명 파일 (.uproject)
├── AITPSProject.slnx        # Visual Studio XML 솔루션 파일
└── Automation_AITPSProject.slnx # 테스트 및 자동화용 솔루션 파일
```

---

## 🛠️ 3. 빌드 및 실행 방법 (Building and Running)

### A. 개발 환경 준비
1. **Visual Studio 2022 이상** 설치를 권장합니다. (C++를 사용한 게임 개발 워크로드 필수)
2. `.uproject` 파일을 우클릭하여 **"Generate Visual Studio project files"**를 선택해 솔루션 파일과 빌드 종속성을 업데이트합니다.

### B. 빌드 명령어 (CLI / Unreal Build Tool)
일반적으로 Visual Studio 또는 언리얼 에디터 내부의 **라이브 코딩(Live Coding, `Ctrl + Alt + F11`)**을 사용하지만, 명령줄(PowerShell)에서도 다음 명령으로 프로젝트를 수동 빌드할 수 있습니다.

```powershell
# Development Editor 구성으로 Win64 플랫폼용 빌드 실행
& "EnginePath\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" AITPSProjectEditor Win64 Development -Project="E:\UNREAL\UNRealProjects\AITPSProject\AITPSProject.uproject" -waitmutex
```
*(참고: `EnginePath`는 사용자 로컬에 설치된 Unreal Engine 5.8의 디렉토리 경로로 대체되어야 합니다.)*

### C. 에디터 실행 방법
```powershell
# 개발용 에디터 실행
& "EnginePath\Binaries\Win64\UnrealEditor.exe" "E:\UNREAL\UNRealProjects\AITPSProject\AITPSProject.uproject"
```

---

## ✍️ 4. 개발 컨벤션 및 규칙 (Development Conventions)

### A. 언리얼 C++ 명명 규칙 (Naming Conventions)
에픽게임즈 공식 코딩 표준을 엄격히 준수합니다.
- **클래스 접두사(Prefixes):**
  - `A`: 월드에 배치 가능한 액터 (예: `AMyCharacter`)
  - `U`: 언리얼 오브젝트 기반 컴포넌트 또는 헬퍼 객체 (예: `UMyMovementComponent`)
  - `F`: 순수 C++ 구조체 (예: `FMyGameData`)
  - `I`: 인터페이스 클래스 (예: `IMyInteractable`)
  - `E`: 열거형 (예: `EMyState`)
- **변수 및 함수:**
  - **파스칼 케이스(PascalCase)**를 사용합니다 (예: `MyCharacterHealth`, `CalculateDamage()`).
  - 불리언(bool) 변수는 접두사 `b`를 붙입니다 (예: `bIsDead`, `bCanSprint`).

### B. 가비지 컬렉션 및 메모리 안전성
- 언리얼의 가비지 컬렉터(GC)에 의해 추적받기 위해 멤버 오브젝트 포인터에는 반드시 `UPROPERTY()` 매크로를 명시합니다.
- 약포인터가 필요한 비액터 참조 등은 `TWeakObjectPtr` 또는 `TSharedPtr` 등을 활용해 순환 참조를 방지합니다.

### C. 입력 시스템 (Enhanced Input)
- 새로운 캐릭터 동작을 구현할 경우, `AITPSProject.Build.cs`에 추가되어 있는 `EnhancedInput` 모듈을 연동합니다.
- `UInputAction`과 `UInputMappingContext` 에셋은 블루프린트에서 세팅하며, C++ 바인딩 시 `UEnhancedInputComponent`를 활용합니다.

---

## 🧪 5. 에셋 및 콘텐츠 가이드

- **에셋 명명 규칙:**
  - 블루프린트 클래스: `BP_` (예: `BP_PlayerCharacter`)
  - 머티리얼: `M_` (예: `M_MainLandscape`)
  - 머티리얼 인스턴스: `MI_` (예: `MI_PlayerBody`)
  - 텍스처: `T_` (예: `T_Brick_Diffuse`)
- **시작 맵:** `Content/_ART/Maps/TPSMap.umap`이 메인 개발 맵이자 에디터 시작 맵으로 구성되어 있습니다. 맵 내부 오브젝트나 조명을 수정할 때는 Substrate 및 레이 트레이싱 기능과의 호환성을 항상 체크해 주세요!

---

## 🏛️ 6. 프로젝트 전용 개발 지침 (Project Specific Guidelines)

### A. 프로젝트 철학 (Philosophy)
- **기능 구현 -> 동작 확인 -> 프롬프트 개선 -> 반복**: 점진적이고 반복적인 개발 사이클을 유지합니다.

### B. 개발 단계 (Development Stages)
- **프로토타입:** 핵심 기능 구현에 집중하며, 단순한 구조로 빠른 실험을 수행합니다.
- **알파:** 구조를 안정화하고, 리팩터링을 통해 시스템을 명확히 분리합니다.
- **베타:** 성능 최적화 및 최종 폴리싱 작업을 진행합니다.

### C. 협업 원칙 (Collaboration)
- 초급~중급 개발자도 쉽게 이해하고 따라갈 수 있도록 명확하고 친절하게 가이드를 작성합니다.
- **책임 분리(Separation of Concerns):** 각 클래스와 모듈의 역할을 명확히 하여 의존성을 낮춥니다.
- **프롬프트 재현성:** 프롬프트만으로도 기능을 재현하고 확장할 수 있는 구조를 지향합니다.

### D. 코딩 규칙 (Coding Rules)
- **한글 주석 필수:** 모든 클래스, 변수, 함수마다 역할을 설명하는 한글 주석을 작성합니다.
- **Tick 최적화:** `Tick`은 기본적으로 비활성화하며, 반드시 필요한 경우에만 신중하게 활성화합니다.
- **디버그 로그 컬러:**
  - 일반 정보: 노란색
  - 오류/위험: 빨간색
- **에디터 노출:** 에디터에서 조정이 필요한 값은 반드시 `Category`를 지정하여 `UPROPERTY`로 노출합니다.

### E. 소스 구조 (Source Structure)
- **헤더(.h):** 외부 노출이 필요한 선언은 `Public` 관점에서 관리합니다.
- **구현(.cpp):** 상세 로직은 `Private` 관점에서 캡슐화하여 구현합니다.

### F. 에디터 연동 (Editor Integration)
- **에셋 생성:** 블루프린트 에셋은 에디터에서 직접 생성하는 것을 원칙으로 합니다.
- **하드코딩 금지:** C++ 코드 내에 에셋 경로를 직접 적지 않으며, 에디터에서 선택할 수 있도록 변수화합니다.
- **데이터 기반:** 수치 및 설정값은 에디터에서 자유롭게 조정 가능하도록 설계합니다.

### G. 응답 형식 (Response Format)
- **새 파일 생성 시:** 파일의 전체 코드를 출력하여 즉시 사용 가능하게 합니다.
- **기존 파일 수정 시:** 바뀐 부분(Surgical Edit)만 명확하게 출력하여 가독성을 높입니다.

---

즐거운 언리얼 엔진 5 개발 시간이 되기를 응원합니다! 🚀✨ 궁금한 사항이 있다면 언제든 물어보세요!
