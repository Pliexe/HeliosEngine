/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Vector2D.h"
#include "Helios/Core/Base.h"

namespace Helios {
	class GameObject;
	class HELIOS_API __OLD__Transform {
	private:

		Vector2D position;
		Size2D size;

		__OLD__Transform* parent;
		GameObject* gameObject;
		std::set<__OLD__Transform*> children;

	public:

		typedef struct Matrix2x2 {
			float x;
			float y;
			float w;
			float h;

			operator D2D1_RECT_F() { return D2D1::RectF(x, y, w, h); }
		};

		__OLD__Transform() = delete;
		__OLD__Transform(GameObject* gm) : gameObject(gm), position(0, 0), size(50, 50), parent(NULL) { }
		__OLD__Transform(GameObject* gm, Vector2D pos, Size2D size) : position(pos), gameObject(gm), size(size), parent(NULL) { }
		__OLD__Transform(GameObject* gm, float x, float y, float width, float height) : gameObject(gm), position(x, y), size(width, height), parent(NULL) { }

		void setParent(__OLD__Transform& __OLD__Transform) {
			parent = &__OLD__Transform;
			__OLD__Transform.children.insert(this);
		}

		void removeParent() {
			parent->children.erase(this);
			parent = NULL;
		}

		/*D2D1_RECT_F get__OLD__Transform() {
			return D2D1::Matrix3x2F::Translation()
		}*/

		bool withinExactBounds(__OLD__Transform& __OLD__Transform) {
			auto pPos = getPosition();
			auto otherPos = __OLD__Transform.getPosition();
			return (
				((pPos[0] + size[0]) > otherPos[0]) &&
				((otherPos[0] + __OLD__Transform.size[0]) < pPos[0]) &&
				((pPos[1] + size[1]) > otherPos[1]) &&
				((otherPos[1] + __OLD__Transform.size[1]) < pPos[1])
			);
		}

		bool withinBounds(__OLD__Transform& __OLD__Transform) {
			auto otherPos = __OLD__Transform.getPosition();
			return (
				withinBounds(otherPos) || 
				withinBounds(otherPos + size)
			);
		}

		bool withinBounds(Vector2D vec) {
			auto pPos = getPosition();
			return (
				(pPos[0] < vec[0]) && (pPos[0] + size[0]) > vec[0] ||
				(pPos[1] < vec[1]) && (pPos[1] + size[1]) > vec[1]				
			);
		}

		//D2D1_RECT_F getRectF();

		Vector2D getPosition() const { return parent == NULL ? (position) : (position + parent->position); }
		Size2D getSize() const { return size; }

		Vector2D getLocalPosition() const { return position; }

		//Matrix2x2 getScreenPosition() const;

		void setPosition(Vector2D vec) { position = vec; }
		void setSize(Size2D size) { this->size = size; }

		void setPosition(float x, float y) { position[0] = x; position[1] = y; }
		void setSize(float width, float height) { size[0] = width; size[1] = height; }

		friend class GameObject;

		extern friend class GameEngine;
	};
}