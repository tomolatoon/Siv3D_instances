// Iframe とシザー矩形
// https://scrapbox.io/Siv3D-instances/Iframe_%E3%81%A8%E3%82%B7%E3%82%B6%E3%83%BC%E7%9F%A9%E5%BD%A2

#include <Siv3D.hpp> // OpenSiv3D v0.6.11

import tomolatoon.iframe;

void Main()
{
	// tomolatoon::Iframe ではなく Iframe と書いても大丈夫にする
	using tomolatoon::Iframe;

	Window::Resize(1'755, 810, Centering::Yes);
	//Scene::SetResizeMode(ResizeMode::Actual);
	Scene::SetBackground(Color(U"#B8C9CD"));

	while (System::Update())
	{
		{
			// Arg::local = Rect() や Arg::global = Rect() で
			// どの座標系に対する Rect であるか指定することも出来る
			// デフォルトは Local(Iframe) 座標系に対して。
			tomolatoon::ScopedIframe2D iframe{Rect(200, 100, 400, 250)};

			// Iframe を設定しているので、Iframe::Rect は Iframe の大きさの Rect を返す
			// 今回返ってくるのは、Rect(0, 0, 400, 250)
			Iframe::Rect().draw(Palette::Beige);

			Shape2D::Star(200, Iframe::CenterF()).draw(Palette::Coral);

			Iframe::Rect().drawFrame(10, 0, Palette::Lightskyblue);
		}

		{
			// Arg::local = Rect() や Arg::global = Rect() で
			// どの座標系に対する Rect であるか指定することも出来る
			// デフォルトは Local(Iframe) 座標系に対して。
			tomolatoon::ScopedScissorRect2D scissor{Rect(700, 400, 400, 250)};

			// Iframe を設定していないので、Iframe::Rect は Scene::Rect と同じものを返す
			Iframe::Rect().draw(Palette::Beige);

			// Graphics2D::GetScissorRect で設定されているシザー矩形を調べられます
			Shape2D::Star(200, Graphics2D::GetScissorRect().center()).draw(Palette::Coral);

			Graphics2D::GetScissorRect().drawFrame(10, 0, Palette::Lightskyblue);
		}
	}
}

