#include <concepts>
#include <type_traits>
#include <utility>

#include <Siv3D.hpp>

namespace tomolatoon
{
	template <class T>
	concept HasMovedBy = requires (const T& t) {
		{ t.movedBy(Point{0, 0}) } -> std::same_as<T>;
	};

	struct Iframe
	{
		/// @brief Scene 上での Iframe 領域を返す
		static Rect RectAtScene() noexcept {
			return Graphics2D::GetViewport().value_or(Scene::Rect());
		}

		/// @brief Local(Iframe) 座標系の T を Global(Scene) 座標系の T に変換する
		/// @param local Local(Iframe) 座標系の T
		/// @return Global(Scene) 座標系の T
		template <class T>
		requires HasMovedBy<T>
		static T ToGlobal(const T& local) noexcept {
			return local.movedBy(Iframe::RectAtScene().tl());
		}

		/// @brief Global(Scene) 座標系の Rect を Local(Iframe) 座標系の Rect に変換する
		/// @param global Global(Scene) 座標系の Rect
		/// @return Local(Iframe) 座標系の Rect
		template <class T>
		requires HasMovedBy<T>
		static T ToLocal(const T& global) noexcept {
			return global.movedBy(-Iframe::RectAtScene().tl());
		}

		/// @brief Scene::Size 相当
		static Point Size() noexcept {
			return RectAtScene().size;
		}

		/// @brief Scene::Width 相当
		static int32 Width() noexcept {
			return RectAtScene().w;
		}

		/// @brief Scene::Height 相当
		static int32 Height() noexcept {
			return RectAtScene().h;
		}

		/// @brief Scene::Center 相当
		static Point Center() noexcept {
			return Rect().center().asPoint();
		}

		/// @brief Scene::CenterF 相当
		static Vec2 CenterF() noexcept {
			return Rect().center();
		}

		/// @brief Scene::Rect 相当
		static Rect Rect() noexcept {
			return {0, 0, Size()};
		}
	};
} // namespace tomolatoon

// ゆるして
namespace s3d::Arg
{
	SIV3D_NAMED_PARAMETER(global);
	SIV3D_NAMED_PARAMETER(local);
} // namespace s3d::Arg

namespace tomolatoon
{
	/// @brief 新しい Iframe/シザー矩形 領域が現在の Iframe/シザー矩形 領域と交差していない場合に、例外を投げるかどうかを設定するフラグ
	using ForceNextIntersectCurrent = YesNo<struct ForceNextIntersectCurrentTag>;

	/// @brief 新しい Iframe/シザー矩形 領域が現在の Iframe/シザー矩形 領域からはみ出している場合に、はみ出した部分を切り取るかどうかを設定するフラグ
	using EnableCroppingNextWithCurrent = YesNo<struct EnableCroppingNextWithCurrentTag>;

	struct ScopedIframe2D
		: s3d::ScopedViewport2D
		, s3d::Transformer2D
	{
	private:

		// @note 入出力する Rect は全て Global(Scene) 座標系であることに注意
		static Rect NewIframeRect(
			const Rect request,
			const bool isCropped,
			const bool isForceNextIntersectCurrent
		) {
			const auto viewport = Iframe::RectAtScene();

			if (isForceNextIntersectCurrent)
			{
				if (not viewport.intersects(request))
				{
					throw Error{
						U"Error: ScopedIframe2D: request is not intersects with current viewport"
					};
				}
			}

			if (isCropped)
			{ return viewport.getOverlap(request); }
			else
			{ return request; }
		}

	public:

		/// @brief Iframe 領域を指定して ScopedIframe2D を作成する
		/// @param local Local(Iframe) 座標系での Iframe 領域
		/// @param isCropped 新しい Iframe 領域が現在の Iframe 領域からはみ出している場合にはみ出した部分を切り取るかどうか
		/// @param isForceNextIntersectCurrent 新しい Iframe 領域が現在の Iframe 領域と交差していない場合に例外を投げるかどうか
		ScopedIframe2D(
			Rect                          local,
			EnableCroppingNextWithCurrent isCropped = EnableCroppingNextWithCurrent::No,
			ForceNextIntersectCurrent isForceNextIntersectCurrent = ForceNextIntersectCurrent::No
		)
			: s3d::ScopedViewport2D(NewIframeRect(
				Iframe::ToGlobal(local),
				isCropped.getBool(),
				isForceNextIntersectCurrent.getBool()
			))
			, s3d::Transformer2D(Mat3x2::Identity(), Mat3x2::Translate(local.tl())) {}

		/// @note 通常使わない（冗長のため）
		ScopedIframe2D(
			Arg::local_<Rect>             local,
			EnableCroppingNextWithCurrent isCropped = EnableCroppingNextWithCurrent::No,
			ForceNextIntersectCurrent isForceNextIntersectCurrent = ForceNextIntersectCurrent::No
		)
			: s3d::ScopedViewport2D(NewIframeRect(
				Iframe::ToGlobal(local.value()),
				isCropped.getBool(),
				isForceNextIntersectCurrent.getBool()
			))
			, s3d::Transformer2D(Mat3x2::Identity(), Mat3x2::Translate(local.value().tl())) {}

		/// @note 通常使わない（Global(Scene) 座標系で指定することはほとんどしない）
		ScopedIframe2D(
			Arg::global_<Rect>            global,
			EnableCroppingNextWithCurrent isCropped = EnableCroppingNextWithCurrent::No,
			ForceNextIntersectCurrent isForceNextIntersectCurrent = ForceNextIntersectCurrent::No
		)
			: s3d::ScopedViewport2D(NewIframeRect(
				global.value(),
				isCropped.getBool(),
				isForceNextIntersectCurrent.getBool()
			))
			, s3d::Transformer2D(
				  Mat3x2::Identity(),
				  Mat3x2::Translate(Iframe::ToLocal(global.value()).tl())
			  ) {}
	};

	enum class PositionBased
	{
		Global, ///< Global(Scene) 座標系
		Local,  ///< Local(Iframe) 座標系
	};

	struct ScopedScissorRect2D: s3d::ScopedRenderStates2D
	{
	private:

		// @note 入出力する Rect は全て Global(Scene) 座標系であることに注意
		static ScopedRenderStates2D NewScissorRect(
			const Rect request,
			const bool isCropped,
			const bool isForceNextIntersectCurrent
		) {
			const Rect scissor = []() {
				const auto sys = Graphics2D::GetScissorRect();

				// シザー矩形が無効の場合は Rect{0, 0, 0, 0} が返ってくるので、Iframe に fallback
				return sys == Rect{0, 0, 0, 0} ? Iframe::RectAtScene() : sys;
			}();

			if (isForceNextIntersectCurrent)
			{
				if (not scissor.intersects(request))
				{
					throw Error{U"Error: ScopedScissorRect2D: request is not intersects with "
					            U"current viewport"};
				}
			}

			const Rect result = [&]() {
				if (isCropped)
				{ return scissor.getOverlap(request); }
				else
				{ return request; }
			}();

			Graphics2D::SetScissorRect(result);

			RasterizerState rs = RasterizerState::Default2D;
			rs.scissorEnable   = true;

			return ScopedRenderStates2D{rs};
		}

	public:

		/// @brief シザー矩形を指定して ScopedRenderStates2D を作成する（フラグで Local(Iframe) 座標系か Global(Scene) 座標系かを指定）
		/// @param rect 新しいシザー矩形の領域
		/// @param base 新しいシザー矩形をどの座標系（Local(Iframe) or Global(Scene) 座標系）で指定したか
		ScopedScissorRect2D(
			Rect                          rect,
			PositionBased                 base      = PositionBased::Local,
			EnableCroppingNextWithCurrent isCropped = EnableCroppingNextWithCurrent::No,
			ForceNextIntersectCurrent isForceNextIntersectCurrent = ForceNextIntersectCurrent::No
		)
			: s3d::ScopedRenderStates2D(NewScissorRect(
				base == PositionBased::Local ? Iframe::ToGlobal(rect) : rect,
				isCropped.getBool(),
				isForceNextIntersectCurrent.getBool()
			)) {}

		/// @brief シザー矩形を指定して ScopedRenderStates2D を作成する（Local(Iframe) 座標系）
		/// @param local 新しいシザー矩形の領域（Local(Iframe) 座標系）
		ScopedScissorRect2D(
			Arg::local_<Rect>             local,
			EnableCroppingNextWithCurrent isCropped = EnableCroppingNextWithCurrent::No,
			ForceNextIntersectCurrent isForceNextIntersectCurrent = ForceNextIntersectCurrent::No
		)
			: s3d::ScopedRenderStates2D(NewScissorRect(
				Iframe::ToGlobal(local.value()),
				isCropped.getBool(),
				isForceNextIntersectCurrent.getBool()
			)) {}

		/// @brief シザー矩形を指定して ScopedRenderStates2D を作成する（Global(Scene) 座標系）
		/// @param global 新しいシザー矩形の領域（Global(Scene) 座標系）
		ScopedScissorRect2D(
			Arg::global_<Rect>            global,
			EnableCroppingNextWithCurrent isCropped = EnableCroppingNextWithCurrent::No,
			ForceNextIntersectCurrent isForceNextIntersectCurrent = ForceNextIntersectCurrent::No
		)
			: s3d::ScopedRenderStates2D(NewScissorRect(
				global.value(),
				isCropped.getBool(),
				isForceNextIntersectCurrent.getBool()
			)) {}
	};
} // namespace tomolatoon
