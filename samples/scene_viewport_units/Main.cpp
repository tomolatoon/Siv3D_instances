// Scene/Viewport(Iframe) Units
// https://scrapbox.io/Siv3D-instances/Scene%2FViewport(Iframe)_Units

#include <Siv3D.hpp> // OpenSiv3D v0.6.11

import tomolatoon.iframe.units;

void Main() {
	using namespace tomolatoon::units;

	Window::Resize(1'000, 1'000, Centering::Yes);
	Window::SetStyle(WindowStyle::Sizable);
	// Scene::SetResizeMode(ResizeMode::Actual);
	Scene::SetBackground(Color(U"#B8C9CD"));

	while (System::Update())
	{
		// _vw/_vh は、Viewport(Iframe) が設定されていない場合は _sw/_sh と同じ値を返す

		// top left
		RectF({5_vw, 5_vh}, {42.5_vw, 42.5_vh}).draw(Palette::Red);

		// top right
		RectF({52.5_vw, 5_vh}, {42.5_vw, 42.5_vh}).draw(Palette::Green);

		// bottom right
		RectF({52.5_vw, 52.5_vh}, {42.5_vw, 42.5_vh}).draw(Palette::Yellow);

		// bottom left
		RectF({5_vw, 52.5_vh}, {42.5_vw, 42.5_vh}).draw(Palette::Blue);
	}
}
