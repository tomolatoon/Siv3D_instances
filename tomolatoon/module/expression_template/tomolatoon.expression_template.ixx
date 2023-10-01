export module tomolatoon.expression_template;
export namespace tomolatoon
{
	template <class L, template <class, class> class Op, class R>
	struct  Expression {
		Expression(const L& l, const R& r)
			: m_lhs(l), m_rhs(r) {}

		auto operator[](std::size_t i) const
		{
			return Op<L, R>()(m_lhs[i], m_rhs[i]);
		}

		// min size
		size_t minsize() const
		{
			return std::min(m_lhs.size(), m_rhs.size());
		}

		// max size
		size_t maxsize() const
		{
			return std::max(m_lhs.size(), m_rhs.size());
		}

	private:
		const L& m_lhs;
		const R& m_rhs;
	};

	template <class L, class R>
	struct Add
	{
		auto operator()(const L& l, const R& r) const
			requires requires{ {(l + r)}; }
		{
			return l + r;
		}
	};

	// Sub
	template <class L, class R>
	struct Sub
	{
		auto operator()(const L& l, const R& r) const
			requires requires{ {(l - r)}; }
		{
			return l - r;
		}
	};

	// Mul
	template <class L, class R>
	struct Mul
	{
		auto operator()(const L& l, const R& r) const
			requires requires{ {(l* r)}; }
		{
			return l * r;
		}
	};

	// Div
	template <class L, class R>
	struct Div
	{
		auto operator()(const L& l, const R& r) const
			requires requires{ {(l / r)}; }
		{
			return l / r;
		}
	};

	// Mod
	template <class L, class R>
	struct Mod
	{
		auto operator()(const L& l, const R& r) const
			requires requires{ {(l% r)}; }
		{
			return l % r;
		}

		auto operator()(const L& l, const R& r) const
			requires requires{ {std::fmod(l, r)}; }
		{
			return std::fmod(l, r);
		}
	};
}
