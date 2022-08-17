/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Vector2D.h"
#include "Wallnut/Core.h"

namespace Wallnut {
	class GameObject;
	class WALLNUT_API Transform {
	private:

		Vector2D position;
		Size2D size;

		Transform* parent;
		GameObject* gameObject;
		std::set<Transform*> children;

	public:

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
				((pPos.m_x + size.m_x) > otherPos.m_x) &&
				((otherPos.m_x + transform.size.m_x) < pPos.m_x) &&
				((pPos.m_y + size.m_y) > otherPos.m_y) &&
				((otherPos.m_y + transform.size.m_y) < pPos.m_y)
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
				(pPos.m_x < vec.m_x) && (pPos.m_x + size.m_x) > vec.m_x ||
				(pPos.m_y < vec.m_y) && (pPos.m_y + size.m_y) > vec.m_y				
			);
		}

		D2D1_RECT_F getRectF();

		Vector2D getPosition() const { return parent == NULL ? (position) : (position + parent->position); }
		Size2D getSize() const { return size; }

		Vector2D getLocalPosition() const { return position; }

		void setPosition(Vector2D vec) { position = vec; }
		void setSize(Size2D size) { this->size = size; }

		void setPosition(float x, float y) { position.m_x = x; position.m_y = y; }
		void setSize(float width, float height) { size.m_x = width; size.m_y = height; }

		friend class GameObject;
	};
}