#ifndef ENVIROMENTAL_VARIABLE_H_
#define ENVIROMENTAL_VARIABLE_H_

class EnvironmentalSetting {
	public:
	/* Parameter for All Algorithm*/

	/* Parameter for Page Rank Algorithm*/
	static constexpr long double PAGE_RANK_ALPHA = 0.85;
	static constexpr long double PAGE_RANK_CONVERGENCE = 0.001;
	static constexpr int PAGE_RANK_MAX_ITERATIONS = 50;
};

#endif /* ENVIROMENTAL_VARIABLE_H_ */
