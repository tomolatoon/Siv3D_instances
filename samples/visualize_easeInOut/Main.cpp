// Easing 及び EaseIn/EaseOut/EaseInOut を可視化
// https://scrapbox.io/Siv3D-instances/Easing_%E5%8F%8A%E3%81%B3_EaseIn%2FEaseOut%2FEaseInOut_%E3%82%92%E5%8F%AF%E8%A6%96%E5%8C%96

#include <Siv3D.hpp> // OpenSiv3D v0.6.11

// 図の構造
//  横軸: 割合（0.0～1.0）
//  縦軸: 値（0.0～1.0、少しはみ出しても大丈夫）

void Main() {
	Window::Resize(1'755, 810, Centering::Yes);
	// Scene::SetResizeMode(ResizeMode::Actual);

	// 表示に使いたいイージング関数を指定
	using EasingFunction                         = double (*)(double);
	EasingFunction                easingFunction = EaseInBack;

	// 本題の s3d::EaseIn, s3d::EaseOut, s3d::EaseInOut
	Array<double (*)(double f(double), const double t) noexcept> ease{EaseIn, EaseOut, EaseInOut};

	// グラフサイズ
	const auto size = Scene::Width() * 0.3;

	// 図の上下にはみ出す部分の大きさ
	const auto extendY = size * 0.2;

	// 見栄えのための上側のマージンと左右のマージン
	const auto topMargin = (Scene::Height() - (size + extendY * 2)) / 2.0;
	const auto xMargin   = (Scene::Width() - size * 3) / 4.0;

	// 横軸の分割数
	const auto split = 100;

	while (System::Update())
	{
		for (auto&& c : step(0, 3))
		{
			// それぞれの図の左端の座標
			const auto left = c * size + (c + 1) * xMargin;

			// 図の縦軸について 0.0 ～ 1.0 の部分
			const auto graph
				= Rect(Vec2(left, topMargin + extendY).asPoint(), Vec2(size, size).asPoint());

			// 図の縦軸について 0.0 ～ 1.0 からはみ出す部分
			const auto background = graph.stretched(0, extendY);

			background.draw(Palette::Lightgray);
			graph.draw(Palette::White);

			// 図の中心に円を描画（EaseInOut の特徴を捉えやすくするため）
			graph.center().asCircle(10).draw(Palette::Darkgray);

			// 横軸は 100 等分してそれぞれ点を表示する
			for (auto&& i : step(0, split))
			{
				// 横軸の値に変換
				auto t = (double)i / split;

				// 横軸の表示位置
				const auto x = left + Math::Lerp(0, size, t);

				// 縦軸の表示位置
				const auto y = Math::Lerp(
					topMargin + extendY + size,
					topMargin + extendY,
					ease[c](easingFunction, t)
				);

				// 点を表示する
				Circle(Vec2(x, y), 3).draw(Palette::Black);
			}
		}
	}
}
