// BudouX を ranges と共に使う
// https://scrapbox.io/Siv3D-instances/BudouX_%E3%82%92_ranges_%E3%81%A8%E5%85%B1%E3%81%AB%E4%BD%BF%E3%81%86

#include <Siv3D.hpp> // OpenSiv3D v0.6.12

#include "json.hpp"

nlohmann::json f(nlohmann::json& j)
{
	return j;
}

void Main() {
	Console.open();

	nlohmann::json json = R"({"a": [1, "2", {"obj": 0}]})"_json;

	nlohmann::json copy = json;

	nlohmann::json& ref = json;

	nlohmann::json&& rref = std::move(json);

	nlohmann::json move = std::move(rref);

	auto&& r =  move["a"];

	for (auto it = move.begin(), end = move.end(); it != end; ++it)
	{
		auto&& r = *it;
		std::cout << r;
	}

	while (System::Update())
	{
		// do something
	}
}
