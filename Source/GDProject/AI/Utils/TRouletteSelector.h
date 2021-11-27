#pragma once

#include "CoreMinimal.h"
#include <type_traits>
#include <algorithm>
#include <utility>
#include <random>
#include <thread>


extern std::minstd_rand SharedRandomEngine;

template <typename T, typename W>
class TRouletteSelector
{
	static_assert(std::is_arithmetic_v<W>, "Only arithmetic types are supported");

	TArray<TPair<T, W>> Entries;

public:
	size_t Size() const { return Entries.Num(); }

	size_t AddEntry(TPair<T, W> Entry)
	{
		Entries.Push(Entry);
		return Size();
	}

	size_t AddEntry(T Entry, W Weight)
	{
		return AddEntry(TPair<T, W>{Entry, Weight});
	}

	T RouletteWheel()
	{
		W Sum = 0;
		for (const auto& Entry : Entries)
		{
			Sum += Entry.Value;
		}
		TArray<long double> NormalizedWeights;
		for (const auto& Entry : Entries)
		{
			NormalizedWeights.Push(static_cast<long double>(Entry.Value) / static_cast<long double>(Sum));
		}

		std::uniform_real_distribution<long double> URD;

		size_t i = 0;
		for (auto Random = URD(SharedRandomEngine, URD.param()); Random >= 0; ++i)
		{
			Random -= NormalizedWeights[i];
		}

		auto Result = std::move(Entries[i - 1].Key);
		Entries.RemoveAt(i - 1);
		return Result;
	}
};
