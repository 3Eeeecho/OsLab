#include <algorithm>
#include <format>
#include <iostream>
#include <list>
#include <queue>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class PageReplacementSimulator {
public:
    PageReplacementSimulator(int num_pages, const std::vector<int>& sequence)
        : num_pages(num_pages)
        , sequence(sequence)
    {
    }
    // 计算LRU算法的命中率
    auto simulateLRU(int frame_size) -> double;

    // 计算FIFO算法的命中率
    auto simulateFIFO(int frame_size) -> double;

    // 计算OPT算法的命中率
    auto simulateOPT(int frame_size) -> double;

    // 计算CLOCK算法的命中率
    auto simulateCLOCK(int frame_size) -> double;

private:
    int num_pages; // 虚拟页面总数
    std::vector<int> sequence; // 指令序列对应的虚拟页面号序列

    // OPT辅助算法
    auto preProcessOPT();
};

// 模拟指令序列的生成
auto generateSequence(int num_instructions, int pages_per_k) -> std::vector<int> // NOLINT
{
    std::vector<int> sequence;
    int current = 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, num_instructions - 1);
    for (int i = 0; i < num_instructions; ++i) {
        sequence.push_back(current / pages_per_k);
        int choice = dis(gen);
        if (choice < num_instructions * 0.5) { // 50%是顺序 NOLINT
            current++;
        } else if (choice < num_instructions * 0.75) { // 25%是跳转到前面 NOLINT
            current = dis(gen) % (current + 1);
        } else { // 25%是跳转到后面
            current = current + 1 + dis(gen) % (num_instructions - current);
        }
        current %= num_instructions; // 确保不超出指令数范围
    }
    return sequence;
}

auto main() -> int
{
    constexpr int num_instructions = 320; // 总共320条指令
    constexpr int pages_per_k = 10; // 每页10条指令
    constexpr int num_pages = 32; // 32页虚存

    std::vector<int> sequence = generateSequence(num_instructions, pages_per_k);
    PageReplacementSimulator simulator(num_pages, sequence);
    // 测试不同的内存容量对命中率的影响
    std::cout << std::format("| {:^20}| {:^16}| {:^16}| {:^16}| {:^16}|\n", "Memory Size (Pages)", "LRU Hit Rate", "FIFO Hit Rate", "OPT Hit Rate", "CLOCK Hit Rate");
    for (int frame_size = 2; frame_size <= num_pages; frame_size++) {
        auto lru_hit_rate = simulator.simulateLRU(frame_size);
        auto fifo_hit_rate = simulator.simulateFIFO(frame_size);
        auto opt_hit_rate = simulator.simulateOPT(frame_size);
        auto clock_hit_rate = simulator.simulateCLOCK(frame_size);
        std::cout << std::format("| {:^20}| {:^16.6f}| {:^16.6f}| {:^16.6f}| {:^16.6f}|\n", frame_size, lru_hit_rate, fifo_hit_rate, opt_hit_rate, clock_hit_rate);
    }
    return 0;
}

auto PageReplacementSimulator::simulateLRU(int frame_size) -> double
{
    std::list<int> lru_list; // LRU链表
    std::unordered_map<int, std::list<int>::iterator> lru_map; // 页面在LRU链表中的位置
    int hits = 0; // 命中次数
    int accesses = 0; // 访问次数

    for (const int currentPage : sequence) {
        accesses++;
        // 如果命中,把命中页放到链表头部(表示最近使用过)
        if (lru_map.find(currentPage) != lru_map.end()) {
            lru_list.erase(lru_map[currentPage]);
            hits++;
        }
        // 如果未命中,把链表尾部页面替换掉
        else {
            if (lru_list.size() == frame_size) {
                int last_page = lru_list.back();
                lru_list.pop_back();
                lru_map.erase(last_page);
            }
        }
        lru_list.push_front(currentPage);
        lru_map[currentPage] = lru_list.begin();
    }

    return static_cast<double>(hits) / accesses;
}

auto PageReplacementSimulator::simulateFIFO(int frame_size) -> double
{
    std::queue<int> fifo_queue; // FIFO队列
    std::unordered_set<int> pageInMemory; // 存储内存中的页面
    int hits = 0;
    int accesses = 0;

    for (const int currentPage : sequence) {
        accesses++;
        if (pageInMemory.find(currentPage) != pageInMemory.end()) {
            hits++; // 页面命中
        } else {
            if (fifo_queue.size() == frame_size) {
                int oldest_page = fifo_queue.front();
                fifo_queue.pop();
                pageInMemory.erase(oldest_page);
            }
            fifo_queue.push(currentPage);
            pageInMemory.insert(currentPage);
        }
    }

    return static_cast<double>(hits) / accesses;
}

auto PageReplacementSimulator::preProcessOPT()
{
    // 每个页面未来出现的位置次序存储下来
    std::unordered_map<int, std::queue<int>> future_access;
    for (int i = 0; i < sequence.size(); ++i) {
        future_access[sequence[i]].push(i);
    }
    return future_access;
}

auto PageReplacementSimulator::simulateOPT(int frame_size) -> double
{
    auto future_access = preProcessOPT();
    std::unordered_set<int> pageInMemory;
    int accesses = 0;
    int hits = 0;
    for (int currentPage : sequence) {
        accesses++;
        // 页面击中
        if (pageInMemory.find(currentPage) != pageInMemory.end()) {
            hits++;
        }
        // 页面未击中
        else {
            // 找到替换页面
            if (pageInMemory.size() >= frame_size) {
                int farthest = -1;
                int removePage = -1;
                for (int page : pageInMemory) {
                    // 选择未来没有出现过的页面替换
                    if (future_access[page].empty()) {
                        removePage = page;
                        break;
                    }
                    // 选择最晚使用的页面进行替换
                    if (future_access[page].front() > farthest) {
                        farthest = future_access[page].front();
                        removePage = page;
                    }
                }
                pageInMemory.erase(removePage);
            }
            pageInMemory.insert(currentPage);
        }
        future_access[currentPage].pop();
    }
    return static_cast<double>(hits) / accesses;
}

struct clockPage {
    int page { -1 };
    bool useBit { false }; // 访问位
};

auto PageReplacementSimulator::simulateCLOCK(int frame_size) -> double
{
    std::vector<clockPage> clockVec(frame_size);
    std::unordered_set<int> pageInMemory;
    int hits = 0;
    int accesses = 0;
    int clockPtr = 0; // 时钟指针

    for (int currentPage : sequence) {
        accesses++;
        // 页面击中
        if (pageInMemory.find(currentPage) != pageInMemory.end()) {
            hits++;
            auto hitPage = std::find_if(clockVec.begin(), clockVec.end(),
                [&](const clockPage& page) { return page.page == currentPage; });
            if (hitPage != clockVec.end()) {
                hitPage->useBit = true;
            }
        }
        // 页面未击中
        else {
            while (!clockVec.empty()) {
                // 直到找到usebit为0的情况
                if (!clockVec[clockPtr].useBit) {
                    pageInMemory.erase(clockVec[clockPtr].page);
                    clockVec[clockPtr].page = currentPage;
                    clockVec[clockPtr].useBit = true;
                    pageInMemory.insert(clockVec[clockPtr].page);
                    clockPtr = (clockPtr + 1) % frame_size; // 实现循环
                    break;
                }
                clockVec[clockPtr].useBit = false;
                clockPtr = (clockPtr + 1) % frame_size;
            }
        }
    }
    return static_cast<double>(hits) / accesses;
}
