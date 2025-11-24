// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "LQTL_TT_Development/Struct/S_CombatResource.h"

#include "BFL_CombatResource.generated.h"

UCLASS()
class UBFL_CombatResource : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Add 2 combat resource struct and return a copy.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Combine (Combat Resource)", CompactNodeTitle = "+", Keywords = "+ add combine"))
	static inline FS_CombatResource CombineResource(const FS_CombatResource& _A, const FS_CombatResource& _B) {
		return _A + _B;
	}

	// Add a flat value to all property of the given struct.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Combine (Float)", CompactNodeTitle = "+", Keywords = "+ add combine"))
	static inline FS_CombatResource CombineFloat(const FS_CombatResource& _A, float _B) {
		return _A + _B;
	}

	// Subtract A to B return a copy.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Subtract (Combat Resource)", CompactNodeTitle = "-", Keywords = "- subtract"))
	static inline FS_CombatResource SubtractResource(const FS_CombatResource& _A, const FS_CombatResource& _B) {
		return _A - _B;
	}

	// Subtract A to B return a copy.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Subtract (Float)", CompactNodeTitle = "-", Keywords = "- subtract"))
	static inline FS_CombatResource SubtractFloat(const FS_CombatResource& _A, float _B) {
		return _A - _B;
	}

	// Multiply 2 combat resource struct and return a copy.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Multiply (Combat Resource)", CompactNodeTitle = "*", Keywords = "* multiply"))
	static inline FS_CombatResource MultResource(const FS_CombatResource& _A, const FS_CombatResource& _B) {
		return _A * _B;
	}

	// Multiply a flat value to all property of the given struct.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Multiply (Float)", CompactNodeTitle = "*", Keywords = "* multiply"))
	static inline FS_CombatResource MultFloat(const FS_CombatResource& _A, float _B) {
		return _A * _B;
	}

	// Divide 2 combat resource struct and return a copy.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Divide (Combat Resource)", CompactNodeTitle = "/", Keywords = "/ divide"))
	static inline FS_CombatResource DivideResource(const FS_CombatResource& _A, const FS_CombatResource& _B) {
		return _A / _B;
	}

	// Divide a flat value to all property of the given struct.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Divide (Float)", CompactNodeTitle = "/", Keywords = "/ divide"))
	static inline FS_CombatResource DivideFloat(const FS_CombatResource& _A, float _B) {
		return _A / _B;
	}



	// Returns true if all properties of A is equal to B.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (Combat Resource)", CompactNodeTitle = "==", Keywords = "== equal"))
	static inline bool EqualResource(const FS_CombatResource& _A, const FS_CombatResource& _B) {
		return _A == _B;
	}

	// Returns true if all properties of A is equal to B.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (Float)", CompactNodeTitle = "==", Keywords = "== equal"))
	static inline bool EqualFloat(const FS_CombatResource& _A, float _B) {
		return _A == _B;
	}

	// Returns true if all properties of A is less than B.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Less (Combat Resource)", CompactNodeTitle = "<", Keywords = "< less"))
	static inline bool LessResource(const FS_CombatResource& _A, const FS_CombatResource& _B) {
		return _A < _B;
	}

	// Returns true if all properties of A is less than B.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Less (Float)", CompactNodeTitle = "<", Keywords = "< less"))
	static inline bool LessFloat(const FS_CombatResource& _A, float _B) {
		return _A < _B;
	}

	// Returns true if all properties of A is less or equal than B.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Less or Equal (Combat Resource)", CompactNodeTitle = "<=", Keywords = "<= less equal"))
	static inline bool LessOrEqualResource(const FS_CombatResource& _A, const FS_CombatResource& _B) {
		return _A <= _B;
	}

	// Returns true if all properties of A is less or equal than B.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Less or Equal (Float)", CompactNodeTitle = "<=", Keywords = "<= less equal"))
	static inline bool LessOrEqualFloat(const FS_CombatResource& _A, float _B) {
		return _A <= _B;
	}

	// Returns true if all properties of A is greater than B.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Greater (Combat Resource)", CompactNodeTitle = ">", Keywords = "> greater"))
	static inline bool GreaterResource(const FS_CombatResource& _A, const FS_CombatResource& _B) {
		return _A > _B;
	}

	// Returns true if all properties of A is greater than B.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Greater (Float)", CompactNodeTitle = ">", Keywords = "> greater"))
	static inline bool GreaterFloat(const FS_CombatResource& _A, float _B) {
		return _A > _B;
	}

	// Returns true if all properties of A is greater or equal than B.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Greater or Equal (Combat Resource)", CompactNodeTitle = ">=", Keywords = ">= greater equal"))
	static inline bool GreaterOrEqualResource(const FS_CombatResource& _A, const FS_CombatResource& _B) {
		return _A >= _B;
	}

	// Returns true if all properties of A is greater or equal than B.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Greater or Equal (Float)", CompactNodeTitle = ">=", Keywords = ">= greater equal"))
	static inline bool GreaterOrEqualFloat(const FS_CombatResource& _A, float _B) {
		return _A >= _B;
	}



	// Clamp value to [Min..Max] bounds.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Clamp (Combat Resource)"))
	static inline FS_CombatResource ClampResource(const FS_CombatResource& _Value, const FS_CombatResource& _Min, const FS_CombatResource& _Max) {
		return _Value.Clamp(_Min, _Max);
	}

	// Clamp value to [Min..Max] bounds.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Clamp (Float)"))
	static inline FS_CombatResource ClampFloat(const FS_CombatResource& _Value, float _Min, float _Max) {
		return _Value.Clamp(_Min, _Max);
	}

	// Get the minimum value of each property between the 2 given structs.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Min (Combat Resource)", CompactNodeTitle = "MIN", Keywords = "min"))
	static inline FS_CombatResource MinResource(const FS_CombatResource& _A, const FS_CombatResource& _B) {
		return _A.Min(_B);
	}

	// Get the minimum value of each property between the given struct and a flat value.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Min (Float)", CompactNodeTitle = "MIN", Keywords = "min"))
	static inline FS_CombatResource MinFloat(const FS_CombatResource& _A, float _B) {
		return _A.Min(_B);
	}

	// Get the maximum value of each property between the 2 given structs.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Max (Combat Resource)", CompactNodeTitle = "MAX", Keywords = "max"))
	static inline FS_CombatResource MaxResource(const FS_CombatResource& _A, const FS_CombatResource& _B) {
		return _A.Max(_B);
	}

	// Get the maximum value of each property between the given struct and a flat value.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Max (Float)", CompactNodeTitle = "MAX", Keywords = "max"))
	static inline FS_CombatResource MaxFloat(const FS_CombatResource& _A, float _B) {
		return _A.Max(_B);
	}
};
