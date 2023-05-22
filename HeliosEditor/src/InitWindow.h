#pragma once

namespace Helios
{
	class InitWindow
	{
	public:
		virtual ~InitWindow() = 0;
		
		virtual void Create(uint32_t width, uint32_t height) = 0;

		virtual void SetTextA(const char* text) = 0;
		virtual void SetTextB(const char* text) = 0;
		virtual void SetProgressA(float progress) = 0;
		virtual void SetProgressB(float progress) = 0;
		virtual void MessageBox(const char* title, const char* text) = 0;

		virtual void Quit() = 0;
	};
}