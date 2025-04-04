#pragma once
#include <vector>
#include <format>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <string>
#include "DataStructures.h"
#include "Common.h"

namespace {
	struct Event {
		std::mutex m;
		std::condition_variable cv;
		bool ready = false;
	};
}

std::vector<std::vector<TableTree>> CKY(const std::string& tokens, const std::unordered_map<std::string, std::string>& rev_prods) {
	int n = tokens.size();
	std::vector<std::vector<TableTree>> table(n * n);

	for (int col = 0; col < n; col++) {
		std::string token(1, tokens[col]);
		const std::string& parent_label = rev_prods.at(token);
		table[indexFlat(col, col, n)].emplace_back(
			parent_label, std::vector<TableTree>{TableTree(token)});
	
		for (int row = col - 1; row > -1; row--) {
			for (int bot = row + 1; bot < col + 1; bot++) {
				std::vector<TableTree>& leftCell = table[indexFlat(row, bot - 1, n)];
				std::vector<TableTree>& botCell = table[indexFlat(bot, col, n)];

				for (int i = 0; i < leftCell.size(); i++) {
					for (int j = 0; j < botCell.size(); j++) {
						TableTree& leftSubtree = leftCell[i];
						TableTree& rightSubtree = botCell[j];
						auto it = rev_prods.find(leftSubtree.label + " " + rightSubtree.label);
						
						if (it != rev_prods.end()) {
							const std::string& parentLabel = it->second;
							table[indexFlat(row, col, n)].emplace_back(
								parentLabel, std::vector<TableTree>{leftSubtree, rightSubtree}
							);
						}
					}
				}
			}
		}
	}
	return std::move(table);
}
static void CKYParallelWorker(const std::string& tokens,
	const std::unordered_map<std::string, std::string>& rev_prods, 
	std::vector<std::vector<TableTree>>& table, int id, int numThreads, std::vector<Event>& events) {

	int n = tokens.size();

	for (int col = id; col < n; col += numThreads) {
		{
			std::string token(1, tokens[col]);
			const std::string& parent_label = rev_prods.at(token);
			int i = indexFlat(col, col, n);
			table[i].emplace_back(
				parent_label, std::vector<TableTree>{TableTree(token)});
			{
				Event& e = events[i];
				std::unique_lock<std::mutex> lock(e.m);
				e.ready = true;
				e.cv.notify_all();
			}
		}
		for (int row = col - 1; row > -1; row--) {
			int this_i = indexFlat(row, col, n);
			Event& thisEvent = events[this_i];
			thisEvent.m.lock();

			for (int bot = row + 1; bot < col + 1; bot++) {
				int left_i = indexFlat(row, bot - 1, n);
				int bot_i = indexFlat(bot, col, n);
				{
					Event& leftEvent = events[left_i];
					std::unique_lock<std::mutex> lock(leftEvent.m);
					leftEvent.cv.wait(lock, [&]() { return leftEvent.ready; });
				}
				std::vector<TableTree>& leftCell = table[left_i];
				std::vector<TableTree>& botCell = table[bot_i];
				std::vector<TableTree>& thisCell = table[this_i];

				for (int i = 0; i < leftCell.size(); i++) {
					for (int j = 0; j < botCell.size(); j++) {
						TableTree& leftSubtree = leftCell[i];
						TableTree& rightSubtree = botCell[j];
						auto it = rev_prods.find(leftSubtree.label + " " + rightSubtree.label);

						if (it != rev_prods.end()) {
							const std::string& parentLabel = it->second;
							thisCell.emplace_back(
								parentLabel, std::vector<TableTree>{leftSubtree, rightSubtree}
							);
						}
					}
				}
			}
			thisEvent.ready = true;
			thisEvent.cv.notify_all();
			thisEvent.m.unlock();
		}
	}
}
std::vector<std::vector<TableTree>> CKYParallel(const std::string& tokens,
	const std::unordered_map<std::string, std::string>& rev_prods, int numThreads = 0) {

	if (numThreads <= 0) numThreads = std::thread::hardware_concurrency();
	
	int n = tokens.size();
	std::vector<std::vector<TableTree>> table(n * n);

	std::vector<Event> events(n * n);

	std::vector<std::thread> threads = std::vector<std::thread>();
	threads.reserve(numThreads);
	
	for (int i = 0; i < numThreads; i++) 
		threads.emplace_back(CKYParallelWorker, cref(tokens), cref(rev_prods), ref(table), i, numThreads, ref(events));

	for (std::thread& t : threads) t.join();

	return std::move(table);
}
