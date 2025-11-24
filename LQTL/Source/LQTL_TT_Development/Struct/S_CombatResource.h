#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include <algorithm>

#include "S_CombatResource.generated.h"

USTRUCT(BlueprintType)
struct FS_CombatResource : public FTableRowBase
{
	GENERATED_BODY()



	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float shield = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float spiritWater = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float spiritOrb = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float stagger = 0;



	inline FS_CombatResource(
		float _Shield,
		float _Health,
		float _SpiritWater,
		float _SpiritOrb,
		float _Stagger)
		:
		shield(_Shield),
		health(_Health),
		spiritWater(_SpiritWater),
		spiritOrb(_SpiritOrb),
		stagger(_Stagger)
	{
	}

	inline FS_CombatResource() = default;



	/*
		Operator Override
	*/
	inline FS_CombatResource operator+(const FS_CombatResource& _Other) const {
		return FS_CombatResource(
			shield + _Other.shield,
			health + _Other.health,
			spiritWater + _Other.spiritWater,
			spiritOrb + _Other.spiritOrb,
			stagger + _Other.stagger
		);
	}

	inline FS_CombatResource operator+(float _Other) const {
		return FS_CombatResource(
			shield + _Other,
			health + _Other,
			spiritWater + _Other,
			spiritOrb + _Other,
			stagger + _Other
		);
	}

	inline FS_CombatResource operator-(const FS_CombatResource& _Other) const {
		return FS_CombatResource(
			shield - _Other.shield,
			health - _Other.health,
			spiritWater - _Other.spiritWater,
			spiritOrb - _Other.spiritOrb,
			stagger - _Other.stagger
		);
	}

	inline FS_CombatResource operator-(float _Other) const {
		return FS_CombatResource(
			shield - _Other,
			health - _Other,
			spiritWater - _Other,
			spiritOrb - _Other,
			stagger - _Other
		);
	}

	inline FS_CombatResource operator*(const FS_CombatResource& _Other) const {
		return FS_CombatResource(
			shield * _Other.shield,
			health * _Other.health,
			spiritWater * _Other.spiritWater,
			spiritOrb * _Other.spiritOrb,
			stagger * _Other.stagger
		);
	}

	inline FS_CombatResource operator*(float _Other) const {
		return FS_CombatResource(
			shield * _Other,
			health * _Other,
			spiritWater * _Other,
			spiritOrb * _Other,
			stagger * _Other
		);
	}

	inline FS_CombatResource operator/(const FS_CombatResource& _Other) const {
		return FS_CombatResource(
			shield / _Other.shield,
			health / _Other.health,
			spiritWater / _Other.spiritWater,
			spiritOrb / _Other.spiritOrb,
			stagger / _Other.stagger
		);
	}

	inline FS_CombatResource operator/(float _Other) const {
		return FS_CombatResource(
			shield / _Other,
			health / _Other,
			spiritWater / _Other,
			spiritOrb / _Other,
			stagger / _Other
		);
	}



	inline bool operator==(const FS_CombatResource& _Other) const {
		return shield == _Other.shield
			&& health == _Other.health
			&& spiritWater == _Other.spiritWater
			&& spiritOrb == _Other.spiritOrb
			&& stagger == _Other.stagger;
	}

	inline bool operator==(float _Other) const {
		return shield == _Other
			&& health == _Other
			&& spiritWater == _Other
			&& spiritOrb == _Other
			&& stagger == _Other;
	}

	inline bool operator<(const FS_CombatResource& _Other) const {
		return shield < _Other.shield
			&& health < _Other.health
			&& spiritWater < _Other.spiritWater
			&& spiritOrb < _Other.spiritOrb
			&& stagger < _Other.stagger;
	}

	inline bool operator<(float _Other) const {
		return shield < _Other
			&& health < _Other
			&& spiritWater < _Other
			&& spiritOrb < _Other
			&& stagger < _Other;
	}

	inline bool operator<=(const FS_CombatResource& _Other) const {
		return shield <= _Other.shield
			&& health <= _Other.health
			&& spiritWater <= _Other.spiritWater
			&& spiritOrb <= _Other.spiritOrb
			&& stagger <= _Other.stagger;
	}

	inline bool operator<=(float _Other) const {
		return shield <= _Other
			&& health <= _Other
			&& spiritWater <= _Other
			&& spiritOrb <= _Other
			&& stagger <= _Other;
	}

	inline bool operator>(const FS_CombatResource& _Other) const {
		return shield > _Other.shield
			&& health > _Other.health
			&& spiritWater > _Other.spiritWater
			&& spiritOrb > _Other.spiritOrb
			&& stagger > _Other.stagger;
	}

	inline bool operator>(float _Other) const {
		return shield > _Other
			&& health > _Other
			&& spiritWater > _Other
			&& spiritOrb > _Other
			&& stagger > _Other;
	}

	inline bool operator>=(const FS_CombatResource& _Other) const {
		return shield >= _Other.shield
			&& health >= _Other.health
			&& spiritWater >= _Other.spiritWater
			&& spiritOrb >= _Other.spiritOrb
			&& stagger >= _Other.stagger;
	}

	inline bool operator>=(float _Other) const {
		return shield >= _Other
			&& health >= _Other
			&& spiritWater >= _Other
			&& spiritOrb >= _Other
			&& stagger >= _Other;
	}



	/*
		FUNCTIONS
	*/
	inline _NODISCARD FS_CombatResource Clamp(const FS_CombatResource& _Min, const FS_CombatResource& _Max) const {
		return FS_CombatResource(
			std::clamp(shield, _Min.shield, _Max.shield),
			std::clamp(health, _Min.health, _Max.health),
			std::clamp(spiritWater, _Min.spiritWater, _Max.spiritWater),
			std::clamp(spiritOrb, _Min.spiritOrb, _Max.spiritOrb),
			std::clamp(stagger, _Min.stagger, _Max.stagger)
		);
	}

	inline _NODISCARD FS_CombatResource Clamp(float _Min, float _Max) const {
		return FS_CombatResource(
			std::clamp(shield, _Min, _Max),
			std::clamp(health, _Min, _Max),
			std::clamp(spiritWater, _Min, _Max),
			std::clamp(spiritOrb, _Min, _Max),
			std::clamp(stagger, _Min, _Max)
		);
	}

	inline _NODISCARD FS_CombatResource Min(const FS_CombatResource& _Other) const {
		return FS_CombatResource(
			std::min(shield, _Other.shield),
			std::min(health, _Other.health),
			std::min(spiritWater, _Other.spiritWater),
			std::min(spiritOrb, _Other.spiritOrb),
			std::min(stagger, _Other.stagger)
		);
	}

	inline _NODISCARD FS_CombatResource Min(float _Other) const {
		return FS_CombatResource(
			std::min(shield, _Other),
			std::min(health, _Other),
			std::min(spiritWater, _Other),
			std::min(spiritOrb, _Other),
			std::min(stagger, _Other)
		);
	}
	
	inline _NODISCARD FS_CombatResource Max(const FS_CombatResource& _Other) const {
		return FS_CombatResource(
			std::max(shield, _Other.shield),
			std::max(health, _Other.health),
			std::max(spiritWater, _Other.spiritWater),
			std::max(spiritOrb, _Other.spiritOrb),
			std::max(stagger, _Other.stagger)
		);			
	}

	inline _NODISCARD FS_CombatResource Max(float _Other) const {
		return FS_CombatResource(
			std::max(shield, _Other),
			std::max(health, _Other),
			std::max(spiritWater, _Other),
			std::max(spiritOrb, _Other),
			std::max(stagger, _Other)
		);
	}
};
