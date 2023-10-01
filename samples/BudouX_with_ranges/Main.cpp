// BudouX を ranges と共に使う
// https://scrapbox.io/Siv3D-instances/BudouX_%E3%82%92_ranges_%E3%81%A8%E5%85%B1%E3%81%AB%E4%BD%BF%E3%81%86

#include <Siv3D.hpp> // OpenSiv3D v0.6.11

import tomolatoon.BudouX;

void Main()
{
	const auto parser = tomolatoon::BudouXParser::Download(U"https://raw.githubusercontent.com/google/budoux/main/budoux/models/ja.json");

	const Font font{FontMethod::MSDF, 48};

	TextAreaEditState textAreaState{
		U"Siv3D（シブスリーディー）は、音や画像、AI を使ったゲームやアプリを、"
		U"モダンな C++ コードで楽しく簡単にプログラミングできるオープンソースのフレームワークです。"};

	double fontSizeSlider = 0.4;

	bool forceReturn = false;

	while (System::Update())
	{
		if (SimpleGUI::TextArea(textAreaState, Vec2{30, 20}, SizeF{740, 100}))
		{
			textAreaState.text = textAreaState.text.removed(U'\n');
		}

		SimpleGUI::Slider(U"Font size", fontSizeSlider, Vec2{30, 130}, 100, 200);
		const double fontSize = (fontSizeSlider * 80 + 16);

		SimpleGUI::CheckBox(forceReturn, U"境界で必ず改行する", Vec2{340, 130});

		{
			Vec2 pos{30, 180};

			for (const auto& s : textAreaState.text | tomolatoon::BudouXBreak(std::ref(parser)))
			{
				const auto text     = font(s);
				const auto advances = text.getXAdvances(fontSize);

				if (pos.x != 30 && (forceReturn || ((pos.x + advances.sum()) > 770)))
				{
					pos.x = 30;
					pos.y += font.height(fontSize);
				}

				double advance = 0;
				size_t start   = 0;
				for (size_t i = 0; i < text.clusters.size(); ++i)
				{
					advance += advances[i];
					if ((pos.x + advance) > 770)
					{
						font(s.substr(start, (text.clusters[i].pos - start))).draw(fontSize, pos);
						pos.x = 30;
						pos.y += font.height(fontSize);
						advance = advances[i];
						start += text.clusters[i].pos;
					}
				}
				font(s.substr(start)).draw(fontSize, pos);
				pos.x += advance;
			}
		}
	}
}
