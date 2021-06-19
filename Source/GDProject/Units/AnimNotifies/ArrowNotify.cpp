// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "ArrowNotify.h"

#include "GDProject/Units/GDArcher.h"
#include "GDProject/Units/Actors/GDArrow.h"

void UArrowNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AGDArcher* Archer = Cast<AGDArcher>(MeshComp->GetOwner());
	if (Archer)
	{
		if (bDrawArrow && Archer->ArrowClass)
		{
			Archer->SpawnArrow();
		}
		else if (bFireArrow && Archer->Arrow)
		{
			Archer->FireArrow();
		}
	}
}
