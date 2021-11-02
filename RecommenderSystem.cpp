#include "RecommenderSystem.h"

#define USER_NOT_FOUND "USER NOT FOUND"
#define CANT_OPEN_FILE "Unable to open file "
#define BAD_EXIT -1


/**
 * This method will accept the moviesAttributeFile as an open stream and transfer
 * the data to the appropriate data structure.
 */
void RecommenderSystem::_loadMoviesAttrFile(std::ifstream &moAtPa)
{
	string line;
	while (std::getline(moAtPa, line))
	{
		std::istringstream iss(line);
		string key;
		iss >> key;
		int val;
		while (iss >> val)
		{
			movies[key].push_back(val);
		}
		moviesNumber = movies.size();
		numOfAtr = movies.begin()->second.size();
	}
}

/**
 * This method will accept the userRanksFile as an open stream and transfer
 * the data to the appropriate data structure.
 */
void RecommenderSystem::_loadUserRanksFile(std::ifstream &usRaPa)
{
	string line, firstLineArg;
	std::getline(usRaPa, line);
	std::istringstream iss(line);
	while (iss >> firstLineArg)
	{
		moviesNames.push_back(firstLineArg);
	}
	while (std::getline(usRaPa, line))
	{
		std::istringstream iss2(line);
		string key, val;
		iss2 >> key;
		for (auto &movie : moviesNames)
		{
			iss2 >> val;
			if (val == "NA")
			{
				users[key][movie] = 0;
			}
			else
			{
				users[key][movie] = stod(val);
			}
		}
	}
}

/**
 * This method will accept two strings as input that contain the path of the
 * two input files and load the information into the recommendation system.
 */
int RecommenderSystem::loadData(const string &moviesAttrPath, const string &userRanksPath)
{
	std::ifstream moAtPa, usRaPa;
	moAtPa.open(moviesAttrPath);
	if (!moAtPa.is_open())
	{
		std::cerr << CANT_OPEN_FILE << moviesAttrPath;
		return BAD_EXIT;
	}
	usRaPa.open(userRanksPath);
	if (!usRaPa.is_open())
	{
		moAtPa.close();
		std::cerr << CANT_OPEN_FILE << userRanksPath;
		return BAD_EXIT;
	}
	else
	{
		_loadMoviesAttrFile(moAtPa);
		_loadUserRanksFile(usRaPa);
		moAtPa.close();
		usRaPa.close();
		return EXIT_SUCCESS;
	}
}

/**
 * This method will accept map as input and normalize its values.
 */
auto RecommenderSystem::_getNormOfVec(const unordered_map<std::string, double> &userMap) const
{
	double numOfRanks = 0;
	double totalSum = 0;
	for (auto &movie:userMap)
	{
		if (movie.second != 0)
		{
			numOfRanks++;
			totalSum += (double) movie.second;
		}
	}
	vector<double> normVec(numOfAtr, 0);
	auto normMap(userMap);
	totalSum /= numOfRanks;
	for (auto &movie:normMap)
	{
		if (movie.second != 0)
		{
			movie.second -= totalSum;
		}
	}
	return normMap;
}

/**
 * This method will accept a normalized userMap as input and calculate its preference vector.
 */
auto RecommenderSystem::_getPrefVec(unordered_map<string, double> &normMap) const
{
	vector<double> vec(numOfAtr, 0);
	for (auto &movie: movies)
	{
		double a = normMap[movie.first];
		if (a != 0)
		{
			for (int i = 0; i < numOfAtr; i++)
			{
				vec[i] += (a * movie.second[i]);
			}
		}


	}
	return vec;
}

/**
 * This method will accept vector as input and calculate its norm.
 */
double RecommenderSystem::_vectorNorm(const vector<double> &vec) const
{
	return std::sqrt(_scalarMulti(vec, vec));

}

/**
 * This method will accept two vectors as input and calculate their scalar multiplication.
 */
double
RecommenderSystem::_scalarMulti(const vector<double> &first, const vector<double> &last) const
{
	double result = 0;
	for (int i = 0; i < numOfAtr; ++i)
	{
		result += (first[i] * last[i]);
	}
	return result;
}

/**
 *This method will accept two vectors as input, a movie feature vector, a user
 * preference vector, and calculate the match between the movie and the user.
 */
double
RecommenderSystem::_getMovieScore(const vector<double> &prefVec, const vector<double> &featVec)
const
{
	return _scalarMulti(prefVec, featVec) / (_vectorNorm(prefVec) * _vectorNorm(featVec));
}

/**
 * The method will accept as input, userMap, and the user's preferences vector and
 * return the movie most suitable for that user.
 */
string
RecommenderSystem::_getMaxSimilarityMov(const unordered_map<string, double> &userMap,
										vector<double> &prefVec) const
{
	string name;
	double bestScore = -DBL_MAX;
	for (auto &movie: moviesNames)
	{
		if (userMap.at(movie) == 0)
		{
			double score = _getMovieScore(prefVec, movies.at(movie));
			if (score > bestScore)
			{
				bestScore = score;
				name = movie;
			}
		}
	}
	return name;
}

/**
 * This method will accept a string as input representing the userName and return a
 * string representing the movie name recommended by Recommendation by Content algorithm.
 */
string RecommenderSystem::recommendByContent(const string &userName) const
{
	auto userMap = users.find(userName);
	if (userMap == users.end())
	{
		return USER_NOT_FOUND;
	}
	auto normMap = _getNormOfVec(users.at(userName));

	auto prefVec = _getPrefVec(normMap);

	return _getMaxSimilarityMov(users.at(userName), prefVec);
}

/**
 * This method receives as input a movie name, a user name, and a natural number
 * K and returns a list of K movies that the user has rated and most similar to
 * the given movie.
 */
auto RecommenderSystem::_getMaxK(const string &movieName, const string &userName, int k) const
{
	auto cmp = [](pair<string, double> &first, pair<string, double> &second) {
		return first.second > second.second;
	                                                                         };

	std::priority_queue<pair<string, double>, vector<pair<string, double>>, decltype(cmp)> outMovies(
			cmp);

	for (auto &movie: movies)
	{
		if (users.at(userName).at(movie.first) != 0)
		{
			double movieRank = _getMovieScore(movie.second, movies.at(movieName));
			pair<std::string, double> a{movie.first, movieRank};

			if ((int) outMovies.size() < k)
			{
				outMovies.push(a);
			}
			else if (movieRank > outMovies.top().second)
			{
				outMovies.pop();
				outMovies.push(a);
			}
		}
	}
	double upSum = 0, doSum = 0;
	while (!outMovies.empty())
	{
		doSum += outMovies.top().second;
		upSum += outMovies.top().second * users.at(userName).at(outMovies.top().first);
		outMovies.pop();
	}
	return upSum / doSum;
}

/**
 * This method will get two strings representing the client's name and the name of the
 * movie for which you want to predict the rating, k a positive integer (the parameter
 * in the collaborative filtering algorithm) representing the number of movies most similar
 * (and rated by userName) to movieName, based on prediction.
 */
double RecommenderSystem::predictMovieScoreForUser(const string &movieName, const string &userName,
												   int k) const
{
	auto userMap = users.find(userName);
	if (userMap == users.end())
	{
		return BAD_EXIT;
	}
	auto movieMap = movies.find(movieName);
	if (movieMap == movies.end())
	{
		return BAD_EXIT;
	}
	auto movieDeq = _getMaxK(movieName, userName, k);
	return movieDeq;

}

/**
 *This method receives as input a string that specifies the name of the customer for which
 * it wants to make a recommendation, and k a positive integer (the parameter in the
 * collaborative filtering algorithm) representing the number of most similar movies
 * (and rated by userName) with which we make the prediction.
 */
string RecommenderSystem::recommendByCF(const string &userName, int k) const
{
	auto userMap = users.find(userName);
	if (userMap == users.end())
	{
		return USER_NOT_FOUND;
	}
	string recMovie;
	auto recMovieScore = DBL_MIN;
	for (auto &movie: moviesNames)
	{
		if (users.at(userName).at(movie) == 0)
		{
			double movieScore = predictMovieScoreForUser(movie, userName, k);
			if (movieScore > recMovieScore)
			{
				recMovie = movie;
				recMovieScore = movieScore;
			}
		}
	}
	return recMovie;
}