# 🏰 Palworld Portfolio (UE5 Project)

## 📋 목차

- [게임 소개](#-게임-소개)
- [주요 스크립트](#-주요-스크립트)
  - [건축 시스템](#-건축-시스템)
  - [팰 교배 시스템](#-팰-교배-시스템)
  - [팰 스탯 시스템](#-팰-스탯-시스템)
  - [플레이어 시스템](#-플레이어-시스템)
- [기술 스택](#️-기술-스택)
- [개발자](#-개발자)

---

## 🎮 게임 소개

**Unreal Engine 5와 C++**를 사용하여 제작한 오픈월드 서바이벌 크래프팅 게임(Palworld 모작) 포트폴리오입니다.  
**Subsystem**을 활용한 모듈형 건축 시스템, **유전 알고리즘** 기반의 팰 교배, 그리고 **DataAsset**을 활용한 확장성 높은 데이터 관리 시스템을 중점적으로 구현하였습니다.

---

## 📜 주요 스크립트

게임의 핵심 메커니즘을 담당하는 주요 클래스와 대표적인 구현 코드를 소개합니다.

---

### 🏗️ 건축 시스템 (Building System)
* **역할:** 플레이어가 월드에 구조물을 배치하고 건설하는 기능을 전담합니다. `GameInstanceSubsystem`으로 구현되어 레벨 이동 간에도 상태를 유지하며 독립적으로 작동합니다.
* **핵심 기능:** `LineTrace`와 `Overlap` 검사를 통해 지면 스냅(Snap) 및 설치 가능 여부를 판별하고, **Dynamic Material**을 통해 시각적 피드백(초록/빨강)을 실시간으로 제공합니다.

<details>
<summary><b>[코드 보기] BuildingSubsystem::UpdatePreview (프리뷰 스냅 및 설치 검사)</b></summary>

```cpp
void UBuildingSubsystem::UpdatePreview()
{
    if (!bIsBuildingMode || !PreviewBuilding || !CurrentBuildingData) return;

    // ... (플레이어 앞 위치 계산 생략) ...

    // 1. 지면 스냅 (라인트레이스)
    FVector TraceStart = Location;
    FVector TraceEnd = Location - FVector(0, 0, 1000.f);
    
    // ... (TraceParams 설정 생략) ...

    if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, TraceParams))
    {
        Location = HitResult.Location; // 지면 위치로 보정
    }

    PreviewBuilding->SetActorLocation(Location);

    // 2. 설치 공간 유효성 검사 (Overlap 체크)
    bool bOverlap = false;
    // ... (Collision Query 파라미터 설정) ...

    for (UStaticMeshComponent* Comp : MeshComponents)
    {
        // 메시 단위로 겹침 검사 수행
        if (World->OverlapAnyTestByObjectType(BoxCenter, ..., FCollisionShape::MakeBox(BoxExtent), Params))
        {
            bOverlap = true;
            break; 
        }
    }

    bCanBuild = !bOverlap;

    // 3. 시각적 피드백 (Dynamic Material 색상 변경)
    FLinearColor Color = bCanBuild ? FLinearColor::White : FLinearColor::Red;
    for (UMaterialInstanceDynamic* MID : PreviewMIDs)
    {
        if (MID) MID->SetVectorParameterValue(FName("PreviewColor"), Color);
    }
}
```
</details>

---

### 🥚 팰 교배 시스템 (Breeding System)
* **역할:** 두 부모 팰(Pal)의 데이터를 기반으로 새로운 자식 팰을 생성합니다.
* **핵심 기능:** 부모의 개체값(IV)과 특성(Trait)을 확률적으로 유전받으며, 부모의 몸 색상을 HSV 색공간에서 혼합하여 자식의 고유한 색상을 결정하는 알고리즘이 구현되어 있습니다.

<details> <summary><b>[코드 보기] APal::Breed (유전 및 색상 혼합 알고리즘)</b></summary>

```cpp
APal* APal::Breed(UObject* WorldContextObject, APal* ParentA, APal* ParentB, FVector SpawnLocation)
{
    // ... (자식 액터 스폰 및 기본 설정) ...

    // 1. 개체값(IV) 유전 로직 (부모 A 30%, 부모 B 30%, 랜덤 40%)
    float HPRoll = FMath::FRand();
    if (HPRoll < 0.3f) Child->HP_IV = ParentA->HP_IV;
    else if (HPRoll < 0.6f) Child->HP_IV = ParentB->HP_IV;
    else Child->HP_IV = FMath::FRandRange(0.0f, 100.0f);
    // ... (공격력, 방어력 IV 유전 동일) ...

    // 2. 특성(Trait) 유전 및 돌연변이
    const float InheritanceChance = 0.6f;
    const float MutationChance = 0.2f;
    // ... (부모 특성 유전 및 랜덤 돌연변이 추가 로직) ...

    // 3. 색상 혼합 (RGB -> HSV 변환 후 Lerp)
    float MixRatio = FMath::FRand();
    FLinearColor ParentA_HSV = ParentA->BodyColor.LinearRGBToHSV();
    FLinearColor ParentB_HSV = ParentB->BodyColor.LinearRGBToHSV();

    FLinearColor Child_HSV;
    Child_HSV.R = FMath::Lerp(ParentA_HSV.R, ParentB_HSV.R, MixRatio); // 색조(Hue) 혼합
    Child_HSV.G = 1.0f; // 채도 고정
    Child_HSV.B = 1.0f; // 명도 고정

    // 등급에 따른 색상 보정 후 최종 적용
    Child->BodyColor = FLinearColor::MakeFromHSV8(Child_HSV.R, ...);
    
    return Child;
}
```
</details>

---

### 📊 팰 스탯 시스템 (Stats System)
* **역할:** 팰의 최종 능력치를 계산하고 관리합니다.

* **핵심 기능:** RecalculateStats 함수를 통해 **기본 종족값 + 개체값(IV) + 특성 배율(Trait Multiplier)**이 복합적으로 적용된 최종 스탯을 산출합니다. 데이터 주도적(Data-Driven) 설계를 통해 다양한 특성을 유연하게 적용합니다.

<details> <summary><b>[코드 보기] APal::RecalculateStats (스탯 연산)</b></summary>

```cpp
void APal::RecalculateStats()
{
    float HPScale_Total = 1.0f;
    float AttackScale_Total = 1.0f;
    
    // 1. 보유한 모든 특성(Trait) 순회 및 배율 합산
    const TMap<EPalTraitType, FPalTrait>& TraitMap = GetTraitMap();
    for (const EPalTraitType& TraitType : Traits)
    {
        if (const FPalTrait* TraitData = TraitMap.Find(TraitType))
        {
            HPScale_Total *= TraitData->HPScale;
            AttackScale_Total *= TraitData->AttackScale;
            // ... (기타 스탯 배율 누적) ...
        }
    }

    // 2. 최종 스탯 계산 공식: (기본값 + 개체값) * 특성 배율
    FinalMaxHP = (BaseMaxHP + HP_IV) * HPScale_Total;
    FinalAttack = (BaseAttack + Attack_IV) * AttackScale_Total;
    FinalDefense = (BaseDefense + Defense_IV) * DefenseScale_Total;

    // 현재 체력 보정
    CurrentHP = FMath::Min(CurrentHP, FinalMaxHP);
}
```
</details>

---

### 🚀 플레이어 시스템 (Player Progression)
* **역할:** 플레이어의 경험치, 레벨, 기술 포인트 등을 관리합니다.
* **핵심 기능:** 경험치 획득 시 레벨업 루프를 처리하고, 레벨에 따른 곡선형 필요 경험치(BaseXP * Level^Exponent)를 계산하여 RPG적인 성장 곡선을 구현했습니다.

<details> <summary><b>[코드 보기] PlayerSystem::LevelUp (성장 시스템)</b></summary>

```cpp
void APlayerSystem::AddXP(int32 Amount)
{
    CurrentXP += Amount;
    // 초과 경험치를 고려한 연속 레벨업 처리
    while (CurrentXP >= NextLevelXP)
    {
        CurrentXP -= NextLevelXP; 
        LevelUp(); 
    }
}

void APlayerSystem::LevelUp()
{
    Level++;
    
    // 지수 함수를 이용한 다음 레벨 필요 경험치 계산 (난이도 조절)
    NextLevelXP = 100 * FMath::Pow(Level, 2.1f);

    // 성장 보상 지급
    StatPoints += 1;
    TechPoints += 4;

    UE_LOG(LogTemp, Log, TEXT("LEVEL UP! Now Level %d"), Level);
}
```
</details>

---

### 🛠️ 기술 스택

* **Engine:** Unreal Engine 5.3
* **Language:** C++
* **Core Systems:**
  * Subsystem: 전역적인 건축 시스템 관리 (UGameInstanceSubsystem)
  * DataAsset: 아이템 및 건축물 데이터의 체계적 관리
  * Algorithm: 유전(Genetic) 알고리즘 및 HSV 색공간 연산

---

<div align="center">

## 👨‍💻 개발자

<br>

**천성준**

<br>
<br>

[![GitHub](https://img.shields.io/badge/GitHub-csj729-181717?style=for-the-badge&logo=github)](https://github.com/csj729/Palworld_Portfolio)

<br>

</div>

---
