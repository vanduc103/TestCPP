/*
 * Logging.cpp
 *
 *  Created on: Dec 9, 2016
 *      Author: duclv
 */

#include "Logging.h"
#include "Util.h"
#include "Table.h"
#include <stdio.h>

namespace std {

Logging::Logging() {
	privateLogBuffer = new vector<logging>();
}

Logging::~Logging() {
	if (privateLogBuffer != NULL) {
		delete privateLogBuffer;
	}
}

vector<Logging::logging>* Logging::publicLogBuffer = new vector<Logging::logging>();


void Logging::saveCheckpoint(Table* table) {
	// create folder log
	Util::createFolder(this->logPath);

	// write content
	string fileName = this->logPath + "/checkpoint_" + to_string(Util::currentMilisecond());
	if (table != NULL) {
		vector<string>* content = new vector<string>();
		content->push_back("Checkpoint start " + to_string(Util::currentMilisecond()));
		content->push_back(table->saveToDisk(this->logPath));
		content->push_back("Checkpoint end " + to_string(Util::currentMilisecond()));
		Util::saveToDisk(content, fileName);
	}
}

void Logging::redoLogUpdate(size_t txIdx, LOG_TX_ACTION txAction){
	logging newLog;
	newLog.txIdx = txIdx;
	newLog.txAction = txAction;
	privateLogBuffer->push_back(newLog);
}

void Logging::redoLogAdd(size_t txIdx, LOG_OBJECT objType, vector<string> logContent) {
	logging newLog;
	newLog.txIdx = txIdx;
	newLog.objType = objType;
	newLog.logContent = logContent;
	privateLogBuffer->push_back(newLog);
}

void Logging::redoLogPublicMerge() {
	// save this private log buffer to public buffer
	publicLogBuffer->insert(publicLogBuffer->end(), this->privateLogBuffer->begin(), this->privateLogBuffer->end());
}

void Logging::redoLogSave() {
	cout << "start log save" << endl;
	string logFileName = this->logPath + "/redo_log_" + to_string(Util::currentMilisecond());
	vector<string>* content = new vector<string>();
	for (size_t i = 0; i < publicLogBuffer->size(); i++) {
		logging log = publicLogBuffer->at(i);
		switch (log.txAction) {
			case TX_START: {
				content->push_back(to_string(log.txIdx) + "|start");
				break;
			}
			case TX_COMMIT: {
				content->push_back(to_string(log.txIdx) + "|commit");
				break;
			}
			case TX_END: {
				content->push_back(to_string(log.txIdx) + "|end");
				break;
			}
		}
		string objValue = "";
		switch (log.objType) {
			case INSERT:
				objValue = to_string(log.txIdx) + "|insert|";
				break;
			case DELTA_SPACE:
				objValue = to_string(log.txIdx) + "|delta_space|";
				break;
			case VERSION_VECVALUE:
				objValue = to_string(log.txIdx) + "|version_vec_value|";
				break;
			case HASHTABLE:
				objValue = to_string(log.txIdx) + "|hashtable|";
				break;
			case VERSION_COLUMN:
				objValue = to_string(log.txIdx) + "|version_column|";
				break;
		}
		if (log.logContent.size() > 0) {
			string tmpValue = "";
			for (size_t i = 0; i < log.logContent.size(); i++) {
				string value = log.logContent.at(i);
				tmpValue += value + "|";
			}
			objValue += tmpValue;
			content->push_back(objValue);
		}
	}
	if (publicLogBuffer->size() > 0) {
		// save to disk
		Util::saveToDisk(content, logFileName);
		publicLogBuffer->resize(0);
	}
	cout << "end log save" << endl;
}

} /* namespace std */
