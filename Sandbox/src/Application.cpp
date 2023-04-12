#include <Helios.h>
#include <Helios/GameObjects/Components/RectangleComponent.h>
#include <Helios/GameObjects/Components/Camera.h>
#include <Helios/Input/Mouse.h>
#include <Helios/Resources/Brushes/SolidBrush.h>
#include <Helios/Resources/Brushes/BitmapBrush.h>
#include <Helios/GameObjects/Components/SpriteComponent.h>

#include "Scripts/CameraMovement.h"
#include "Scripts/CubeCircle.h"

class Game : public Helios::Application
{
private:
	//Helios::Bitmap image;
public:


	void Init() override {

		using namespace Helios;


		SceneRegistry::AddScene(L"Example Scene", [this](Scene& scene) {
			//MessageBox(getHwnd(), L"WORKS", L"L", MB_OK);

			/*GameObject& obj = Helios::GameObject::InstantiateObject();

			obj.AddComponent(*(new RectangleComponent(SolidBrush(164, 212, 53))));
			obj.getTransform().setSize(500, 500);
			obj.AddComponent(new CubeCircle());

			GameObject& obj2 = Helios::GameObject::InstantiateObject();
			obj2.getTransform().setPosition(Vector2(505, 550));
			obj2.AddComponent(*(new RectangleComponent(SolidBrush(255, 255, 255))));
			obj2.getTransform().setSize(54, 100);

			auto& camObj = GameObject::CreateMainCamera(Vector2(25.0f, 25.0f));
			auto cs = new CameraMovement();

			obj.AddComponent(*(new RectangleComponent(SolidBrush(255, 255, 255))));

			obj2.AddComponent(cs);

			CameraComponent* cam = (camObj.GetComponent<CameraComponent>());

			std::cout << "Is Null: " << (cam == nullptr) << std::endl;

			if (cam != nullptr) {
				cam->SetBackgroundColor(144, 255, 55);
			}

			Mouse::SetCursorState(Mouse::Locked);*/
			
			Bitmap image(L"resources\\unknown.png");

			GameObject& obj = GameObject::InstantiateObject();

			obj.AddComponent(*(new RectangleComponent(SolidBrush(164, 212, 53))));
			obj.getTransform().setSize(15, 15);
			obj.getTransform().setPosition(500 - 20, 500 - 20);
			//obj.AddComponent(new CubeCircle());

			auto& camObj = GameObject::CreateMainCamera();
			camObj.AddComponent(new CameraMovement());

			GameObject& obj2 = Helios::GameObject::InstantiateObject();

			obj2.AddComponent(*(new RectangleComponent(BitmapBrush(L"resources\\unknown.png"))));
			obj2.getTransform().setPosition(-250, -250);
			obj2.getTransform().setSize(500, 500);
			obj2.AddComponent(new CubeCircle());


			GameObject& obj3 = Helios::GameObject::InstantiateObject();

			obj3.AddComponent(*(new SpriteComponent(Bitmap(L"resources\\unknown.png"))));
			obj3.getTransform().setPosition(-100, -100);
			obj3.getTransform().setSize(50, 50);
			//obj.AddComponent(new CubeCircle());
		});

		SceneRegistry::LoadScene(L"Example Scene");
	}
};

Helios::Application* Helios::CreateApplication() { return new Game(); }