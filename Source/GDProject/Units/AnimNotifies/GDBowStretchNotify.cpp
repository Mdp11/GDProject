// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDBowStretchNotify.h"

#include "GDProject/Units/GDArcher.h"
#include "GDProject/Units/Actors/GDBow.h"

void UGDBowStretchNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AGDArcher* Archer = Cast<AGDArcher>(MeshComp->GetOwner());
	if (Archer)
	{
		AGDBow* Bow = Archer->GetBow();
		if (bPull)
		{
			Bow->bBent = true;
			Bow->AttachCablesTo(MeshComp, Archer->CablesAttachSocketName);
		}
		else
		{
			Bow->bBent = false;
			Bow->AttachCablesTo(Bow->GetMesh(), Bow->GetIdleBowCablesSocketName());
		}
	}
}
