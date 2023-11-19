#include <array>
#include "tomolatoon.iframe.hpp"
#include "../expression_template/tomolatoon.expression_template.hpp"

namespace tomolatoon::units
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

	inline double per(const double source, const double per) noexcept;

	inline double vw(const double p) noexcept;

	inline double vh(const double p) noexcept;

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

	inline double sw(const double p) noexcept;

	inline double sh(const double p) noexcept;

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
		inline double operator""_vw(const unsigned long long per) noexcept;

		inline double operator""_vw(const long double per) noexcept;

		inline double operator""_vh(const unsigned long long per) noexcept;

		inline double operator""_vh(const long double per) noexcept;

		inline vwf operator""_vwf(const unsigned long long per) noexcept;

		inline vwf operator""_vwf(const long double per) noexcept;

		inline vhf operator""_vhf(const unsigned long long per) noexcept;

		inline vhf operator""_vhf(const long double per) noexcept;

		inline double operator""_sw(const unsigned long long per) noexcept;

		inline double operator""_sw(const long double per) noexcept;

		inline double operator""_sh(const unsigned long long per) noexcept;

		inline double operator""_sh(const long double per) noexcept;

		inline swf operator""_swf(const unsigned long long per) noexcept;

		inline swf operator""_swf(const long double per) noexcept;

		inline shf operator""_shf(const unsigned long long per) noexcept;

		inline shf operator""_shf(const long double per) noexcept;
	} // namespace literals

} // namespace tomolatoon::units

namespace tomolatoon::units
{
	inline double per(const double source, const double per) noexcept {
		return per * 0.01 * source;
	}

	inline double vw(const double p) noexcept {
		return per(Iframe::Width(), p);
	}

	inline double vh(const double p) noexcept {
		return per(Iframe::Height(), p);
	}

	inline double sw(const double p) noexcept {
		return per(Scene::Width(), p);
	}

	inline double sh(const double p) noexcept {
		return per(Scene::Height(), p);
	}

	inline namespace literals
	{
		inline double operator""_vw(const unsigned long long per) noexcept {
			return vw((double)per);
		}

		inline double operator""_vw(const long double per) noexcept {
			return vw((double)per);
		}

		inline double operator""_vh(const unsigned long long per) noexcept {
			return vh((double)per);
		}

		inline double operator""_vh(const long double per) noexcept {
			return vh((double)per);
		}

		inline vwf operator""_vwf(const unsigned long long per) noexcept {
			return vwf((double)per);
		}

		inline vwf operator""_vwf(const long double per) noexcept {
			return vwf((double)per);
		}

		inline vhf operator""_vhf(const unsigned long long per) noexcept {
			return vhf((double)per);
		}

		inline vhf operator""_vhf(const long double per) noexcept {
			return vhf((double)per);
		}

		inline double operator""_sw(const unsigned long long per) noexcept {
			return sw((double)per);
		}

		inline double operator""_sw(const long double per) noexcept {
			return sw((double)per);
		}

		inline double operator""_sh(const unsigned long long per) noexcept {
			return sh((double)per);
		}

		inline double operator""_sh(const long double per) noexcept {
			return sh((double)per);
		}

		inline swf operator""_swf(const unsigned long long per) noexcept {
			return swf((double)per);
		}

		inline swf operator""_swf(const long double per) noexcept {
			return swf((double)per);
		}

		inline shf operator""_shf(const unsigned long long per) noexcept {
			return shf((double)per);
		}

		inline shf operator""_shf(const long double per) noexcept {
			return shf((double)per);
		}
	} // namespace literals
} // namespace tomolatoon::units
