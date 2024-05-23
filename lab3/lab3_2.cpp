#include <iostream>
#include <queue>
#include <random>
#include <unordered_set>
#include <vector>

class PageReplacement {
public:
    PageReplacement() { initialInstructions(); }
    ~PageReplacement() = default;

    void FIFO(); // FIFO算法
    void LRU(); // LRU算法
    void print() const;

private:
    auto initialInstructions() -> void; // 初始化指令集

    std::queue<int> m_pageQueue; // 页面访问顺序队列
    std::vector<int> m_instructions; // 指令集
    std::unordered_set<int> m_pageSet; // 页表
    int m_missPageCounts { 0 }; // 缺页次数

    const size_t instructionNumber { 320 };
    const size_t physicalMemory { 31 };
};

auto main() -> int
{
    PageReplacement p1;
    p1.LRU();
    std::cout << "LRU页命中率: ";
    p1.print();
    /*p1.FIFO();
    cout << "FIFO页命中率: ";
    p1.print();*/
    return 0;
}

void PageReplacement::FIFO()
{
    m_missPageCounts = 0;
    for (int i = 0; i < instructionNumber; ++i) {
        // 没有页命中
        if (m_pageSet.find(m_instructions[i]) == m_pageSet.end()) {
            // 内存满了需要换出
            if (m_pageSet.size() == physicalMemory) {
                m_pageSet.erase(m_pageSet.begin());
            }
            m_pageSet.insert(m_instructions[i]);
            m_missPageCounts++;
        }
        // 发生页命中
        // do nothing...
    }
}

void PageReplacement::LRU()
{
    m_missPageCounts = 0;
    for (int i = 0; i < instructionNumber; ++i) {
        // 没有页命中
        if (m_pageSet.find(m_instructions[i]) == m_pageSet.end()) {
            // 内存满了需要换出
            if (m_pageSet.size() == physicalMemory) {
                int oldestPage = m_pageQueue.front();
                m_pageQueue.pop();
                m_pageSet.erase(oldestPage);
            }
            m_pageSet.insert(m_instructions[i]);
            m_pageQueue.push(m_instructions[i]);
            m_missPageCounts++;
        } else // 页命中
        {
            // 更新页面访问顺序队列
            std::queue<int> tempQueue;
            while (!m_pageQueue.empty()) {
                int currentPage = m_pageQueue.front();
                m_pageQueue.pop();
                if (currentPage != m_instructions[i]) {
                    tempQueue.push(currentPage);
                }
            }
            tempQueue.push(m_instructions[i]);
            m_pageQueue = tempQueue;
        }
    }
}

void PageReplacement::print() const
{
    /*for (const auto& i : m_instructions)
            cout << i << endl;*/
    std::cout << 1.0 - static_cast<double>(m_missPageCounts) / static_cast<double>(instructionNumber) << std::endl;
}

void PageReplacement::initialInstructions()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int start_address_min = 0;
    int start_address_max = 319;

    std::uniform_int_distribution<int> start_address_dist(start_address_min, start_address_max);

    for (int i = 0; i < instructionNumber; ++i) {
        int current_address = start_address_dist(gen);
        std::uniform_int_distribution<int> front_address_dist(start_address_min, current_address);
        std::uniform_int_distribution<int> back_address_dist((current_address + 2) % (start_address_max + 1), start_address_max); // 防止越界

        int front_address = front_address_dist(gen); // 前顺序
        int back_address = back_address_dist(gen); // 后顺序

        // 指令放入
        m_instructions.push_back(current_address);
        m_instructions.push_back(front_address);
        m_instructions.push_back(back_address);
    }
    m_instructions.resize(instructionNumber);

    // 转换为虚拟页面地址
    for (size_t i = 0; i < instructionNumber; ++i) {
        m_instructions[i] /= 10;
    }
}
