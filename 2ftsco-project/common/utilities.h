#ifndef CNDP_COMMON_UTILITIES_H_
#define CNDP_COMMON_UTILITIES_H_

#include "common_system_header.h"

namespace cndp {
namespace common {

class Utilities final {
	/*It contains the general utitlity methods for the project.*/
	public:
	Utilities()
	{
		/*It contains the general utitlity methods for the project.*/
	}
	virtual ~Utilities()
	{
	}
	static int TOTAL_SLEEP_MICROSECONDS;

	template <typename T> static string GetStringValue(T val)
	{
		stringstream stream;
		stream << val;
		return stream.str();
	}
	static int GetLogarithmValue(int param, int base)
	{
		return ceil(((double)log2(param) / (double)log2(base)));
	}
	static int GetPower(int base, int power)
	{
		return (int)(pow(base, power));
	}
	static string GetCurrentDateAndTime()
	{

		time_t now = time(0);
		struct tm tstruct;
		char buf[80];
		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%Y/%m/%d @ %X", &tstruct);
		return buf;
	}
	static string GetFileName(string FileName)
	{
		string output_file_name;
		vector<string> vList = SplitString(".", FileName);
		int size = (int)vList.size();
		for (int i = 0; i < (size - 1); i++) {
			output_file_name += vList.at(i);
		}
		return output_file_name;
	}
	
	// trim string from both ends
	static string GetRatioString(int n, int m)
	{

		float val = (float)(m / (float)n);
		stringstream stream;
		stream << std::setprecision(2) << std::fixed << val << endl;
		string finalString = stream.str();
		// replace(finalString.begin(), finalString.end(), '.', '-');
		return Utilities::TrimString(finalString);
	}
	
	template <typename T> static string to_string(T val)
	{
		stringstream stream;
		stream << val;
		return stream.str();
	}
	
	static string &TrimString(string &s)
	{
		return Utilities::Ltrim(Utilities::Rtrim(s));
	}
	
	template <typename T> static void SortElementsInAscendingOrder(vector<T> *elements)
	{

		// the default sort operator is <
		sort(elements->begin(), elements->end());
	}
	template <typename T> static void SortElementsInDescendingOrder(vector<T> *elements)
	{

		// sort by using lambda expression
		sort(elements->begin(), elements->end(), [](T a, T b) { return a > b; });

		/*		// sort using a standard library compare function object
		 sort(elements->begin(), elements->end(), greater<T>());

		 //sort using custom function
		 sort(elements->begin(), elements->end(), compare);*/
	}

	template <typename T>
	static bool InterpolationSearchInElements(vector<T> *elements, T searching_element, int size)
	{

		int low = 0;
		int high = size - 1;
		int mid;

		while (elements->at(high) != elements->at(low) && searching_element >= elements->at(low) &&
			   searching_element <= elements->at(high)) {

			mid = low + (searching_element - elements->at(low)) *
							((high - low) / (elements->at(high) - elements->at(low)));

			if (elements->at(mid) < searching_element) {
				low = mid + 1;
			}
			else if (searching_element < elements->at(mid)) {
				high = mid - 1;
			}
			else {
				return true;
			}
		}

		if (searching_element == elements->at(low)) {
			return true;
		}
		else {
			return false;
		}
	}
	static int SortThePartOfArray(int *array, int vertex, int first_position, int last_position)
	{

		bool change = false;
		for (int i = first_position; i <= last_position; i++) {

			if (array[i] != Utilities::NULL_VERTEX) {
				continue;
			}

			for (int j = last_position; j >= i; j--) {
				if (array[j] != Utilities::NULL_VERTEX) {
					last_position = j;
					break;
				}
			}

			if (i != last_position) {
				std::swap(array[i], array[last_position]);
			}
			change = true;
		}

		if (!change) {
			cerr << "Nothing change from Utilities::sortThePartOfArray." << endl;
			exit(-1);
		}

		// now find the final postion.
		int final_position = 0;
		for (int i = last_position; i >= first_position; i--) {
			if (array[i] != Utilities::NULL_VERTEX) {
				final_position = i;
				break;
			}
		}
		return final_position;
	}

	static void SortTuples(int row_size, int column_size, int *tuples)
	{

		int total_elements = row_size * column_size;

		int *temp_tuple = new int[total_elements];
		int *rows = new int[row_size + 2];

		for (int column_index = (column_size - 1); column_index >= 0; column_index--) {

			for (int j = 0; j <= row_size; j++) {
				rows[j] = 0; // initialize
			}

			// count how many times every elements apears in that column (columnIndex).
			for (int e = 0; e < row_size; e++) {
				rows[tuples[column_size * e + column_index] + 1]++;
			}

			// cumulative value of the counts for that column (columnIndex)
			for (int j = 1; j <= row_size; j++) {
				rows[j] += rows[j - 1];
			}

			// sort the row according to the elements of that column (columnIndex)
			//  And then put int right array
			for (int row_index = 0; row_index < row_size; row_index++) {

				int pre_row_index = row_index * column_size + column_index;

				int k = rows[tuples[pre_row_index]];

				int new_row_index = k * column_size;

				for (int e = 0; e < column_size; e++) {
					temp_tuple[new_row_index + e] = tuples[column_size * row_index + e];
				}

				// increase the index for the row of that element
				rows[tuples[pre_row_index]]++;
			}

			// copy the triples to the original array
			for (int j = 0; j < total_elements; j++) {
				tuples[j] = temp_tuple[j];
			}
		}
		delete[] temp_tuple;
		delete[] rows;
	}
	static unsigned long long CalculateNchooseK(unsigned long long n, unsigned long long k)
	{
		if (k > n) {
			return 0;
		}
		else if (n <= 1) {
			return 0;
		}

		unsigned long long final_value = 1;
		for (unsigned long long d = 1; d <= k; ++d) {

			final_value *= n;
			final_value /= d;
			n = n - 1;
		}
		return final_value;
	}
	/*
	 It will return the number in range between minimum number to maximum number.
	 */
	static int GetRandomNumber(int minimum_number, int maximum_number)
	{

		if (maximum_number == minimum_number) {
			return maximum_number;
		}

		timespec seed_time;
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &seed_time);
		srand(seed_time.tv_nsec);
		int number = rand() % (maximum_number - minimum_number + 1);
		Utilities::TOTAL_SLEEP_MICROSECONDS++;
		return number;
	}

	static vector<string> SplitString(const string &delimiter, const string &input_string)
	{

		vector<string> arr;

		int str_length = input_string.length();
		int delimiter_length = delimiter.length();
		if (delimiter_length == 0) {
			return arr; // no change
		}

		int i = 0;
		int k = 0;
		while (i < str_length) {
			int j = 0;
			while (i + j < str_length && j < delimiter_length && input_string[i + j] == delimiter[j]) {
				j++;
			}
			if (j == delimiter_length) // found delimiter
			{
				arr.push_back(input_string.substr(k, i - k));
				i += delimiter_length;
				k = i;
			}
			else {
				i++;
			}
		}
		arr.push_back(input_string.substr(k, i - k));
		return arr;
	}
	
	// trim from start
	static string &Ltrim(string &s)
	{
		// s.erase(s.begin(), find_if(s.begin(), s.end(), not1(std::ptr_fun<int, int>(isspace))));
		s.erase(s.begin(), find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);}));
		return s;
	}

	// trim from end
	static string &Rtrim(string &s)
	{
		// s.erase(find_if(s.rbegin(), s.rend(), not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
		s.erase(find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);}).base(), s.end());
		return s;
	}
	

	static const int NULL_VERTEX = 0;
};
} // namespace common
} // namespace cndp
#endif /* CNDP_COMMON_UTILITIES_H_ */
