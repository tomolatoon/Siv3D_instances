#include <Siv3D.hpp>

namespace tomolatoon
{
	/// @brief ドラッグの状態を追跡するクラス
	/// @details ドラッグの状態及び、開始位置と終了位置を追跡する
	struct DragTracker
	{
		/// @brief ドラッグの状態
		enum class State : int8
		{
			none    = 0b00000000, ///< ドラッグしていない
			down    = 0b00000001, ///< ドラッグ開始
			pressed = 0b00000010, ///< ドラッグ中
			up      = 0b00000100, ///< ドラッグ終了
		};

		/// @brief コンストラクタ
		/// @param input ドラッグ判定に用いる入力キー等
		DragTracker(InputGroup input = MouseL)
			: m_input(input) {}

		/// @brief ドラッグの状態を更新する
		/// @param isOverStartRegion ドラッグを開始出来る位置にある場合に true を渡す
		/// @return ドラッグ中（離したタイミングも含む）なら true を返す
		bool update(bool isOverStartRegion) noexcept {
			// 既にドラッグが開始しているか
			if (pressed())
			{
				// 既にドラッグが開始していれば、領域外に出ていても押し続けられてさえいれば良い
				if (m_input.pressed())
				{ m_state = State::pressed; }
				else
				{ m_state = State::up; }
			}
			else
			{
				// ドラッグ開始領域にカーソルがあり、かつ入力があればドラッグ開始
				if (isOverStartRegion && m_input.down())
				{ m_state = State::down; }

				// ドラッグ終了後は状態を none に戻す
				if (m_state == State::up)
				{ m_state = State::none; }
			}

			switch (m_state)
			{
			case State::down:
			{
				m_from = Cursor::PosF();
				m_to   = none;
			}
			break;
			case State::up:
			{
				m_to = Cursor::PosF();
			}
			break;
			default: break;
			}

			return duringDragging();
		}

		/// @brief そのフレームでドラッグが開始したか
		bool down() const noexcept {
			return m_state == State::down;
		}

		/// @brief そのフレームでドラッグ中か
		bool pressed() const noexcept {
			return m_state == State::down || m_state == State::pressed;
		}

		/// @brief そのフレームでドラッグが終了したか
		bool up() const noexcept {
			return m_state == State::up;
		}

		/// @brief そのフレームでドラッグしているか
		bool duringDragging() const noexcept {
			return m_state != State::none;
		}

		/// @brief ドラッグの状態を取得する
		/// @return ドラッグの状態
		State state() const noexcept {
			return m_state;
		}

		/// @brief ドラッグの開始位置を取得する
		/// @return ドラッグの開始位置
		/// @note 一度もドラッグしていない場合は none を返す
		Optional<Vec2> from() const noexcept {
			return m_from;
		}

		/// @brief ドラッグの終了位置を取得する
		/// @return ドラッグの終了位置
		/// @note 一度もドラッグしていない場合と、ドラッグが開始してから終了する前は none を返す
		Optional<Vec2> to() const noexcept {
			return m_to;
		}

	private:

		InputGroup m_input;

		State m_state = State::none;

		Optional<Vec2> m_from, m_to;
	};
} // namespace tomolatoon
