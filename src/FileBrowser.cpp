#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

struct SearchOption
{
	bool help = false;
	std::string entry;
	std::string dir;
	int maxDepth = -1;
	int maxResult = -1;
};

static std::string toLower(std::string s) {
	for (char& c : s) {
		if (c >= 'A' && c <= 'Z') {
			c = c - 'A' + 'a';
		}
	}
	return s;
}

static std::string toLower(std::u8string u8s) {
	std::string s(reinterpret_cast<const char*>(u8s.data()), u8s.length());
	for (char8_t& c : u8s) {
		if (c >= 'A' && c <= 'Z') {
			c = c - 'A' + 'a';
		}
	}
	return s;
}

static bool checkFlags(std::vector<std::string>& args, SearchOption& options) {
	if (args.size() < 2) {
		std::cerr << "Error: missing required arguments.\n";
		std::cerr << "Use 'search -h' to display help.\n";
		return false;
	}

	std::string arg;
	for (size_t i = 1; i < args.size(); i++) {
		arg = args[i];
		if (i == 1) {
			options.entry = arg;
		}
		else if (arg == "-h") {
			options.help = true;
		}
		else if (i + 1 < args.size() && arg == "-d") {
			options.dir = args[++i];
		}
		else if (i + 1 < args.size() && arg == "--max-results") {
			try {
				options.maxResult = std::stoi(args[++i]);
			}
			catch (const std::exception& e) {
				std::cerr << "Error: Invalid number for --max-results: " << e.what() << std::endl;
			}
		}
		else if (i + 1 < args.size() && arg == "--max-depth") {
			try {
				options.maxDepth = std::stoi(args[++i]);
			}
			catch (const std::exception& e) {
				std::cerr << "Error: Invalid number for --max-depth: " << e.what() << std::endl;
			}
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

static void searchFiles(const SearchOption& opt) {
	if (!fs::exists(opt.dir)) return;
	if (!fs::is_directory(opt.dir)) return;

	auto start = std::chrono::steady_clock::now();
	const std::string targetLower = toLower(opt.entry);
	std::string fileName;
	std::string pathStr;

	std::error_code ec;
	fs::recursive_directory_iterator it(opt.dir, fs::directory_options::skip_permission_denied);
	int resoults = 0;

	for (const auto& entry : it) {
		if (opt.maxDepth > 0 && it.depth() > opt.maxDepth) { it.disable_recursion_pending(); continue; }
		if (opt.maxResult > 0 && opt.maxResult <= resoults) { break; }

		auto status = entry.status(ec);
		if (ec || status.type() != fs::file_type::regular) continue;

		fileName = toLower(entry.path().filename().u8string());

		if (fileName.find(targetLower) != std::string::npos) {
			auto p = entry.path().u8string();
			pathStr = reinterpret_cast<const char*>(p.data());
			std::cout << "[" << resoults << "]" << pathStr << std::endl;

			resoults++;
		}
	}
	auto end = std::chrono::steady_clock::now();
	auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "\nSearch summary:\n";
	std::cout << "  Files found: " << resoults << "\n";
	std::cout << "  Time elapsed: ";

	if (total_ms.count() < 1000) {
		std::cout << total_ms << " ms\n";
	}
	else {
		long long ms = total_ms.count() % 1000;
		long long total_sec = total_ms.count() / 1000;
		long long sec = total_sec % 60;
		long long total_min = total_sec / 60;
		long long min = total_min % 60;
		long long hours = total_min / 60;

		if (hours > 0) std::cout << hours << "h ";
		if (min > 0 || hours > 0) std::cout << min << "m ";
		std::cout << sec << "s " << ms << "ms\n";
	}

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
					"  --max-results <n>    Maximum number of files to return\n"
					"  --max-depth <n>      Maximum recursion depth\n"
					"  -h, --help           Display this help message\n" << std::endl;
			}
			else {
				searchFiles(opt);
			}
		}
	}
}