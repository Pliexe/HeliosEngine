/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Vector2D.h"
#include "Helios/Core.h"

namespace Helios {
	class GameObject;
	class HELIOS_API Transform {
	private:

		Vector2D position;
		Size2D size;

		Transform* parent;
		GameObject* gameObject;
		std::set<Transform*> children;

	public:

		typedef struct Matrix2x2 {
			float x;
			float y;
			float w;
			float h;

			operator D2D1_RECT_F() { return D2D1::RectF(x, y, w, h); }
		};

		Transform() = delete;
		Transform(GameObject* gm) : gameObject(gm), position(0, 0), size(50, 50), parent(NULL) { }
		Transform(GameObject* gm, Vector2D pos, Size2D size) : position(pos), gameObject(gm), size(size), parent(NULL) { }
		Transform(GameObject* gm, float x, float y, float width, float height) : gameObject(gm), position(x, y), size(width, height), parent(NULL) { }

		void setParent(Transform& transform) {
			parent = &transform;
			transform.children.insert(this);
		}

		void removeParent() {
			parent->children.erase(this);
			parent = NULL;
		}

		/*D2D1_RECT_F getTransform() {
			return D2D1::Matrix3x2F::Translation()
		}*/

		bool withinExactBounds(Transform& transform) {
			auto pPos = getPosition();
			auto otherPos = transform.getPosition();
			return (
				((pPos.vec2[0] + size.vec2[0]) > otherPos.vec2[0]) &&
				((otherPos.vec2[0] + transform.size.vec2[0]) < pPos.vec2[0]) &&
				((pPos.vec2[1] + size.vec2[1]) > otherPos.vec2[1]) &&
				((otherPos.vec2[1] + transform.size.vec2[1]) < pPos.vec2[1])
			);
		}

		bool withinBounds(Transform& transform) {
			auto otherPos = transform.getPosition();
			return (
				withinBounds(otherPos) || 
				withinBounds(otherPos + size)
			);
		}

		bool withinBounds(Vector2D vec) {
			auto pPos = getPosition();
			return (
				(pPos.vec2[0] < vec.vec2[0]) && (pPos.vec2[0] + size.vec2[0]) > vec.vec2[0] ||
				(pPos.vec2[1] < vec.vec2[1]) && (pPos.vec2[1] + size.vec2[1]) > vec.vec2[1]				
			);
		}

		D2D1_RECT_F getRectF();

		Vector2D getPosition() const { return parent == NULL ? (position) : (position + parent->position); }
		Size2D getSize() const { return size; }

		Vector2D getLocalPosition() const { return position; }

		Matrix2x2 getScreenPosition() const;

		void setPosition(Vector2D vec) { position = vec; }
		void setSize(Size2D size) { this->size = size; }

		void setPosition(float x, float y) { position.vec2[0] = x; position.vec2[1] = y; }
		void setSize(float width, float height) { size.vec2[0] = width; size.vec2[1] = height; }

		friend class GameObject;

		extern friend class GameEngine;
	};
}