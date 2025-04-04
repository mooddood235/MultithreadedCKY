#include <iostream>
#include <chrono>
#include <fstream>
#include "DataStructures.h"
#include "CKY.h"
#include "Common.h"

std::vector<std::pair<int, float>> TestCKYAmbs(int exprLength, int maxAmbs);
std::vector<std::pair<int, float>> TestCKYParallelAmbs(int exprLength, int maxAmbs, int numThreads = 0);
std::vector<std::pair<int, float>> TestCKYSimple(int maxLength, bool bracket, int split);
std::vector<std::pair<int, float>> TestCKYParallelSimple(int maxLength, bool bracket, int split, int numThreads = 0);

void WriteTest(const std::string path, const std::vector<std::pair<int, float>>& res);

int main() {	
	std::cout << std::thread::hardware_concurrency() << std::endl;
	auto CKYSimpleBracketRes = TestCKYSimple(300, true, 0);
	auto CKYParallelSimpleBracketRes = TestCKYParallelSimple(300, true, 0);

	auto CKYSimpleRes = TestCKYSimple(14, false, 1);
	auto CKYParallelSimpleRes = TestCKYParallelSimple(14, false, 1);
	
	auto CKYAmbsRes = TestCKYAmbs(11, 0);
	auto CKYParallelAmbsRes = TestCKYParallelAmbs(11, 0);
	
	WriteTest("SingleSimpleBracket", CKYSimpleBracketRes);
	WriteTest("MultiSimpleBracket", CKYParallelSimpleBracketRes);

	WriteTest("SingleSimple", CKYSimpleRes);
	WriteTest("MultiSimple", CKYParallelSimpleRes);

	WriteTest("SingleAmbs", CKYAmbsRes);
	WriteTest("MultiAmbs", CKYParallelAmbsRes);

	return 0;
}

std::vector<std::pair<int, float>> TestCKYAmbs(int exprLength, int maxAmbs) {
	if (maxAmbs <= 0) maxAmbs = CatalanNumber(exprLength);

	std::vector<std::pair<int, float>> res = std::vector<std::pair<int, float>>();
	res.reserve(exprLength);

	std::string expr(exprLength, 'a');

	for (int i = 1; i < exprLength; i++) {
		int ambCount = CatalanNumber(i);
		std::cout << "TestCKYAmbs: " << i << " " << ambCount << " " << maxAmbs << std::endl;

		std::vector<std::string> ambs = nAmbs(expr, ambCount, 10);
		if (ambs.size() <= 0) continue;
		float avg = 0;

		for (const std::string& amb : ambs) {
			auto start = std::chrono::high_resolution_clock::now();
			CKY(amb, rev_prods);
			auto end = std::chrono::high_resolution_clock::now();

			float time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			avg += time;
		}
		avg /= ambs.size();
		res.emplace_back(ambCount, avg);
	}
	return std::move(res);
}
std::vector<std::pair<int, float>> TestCKYParallelAmbs(int exprLength, int maxAmbs, int numThreads) {
	if (maxAmbs <= 0) maxAmbs = CatalanNumber(exprLength);

	std::vector<std::pair<int, float>> res = std::vector<std::pair<int, float>>();
	res.reserve(exprLength);

	std::string expr(exprLength, 'a');

	for (int i = 1; i < exprLength; i++) {
		int ambCount = CatalanNumber(i);
		std::cout << "TestCKYParallelAmbs: " << i << " " << ambCount << " " << maxAmbs << std::endl;

		std::vector<std::string> ambs = nAmbs(expr, ambCount, 10);
		if (ambs.size() <= 0) continue;

		float avg = 0;

		for (const std::string& amb : ambs) {
			auto start = std::chrono::high_resolution_clock::now();
			CKYParallel(amb, rev_prods, numThreads);
			auto end = std::chrono::high_resolution_clock::now();

			float time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			avg += time;
		}
		avg /= ambs.size();
		res.emplace_back(ambCount, avg);
	}
	return std::move(res);
}
std::vector<std::pair<int, float>> TestCKYSimple(int maxLength, bool bracket, int split) {
	std::vector<std::pair<int, float>> res = std::vector<std::pair<int, float>>();
	res.reserve(maxLength);

	for (int len = 1; len <= maxLength; len++) {
		std::cout << "TestCKYSimple: " << len << std::endl;
		std::string expr = bracket ? oneAmb(len) : std::string(len, 'a');

		auto start = std::chrono::high_resolution_clock::now();
		CKY(expr, rev_prods);
		auto end = std::chrono::high_resolution_clock::now();

		float time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		res.emplace_back(len, time);
	}
	return std::move(res);
}
std::vector<std::pair<int, float>> TestCKYParallelSimple(int maxLength, bool bracket, int split, int numThreads) {
	std::vector<std::pair<int, float>> res = std::vector<std::pair<int, float>>();
	res.reserve(maxLength);

	for (int len = 1; len <= maxLength; len++) {
		std::cout << "TestCKYParallelSimple: " << len << std::endl;
		std::string expr = bracket ? oneAmb(len) : std::string(len, 'a');

		auto start = std::chrono::high_resolution_clock::now();
		CKYParallel(expr, rev_prods, numThreads);
		auto end = std::chrono::high_resolution_clock::now();

		float time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		res.emplace_back(len, time);
	}
	return std::move(res);
}


void WriteTest(const std::string path, const std::vector<std::pair<int, float>>& res) {
	std::ofstream file(path);

	if (!file.is_open()) {
		std::cerr << "Error opening file at path: " << path << std::endl;
		return;
	}
	for (const auto& p : res) {
		file << p.first << " " << p.second / 1000000 << std::endl;
	}
	file.close();
}