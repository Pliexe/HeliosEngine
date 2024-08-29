#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios
{
	namespace Message
	{
		enum class Result
		{
			None,
			Ok,
			Cancel,
			Abort,
			Retry,
			Ignore,
			Yes,
			No,
		};

		enum Flags : bitmask_t
		{
			Ok = BIT(0),
			OkCancel = BIT(1),
			AbortRetryIgnore = BIT(2),
			YesNoCancel = BIT(3),
			YesNo = BIT(4),

			IconQuestion = BIT(10),
			IconInformation = BIT(11),
			IconWarning = BIT(12),
			IconError = BIT(13),
		};

		inline Flags operator | (Flags lhs, Flags rhs)
		{
			using T = std::underlying_type_t <Flags>;
			return static_cast<Flags>(static_cast<T>(lhs) | static_cast<T>(rhs));
		}

		inline Flags& operator |= (Flags& lhs, Flags rhs)
		{
			lhs = lhs | rhs;
			return lhs;
		}
	}


	namespace WindowStyles
	{
		enum Value : bitmask_t
		{
			None = 0,
			Decorated = BIT(0),
			Resizable = BIT(1),
			//Minimized	= BIT(2),
			Maximized = BIT(3),
			Show = BIT(4),
			AlwaysOnTop = BIT(5),
		};

		inline Value operator | (Value lhs, Value rhs)
		{
			using T = std::underlying_type_t <Value>;
			return static_cast<Value>(static_cast<T>(lhs) | static_cast<T>(rhs));
		}

		inline Value& operator |= (Value& lhs, Value rhs)
		{
			lhs = lhs | rhs;
			return lhs;
		}
	}
}