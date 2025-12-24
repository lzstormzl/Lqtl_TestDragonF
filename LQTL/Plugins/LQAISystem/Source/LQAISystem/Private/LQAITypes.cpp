// Copyright Tale Weavers


#include "LQAITypes.h"

DEFINE_LOG_CATEGORY(LogAI);

static bool bDebugLQAI = false;
static FAutoConsoleVariableRef CVarDebugLQAI(
	TEXT("LQ.IA.EnableDebug"),
	bDebugLQAI,
	TEXT("Enable LQ AI debug.\nFalse = Off, True = On"),
	ECVF_Cheat
);

namespace LQAI::Goal
{
	UE_DEFINE_GAMEPLAY_TAG(Idle, "LQ.AI.Goal.NonCombat.Idle");
	UE_DEFINE_GAMEPLAY_TAG(Patrol, "LQ.AI.Goal.NonCombat.Patrol");
	UE_DEFINE_GAMEPLAY_TAG(Retreat, "LQ.AI.Goal.NonCombat.Retreat");
	UE_DEFINE_GAMEPLAY_TAG(SpiritBreak, "LQ.AI.Goal.Combat.SpiritBreak");
	UE_DEFINE_GAMEPLAY_TAG(SoloCombat, "LQ.AI.Goal.Combat.SoloCombat");
	UE_DEFINE_GAMEPLAY_TAG(GroupCombat, "LQ.AI.Goal.Combat.GroupCombat");
}

namespace LQAI::StateTree
{
	UE_DEFINE_GAMEPLAY_TAG(CombatStateTree, "LQ.AI.StateTree.Combat");
	UE_DEFINE_GAMEPLAY_TAG(NonCombatStateTree, "LQ.AI.StateTree.NonCombat");
}

namespace LQAI::BehaviorTree
{
	UE_DEFINE_GAMEPLAY_TAG(Idle, "LQ.AI.BehaviorTree.Idle");
	UE_DEFINE_GAMEPLAY_TAG(SingleCombat, "LQ.AI.BehaviorTree.SingleCombat");
}

