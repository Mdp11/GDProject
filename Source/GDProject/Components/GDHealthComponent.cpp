// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDHealthComponent.h"

UGDHealthComponent::UGDHealthComponent()
{
	DefaultHealth = 50.f;
}

bool UGDHealthComponent::HasFullHealth() const
{
	return CurrentHealth == DefaultHealth;
}

float UGDHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

float UGDHealthComponent::GetMaxHealth() const
{
	return DefaultHealth;
}

void UGDHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = DefaultHealth;

	AActor* Owner = GetOwner();
	if (Owner)
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UGDHealthComponent::OnTakeAnyDamage);
	}
}

void UGDHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                         AController* InstigatedBy, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, DefaultHealth);
	if (Damage > 0.f)
	{
		OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s was healed by %f hp and now has %f!"), *GetOwner()->GetName(), Damage,
		       CurrentHealth);
	}
}
