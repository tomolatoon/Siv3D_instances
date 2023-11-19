#include <Siv3D.hpp>

namespace tomolatoon
{
	namespace Arg
	{
		SIV3D_NAMED_PARAMETER(value);
		SIV3D_NAMED_PARAMETER(ratio);
	} // namespace Arg

	struct LerpTransition
	{
		static constexpr double InvLerp(double start, double finish, double init) {
			return (finish - start) ? Math::InvLerp(start, finish, init) : 0.0;
		}

		struct ValueRange
		{
			double start;
			double finish;

			constexpr ValueRange() = default;

			constexpr ValueRange(double start, double finish)
				: start(start), finish(finish) {}
		};

		constexpr LerpTransition(
			const Duration&     inDuration,
			const Duration&     outDuration,
			double              start,
			double              finish,
			Arg::ratio_<double> init
		)
			: m_transition(inDuration, outDuration, init.value()), m_range(start, finish) {}

		constexpr LerpTransition(
			const Duration&     inDuration,
			const Duration&     outDuration,
			double              start,
			double              finish,
			Arg::value_<double> init
		)
			: m_transition(inDuration, outDuration, InvLerp(start, finish, init.value()))
			, m_range(start, finish) {}

		constexpr LerpTransition& range(double start, double finish) {
			m_range = ValueRange(start, finish);
			return *this;
		}

		constexpr ValueRange range() const {
			return m_range;
		}

		constexpr void update(bool in, double deltaSec = Scene::DeltaTime()) {
			m_transition.update(in, deltaSec);
		}

		constexpr void reset(Arg::ratio_<double> ratio) {
			m_transition.reset(ratio.value());
		}

		constexpr void reset(Arg::value_<double> value) {
			m_transition.reset(InvLerp(m_range.start, m_range.finish, value.value()));
		}

		// ratio が取りたければ、transition().value() を使う
		constexpr double value() const {
			return Math::Lerp(m_range.start, m_range.finish, transition().value());
		}

		constexpr bool isStart() const {
			return transition().isZero();
		}

		constexpr bool isFinish() const {
			return transition().isOne();
		}

		constexpr double easeIn(double easingFunction(double) = Easing::Quart) const {
			return Math::Lerp(m_range.start, m_range.finish, transition().easeIn(easingFunction));
		}

		constexpr double easeOut(double easingFunction(double) = Easing::Quart) const {
			return Math::Lerp(m_range.start, m_range.finish, transition().easeOut(easingFunction));
		}

		constexpr double easeInOut(double easingFunction(double) = Easing::Quart) const {
			return Math::Lerp(
				m_range.start,
				m_range.finish,
				transition().easeInOut(easingFunction)
			);
		}

		constexpr const Transition& transition() const {
			return m_transition;
		}

	private:

		Transition m_transition;
		ValueRange m_range;
	};
} // namespace tomolatoon
