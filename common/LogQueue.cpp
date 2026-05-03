#pragma once

#include <fstream> 
#include <string>
#include <queue>
#include <iostream>

class LogQueue {
	public:
		void log(const std::string& tag, const std::string& text) {
			std::string value = "[" + tag + "] " + text + "\n";
			std::cout << value;
			mainQueue.push(value);
		}

		void generateLogFile(const std::string& sessionId) {
			if (mainQueue.empty()) return;

			std::string filename = "log_" + sessionId + ".txt";
			std::ofstream file(filename);

			if (!file.is_open()) {
				throw std::runtime_error("Nao foi possivel salvar o resultado em: " + filename);
			}

			do {
				file << mainQueue.front();
				mainQueue.pop();
			} while ((!mainQueue.empty()));

			file.close();

			std::cout << "\nResultado salvo em " << filename << std::endl;
		}

		void clearLog() { mainQueue = {}; }

	private:
		std::queue<std::string> mainQueue = {};
};