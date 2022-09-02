#include <Wallnut.h>
#include <Wallnut/GameObjects/Components/RectangleComponent.h>
#include <Wallnut/GameObjects/Components/Camera.h>
#include <Wallnut/Input/Mouse.h>
#include <Wallnut/Resources/Brushes/SolidBrush.h>
#include <Wallnut/Resources/Brushes/BitmapBrush.h>
#include <Wallnut/GameObjects/Components/SpriteComponent.h>

#include "Scripts/CameraMovement.h"
#include "Scripts/CubeCircle.h"

class Game : public Wallnut::Application
{
private:
	//Wallnut::Bitmap image;
public:


	void Init() override {

		using namespace Wallnut;


		SceneManager::AddScene(L"Example Scene", [this](Scene& scene) {
			//MessageBox(getHwnd(), L"WORKS", L"L", MB_OK);

			/*GameObject& obj = Wallnut::GameObject::InstantiateObject();

			obj.AddComponent(*(new RectangleComponent(SolidBrush(164, 212, 53))));
			obj.getTransform().setSize(500, 500);
			obj.AddComponent(new CubeCircle());

			GameObject& obj2 = Wallnut::GameObject::InstantiateObject();
			obj2.getTransform().setPosition(Vector2D(505, 550));
			obj2.AddComponent(*(new RectangleComponent(SolidBrush(255, 255, 255))));
			obj2.getTransform().setSize(54, 100);

			auto& camObj = GameObject::CreateMainCamera(Vector2D(25.0f, 25.0f));
			auto cs = new CameraMovement();

			obj.AddComponent(*(new RectangleComponent(SolidBrush(255, 255, 255))));

			obj2.AddComponent(cs);

			Camera* cam = (camObj.GetComponent<Camera>());

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

			GameObject& obj2 = Wallnut::GameObject::InstantiateObject();

			obj2.AddComponent(*(new RectangleComponent(BitmapBrush(L"resources\\unknown.png"))));
			obj2.getTransform().setPosition(-250, -250);
			obj2.getTransform().setSize(500, 500);
			obj2.AddComponent(new CubeCircle());


			GameObject& obj3 = Wallnut::GameObject::InstantiateObject();

			obj3.AddComponent(*(new SpriteComponent(Bitmap(L"resources\\unknown.png"))));
			obj3.getTransform().setPosition(-100, -100);
			obj3.getTransform().setSize(50, 50);
			//obj.AddComponent(new CubeCircle());
		});

		SceneManager::LoadScene(L"Example Scene");
	}
};

Wallnut::Application* Wallnut::CreateApplication() { return new Game(); }