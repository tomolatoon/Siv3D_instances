#include <ranges>
#include <Siv3D.hpp>
#include "rivet.hpp"

namespace tomolatoon
{
	struct BudouXParser
	{
		using Model = HashTable<String, HashTable<String, int32>>;

		BudouXParser(Model model, Optional<int32> totalScore = none)
			: m_totalScore{totalScore.value_or(0)}, m_model{std::move(model)} {
			if (not totalScore)
			{
				for (const auto& [featureKey, group] : m_model)
				{
					for (const auto& [sequence, score] : group) { m_totalScore += score; }
				}
			}
		}

		BudouXParser() = default;

		BudouXParser(const BudouXParser&) = default;

		BudouXParser(BudouXParser&&) = default;

		BudouXParser& operator=(const BudouXParser&) = default;

		BudouXParser& operator=(BudouXParser&&) = default;

		explicit operator bool() const {
			return (not m_model.empty());
		}

		int32 getFeatureScore(StringView featureKey, StringView sequence) const {
			if (const auto itGroup = m_model.find(featureKey); itGroup != m_model.end())
			{
				const auto& group = itGroup->second;

				if (const auto itScore = group.find(sequence); itScore != group.end())
				{
					const auto& score = itScore->second;

					return score;
				}
			}

			return 0;
		}

		// target で指定された文字について、全ての Feature におけるスコアを合計した値を返す
		int32 getScore(StringView sequence, int64 target) const {
			constexpr std::tuple<StringView, int32, int32> Features[]{
				{U"UW1", -3, 1},
				{U"UW2", -2, 1},
				{U"UW3", -1, 1},
				{U"UW4",  0, 1},
				{U"UW5",  1, 1},
				{U"UW6",  2, 1},
				{U"BW1", -2, 2},
				{U"BW2", -1, 2},
				{U"BW3",  0, 2},
				{U"TW1", -3, 3},
				{U"TW2", -2, 3},
				{U"TW3", -1, 3},
				{U"TW4",  0, 3},
			};

			int32 score = 0;

			for (const auto& [key, pos, n] : Features)
			{
				if ((0 <= (target + pos)) && ((target + pos) < static_cast<int64>(sequence.size())))
				{ score += getFeatureScore(key, sequence.substr((target + pos), n)); }
			}

			return score;
		}

		bool overBoundaryScore(int32 score) const {
			return (score * 2 > m_totalScore);
		}

		bool parseCharacter(StringView sentence, int64 target) const {
			return overBoundaryScore(getScore(sentence, target));
		}

		Array<size_t> parseBoundaries(StringView sentence) const {
			Array<size_t> result;

			for (int64 i = 1; i < static_cast<int64>(sentence.size()); ++i)
			{
				if (overBoundaryScore(getScore(sentence, i)))
				{ result.push_back(i); }
			}
			return result;
		}

		Array<String> parse(StringView sentence) const {
			Array<String> result;

			size_t start = 0;

			for (size_t boundary : parseBoundaries(sentence))
			{
				result.emplace_back(sentence.substr(start, (boundary - start)));

				start = boundary;
			}

			result.emplace_back(sentence.substr(start));

			return result;
		}

		Array<StringView> parseView(StringView sentence) const {
			Array<StringView> result;

			size_t start = 0;

			for (size_t boundary : parseBoundaries(sentence))
			{
				result.push_back(sentence.substr(start, (boundary - start)));

				start = boundary;
			}

			result.push_back(sentence.substr(start));

			return result;
		}

		int32 getTotalScore() const {
			return m_totalScore;
		}

		const Model& getModel() const& {
			return m_model;
		}

		Model&& getModel() && {
			return std::move(m_model);
		}

		friend bool operator==(const BudouXParser& lhs, const BudouXParser& rhs) = default;

		static BudouXParser Parse(const JSON& modelJSON) {
			Model model;
			int32 totalScore = 0;

			for (const auto& [featureKey, groupJSON] : modelJSON)
			{
				auto& group = model[featureKey];

				for (const auto& [sequence, scoreJSON] : groupJSON)
				{
					const int32 score = scoreJSON.getOr<int32>(0);

					group[sequence] = score;

					totalScore += score;
				}
			}

			return BudouXParser{std::move(model), totalScore};
		}

		template <class Reader, std::enable_if_t<std::is_base_of_v<IReader, Reader>>* = nullptr>
		static BudouXParser Load(Reader&& reader) {
			return Parse(JSON::Load(std::forward<Reader>(reader)));
		}

		static BudouXParser Load(FilePathView path) {
			return Load(BinaryReader{path});
		}

		static BudouXParser Download(URLView url) {
			MemoryWriter writer;

			SimpleHTTP::Get(url, {}, writer);

			return Load(MemoryReader{writer.retrieve()});
		}

	private:

		int32 m_totalScore = 0;

		Model m_model = {};
	};

	struct as_sentinel_tag
	{};

	template <std::ranges::input_range View>
	requires std::ranges::view<View>
	      && requires { requires sizeof std::ranges::range_value_t<View> == 4; }
	struct BudouXBreakView: std::ranges::view_interface<BudouXBreakView<View>>
	{
		template <bool IsConst>
		friend struct iterator;

		template <bool IsConst>
		struct sentinel;

		template <bool IsConst>
		struct iterator
		{
			using I = std::ranges::iterator_t<std::conditional_t<IsConst, const View, View>>;

			using Parent = std::conditional_t<IsConst, const BudouXBreakView, BudouXBreakView>;

			using difference_type = ptrdiff_t;

			using value_type = String;

			using iterator_concept = std::conditional_t<
				std::forward_iterator<I>,
				std::forward_iterator_tag,
				std::input_iterator_tag>;

			// clang-format off
			iterator() requires std::default_initializable<I>
			{
				++m_bufTarget;

				// m_parent が nullptr なのでまずい
				//++(*this);
			}

			iterator(iterator&&) = default;

			iterator(const iterator&) requires std::copy_constructible<I> = default;

			// clang-format on

			iterator(iterator<!IsConst> it)
			requires IsConst && std::convertible_to<std::ranges::sentinel_t<View>, I>
				: m_parent{it.m_parent}
				, m_it{std::move(it.m_it)}
				, m_bufTarget{std::move(it.m_bufTarget)}
				, m_cur{std::move(it.m_cur)}
				, m_isSentinel{it.m_cur} {}

			iterator& operator=(iterator&&) = default;

			// clang-format off
			iterator& operator=(const iterator&) requires std::copyable<I> = default;

			// clang-format on

			iterator(Parent& parent, I it)
				: m_parent{std::addressof(parent)}, m_it{std::move(it)}, m_isSentinel{false} {
				++(*this);
			}

			iterator(as_sentinel_tag, Parent& parent, I it)
				: m_parent{std::addressof(parent)}, m_it{std::move(it)}, m_isSentinel{true} {
				++(*this);
			}

			String operator*() const& {
				return m_cur;
			}

			String operator*() && {
				return std::move(m_cur);
			}

			friend String iter_move(const iterator& it) {
				return std::move(it.m_cur);
			}

			iterator& operator++() {
				// m_buf はこんな感じになるようにバッファリングする
				//              ↓ ここを基準にするお気持ち
				// [-3][-2][-1][0][1][2]

				if (isEnd())
				{
					throw Error{
						U"[BudouXBreakView::Iterator::operator++]: cannnot advance over end."
					};
				}

				m_cur.clear();

				const auto sen = std::ranges::end(m_parent->m_view);

				// 最初用（先読み）
				if (m_buf.size() == 0)
				{
					for (; m_it != sen; ++m_it)
					{
						m_buf.push_back(*m_it);

						// 後の都合上、早期 break する
						if (m_buf.size() == 6)
						{ break; }
					}
				}

				// 最後用（終端状態へ）
				// 終端を指すイテレータが生成された場合、コンストラクタで呼び出されるときにここに入る
				// なお、通常ルーチンの直後に入れると、最終状態になった時に続けて終端状態になってしまう
				if (m_bufTarget == m_buf.size())
				{ ++m_bufTarget; }

				// 通常ルーチン
				for (; m_bufTarget < m_buf.size();)
				{
					// 常にやる更新処理
					m_cur.push_back(m_buf[m_bufTarget]);

					// 最初に来た時 or 終端に来た時 の更新処理
					if (m_bufTarget < 3 || m_it == sen)
					{ ++m_bufTarget; }
					// 通常繰り返し期間のの更新処理
					else
					{
						++m_it;

						if (m_it != sen)
						{ m_buf.rotate(1).back() = *m_it; }
						else
						{
							// 最終状態へ向けて m_bufTarget を進める
							++m_bufTarget;
						}
					}

					if (m_parent->getPerserRef().parseCharacter(m_buf, m_bufTarget))
					{ break; }
				}

				return *this;
			}

			iterator& operator++(int) {
				++*this;
				return *this;
			}

			iterator operator++(int)
			requires std::forward_iterator<I>
			{
				iterator tmp = *this;
				++(*this);
				return tmp;
			}

			// 終端状態かどうか
			bool isEnd() const {
				return m_bufTarget == m_buf.size() + 1;
			}

			friend bool operator==(const iterator& it, std::default_sentinel_t) {
				return it.isEnd();
			}

			friend bool operator==(const iterator& lhs, const iterator& rhs) {
				// lhs だけが sentinel
				//  rhs.isEnd() && lhs.m_it == rhs.m_it;
				// rhs だけが sentinal
				//  lhs.isEnd() && lhs.m_it == rhs.m_it;
				// lhs と rhs がどちらも sentinel
				//  lhs.m_it == rhs.m_it; (lhs.isEnd() && rhs.isEnd() は必ず true になる)
				// lhs と rhs がどちらも sentinel でない
				//  lhs.m_it == rhs.m_it && lhs.m_bufTarget == rhs.m_bufTarget;

				if (lhs.m_isSentinel || rhs.m_isSentinel)
				{ return lhs.isEnd() && rhs.isEnd() && lhs.m_it == rhs.m_it; }
				else
				{ return lhs.m_it == rhs.m_it && lhs.m_bufTarget == rhs.m_bufTarget; }
			}

		private:

			Parent* m_parent = nullptr;

			I m_it;

			// BudouX の解析の都合上、前読み 2 と後読み 3 を含めた 6 文字をバッファリングするので
			// [-3][-2][-1][0][1][2] という雰囲気でやる（実際には m_bufTarget が中心）
			String m_buf;

			// 初期状態: 0、但しコンストラクタで 1 になる
			// 通常状態: [1, m_buf.size() - 1]
			// 最終状態: m_buf.size()、全ての要素を読み終わった時
			// 終端状態: m_buf.size() + 1
			size_t m_bufTarget = 0;

			String m_cur;

			bool m_isSentinel = true;
		};

		template <bool IsConst>
		struct sentinel
		{
			using S = std::ranges::sentinel_t<std::conditional_t<IsConst, View, const View>>;

			sentinel() = default;

			sentinel(S sen)
				: m_sen{std::move(sen)} {}

			sentinel(sentinel&&) = default;

			sentinel(const sentinel&) = default;

			sentinel(sentinel<!IsConst> sen)
			requires IsConst && std::convertible_to<std::ranges::sentinel_t<View>, S>
				: m_sen{std::move(sen.m_sen)} {}

			sentinel& operator=(sentinel&&) = default;

			sentinel& operator=(const sentinel&) = default;

			friend bool operator==(const iterator<IsConst>& it, const sentinel&) {
				return it.isEnd();
			}

		private:

			S m_sen;
		};

		// clang-format off
		BudouXBreakView() requires std::default_initializable<View> = default;

		// clang-format on

		BudouXBreakView(View view, const BudouXParser& parser)
			: m_view{std::move(view)}, m_parser{parser} {}

		BudouXBreakView(View view, BudouXParser&& parser)
			: m_view{std::move(view)}, m_parser{std::move(parser)} {}

		BudouXBreakView(View view, std::reference_wrapper<BudouXParser> parser)
			: m_view{std::move(view)}, m_parser{std::ref(std::as_const(parser.get()))} {}

		BudouXBreakView(View view, std::reference_wrapper<const BudouXParser> parser)
			: m_view{std::move(view)}, m_parser{std::move(parser)} {}

		auto begin() {
			return iterator<false>{*this, std::ranges::begin(m_view)};
		}

		auto end() {
			return sentinel<false>{std::ranges::end(m_view)};
		}

		auto end()
		requires std::ranges::common_range<View>
		{
			return iterator<false>{as_sentinel_tag{}, *this, std::ranges::end(m_view)};
		}

		auto begin() const {
			return iterator<true>{*this, std::ranges::begin(m_view)};
		}

		auto end() const {
			return sentinel<true>{std::ranges::end(m_view)};
		}

		auto end() const
		requires std::ranges::common_range<const View>
		{
			return iterator<true>{as_sentinel_tag{}, *this, std::ranges::end(m_view)};
		}

		const BudouXParser& getPerserRef() const {
			return m_parser.index() == 0 ? std::get<0>(m_parser) : std::get<1>(m_parser).get();
		}

	private:

		View m_view;

		std::variant<BudouXParser, std::reference_wrapper<const BudouXParser>> m_parser;
	};

	template <class Range, class T>
	BudouXBreakView(Range&& view, T parser) -> BudouXBreakView<std::views::all_t<Range>>;

	static_assert(std::ranges::input_range<BudouXBreakView<std::views::all_t<String>>>);
	static_assert(std::ranges::input_range<const BudouXBreakView<std::views::all_t<String>>>);
	static_assert(std::ranges::forward_range<BudouXBreakView<std::views::all_t<String>>>);
	static_assert(std::ranges::forward_range<const BudouXBreakView<std::views::all_t<String>>>);
	static_assert(std::input_or_output_iterator<
				  BudouXBreakView<std::views::all_t<String>>::iterator<true>>);
	static_assert(std::input_or_output_iterator<
				  BudouXBreakView<std::views::all_t<String>>::iterator<false>>);
	static_assert(std::input_iterator<BudouXBreakView<std::views::all_t<String>>::iterator<false>>);
	static_assert(std::input_iterator<BudouXBreakView<std::views::all_t<String>>::iterator<true>>);
	static_assert(std::forward_iterator<
				  BudouXBreakView<std::views::all_t<String>>::iterator<false>>);
	static_assert(std::forward_iterator<
				  BudouXBreakView<std::views::all_t<String>>::iterator<true>>);
} // namespace tomolatoon

namespace tomolatoon::detail
{
	struct BudouXBreakAdaptor: rivet::range_adaptor_base<BudouXBreakAdaptor>
	{
		template <std::ranges::viewable_range R, class T>
		constexpr auto operator()(R&& r, T&& parser) const {
			return BudouXBreakView{std::forward<R>(r), std::forward<T>(parser)};
		}

		RIVET_USING_BASEOP;
	};
} // namespace tomolatoon::detail

namespace tomolatoon
{
	inline constexpr detail::BudouXBreakAdaptor BudouXBreak;
} // namespace tomolatoon
