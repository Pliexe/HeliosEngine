#pragma once

#include "Helios/Resources/Buffer.h"

namespace Helios
{
	template <typename T>
	class Test1 : public ConstantBuffer<T>
	{
	public:
		Test1(uint32_t size);
		Test1(const void* data, uint32_t size);
		~Test1();

		void BindPS(uint32_t slot) override;
		void BindVS(uint32_t slot) override;
		void UnbindPS() const override;
		void UnbindVS() const override;
		void SetData(T data) override;
	};

	template <typename T>
	Test1<T>::Test1(uint32_t size)
	{
	}

	template <typename T>
	Test1<T>::Test1(const void* data, uint32_t size)
	{
	}

	template <typename T>
	Test1<T>::~Test1()
	{
	}

	template <typename T>
	void Test1<T>::BindPS(uint32_t slot)
	{
	}

	template <typename T>
	void Test1<T>::BindVS(uint32_t slot)
	{
	}

	template <typename T>
	void Test1<T>::UnbindPS() const
	{
	}

	template <typename T>
	void Test1<T>::UnbindVS() const
	{
	}

	template <typename T>
	void Test1<T>::SetData(T data)
	{
	}
}
