#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

struct SearchOption
{
	bool help = false;
	std::string entry = "";
	std::string dir = "";
};

static std::string toLower(std::string s) {

	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
		return std::tolower(c);
		});
	return s;
}

static bool checkFlags(std::vector<std::string>& args, SearchOption& options) {
	if (args.size() < 2) {
		std::cerr << "Error: missing required arguments.\n";
		std::cerr << "Use 'search -h' to display help.\n";
		return false;
	}

	static std::string arg;
	for (size_t i = 1; i < args.size(); i++) {
		arg = args[i];
		if (i == 1) {
			options.entry = arg;
		}
		else if (arg == "-h") {
			options.help = true;
		}
		else if (i + 1 < args.size() && arg == "-d") {
			options.dir = args[i + 1];
		}
	}
	return true;
}

static void parseString(const std::string& input, std::vector<std::string>& args) {
	std::string current;

	for (char c : input) {
		if (c == '"') {
			continue;
		}
		else if (c != ' ') {
			current += c;
		}
		else {
			args.push_back(current);
			current.clear();
		}
	}
	if (!current.empty()) {
		args.push_back(current);
		current.clear();
	}
}

static void searchFiles(const std::string& target, const std::string& directory) {
	if (!fs::exists(directory)) return;
	if (!fs::is_directory(directory)) return;

	auto start = std::chrono::steady_clock::now();
	const std::string targetLower = toLower(target);
	int i = 0;
	for (const auto& entry : fs::recursive_directory_iterator(directory, fs::directory_options::skip_permission_denied)) {

		if (!entry.is_regular_file()) continue;

		std::string fileName = toLower(entry.path().filename().string());

		if (fileName.find(targetLower) != std::string::npos) {
			std::cout << "[" << i << "]" << entry.path() << std::endl;
			i++;
		}
	}
	auto end = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "\nSearch summary:\n";
	std::cout << "  Files found: " << i << "\n";
	std::cout << "  Time elapsed: " << diff.count() << " ms\n";

}

int main() {
	std::string input;
	std::vector<std::string> args;

	std::getline(std::cin, input);

	parseString(input, args);

	if (args.empty()) return 1;
	if (args[0] == "search") {
		SearchOption opt;
		if (checkFlags(args, opt)) {
			if (opt.help || opt.entry.empty() || opt.dir.empty()) {
				std::cout <<
					"Usage:\n"
					"  search <pattern> -d <directory> [options]\n\n"
					"Arguments:\n"
					"  <pattern>            File name or part of the file name to search for\n\n"
					"Options:\n"
					"  -d <directory>       Root directory to search in (required)\n"
					"  -h                   Display this help message\n" << std::endl;
			}
			else {
				std::cout << "Search pattern: " << opt.entry << "\n";
				std::cout << "Search directory: " << opt.dir << "\n\n";
				searchFiles(opt.entry, opt.dir);
			}
		}
	}
}