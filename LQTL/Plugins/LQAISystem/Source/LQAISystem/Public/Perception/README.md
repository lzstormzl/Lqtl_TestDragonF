# LQAI Perception System - Documentation

## Overview

The LQAI Perception System is a lightweight, data-driven AI perception framework that uses **weight-based target prioritization**. Unlike Unreal's heavy AIPerception system, this uses struct-based handles for efficient runtime performance.

**Key Features:**
- **Multiple perception types**: Range, Sight (cone-based), and Tag-based
- **Weight-based target selection**: Automatically selects highest priority target
- **Flexible configuration**: Configure handles in editor or add/remove at runtime
- **Debug visualization**: Built-in visual debugging tools
- **Performance-focused**: Struct-based (no UObject overhead)

---

## Core Concepts

### 1. Component + Handles Architecture
- **`ULQAIPerceptionComponent`** - Actor component that manages perception
- **Perception Handles** - Struct-based "sensors" that detect targets and report perception
- **Weight System** - Each handle contributes weight when perceiving; highest total weight = current target

### 2. Weight Calculation
```
Effective Weight = Handle.Weight × WeightModifier(0.0-1.0)
Total Weight = Sum of all perceiving handles' effective weights
Current Target = Actor with highest total weight
```

**Example:**
```
AI perceives Player and Enemy:

Handle: Sight (Weight=2.0)    → Player: 0.8 modifier (close, centered)  = 1.6
                              → Enemy: 0.3 modifier (far, edge)         = 0.6
Handle: Range (Weight=1.0)    → Player: 1.0 modifier (very close)      = 1.0
                              → Enemy: not in range                     = 0.0
Handle: Threat (Weight=3.0)   → Player: 0.0 (no threat tag)            = 0.0
                              → Enemy: 1.0 (has threat tag)             = 3.0

Player Total Weight: 1.6 + 1.0 = 2.6
Enemy Total Weight:  0.6 + 3.0 = 3.6

Current Target = Enemy (3.6 > 2.6)
```

---

## Quick Start

### Step 1: Add Component to AI Actor

**C++:**
```cpp
UCLASS()
class AMyAICharacter : public ACharacter
{
    GENERATED_BODY()

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    ULQAIPerceptionComponent* PerceptionComponent;
};

AMyAICharacter::AMyAICharacter()
{
    PerceptionComponent = CreateDefaultSubobject<ULQAIPerceptionComponent>(TEXT("PerceptionComponent"));
}
```

**Blueprint:**
1. Open AI character blueprint
2. Add Component → Search "LQAI Perception Component"
3. Configure in Details panel

### Step 2: Configure Perception Handles

Add handles in the component's **Default Handles** array:

1. **Add Range Handle:**
   - Struct Type: `FLQAIPerceptionHandle_Range`
   - HandleTag: `AI.Perception.ProximityAlert`
   - Weight: `1.0`
   - DetectionRadius: `500.0`

2. **Add Sight Handle:**
   - Struct Type: `FLQAIPerceptionHandle_Sight`
   - HandleTag: `AI.Perception.Vision`
   - Weight: `2.0`
   - SightRadius: `2000.0`
   - SightHalfAngle: `60.0`

3. **Add Threat Detection:**
   - Struct Type: `FLQAIPerceptionHandle_Tag`
   - HandleTag: `AI.Perception.ThreatDetection`
   - Weight: `5.0`
   - RequiredTagQuery: `Enemy.Threat`

### Step 3: Query Current Target

```cpp
AActor* CurrentTarget = PerceptionComponent->GetCurrentTarget();

if (CurrentTarget)
{
    float TargetWeight = PerceptionComponent->GetTargetWeight(CurrentTarget);
    // Use CurrentTarget for AI behavior
}
```

---

## Perception Handle Types

### 1. Range Handle (`FLQAIPerceptionHandle_Range`)

Detects targets within distance range.

**Configuration Properties:**

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `DetectionRadius` | float | 1000.0 | Maximum detection distance |
| `MinDistance` | float | 0.0 | Minimum detection distance |
| `bRequireLineOfSight` | bool | false | Requires unobstructed line to target |
| `TraceChannel` | ECollisionChannel | Visibility | Collision channel for LOS trace |
| `TargetClasses` | TArray | Empty | Filter by actor class (empty = all) |
| `Weight` | float | 1.0 | Base weight for target selection |

**Weight Modifier Behavior:**
- Linear falloff: `1.0` at MinDistance → `0.0` at DetectionRadius
- Closer targets get higher weight

**Use Cases:**
- Proximity detection
- "Aggro radius" behavior
- Personal space awareness

**Example Configuration:**
```cpp
FLQAIPerceptionHandle_Range RangeHandle;
RangeHandle.HandleTag = FGameplayTag::RequestGameplayTag("AI.Perception.Proximity");
RangeHandle.Weight = 1.0f;
RangeHandle.DetectionRadius = 800.0f;
RangeHandle.MinDistance = 100.0f;
RangeHandle.bRequireLineOfSight = false;
```

---

### 2. Sight Handle (`FLQAIPerceptionHandle_Sight`)

Detects targets within a cone with line-of-sight check.

**Configuration Properties:**

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `SightRadius` | float | 2000.0 | Maximum sight distance |
| `SightHalfAngle` | float | 60.0 | Half-angle of cone in degrees |
| `TraceChannel` | ECollisionChannel | Visibility | Collision channel for LOS trace |
| `bUseEyeLocation` | bool | true | Use pawn eye location vs actor location |
| `TargetClasses` | TArray | Empty | Filter by actor class (empty = all) |
| `Weight` | float | 1.0 | Base weight for target selection |

**Weight Modifier Behavior:**
```cpp
DistanceFactor = 1.0 - (Distance / SightRadius)
AngleFactor = Normalized angle from cone edge (0.0) to center (1.0)
WeightModifier = DistanceFactor × AngleFactor
```
- Targets closer + more centered = higher weight

**Use Cases:**
- Vision-based detection
- Stealth gameplay
- FOV-aware AI

**Example Configuration:**
```cpp
FLQAIPerceptionHandle_Sight SightHandle;
SightHandle.HandleTag = FGameplayTag::RequestGameplayTag("AI.Perception.Vision");
SightHandle.Weight = 2.0f;
SightHandle.SightRadius = 2000.0f;
SightHandle.SightHalfAngle = 60.0f;
SightHandle.bUseEyeLocation = true;
SightHandle.TraceChannel = ECC_Visibility;
```

---

### 3. Tag Handle (`FLQAIPerceptionHandle_Tag`)

Detects actors with specific gameplay tags.

**Configuration Properties:**

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `RequiredTagQuery` | FGameplayTagQuery | Empty | Gameplay tag query to match |
| `bRequireRange` | bool | false | Also check distance |
| `MaxRange` | float | 1000.0 | Maximum range if required |
| `TargetClasses` | TArray | Empty | Filter by actor class (empty = all) |
| `Weight` | float | 1.0 | Base weight for target selection |

**Weight Modifier Behavior:**
- Without range: `1.0` (binary - has tags or not)
- With range: `1.0 - (Distance / MaxRange)`

**Use Cases:**
- Threat detection (e.g., "Enemy" tag)
- Ally recognition
- Objective tracking (e.g., "Collectible" tag)

**Example Configuration:**
```cpp
FLQAIPerceptionHandle_Tag TagHandle;
TagHandle.HandleTag = FGameplayTag::RequestGameplayTag("AI.Perception.Threat");
TagHandle.Weight = 5.0f;
TagHandle.RequiredTagQuery = FGameplayTagQuery::BuildQuery(
    FGameplayTagQueryExpression().AllTagsMatch()
    .AddTag(FGameplayTag::RequestGameplayTag("Enemy.Threat"))
);
TagHandle.bRequireRange = true;
TagHandle.MaxRange = 1500.0f;
```

---

## Runtime Handle Management

### Adding Handles at Runtime

```cpp
void AMyAICharacter::EnableCombatPerception()
{
    // Create sight handle
    TInstancedStruct<FLQAIPerceptionHandle> SightHandleStruct;
    FLQAIPerceptionHandle_Sight SightHandle;
    SightHandle.HandleTag = FGameplayTag::RequestGameplayTag("AI.Perception.CombatSight");
    SightHandle.Weight = 3.0f;
    SightHandle.SightRadius = 1500.0f;
    SightHandle.SightHalfAngle = 90.0f;

    SightHandleStruct.InitializeAs<FLQAIPerceptionHandle_Sight>(SightHandle);
    PerceptionComponent->AddHandle(SightHandleStruct);
}
```

### Removing Handles

```cpp
void AMyAICharacter::DisableCombatPerception()
{
    FGameplayTag HandleTag = FGameplayTag::RequestGameplayTag("AI.Perception.CombatSight");
    PerceptionComponent->RemoveHandle(HandleTag);
}
```

### Clearing All Handles

```cpp
void AMyAICharacter::ResetPerception()
{
    PerceptionComponent->ClearHandles();
}
```

### Checking if Handle Exists

```cpp
bool bHasVision = PerceptionComponent->HasHandle(
    FGameplayTag::RequestGameplayTag("AI.Perception.Vision")
);
```

---

## Querying Perception

### Get Current Target

```cpp
// Get the highest-weight target
AActor* CurrentTarget = PerceptionComponent->GetCurrentTarget();

if (CurrentTarget)
{
    float TargetWeight = PerceptionComponent->GetTargetWeight(CurrentTarget);
    UE_LOG(LogTemp, Log, TEXT("UMyAICharacter::Tick: Targeting (%s) with weight (%.2f)"),
        *CurrentTarget->GetName(), TargetWeight);
}
```

### Get Highest Weight Target (Without Lock Logic)

```cpp
// Always returns highest weight, ignoring target lock
AActor* HighestWeightTarget = PerceptionComponent->GetHighestWeightTarget();
```

### Check if Perceiving Specific Actor

```cpp
bool bCanSeePlayer = PerceptionComponent->HasAnyPerception(PlayerActor);

if (bCanSeePlayer)
{
    const FLQAIPerceptionScore* Score = PerceptionComponent->GetPerceptionScore(PlayerActor);

    if (Score)
    {
        UE_LOG(LogTemp, Log, TEXT("UMyAICharacter::CheckPlayer: Total weight (%.2f)"), Score->TotalWeight);

        // Check which handles perceive the player
        for (const FGameplayTag& HandleTag : Score->PerceivingHandles)
        {
            UE_LOG(LogTemp, Log, TEXT("UMyAICharacter::CheckPlayer: Handle (%s) perceives player"),
                *HandleTag.ToString());
        }

        // Check if perception is retained (vs direct)
        if (Score->bAnyRetained)
        {
            UE_LOG(LogTemp, Log, TEXT("UMyAICharacter::CheckPlayer: Using retained perception"));
        }
    }
}
```

### Get All Perceived Actors

```cpp
const TMap<TWeakObjectPtr<AActor>, FLQAIPerceptionScore>& PerceivedActors =
    PerceptionComponent->GetPerceivedActors();

for (const auto& Pair : PerceivedActors)
{
    if (Pair.Key.IsValid())
    {
        AActor* PerceivedActor = Pair.Key.Get();
        const FLQAIPerceptionScore& Score = Pair.Value;

        UE_LOG(LogTemp, Log, TEXT("UMyAICharacter::ScanTargets: Perceiving (%s) with weight (%.2f)"),
            *PerceivedActor->GetName(), Score.TotalWeight);
    }
}
```

---

## Target Locking

Prevent AI from switching targets during combat:

### Enable/Disable Target Lock

```cpp
// Enable target lock (prevents auto-switching)
PerceptionComponent->SetTargetLockEnabled(true);

// Check lock status
bool bLocked = PerceptionComponent->IsTargetLocked();

// Release lock
PerceptionComponent->SetTargetLockEnabled(false);
```

### Manually Set Target

```cpp
// Set specific target and enable lock
PerceptionComponent->SetCurrentTarget(EnemyActor);

// This automatically enables target lock
// AI will stay on this target until:
// 1. Target is no longer perceived by any handle
// 2. Lock is manually disabled
```

### Clear Target

```cpp
// Clear current target and disable lock
PerceptionComponent->ClearCurrentTarget();
```

### Target Switch Hysteresis

Configure in component properties to prevent oscillation:

```cpp
// Set hysteresis threshold (0.0 - 1.0)
// 0.1 = new target needs 10% higher weight to switch
PerceptionComponent->TargetSwitchHysteresis = 0.1f;
```

**How Hysteresis Works:**
```
Current Target Weight: 10.0
Hysteresis Threshold: 0.1 (10%)
Required Weight to Switch: 10.0 × (1 + 0.1) = 11.0

New target needs weight > 11.0 to become current target
```

---

## Event Delegates

### Subscribe to Perception Events

```cpp
void AMyAICharacter::BeginPlay()
{
    Super::BeginPlay();

    // Perception gained
    PerceptionComponent->OnPerceptionGained.AddDynamic(this, &AMyAICharacter::OnPerceptionGained);

    // Perception lost
    PerceptionComponent->OnPerceptionLost.AddDynamic(this, &AMyAICharacter::OnPerceptionLost);

    // Target changed
    PerceptionComponent->OnCurrentTargetChanged.AddDynamic(this, &AMyAICharacter::OnTargetChanged);
}
```

### Perception Gained Handler

```cpp
UFUNCTION()
void AMyAICharacter::OnPerceptionGained(AActor* TargetActor, FGameplayTag HandleTag, bool bPerceived)
{
    UE_LOG(LogTemp, Log, TEXT("AMyAICharacter::OnPerceptionGained: Now perceiving (%s) via handle (%s)"),
        *TargetActor->GetName(), *HandleTag.ToString());

    // React to detection (e.g., play alert animation)
}
```

### Perception Lost Handler

```cpp
UFUNCTION()
void AMyAICharacter::OnPerceptionLost(AActor* TargetActor, FGameplayTag HandleTag, bool bPerceived)
{
    UE_LOG(LogTemp, Log, TEXT("AMyAICharacter::OnPerceptionLost: Lost perception of (%s) via handle (%s)"),
        *TargetActor->GetName(), *HandleTag.ToString());

    // React to loss (e.g., enter search mode)
}
```

### Current Target Changed Handler

```cpp
UFUNCTION()
void AMyAICharacter::OnTargetChanged(AActor* OldTarget, AActor* NewTarget, float NewTargetWeight)
{
    if (NewTarget)
    {
        UE_LOG(LogTemp, Log, TEXT("AMyAICharacter::OnTargetChanged: Target switched to (%s) with weight (%.2f)"),
            *NewTarget->GetName(), NewTargetWeight);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("AMyAICharacter::OnTargetChanged: Target cleared"));
    }

    // React to target change (e.g., update aim, switch animation state)
}
```

---

## Debug Visualization

### Enable Auto-Debug Display

**In Editor:**
1. Select component
2. Enable `bDrawDebugEveryTick` in Details panel under "LQAI|Perception|Debug"

**In C++:**
```cpp
PerceptionComponent->bDrawDebugEveryTick = true;
```

### Manual Debug Draw

```cpp
// Show for 5 seconds
PerceptionComponent->DrawDebugPerception(5.0f);

// Show for one frame
PerceptionComponent->DrawDebugPerception(0.0f);
```

### Debug Display Elements

- **White text** above AI showing:
  - Current target name
  - Total weight
  - Active handles (comma-separated)
  - Retention status ("Retained: Yes")
  - Lock status ("Locked: Yes")
- **Yellow line** to current target (thickness: 2.0)
- **Green lines** to other perceived actors (thickness: 1.0)
- **Green weight labels** above each perceived actor

**Example Output:**
```
Target: BP_Enemy_C_1
Weight: 3.60
Handles: AI.Perception.Vision, AI.Perception.Threat
Locked: Yes
```

---

## Best Practices

### 1. Weight Configuration Strategy

Use weight hierarchy to prioritize targets:

| Priority Level | Weight Range | Usage |
|---------------|--------------|-------|
| Critical | 10.0+ | Immediate threats, primary objectives |
| High | 5.0 - 9.9 | Combat targets, important NPCs |
| Medium | 2.0 - 4.9 | Vision, secondary objectives |
| Low | 0.5 - 1.9 | Proximity awareness, ambient detection |
| Minimal | 0.1 - 0.4 | Background awareness |

**Example Weight Tuning:**
```cpp
// Combat AI
Threat Handle: Weight = 10.0   // Armed enemies
Sight Handle:  Weight = 3.0    // Visual awareness
Range Handle:  Weight = 1.0    // Proximity bonus

// Stealth AI
Vision Handle: Weight = 5.0    // Vision is critical
Sound Handle:  Weight = 3.0    // Audio detection
Tag Handle:    Weight = 10.0   // Once spotted (Player.Detected tag)

// Support AI
Ally Handle:   Weight = 2.0    // Stay near allies
Enemy Handle:  Weight = 5.0    // Avoid enemies
Objective:     Weight = 3.0    // Follow objectives
```

### 2. Performance Optimization

**Use Target Class Filtering:**
```cpp
// Bad - Iterates ALL actors in world
SightHandle.TargetClasses.Empty();

// Good - Only scans Pawns
SightHandle.TargetClasses.Add(APawn::StaticClass());
```

**Limit Handle Count:**
- Each handle adds processing cost per tick
- Use 2-4 handles for typical AI
- Combine similar checks into one handle when possible

**Adjust Component Tick Settings:**
```cpp
// Reduce tick frequency if not critical
PerceptionComponent->PrimaryComponentTick.TickInterval = 0.2f; // 5 times per second
```

### 3. Handle Lifecycle Management

**Dynamic Handle Management:**
```cpp
// Add handles based on AI state
void AMyAICharacter::EnterCombatMode()
{
    // Add combat-specific perception
    AddThreatDetectionHandle();

    // Increase vision sensitivity
    if (FLQAIPerceptionHandle_Sight* SightHandle = GetHandle<FLQAIPerceptionHandle_Sight>("AI.Perception.Vision"))
    {
        SightHandle->SightHalfAngle = 120.0f; // Wider FOV
        SightHandle->Weight = 5.0f; // Higher priority
    }
}

void AMyAICharacter::ExitCombatMode()
{
    // Remove combat perception
    PerceptionComponent->RemoveHandle(FGameplayTag::RequestGameplayTag("AI.Perception.Threat"));

    // Reset vision to patrol settings
    if (FLQAIPerceptionHandle_Sight* SightHandle = GetHandle<FLQAIPerceptionHandle_Sight>("AI.Perception.Vision"))
    {
        SightHandle->SightHalfAngle = 60.0f;
        SightHandle->Weight = 2.0f;
    }
}
```

### 4. Common Perception Patterns

**Combat Awareness Pattern:**
```cpp
// Configuration:
// - Sight: Visual awareness (Weight: 2.0, Radius: 2000, Angle: 90°)
// - Range: Close proximity bonus (Weight: 1.0, Radius: 800)
// - Threat: Armed enemies prioritized (Weight: 5.0, Tag: "Enemy.Combat")

Result: Armed enemies in close range get highest priority
```

**Stealth Detection Pattern:**
```cpp
// Configuration:
// - Sight: Primary detection (Weight: 3.0, Radius: 1500, Angle: 60°)
// - Tag: Once spotted, high priority (Weight: 10.0, Tag: "Player.Detected")

Result: Once player is spotted and tagged, they remain highest priority
```

**Ally Coordination Pattern:**
```cpp
// Configuration:
// - Tag: Nearby allies (Weight: 1.0, Tag: "Team.Ally", Range: 2000)
// - Tag: Leader (Weight: 3.0, Tag: "Team.Leader", Range: 3000)

Result: AI follows leader, stays near allies
```

**Multi-Objective Pattern:**
```cpp
// Configuration:
// - Tag: Primary objective (Weight: 5.0, Tag: "Objective.Primary")
// - Tag: Secondary objective (Weight: 2.0, Tag: "Objective.Secondary")
// - Range: Nearby interactables (Weight: 1.0, Radius: 500)

Result: Prioritizes objectives based on importance
```

---

## Advanced Usage

### Custom Perception Handles

Create new handle types by inheriting from `FLQAIPerceptionHandle`:

```cpp
USTRUCT(BlueprintType)
struct FLQAIPerceptionHandle_Sound : public FLQAIPerceptionHandle
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Sound")
    float HearingRadius = 1500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Sound")
    float MinSoundVolume = 0.3f;

    virtual void TickPerception(const FLQAIPerceptionContext& Context, float DeltaTime) override
    {
        // Implement sound-based detection logic
        // Scan for actors implementing ISoundEmitterInterface
        // Report perception based on sound volume and distance
    }

    virtual float GetWeightModifier(AActor* Target, const FLQAIPerceptionContext& Context) const override
    {
        // Louder sounds = higher weight
        // Closer sounds = higher weight
        return VolumeModifier * DistanceModifier;
    }

    virtual FString GetDebugName() const override
    {
        return FString::Printf(TEXT("Sound(%.0f)"), HearingRadius);
    }
};
```

### Accessing Component from Handles

Handles receive `FLQAIPerceptionContext` which provides access to the component:

```cpp
void FMyCustomHandle::TickPerception(const FLQAIPerceptionContext& Context, float DeltaTime)
{
    if (!Context.IsValid())
    {
        return;
    }

    // Get owner actor
    AActor* Owner = Context.GetOwner();

    // Get world
    UWorld* World = Context.GetWorld();

    // Get perception component
    ULQAIPerceptionComponent* PerceptionComp = Context.PerceptionComponent.Get();

    // Report perception
    if (PerceptionComp && TargetActor)
    {
        float WeightMod = GetWeightModifier(TargetActor, Context);
        PerceptionComp->ReportPerception(HandleTag, TargetActor, WeightMod, false);
    }
}
```

---

## Troubleshooting

### No Targets Detected

**Check:**
1. Component is ticking (`PrimaryComponentTick.bCanEverTick = true`)
2. Handles are enabled (`Handle.bEnabled = true`)
3. HandleTag is valid and unique
4. TargetClasses filter allows target actors
5. Target is within detection radius
6. Line of sight is not blocked (if `bRequireLineOfSight = true`)

**Debug:**
```cpp
PerceptionComponent->bDrawDebugEveryTick = true;
```

### Target Switching Too Rapidly

**Solutions:**
1. Increase `TargetSwitchHysteresis` (e.g., 0.2 for 20% difference required)
2. Enable target lock during combat
3. Adjust handle weights to create clearer priority differences

**Debug:**
```cpp
PerceptionComponent->OnCurrentTargetChanged.AddDynamic(this, &AMyAI::LogTargetSwitch);

void AMyAI::LogTargetSwitch(AActor* Old, AActor* New, float Weight)
{
    UE_LOG(LogTemp, Warning, TEXT("AMyAI::LogTargetSwitch: Switch from (%s) to (%s) with weight (%.2f)"),
        Old ? *Old->GetName() : TEXT("None"),
        New ? *New->GetName() : TEXT("None"),
        Weight);
}
```

### Performance Issues

**Optimize:**
1. Use `TargetClasses` filtering
2. Reduce component tick frequency
3. Limit number of handles per AI
4. Use spatial partitioning for target scanning

**Profile:**
```cpp
// Enable logging in LQAIPerceptionComponent.cpp
UE_LOG(LogTemp, Verbose, TEXT("ULQAIPerceptionComponent::TickComponent: Processing (%d) handles for (%s)"),
    RuntimeHandles.Num(), *GetOwner()->GetName());
```

### Handles Not Being Created

**Check:**
1. Struct type is correctly set in `TInstancedStruct<FLQAIPerceptionHandle>`
2. HandleTag is valid (`HandleTag.IsValid()` returns true)
3. No duplicate HandleTags in same component
4. Struct is properly initialized before adding

**Example Proper Initialization:**
```cpp
TInstancedStruct<FLQAIPerceptionHandle> HandleStruct;
FLQAIPerceptionHandle_Range RangeHandle;
RangeHandle.HandleTag = FGameplayTag::RequestGameplayTag("AI.Perception.Range");
RangeHandle.Weight = 1.0f;
RangeHandle.DetectionRadius = 1000.0f;

HandleStruct.InitializeAs<FLQAIPerceptionHandle_Range>(RangeHandle);
int32 Index = PerceptionComponent->AddHandle(HandleStruct);

if (Index == INDEX_NONE)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to add perception handle"));
}
```

---

## Architecture Reference

### Class Hierarchy

```
UActorComponent
    └── ULQAIPerceptionComponent
            │
            ├── Manages: TArray<TInstancedStruct<FLQAIPerceptionHandle>>
            ├── Aggregates: TMap<AActor*, FLQAIPerceptionScore>
            └── Selects: AActor* CurrentTarget

FLQAIPerceptionHandle (Base)
    ├── FLQAIPerceptionHandle_Range (Distance-based)
    ├── FLQAIPerceptionHandle_Sight (Cone + LOS)
    └── FLQAIPerceptionHandle_Tag (Gameplay tags)
```

### Data Flow

```
[Tick] → Component.TickComponent
           │
           ├─→ For each Handle:
           │     Handle.TickPerception(Context, DeltaTime)
           │       │
           │       ├─→ Scan for targets (via TargetClasses or overlap)
           │       │
           │       └─→ For each detected target:
           │             Component.ReportPerception(HandleTag, Target, WeightModifier)
           │
           ├─→ Aggregate weights per actor:
           │     PerceivedActors[Actor].TotalWeight = Sum(Handle.Weight × Modifier)
           │
           └─→ Update current target:
                 CurrentTarget = HighestWeightActor (with hysteresis + lock logic)
```

### Weight Aggregation

```
Actor: Player
    Perceiving Handles:
        - AI.Perception.Vision:    Weight=2.0 × Modifier=0.8 = 1.6
        - AI.Perception.Proximity: Weight=1.0 × Modifier=1.0 = 1.0
    TotalWeight: 1.6 + 1.0 = 2.6
    PerceivingHandles: [AI.Perception.Vision, AI.Perception.Proximity]
    bAnyRetained: false
```

---

## API Reference

### ULQAIPerceptionComponent

**Handle Management:**
- `int32 AddHandle(const TInstancedStruct<FLQAIPerceptionHandle>& Handle)`
- `bool RemoveHandle(FGameplayTag HandleTag)`
- `void ClearHandles()`
- `bool HasHandle(FGameplayTag HandleTag) const`

**Perception Reporting (Called by Handles):**
- `void ReportPerception(FGameplayTag HandleTag, AActor* Target, float WeightModifier, bool bIsRetained = false)`
- `void ClearPerception(FGameplayTag HandleTag, AActor* Target)`
- `void ClearAllPerceptionsForHandle(FGameplayTag HandleTag)`

**Queries:**
- `bool HasAnyPerception(AActor* Target) const`
- `const FLQAIPerceptionScore* GetPerceptionScore(AActor* Target) const`
- `const TMap<TWeakObjectPtr<AActor>, FLQAIPerceptionScore>& GetPerceivedActors() const`

**Target Selection:**
- `AActor* GetHighestWeightTarget() const`
- `float GetTargetWeight(AActor* Target) const`
- `AActor* GetCurrentTarget() const`

**Target Lock:**
- `void SetTargetLockEnabled(bool bEnabled)`
- `bool IsTargetLocked() const`
- `void SetCurrentTarget(AActor* NewTarget)`
- `void ClearCurrentTarget()`

**Debug:**
- `void DrawDebugPerception(float Duration = 0.0f) const`

**Delegates:**
- `FOnLQAIPerceptionChanged OnPerceptionGained`
- `FOnLQAIPerceptionChanged OnPerceptionLost`
- `FOnLQAICurrentTargetChanged OnCurrentTargetChanged`

### FLQAIPerceptionHandle (Base)

**Lifecycle:**
- `virtual void StartPerception(const FLQAIPerceptionContext& Context)`
- `virtual void TickPerception(const FLQAIPerceptionContext& Context, float DeltaTime)`
- `virtual void EndPerception(const FLQAIPerceptionContext& Context)`

**Weight:**
- `virtual float GetWeightModifier(AActor* Target, const FLQAIPerceptionContext& Context) const`
- `float ComputeEffectiveWeight(AActor* Target, const FLQAIPerceptionContext& Context) const`

**Utility:**
- `virtual void Reset()`
- `virtual FString GetDebugName() const`

### FLQAIPerceptionContext

**Validation:**
- `bool IsValid() const`

**Accessors:**
- `AActor* GetOwner() const`
- `UWorld* GetWorld() const`

### FLQAIPerceptionScore

**Properties:**
- `float TotalWeight` - Sum of all handle contributions
- `TMap<FGameplayTag, float> HandleWeights` - Per-handle weight breakdown
- `TArray<FGameplayTag> PerceivingHandles` - Active handle tags
- `bool bAnyRetained` - Whether any perception is retained

**Methods:**
- `void RecalculateTotalWeight()`
- `bool operator>(const FLQAIPerceptionScore& Other) const`

---

## File Locations

**Public Headers:**
- `LQAIPerceptionComponent.h` - Main component class
- `LQAIPerceptionHandle.h` - Base and derived handle structs
- `LQPerceptionTypes.h` - Context and score structs

**Private Implementation:**
- `LQAIPerceptionComponent.cpp` - Component implementation
- `LQAIPerceptionHandle.cpp` - Handle implementations
- `LQPerceptionTypes.cpp` - Helper implementations

**Full Path:**
```
Plugins/LQAISystem/Source/LQAISystem/
    ├── Public/Perception/
    │   ├── LQAIPerceptionComponent.h
    │   ├── LQAIPerceptionHandle.h
    │   ├── LQPerceptionTypes.h
    │   └── README.md (this file)
    └── Private/Perception/
        ├── LQAIPerceptionComponent.cpp
        ├── LQAIPerceptionHandle.cpp
        └── LQPerceptionTypes.cpp
```

---

## Version History

**v1.0 - Initial Implementation**
- ULQAIPerceptionComponent with handle management
- FLQAIPerceptionHandle base struct with lifecycle
- FLQAIPerceptionHandle_Range (distance-based)
- FLQAIPerceptionHandle_Sight (cone-based with LOS)
- FLQAIPerceptionHandle_Tag (gameplay tag matching)
- Weight-based target prioritization
- Target locking and hysteresis
- Debug visualization
- Perception change delegates

---

## License

Copyright Tale Weavers
