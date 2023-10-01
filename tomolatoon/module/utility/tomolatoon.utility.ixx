module;
#include <Siv3D.hpp>
#include <optional>
#include <compare>
#include <concepts>

export module tomolatoon.utility;
export namespace tomolatoon
{
	template <class T>
	using Optional = std::optional<T>;

	constexpr double Sign(double d) noexcept
	{
		return (int)(d > 0) - (int)(d < 0);
	}

	constexpr double rSign(double d) noexcept
	{
		return -1 * Sign(d);
	}

	bool isURL(FilePathView fp) noexcept;

	template <std::floating_point F>
	F modulo(F val, F mod)
	{
		if (mod)
		{
			return val > 0 ? Fmod(val, mod) : mod + Fmod(val, mod);
		}
		else
		{
			return val;
		}
	}

	namespace Cursor
	{
		void Update() noexcept;

		Point Delta() noexcept;

		Vec2 DeltaF() noexcept;

		Vec2 Velocity() noexcept;

		Vec2 PreviousVelocity() noexcept;

		Vec2 Acceleration() noexcept;
	} // namespace Cursor

} // namespace tomolatoon

module:private;
namespace tomolatoon
{
	bool isURL(FilePathView fp) noexcept
	{
		const auto protocol = Array{
			U"http://"_s,
			U"https://"_s,
		};

		for (auto&& e : protocol)
		{
			if (fp.starts_with(e))
			{
				return true;
			}
		}

		return false;
	}

	namespace Cursor
	{
		namespace
		{
			Vec2 previousVelocity = {};
			Vec2 velocity = {};
			Vec2 acceleration = {};
		} // namespace

		void Update() noexcept
		{
			previousVelocity = velocity;
			velocity = DeltaF() / Scene::DeltaTime();

			acceleration = (velocity - previousVelocity) / Scene::DeltaTime();
		}

		Point Delta() noexcept
		{
			return s3d::Cursor::Delta();
		}

		Vec2 DeltaF() noexcept
		{
			return s3d::Cursor::DeltaF();
		}

		Vec2 Velocity() noexcept
		{
			return velocity;
		}

		Vec2 PreviousVelocity() noexcept
		{
			return previousVelocity;
		}

		Vec2 Acceleration() noexcept
		{
			return acceleration;
		}
	} // namespace Cursor
}
