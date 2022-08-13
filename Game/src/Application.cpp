#include <Wallnut.h>

class Game : public Wallnut::Application
{
public:
	void Init() override {
		Wallnut::SceneManager::AddScene(L"Example Scene", [this](Wallnut::Scene& scene) {
			MessageBox(getHwnd(), L"WORKS", L"L", MB_OK);

			Wallnut::GameObject::InstantiateObject();
		});

		Wallnut::SceneManager::LoadScene(L"Example Scene");
	}
};

Wallnut::Application* Wallnut::CreateApplication() { return new Game(); }