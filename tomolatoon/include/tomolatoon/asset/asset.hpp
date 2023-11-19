#include <Siv3D.hpp>

namespace tomolatoon
{
	template <class T>
	struct type_t
	{
		using type = T;
	};

	template <class T>
	inline constexpr type_t<T> type = {};

	namespace
	{
		template <class Target, class... Args>
		struct ArgumentsHolder
		{
			Target operator()() {
				return std::make_from_tuple(std::move(args));
			}

			Target operator()() const {
				return std::make_from_tuple(args);
			}

			std::tuple<Args...> args;
		};

		template <class T>
		concept Hashable = requires (T x) {
			{ std::hash<T>{}(x) } -> std::convertible_to<size_t>;
		};
	} // namespace

	template <
		class Value,
		class Key    = String,
		class KeyRef = std::conditional_t<std::same_as<Key, String>, StringView, const Key&>>
	requires std::convertible_to<KeyRef, Key> && Hashable<Key>
	struct AssetTraits
	{
		type_t<Value>  valueType  = {};
		type_t<Key>    keyType    = {};
		type_t<KeyRef> keyRefType = {};

		/// @brief 構築に必要な引数を保持（コピー）するかどうか
		///
		/// @note
		/// true の場合、構築に必要な引数全てがコピー可能であること。
		/// false の場合、構築に必要な引数全てがムーブ可能であること。
		///
		/// @remark
		/// false の場合、Register で登録した引数は、Load 時にムーブを伴って使用されて消失する。
		/// したがって、Release した後、再度 Load するには Register を再実行する必要がある。
		bool holdArguments = true;
	};

	template <AssetTraits Trait>
	struct Asset
	{
		using Value_t  = decltype(Trait.valueType)::type;
		using Key_t    = decltype(Trait.keyType)::type;
		using KeyRef_t = decltype(Trait.keyRefType)::type;

		Asset(KeyRef_t key)
			: m_key(key) {
			if (not Load(key))
			{ throw Error(U"[Asset::Asset]: Asset `{}` is not registered"_fmt(key)); }
		}

		template <class... Args>
		static void Register(KeyRef_t key, Args&&... args) noexcept {
			if constexpr (Trait.holdArguments)
			{
				m_assetConstructors.emplace(
					Key_t(key),
					std::as_const(
						ArgumentsHolder{.args = std::make_tuple(std::forward<Args>(args)...)}
					)
				);
			}
			else
			{
				m_assetConstructors.emplace(
					Key_t(key),
					ArgumentsHolder{.args = std::make_tuple(std::forward<Args>(args)...)}
				);
			}
		}

		static bool Load(KeyRef_t key) {
			if (IsReady(key))
			{ return true; }

			if (IsRegistered(key))
			{
				try
				{
					m_assets.emplace(Key_t(key), m_assetConstructors[key]());

					if constexpr (not Trait.holdArguments)
					{ m_assetConstructors.erase(key); }
					return true;
				}
				catch (...)
				{ return false; }
			}
			else
			{ return false; }
		}

		static bool IsRegistered(KeyRef_t key) {
			return m_assetConstructors.contains(key);
		}

		static bool IsReady(KeyRef_t key) {
			return m_assets.contains(key);
		}

		operator Value_t&() {
			return m_assets[m_key];
		}

		operator const Value_t&() const {
			return std::as_const(m_assets[m_key]);
		}

		Value_t& operator()() {
			return m_assets[m_key];
		}

		const Value_t& operator()() const {
			return std::as_const(m_assets[m_key]);
		}

		static void Release(KeyRef_t key) {
			m_assets.erase(key);
		}

		static void ReleaseAll() {
			m_assets.clear();
		}

		static void Unregister(KeyRef_t key) {
			m_assets.erase(key);
			m_assetConstructors.erase(key);
		}

		static void UnregisterAll() {
			m_assets.clear();
			m_assetConstructors.clear();
		}

	private:

		Key_t m_key;

		inline static HashTable<Key_t, std::function<T()>> m_assetConstructors;

		inline static HashTable<Key_t, T> m_assets;
	};
} // namespace tomolatoon
