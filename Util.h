/*
 * Util.h
 *
 *  Created on: Oct 20, 2016
 *      Author: duclv
 *      Description: Utilities functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>

#ifndef UTIL_H_
#define UTIL_H_

using namespace std;
namespace fs = boost::filesystem;

class Util {
public:
	// compute the number of row ids selected
	static size_t rowSelectedSize(vector<bool>* vRowId) {
		size_t count = 0;
		for (bool ele : *vRowId) {
			if (ele) ++count;
		}
		return count;
	}

	static size_t currentMilisecond() {
		return chrono::duration_cast < chrono::milliseconds
					> (chrono::steady_clock::now().time_since_epoch()).count();
	}

	static void saveToDisk(vector<string>* content, string fileName) {
		if (content != NULL) {
			std::ofstream out(fileName);
			for (string s : (*content)) {
				out << s << endl;
			}
			out.flush();
			out.close();
			delete content;
		}
	}

	static void readFromDisk(vector<string>* content, string fileName) {
		if (content != NULL) {
			ifstream infile(fileName);
			string line;
			while(getline(infile, line)) {
				content->push_back(line);
			}
			infile.close();
		}
	}

	static void createFolder(string folderPath) {
		fs::path dir(folderPath);
		fs::remove_all(dir);
		fs::create_directory(dir);
	}

	// get latest file from file pattern
	static string getLatestFile(string folderPath, string pattern) {
		string latestFileName = "";
		long maxTime = 0;
		fs::path dir(folderPath);
		if (fs::exists(dir) && fs::is_directory(dir)) {
			fs::directory_iterator end_iter;
			for ( fs::directory_iterator dir_itr( dir );
				  dir_itr != end_iter;
				  ++dir_itr ) {
				if ( fs::is_regular_file( dir_itr->status() ) ) {
					string filename = dir_itr->path().filename().string();
					cout << filename << endl;
					size_t i = filename.find(pattern);
					if (i != string::npos) {
						//found
						long time = stol(filename.substr(i+1));
						if (time > maxTime)
							latestFileName = filename;
					}
				}
			}
		}
		return latestFileName;
	}

	// get all file from a specific time
	static vector<string> getNewestFiles(string folderPath, string pattern, long aTime) {
		vector<string> newestFiles;
		fs::path dir(folderPath);
		if (fs::exists(dir) && fs::is_directory(dir)) {
			fs::directory_iterator end_iter;
			for ( fs::directory_iterator dir_itr( dir );
				  dir_itr != end_iter;
				  ++dir_itr ) {
				if ( fs::is_regular_file( dir_itr->status() ) ) {
					string filename = dir_itr->path().filename().string();
					cout << filename << endl;
					size_t i = filename.find(pattern);
					if (i != string::npos) {
						//found
						long time = stol(filename.substr(i+1));
						if (time >= aTime)
							newestFiles.push_back(filename);
					}
				}
			}
		}
		return newestFiles;
	}

	static void parseContentToVector(vector<string>* data, string content, string delim) {
		string token;
		size_t last = 0; size_t next = 0;
		while ((next = content.find(delim, last)) != string::npos) {
			token = content.substr(last, next - last);
			last = next + delim.length();
			if(token != "") data->push_back(token);
		}
		// get the last token
		token = content.substr(last);
		if(token != "") data->push_back(token);
	}

};

#endif /* UTIL_H_ */
