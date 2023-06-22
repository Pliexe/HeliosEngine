#pragma once
#include "pch.h"

namespace Helios
{
	class InitWindow
	{
	public:		
		virtual void Create(uint32_t width, uint32_t height) = 0;
		static InitWindow* Create();

		void SetA(std::string text, float progress) { SetTextA(text); SetProgressA(progress); }
		void SetB(std::string text, float progress) { SetTextB(text); SetProgressB(progress); }
		virtual void SetTextA(std::string text) = 0;
		virtual void SetTextB(std::string text) = 0;
		virtual void SetProgressA(float progress) = 0;
		virtual void SetProgressB(float progress) = 0;
		virtual void ShowMessage(const char* title, const char* text) = 0;

		virtual void Quit() = 0;
	};

	
}
