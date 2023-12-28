#pragma once

#include <chrono>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <thread>
#include <vector>

#include "Cypher.h"

constexpr int WINDOW_WIDTH = 96;
constexpr int WINDOW_HEIGHT = 64;

class Entity
{
public:
	enum class Tag
	{
		UNKNOWN = 0,
		PLAYER,
		BALL,
		BRICK
	};

	Entity(float x, float y, float width, float height, Tag tag, Cypher::Color::Foreground color = Cypher::Color::Foreground::WHITE) :
		m_x(x),
		m_y(y),
		m_dx(0.0f),
		m_dy(0.0f),
		m_sprite(static_cast<SHORT>(width), static_cast<SHORT>(height)),
		m_active(true),
		m_tag(tag)
	{
		for (size_t y = 0; y < m_sprite.GetHeight(); ++y)
		{
			for (size_t x = 0; x < m_sprite.GetWidth(); ++x)
			{
				m_sprite.SetGlyph(x, y, Cypher::Pixel::FULL);
				m_sprite.SetColor(x, y, color);
			}
		}
	}

	virtual void Update(float deltaTime) {}

	virtual void FixedUpdate(float timeStep) {}

	virtual void Render() const
	{
		Cypher::Console::GetRenderer().DrawSprite(m_x, m_y, m_sprite);

		Cypher::AABB<float> swept = GetSweptAABB();
		Cypher::Console::GetRenderer().Draw(swept.minX, swept.minY, Cypher::Pixel::THREE_FOURTHS, Cypher::Color::FG::YELLOW);
		Cypher::Console::GetRenderer().Draw(swept.maxX, swept.maxY, Cypher::Pixel::THREE_FOURTHS, Cypher::Color::FG::YELLOW);

		Cypher::AABB<float> inflated = GetInflatedAABB();
		Cypher::Console::GetRenderer().Draw(inflated.minX, inflated.minY, Cypher::Pixel::THREE_FOURTHS, Cypher::Color::FG::BLUE);
		Cypher::Console::GetRenderer().Draw(inflated.maxX, inflated.maxY, Cypher::Pixel::THREE_FOURTHS, Cypher::Color::FG::BLUE);
	}

	virtual void OnCollision(const Entity& other, float impactTime) {}

	virtual void Reset() {}

	std::optional<float> GetTimeOfImpact(const Entity& other) const
	{
		const Cypher::AABB<float>& aCurr = this->GetAABB();
		const Cypher::AABB<float>& bCurr = other.GetAABB();

		Cypher::AABB<float> aFuture = aCurr;
		aFuture.minX += this->GetDX();
		aFuture.maxX += this->GetDX();
		aFuture.minY += this->GetDY();
		aFuture.maxY += this->GetDY();

		Cypher::AABB<float> bFuture = bCurr;
		bFuture.minX += other.GetDX();
		bFuture.maxX += other.GetDX();
		bFuture.minY += other.GetDY();
		bFuture.maxY += other.GetDY();

		float relVelX = other.GetDX() - this->GetDX();
		float relVelY = other.GetDY() - this->GetDY();

		float tFirst = 0.0f;
		float tLast = 1.0f;

		if (relVelX < 0)
		{
			if (aFuture.maxX < bCurr.minX || aCurr.minX > bFuture.maxX)
				return std::nullopt;
			tFirst = std::max<float>(tFirst, (bCurr.minX - aFuture.maxX) / relVelX);
			tLast = std::min<float>(tLast, (bFuture.maxX - aCurr.minX) / relVelX);
		}
		else if (relVelX > 0)
		{
			if (aCurr.maxX < bFuture.minX || aFuture.minX > bCurr.maxX)
				return std::nullopt;
			tFirst = std::max<float>(tFirst, (bFuture.minX - aCurr.maxX) / relVelX);
			tLast = std::min<float>(tLast, (bCurr.maxX - aFuture.minX) / relVelX);
		}
		else if (aCurr.maxX <= bCurr.minX || aCurr.minX >= bCurr.maxX)
		{
			return std::nullopt;
		}

		if (relVelY < 0)
		{
			if (aFuture.maxY < bCurr.minY || aCurr.minY > bFuture.maxY)
				return std::nullopt;
			tFirst = std::max<float>(tFirst, (bCurr.minY - aFuture.maxY) / relVelY);
			tLast = std::min<float>(tLast, (bFuture.maxY - aCurr.minY) / relVelY);
		}
		else if (relVelY > 0)
		{
			if (aCurr.maxY < bFuture.minY || aFuture.minY > bCurr.maxY)
				return std::nullopt;
			tFirst = std::max<float>(tFirst, (bFuture.minY - aCurr.maxY) / relVelY);
			tLast = std::min<float>(tLast, (bCurr.maxY - aFuture.minY) / relVelY);
		}
		else if (aCurr.maxY <= bCurr.minY || aCurr.minY >= bCurr.maxY)
		{
			return std::nullopt;
		}

		if (tFirst > tLast)
			return std::nullopt; // No collision

		return (tFirst >= 0.0f && tFirst <= 1.0f) ? std::optional<float>(tFirst) : std::nullopt;
	}

	Cypher::AABB<float> GetAABB() const { return { m_x, m_y, static_cast<float>(m_sprite.GetWidth()), static_cast<float>(m_sprite.GetHeight()) }; }

	Cypher::AABB<float> GetSweptAABB() const
	{
		float futureX = m_x + m_dx;
		float futureY = m_y + m_dy;
		return { futureX, futureY, futureX + m_sprite.GetWidth(), futureY + m_sprite.GetHeight() };
	}

	Cypher::AABB<float> GetInflatedAABB() const
	{
		float minX = m_x - m_sprite.GetWidth() * 0.5f;
		float minY = m_y - m_sprite.GetHeight() * 0.5f;
		float maxX = m_x + m_sprite.GetWidth() + m_sprite.GetWidth() * 0.5f;
		float maxY = m_y + m_sprite.GetHeight() + m_sprite.GetHeight() * 0.5f;
		return { minX, minY, maxX, maxY };
	}

	float GetX() const { return m_x; }
	float GetY() const { return m_y; }
	float GetDX() const { return m_dx; }
	float GetDY() const { return m_dx; }
	Cypher::Sprite& GetSprite() { return m_sprite; }
	const Cypher::Sprite& GetSprite() const { return m_sprite; }
	bool IsActive() const { return m_active; }
	void Activate() { m_active = true; }
	void Deactivate() { m_active = false; }
	Tag GetTag() const { return m_tag; }

protected:
	float m_x;
	float m_y;
	float m_dx;
	float m_dy;
	Cypher::Sprite m_sprite;
	bool m_active;
	Tag m_tag;
};

class Ball : public Entity
{
public:
	static constexpr float MIN_SPEED = 0.75f;
	static constexpr float MAX_SPEED = 1.25f;

	Ball(float sx = WINDOW_WIDTH * 0.5f, float sy = WINDOW_HEIGHT * 0.5f) : Entity(sx, sy, 1, 1, Tag::BALL, Cypher::Color::FG::RED), m_torque(0.0f)
	{
		m_dx = rand() % 2 == 0 ? -0.5f : 0.5f;
		m_dy = 0.5f;
	}

	void Update(float deltaTime) override
	{
		Entity::Update(deltaTime);
	}

	void FixedUpdate(float timeStep) override
	{
		m_dx += m_torque;
		CheckBounds();
		m_x += m_dx;
		m_y += m_dy;

		float dampingFactor = 0.667f;
		m_torque *= dampingFactor;
	}

	void OnCollision(const Entity& other, float impactTime)
	{
		m_x += m_dx * impactTime;
		m_y += m_dy * impactTime;

		Cypher::Vector2f collisionPoint = CalculateCollisionPoint(this->GetAABB(), other.GetAABB());
		Cypher::Vector2f collisionNormal = CalculateCollisionNormal(this->GetAABB(), other.GetAABB(), collisionPoint);

		Cypher::Vector2f ballVelocity = Reflect(m_dx, m_dy, collisionNormal);

		float spinFactor = other.GetDX() * 0.1f;
		float newSpeed = ballVelocity.Magnitude() + spinFactor;
		newSpeed = std::clamp(newSpeed, MIN_SPEED, MAX_SPEED);

		ballVelocity.Normalize();
		ballVelocity.x *= newSpeed;
		ballVelocity.y *= newSpeed;

		m_dx = ballVelocity.x;
		m_dy = ballVelocity.y;
		float remainingTime = 1.0f - impactTime;
		m_x += m_dx * remainingTime;
		m_y += m_dy * remainingTime;

		if (std::abs(m_dx) < MIN_SPEED)
			m_dx = MIN_SPEED * (m_dx > 0 ? 1 : -1);
		if (std::abs(m_dy) < MIN_SPEED)
			m_dy = MIN_SPEED * (m_dy > 0 ? 1 : -1);

		PostProcessVelocity();
	}

	void PostProcessVelocity()
	{
		if (std::abs(m_dx) <= 0.1f)
			m_dx *= 2;
	}

private:
	void CheckBounds()
	{
		if (m_x <= 0 || m_x >= WINDOW_WIDTH)
			m_dx *= -1;
		if (m_y <= 0)
			m_dy *= -1;
		if (m_y >= WINDOW_HEIGHT)
		{
			m_x = WINDOW_WIDTH * 0.5f;
			m_y = WINDOW_HEIGHT * 0.5f;
		}
	}

	Cypher::Vector2f CalculateCollisionPoint(const Cypher::AABB<float>& ballAABB, const Cypher::AABB<float>& brickAABB)
	{
		float ballCenterX = (ballAABB.minX + ballAABB.maxX) * 0.5f;
		float ballCenterY = (ballAABB.minY + ballAABB.maxY) * 0.5f;

		float closestX = std::max<float>(brickAABB.minX, std::min<float>(ballCenterX, brickAABB.maxX));
		float closestY = std::max<float>(brickAABB.minY, std::min<float>(ballCenterY, brickAABB.maxY));

		return Cypher::Vector2f(closestX, closestY);
	}

	Cypher::Vector2f CalculateCollisionNormal(const Cypher::AABB<float>& ballAABB, const Cypher::AABB<float>& brickAABB, const Cypher::Vector2f& collisionPoint)
	{
		float ballCenterX = (ballAABB.minX + ballAABB.maxX) * 0.5f;
		float ballCenterY = (ballAABB.minY + ballAABB.maxY) * 0.5f;

		float dx = ballCenterX - collisionPoint.x;
		float dy = ballCenterY - collisionPoint.y;

		if (std::abs(dx) > std::abs(dy))
			return (dx > 0) ? Cypher::Vector2f(1, 0) : Cypher::Vector2f(-1, 0);
		else
			return (dy > 0) ? Cypher::Vector2f(0, 1) : Cypher::Vector2f(0, -1);
	}

	Cypher::Vector2f Reflect(float dx, float dy, const Cypher::Vector2f& normal)
	{
		float dotProduct = dx * normal.x + dy * normal.y;
		return Cypher::Vector2f(dx - 2 * dotProduct * normal.x, dy - 2 * dotProduct * normal.y);
	}

	float m_torque;
};

struct Paddle : public Entity
{
	Paddle(float sx = 0.0f, float sy = WINDOW_HEIGHT * 0.85f) : Entity(sx, sy, 12, 1, Tag::PLAYER, Cypher::Color::FG::RED)
	{
	}

	void Update(float deltaTime) override
	{
		m_x = std::max<float>(0, Cypher::Console::GetMouseX() - m_sprite.GetWidth() * 0.5f);
		if (m_x + m_sprite.GetWidth() > WINDOW_WIDTH)
			m_x = static_cast<float>(WINDOW_WIDTH) - m_sprite.GetWidth();
	}
};

struct Brick : public Entity
{
	Brick(float sx, float sy, float width, float height, Cypher::Color::Foreground color) : Entity(sx, sy, width, height, Tag::BRICK, color) {}

	void Update(float deltaTime) override
	{
	}

	void OnCollision(const Entity& other, float impactTime) override
	{
		switch (other.GetTag())
		{
		case Entity::Tag::BALL:
			m_active = false;
			break;
		default:
			break;
		}
	}

	void Reset() override
	{
		m_active = true;
	}
};

class Level
{
public:
	Level() : m_allocator()
	{
		auto memory = m_allocator.Allocate(sizeof(Paddle));
		auto paddle = new (memory) Paddle();
		m_entities.emplace_back(paddle);

		auto memory2 = m_allocator.Allocate(sizeof(Ball));
		auto ball = new (memory2) Ball();
		m_entities.emplace_back(ball);

		GenerateBricks();
	}

	~Level()
	{
		for (void* entity : m_entities)
		{
			m_allocator.Deallocate(&entity);
		}
	}

	void Update(float deltaTime)
	{
		int activeBrickCount = 0;
		for (auto& entity : m_entities)
		{
			if (entity->IsActive())
				entity->Update(deltaTime);

			if (auto* brick = dynamic_cast<Brick*>(entity))
			{
				if (brick->IsActive())
					++activeBrickCount;
			}
		}

		if (activeBrickCount == 0)
			ResetLevel();
	}

	void FixedUpdate(float timeStep)
	{
		Collision();
		for (auto& entity : m_entities)
		{
			entity->FixedUpdate(timeStep);
		}
	}

	void Render()
	{
		for (auto& entity : m_entities)
		{
			if (entity->IsActive())
				entity->Render();
		}
	}

private:
	void GenerateBricks()
	{
		const int brickWidth = WINDOW_WIDTH / 16;
		const int brickHeight = static_cast<int>(brickWidth * 0.4);
		const int rowCount = 6;
		const int bricksPerRow = WINDOW_WIDTH / brickWidth;
		const int brickCount = bricksPerRow * rowCount;

		Cypher::Color::FG colors[rowCount] =
		{
			Cypher::Color::FG::DARK_RED,
			Cypher::Color::FG::RED,
			Cypher::Color::FG::PURPLE,
			Cypher::Color::FG::DARK_YELLOW,
			Cypher::Color::FG::GREEN,
			Cypher::Color::FG::DARK_CYAN
		};

		for (int i = 0; i < brickCount; ++i)
		{
			float x = static_cast<float>(i % bricksPerRow) * brickWidth;
			float y = WINDOW_HEIGHT * 0.2f + (i / bricksPerRow) * brickHeight;

			auto memory = m_allocator.Allocate(sizeof(Brick));
			auto brick = new (memory) Brick(x, y, brickWidth, brickHeight, colors[i / bricksPerRow]);
			m_entities.emplace_back(brick);
		}
	}

	void Collision()
	{
		for (auto& a : m_entities)
		{
			if (!a->IsActive())
				continue;

			float earliestImpactTime = 1.0f;
			Entity* firstImpactEntity = nullptr;

			for (auto& b : m_entities)
			{
				if (!b->IsActive() || a == b)
					continue;

				if (a->GetInflatedAABB().Intersects(b->GetInflatedAABB()))
				{
					auto impactTime = a->GetTimeOfImpact(*b);
					if (impactTime && *impactTime < earliestImpactTime)
					{
						earliestImpactTime = *impactTime;
						firstImpactEntity = b;
						break;
					}
				}
			}
			if (firstImpactEntity)
			{
				a->OnCollision(*firstImpactEntity, earliestImpactTime);
				firstImpactEntity->OnCollision(*a, earliestImpactTime);
			}
		}
	}

	void ResetLevel()
	{
		for (auto& entity : m_entities)
		{
			entity->Reset();
			entity->Update(0);
			entity->Render();
		}
	}

	std::vector<Entity*> m_entities;
	Cypher::StackAllocator<> m_allocator;
};

Level level;

void Update(float deltaTime)
{
	level.Update(deltaTime);
}

void FixedUpdate(float timeStep)
{
	level.FixedUpdate(timeStep);
}

void Render()
{
	level.Render();
}