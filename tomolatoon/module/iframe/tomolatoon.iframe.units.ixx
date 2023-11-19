module;
#include <array>

export module tomolatoon.iframe.units;
import tomolatoon.iframe;
import tomolatoon.expression_template;

export namespace tomolatoon::units
{
	struct Percent
	{
		using Basis = double (*)(const double);

		Percent(Basis basis, double per = 1.0) noexcept
			: m_basis(basis), m_per(per) {}

		double per() const noexcept {
			return m_per;
		}

		double value() const noexcept {
			return m_basis(m_per);
		}

		double operator[]([[maybe_unused]] size_t i) const noexcept {
			return m_basis(m_per);
		}

		operator double() const noexcept {
			return m_basis(m_per);
		}

	private:

		Basis  m_basis;
		double m_per;
	};

	// ET = Percent, Expression
	template <class ET>
	struct Length
	{
		Length(ET et)
			: m_et(std::move(et)) {}

		explicit operator double() const noexcept {
			return m_et[0];
		}

		// Add
		template <class ET>
		Length operator+(const Length<ET>& rhs) const noexcept {
			return Length(Expression<decltype(*this), Add, decltype(rhs)>(this->m_et, rhs.m_et));
		}

		// Sub
		template <class ET>
		Length operator-(const Length<ET>& rhs) const noexcept {
			return Length(Expression<decltype(*this), Sub, decltype(rhs)>(this->m_et, rhs.m_et));
		}

		// Mul
		Length operator*(const double rhs) const noexcept {
			return Length(Expression<decltype(*this), Mul, decltype(rhs)>(this->m_et, rhs));
		}

		// Div
		template <class ET>
		Length operator/(const Length<ET>& rhs) const noexcept {
			return Length(Expression<decltype(*this), Div, decltype(rhs)>(this->m_et, rhs.m_et));
		}

	private:

		ET m_et;
	};

	double per(const double source, const double per) noexcept;

	double vw(const double p) noexcept;

	double vh(const double p) noexcept;

	struct vwf: Length<Percent>
	{
		vwf(double per)
			: Length(Percent(vw, per)) {}
	};

	struct vhf: Length<Percent>
	{
		vhf(double per)
			: Length(Percent(vh, per)) {}
	};

	double sw(const double p) noexcept;

	double sh(const double p) noexcept;

	struct swf: Length<Percent>
	{
		swf(double per)
			: Length(Percent(sw, per)) {}
	};

	struct shf: Length<Percent>
	{
		shf(double per)
			: Length(Percent(sh, per)) {}
	};

	inline namespace literals
	{
		double operator""_vw(const unsigned long long per) noexcept;

		double operator""_vw(const long double per) noexcept;

		double operator""_vh(const unsigned long long per) noexcept;

		double operator""_vh(const long double per) noexcept;

		vwf operator""_vwf(const unsigned long long per) noexcept;

		vwf operator""_vwf(const long double per) noexcept;

		vhf operator""_vhf(const unsigned long long per) noexcept;

		vhf operator""_vhf(const long double per) noexcept;

		double operator""_sw(const unsigned long long per) noexcept;

		double operator""_sw(const long double per) noexcept;

		double operator""_sh(const unsigned long long per) noexcept;

		double operator""_sh(const long double per) noexcept;

		swf operator""_swf(const unsigned long long per) noexcept;

		swf operator""_swf(const long double per) noexcept;

		shf operator""_shf(const unsigned long long per) noexcept;

		shf operator""_shf(const long double per) noexcept;
	} // namespace literals

} // namespace tomolatoon::units

module :private;

namespace tomolatoon::units
{
	double per(const double source, const double per) noexcept {
		return per * 0.01 * source;
	}

	double vw(const double p) noexcept {
		return per(Iframe::Width(), p);
	}

	double vh(const double p) noexcept {
		return per(Iframe::Height(), p);
	}

	double sw(const double p) noexcept {
		return per(Scene::Width(), p);
	}

	double sh(const double p) noexcept {
		return per(Scene::Height(), p);
	}

	inline namespace literals
	{
		double operator""_vw(const unsigned long long per) noexcept {
			return vw((double)per);
		}

		double operator""_vw(const long double per) noexcept {
			return vw((double)per);
		}

		double operator""_vh(const unsigned long long per) noexcept {
			return vh((double)per);
		}

		double operator""_vh(const long double per) noexcept {
			return vh((double)per);
		}

		vwf operator""_vwf(const unsigned long long per) noexcept {
			return vwf((double)per);
		}

		vwf operator""_vwf(const long double per) noexcept {
			return vwf((double)per);
		}

		vhf operator""_vhf(const unsigned long long per) noexcept {
			return vhf((double)per);
		}

		vhf operator""_vhf(const long double per) noexcept {
			return vhf((double)per);
		}

		double operator""_sw(const unsigned long long per) noexcept {
			return sw((double)per);
		}

		double operator""_sw(const long double per) noexcept {
			return sw((double)per);
		}

		double operator""_sh(const unsigned long long per) noexcept {
			return sh((double)per);
		}

		double operator""_sh(const long double per) noexcept {
			return sh((double)per);
		}

		swf operator""_swf(const unsigned long long per) noexcept {
			return swf((double)per);
		}

		swf operator""_swf(const long double per) noexcept {
			return swf((double)per);
		}

		shf operator""_shf(const unsigned long long per) noexcept {
			return shf((double)per);
		}

		shf operator""_shf(const long double per) noexcept {
			return shf((double)per);
		}
	} // namespace literals
} // namespace tomolatoon::units
