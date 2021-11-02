#ifndef EX5_RECOMMENDERSYSTEM_H
#define EX5_RECOMMENDERSYSTEM_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <queue>
#include <cfloat>

using std::string;
using std::vector;
using std::unordered_map;
using std::pair;


/**
 * RecommenderSystem class.
 */
class RecommenderSystem
{
public:

	/**
	 * This method will accept two strings as input that contain the path of the
	 * two input files and load the information into the recommendation system.
	 *
	 * @param moviesAttrPath: path to movies attributes file.
	 * @param userRanksPath: path to user ranks file.
	 * @return 0 if succeed, 1 otherwise.
	 */
	int loadData(const string &moviesAttrPath, const string &userRanksPath);

	/**
	 * This method will accept a string as input representing the userName and return a
	 * string representing the movie name recommended by Recommendation by Content algorithm.
	 *
	 * @param userName: a string with the userName.
	 * @return a string representing the movie name or USER_NOT_FOUND if the user dont exist.
	 */
	string recommendByContent(const string &userName) const;

	/**
	 * This method will get two strings representing the client's name and the name of the
	 * movie for which you want to predict the rating, k a positive integer (the parameter
	 * in the collaborative filtering algorithm) representing the number of movies most similar
	 * (and rated by userName) to movieName, based on prediction.
	 *
	 * @param movieName: the name of the movie for which you want to predict the rating
	 * @param userName: a user name.
	 * @param k: k a positive integer (the parameter in the collaborative filtering algorithm).
	 * @return -1 if the user or movie do not exist,the prediction otherwise.
	 */
	double
	predictMovieScoreForUser(const string &movieName, const string &userName, int k) const;

	/**
	 *This method receives as input a string that specifies the name of the customer for which
	 * it wants to make a recommendation, and k a positive integer (the parameter in the
	 * collaborative filtering algorithm) representing the number of most similar movies
	 * (and rated by userName) with which we make the prediction.
	 * @param userName: string that specifies the name of the customer for which it wants to
	 * 					make a recommendation
	 * @param k: a positive integer (the parameter in the collaborative filtering algorithm)
	 * @return the movie we recommend to userName by a collaborative filtering method.
	 */
	string recommendByCF(const string &userName, int k) const;


private:

	/**
	 * This method will accept the moviesAttributeFile as an open stream and transfer
	 * the data to the appropriate data structure.
	 *
	 * @param moAtPa: moviesAttributeFile as an open stream.
	 */
	void _loadMoviesAttrFile(std::ifstream &moAtPa);

	/**
	 * This method will accept the userRanksFile as an open stream and transfer
	 * the data to the appropriate data structure.
	 *
	 * @param usRaPa: userRanksFile as an open stream.
	 */
	void _loadUserRanksFile(std::ifstream &usRaPa);

	/**
	 * This method will accept map as input and normalize its values.
	 *
	 * @param userMap: map to normalize.
	 * @return Copy of the normalized map.
	 */
	auto _getNormOfVec(const unordered_map<string, double> &userMap) const;

	/**
	 * This method will accept a normalized userMap as input and calculate its preference vector.
	 *
	 * @param normMap: normalized userMap.
	 * @return the preference vector of the normMap.
	 */
	auto _getPrefVec(unordered_map<string, double> &normMap) const;

	/**
	 * This method will accept vector as input and calculate its norm.
	 *
	 * @param vec: vector to normalize.
	 * @return the norm of vec.
	 */
	double _vectorNorm(const vector<double> &vec) const;

	/**
	 * This method will accept two vector as input and calculate their scalar multiplication.
	 *
	 * @param first: the first vector.
	 * @param last: the second vector.
	 * @return the result of the scalar multiplication.
	 */
	double _scalarMulti(const vector<double> &first, const vector<double> &last) const;

	/**
	 * This method will accept two vectors as input, a movie feature vector, a user
	 * preference vector, and calculate the match between the movie and the user.
	 *
	 * @param prefVec: a user preference vector.
	 * @param featVec: a movie feature vector.
	 * @return the score of the match between the movie and the user.
	 */
	double _getMovieScore(const vector<double> &prefVec, const vector<double> &featVec) const;

	/**
	 * The method will accept as input, userMap, and the user's preferences vector and
	 * return the movie most suitable for that user.
	 *
	 * @param userMap: userMap.
	 * @param prefVec: the user's preferences vector.
	 * @return the movie most suitable for that user.
	 */
	string _getMaxSimilarityMov(const unordered_map<string, double> &userMap, vector<double> &prefVec)
	const;

	/**
	 * This method receives as input a movie name, a user name, and a natural number
	 * K and returns a list of K movies that the user has rated and most similar to
	 * the given movie.
	 *
	 * @param movieName: a movie name.
	 * @param userName: a user name.
	 * @param k: a natural number.
	 * @return a list of K movies that the user has rated and most similar to the given movie.
	 */
	auto _getMaxK(const string &movieName, const string &userName, int k) const ;

	/**
	 * map that holds the movie names as keys, the value of each key is the feature
	 * vector of the same movie used as the key.
	 */
	unordered_map<string, vector<double>> movies;

	/**
	 * map that holds the userNames as keys, the value of each key is a map when the
	 * key is a movie and the value is the score that the user gave to the same movie.
	 */
	unordered_map<string, unordered_map<string, double>> users;

	/**
	 * a vector with the ordered movies names.
	 */
	vector<string> moviesNames;

	/**
	 * the num of movies.
	 */
	int moviesNumber = 0;

	/**
	 * the num of attributes.
	 */
	int numOfAtr = 0;
};


#endif //EX5_RECOMMENDERSYSTEM_H
