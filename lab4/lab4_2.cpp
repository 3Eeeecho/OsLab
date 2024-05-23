#include <algorithm>
#include <iostream>
#include <random>
#include <ranges>
#include <vector>

static auto generateRequests(int num_requests, int disk_size) -> std::vector<int>
{
    std::vector<int> requests;
    std::mt19937 generator(time(nullptr)); // Seed the random number generator
    std::uniform_int_distribution<int> distribution(0, disk_size - 1);

    requests.reserve(num_requests);
    for (int i = 0; i < num_requests; i++) {
        requests.push_back(distribution(generator));
    }

    return requests;
}

static void scanAlgorithm(const std::vector<int>& requests, int head, int disk_size)
{
    std::vector<int> left;
    std::vector<int> right;
    std::vector<int> seek_sequence;
    int seek_count = 0;

    // 左右序列初始化
    for (int request : requests) {
        if (request < head) {
            left.push_back(request);
        }
        if (request > head) {
            right.push_back(request);
        }
    }

    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    // 寻道右边序列
    for (int request : right) {
        seek_sequence.push_back(request);
        seek_count += std::abs(request - head);
        head = request;
    }

    // 寻道左边序列
    for (int& request : std::ranges::reverse_view(left)) {
        seek_sequence.push_back(request);
        seek_count += std::abs(request - head);
        head = request;
    }
    std::cout << "SCAN算法:" << std::endl;
    std::cout << "寻道顺序:";
    for (int i = 0; i < seek_sequence.size(); i++) {
        std::cout << seek_sequence[i];
        if (i != seek_sequence.size() - 1) {
            std::cout << " -> ";
        }
    }
    std::cout << std::endl;
    std::cout << "平均寻道距离： " << static_cast<double>(seek_count) / seek_sequence.size() << std::endl;
}

static void cScanAlgorithm(const std::vector<int>& requests, int head, int disk_size)
{
    std::vector<int> left;
    std::vector<int> right;
    std::vector<int> seek_sequence;
    int seek_count = 0;

    // 左右序列初始化
    for (int request : requests) {
        if (request < head) {
            left.push_back(request);
        }
        if (request > head) {
            right.push_back(request);
        }
    }

    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    // 寻道右边序列
    for (int request : right) {
        seek_sequence.push_back(request);
        seek_count += std::abs(request - head);
        head = request;
    }

    // 寻道左边序列
    for (int request : left) {
        seek_sequence.push_back(request);
        seek_count += std::abs(request - head);
        head = request;
    }
    std::cout << "CSCAN算法:" << std::endl;
    std::cout << "寻道顺序:";
    for (int i = 0; i < seek_sequence.size(); i++) {
        std::cout << seek_sequence[i];
        if (i != seek_sequence.size() - 1) {
            std::cout << " -> ";
        }
    }
    std::cout << std::endl;
    std::cout << "平均寻道距离： " << static_cast<double>(seek_count) / seek_sequence.size() << std::endl;
}

auto main() -> int
{
    constexpr int numRequests = 10;
    constexpr int head = 90;
    constexpr int disk_size = 200;
    std::vector<int> requests = std::move(generateRequests(numRequests, disk_size));

    scanAlgorithm(requests, head, disk_size);
    std::cout << std::endl;
    cScanAlgorithm(requests, head, disk_size);
    return 0;
}