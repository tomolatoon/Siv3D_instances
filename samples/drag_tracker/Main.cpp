// DragTracker
// https://scrapbox.io/Siv3D-instances/DragTracker

#include <Siv3D.hpp> // OpenSiv3D v0.6.11

import tomolatoon.drag_tracker;

void Main() {
	Window::Resize(1'755, 810, Centering::Yes);
	// Scene::SetResizeMode(ResizeMode::Actual);
	Scene::SetBackground(Color(U"#B8C9CD"));

	// ドラッグの状態を追跡するクラスのインスタンスを生成
	tomolatoon::DragTracker trackerForCircle, trackerForLerpCircle;

	// 円の位置
	Vec2 posForCircle = Scene::Size() * 0.25, posForLerpCircle = Scene::Size() * 0.75;

	// LerpCircle の位置を変更する際に用いるトランジション
	Transition transitionForLerpCircle{0.5s, 0.0s, 1.0};

	while (System::Update())
	{
		{
			const auto circle = Circle(posForCircle, 50);

			auto color = Palette::White;

			// マウスを乗せてるとき
			if (circle.mouseOver())
			{ color = Palette::Skyblue; }

			// ドラッグしているとき
			if (trackerForCircle.update(circle.mouseOver()))
			{
				// マウスに追尾
				posForCircle = Cursor::PosF();

				color = Palette::Orange;
			}

			circle.draw(color);
		}

		{
			const auto circle = Circle(posForLerpCircle, 50);

			auto color = Palette::Lightgreen;

			// ドラッグしているとき
			if (trackerForLerpCircle.update(circle.mouseOver()))
			{
				transitionForLerpCircle.reset();

				// 強めの緑
				color = Color(U"#62D662");
			}
			else
			{
				// ドラッグ終了後はトランジションを進める
				transitionForLerpCircle.update(true);

				const auto from = trackerForLerpCircle.from().value_or(posForLerpCircle);
				const auto to   = trackerForLerpCircle.to().value_or(posForLerpCircle);

				// 次の位置を線形補間で計算
				posForLerpCircle
					= Math::Lerp(from, to, transitionForLerpCircle.easeOut(EaseInBack));

				// ドラッグ終了後でトランジションが終わっていないとき
				if (not transitionForLerpCircle.isOne())
				{
					// 行き先の位置に円を描画
					circle.stretched(10).setPos(to).drawFrame(10, color);
				}
			}

			circle.draw(color);
		}
	}
}
