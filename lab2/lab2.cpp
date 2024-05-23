#include <algorithm>

#include <compare>
#include <format>

#include <iostream>

#include <optional>

#include <random>

#include <string_view>

#include <vector>

using namespace std;
// 进程数
const int PROCESSNUMEBR = 5;

// 资源结构体
struct resource {

    int m_aSrc;
    int m_bSrc;
    int m_cSrc;

    // 构造函数
    resource() = default;
    resource(int a, int b, int c)
        : m_aSrc(a)
        , m_bSrc(b)
        , m_cSrc(c)
    {
    }
    resource(const resource& src) = default;
    // 参数是否大于0
    [[nodiscard]] auto invalidArg() const -> bool { return m_aSrc < 0 || m_bSrc < 0 || m_cSrc < 0; }

    // 运算符重载
    auto operator=(const resource& rhs) -> resource& = default;
    auto operator+=(const resource& rhs) -> resource&;
    auto operator-(const resource& rhs) const -> resource;
    auto operator-=(const resource& rhs) -> resource&;

    [[nodiscard]] auto operator==(const resource& rhs) const -> bool;
    [[nodiscard]] auto operator<=>(const resource& rhs) const -> partial_ordering;
};

// 进程类
class process {
public:
    // 构造函数
    process() = default;
    process(string_view s, const resource& max, const resource& allocation)
        : m_name(s.data())
        , m_Max(max)
        , m_Allocation(allocation)
        , m_Need(m_Max - m_Allocation)
    {
    }
    process(const process& src);

    // 获取函数
    [[nodiscard]] auto getMax() const -> const resource& { return m_Max; }
    [[nodiscard]] auto getAllocation() const -> const resource& { return m_Allocation; }
    [[nodiscard]] auto getNeed() const -> const resource& { return m_Need; }
    [[nodiscard]] auto getName() const -> const string& { return m_name; }
    // 设置函数
    void setNeed(const resource& src);

    // 重载比较运算符
    [[nodiscard]] auto operator==(const process& rhs) const -> bool;

private:
    string m_name; // 进程名
    resource m_Max {};
    resource m_Allocation {};
    resource m_Need {};
};

// 银行家类
class Bank {
public:
    // 构造函数
    Bank() { generateProcess(); }
    ~Bank() = default;

    // 安全性算法
    auto safetyAlgorithm() -> optional<vector<resource>>;
    // 银行家算法
    void bankAlgorithm();
    // 获取函数
    auto getProcesses() -> const vector<process>& { return processes; }
    auto getsafetyProcesses() -> const vector<process>& { return safetyProcesses; }
    auto getavailable() -> const resource& { return available; }

    // 设置函数
    auto setProcess(const resource& src, const string& name) -> bool;

private:
    // 辅助函数
    auto findLessNeedProcess() -> optional<process>;
    void generateProcess();

    vector<process> initProcesses;
    vector<process> processes;
    vector<process> safetyProcesses;
    resource initAvailable { MAXRESOURCEA, MAXRESOURCEB, MAXRESOURCEC };
    resource available { MAXRESOURCEA, MAXRESOURCEB, MAXRESOURCEC };

    // 最大资源数
    static const int MAXRESOURCEA = 20;
    static const int MAXRESOURCEB = 20;
    static const int MAXRESOURCEC = 20;
};

// 打印进程(辅助函数)
void printProcessInfo(const process& p, bool includeAvailable = false, const resource& available = {})
{
    if (includeAvailable) {

        cout << format(" |{0:^3}   {1:^3}   {2:^3}|{0:^3}   {1:^3}   {2:^3}|{0:^3}   {1:^3}   {2:^3}|{0:^3}   {1:^3}   {2:^3}|\n", "A", "B", "C");
    } else {
        cout << format(" |{0:^3}   {1:^3}   {2:^3}|{0:^3}   {1:^3}   {2:^3}|{0:^3}   {1:^3}   {2:^3}|\n", "A", "B", "C");
    }
    cout << format("   |{:^3}   {:^3}   {:^3}|{:^3}   {:^3}   {:^3}|{:^3}   {:^3}   {:^3}|", p.getMax().m_aSrc, p.getMax().m_bSrc, p.getMax().m_cSrc,
        p.getAllocation().m_aSrc, p.getAllocation().m_bSrc, p.getAllocation().m_cSrc,
        p.getNeed().m_aSrc, p.getNeed().m_bSrc, p.getNeed().m_cSrc);
    if (includeAvailable) {
        cout << format("{:^3}   {:^3}   {:^3}|", available.m_aSrc, available.m_bSrc, available.m_cSrc);
    }
    cout << endl;
}

// 打印包含 availableVec 的表格
void printProcess(const vector<process>& Processes, const vector<resource>& availableVec)
{
    cout << format("   |{:^15}|{:^15}|{:^15}|{:^15}|\n", "Max", "Allocation", "Need", "Work+Allocation");
    cout << "--------------------------------------------------------------------\n";
    for (int i = 0; i < PROCESSNUMEBR; ++i) {
        cout << Processes.at(i).getName();
        printProcessInfo(Processes.at(i), true, availableVec[i]);
        cout << "--------------------------------------------------------------------\n";
    }
}

// 打印不包含 availableVec 的表格
void printProcessWithoutAvailable(const vector<process>& processes, const resource& available)
{
    cout << format("   |{:^15}|{:^15}|{:^15}|\n", "Max", "Allocation", "Need");
    cout << "----------------------------------------------------\n";
    for (int i = 0; i < PROCESSNUMEBR; ++i) {
        cout << processes.at(i).getName();
        printProcessInfo(processes.at(i));
        cout << "----------------------------------------------------\n";
    }
    cout << format("Available resource :\n A:{}  B:{}  C:{}", available.m_aSrc, available.m_bSrc, available.m_cSrc);
}

// 随机生成进程资源情况
void Bank::generateProcess()
{
    // 设置随机数种子
    random_device seed;
    mt19937 gen(seed());
    uniform_int_distribution<int> maxA(1, MAXRESOURCEA / 2);
    uniform_int_distribution<int> maxB(1, MAXRESOURCEB / 2);
    uniform_int_distribution<int> maxC(1, MAXRESOURCEC / 2);

    const int maxTries = 100; // 设置尝试次数限制

    for (int tries = 0; tries < maxTries; ++tries) {
        processes.clear(); // 清空进程列表
        available = { MAXRESOURCEA, MAXRESOURCEB, MAXRESOURCEC }; // 重置可用资源

        for (int i = 0; i < PROCESSNUMEBR; ++i) {
            // 生成参数
            string name = "P" + to_string(i);
            // 限制随机生成的资源数目在当前可用资源的范围内
            resource randomMax {
                min(maxA(gen), available.m_aSrc),
                min(maxB(gen), available.m_bSrc),
                min(maxC(gen), available.m_cSrc)
            };
            resource randomAllocation {
                uniform_int_distribution<int>(1, randomMax.m_aSrc)(gen),
                uniform_int_distribution<int>(1, randomMax.m_bSrc)(gen),
                uniform_int_distribution<int>(1, randomMax.m_cSrc)(gen)
            };

            // 更新资源
            process randomProcess { name, randomMax, randomAllocation };
            initProcesses.push_back(randomProcess);
            processes.push_back(randomProcess);
            available -= randomAllocation;
            initAvailable -= randomAllocation;
        }

        if (!(available < resource { 0, 0, 0 })) {
            break; // 成功生成资源且可用资源非负，退出尝试循环
        }
    }
}

// 设置进程
auto Bank::setProcess(const resource& src, const string& name) -> bool
{
    // 查找是否存在符合进程
    auto iter = find_if(initProcesses.begin(), initProcesses.end(), [&](const process& proc) { return proc.getName() == name; });
    // 如果符合就更新进程资源数目
    if (iter != initProcesses.end()) {
        iter->setNeed(src);
        return true;
    }
    return false;
}

// 查找可以运行的进程
auto Bank::findLessNeedProcess() -> optional<process>
{
    auto iter_safe = find_if(processes.begin(), processes.end(), [&](const process& rhs) { return rhs.getNeed() <=> available == std::partial_ordering::less; });
    if (iter_safe != processes.end()) {
        return *iter_safe;
    }
    return std::nullopt;
}

// 安全性算法(对初始时刻的资源分配进行分析)
auto Bank::safetyAlgorithm() -> optional<vector<resource>>
{
    vector<resource> resourceVec;

    while (!processes.empty()) {
        auto maybeProcess = findLessNeedProcess();
        // 如果找到可以运行的进程
        if (maybeProcess.has_value()) {
            process currProcess = maybeProcess.value();
            // 处理进程
            available += currProcess.getAllocation();
            safetyProcesses.push_back(currProcess);
            resourceVec.push_back(available);
            // 移除进程
            auto iter = find(processes.begin(), processes.end(), currProcess);
            if (iter != processes.end()) {
                processes.erase(iter);
            }
        }
        // 无法找到可以运行的进程,不存在安全序列
        else {
            return std::nullopt;
        }
    }
    return resourceVec;
}

// 银行家算法
void Bank::bankAlgorithm()
{
    while (true) {
        // 重置资源
        available = initAvailable;
        safetyProcesses.clear();
        safetyProcesses.resize(0);

        // 判断是否存在安全序列
        auto availableVec = safetyAlgorithm();
        // 找到安全序列
        if (availableVec.has_value()) {
            cout << "安全序列如下: " << endl;
            printProcess(getsafetyProcesses(), availableVec.value());
        } else {
            cout << "未能找到安全序列.\n";
            return;
        }

        // 用户输入进程要申请分配的资源数目

        int requestNum { 0 };
        cout << format("请输入0~{}号进程来请求资源: ", PROCESSNUMEBR - 1);
        // 判断输入是否合法
        while (true) {
            cin >> requestNum;
            if (cin.fail()) {
                cout << "输入非法进程号，请输入一个整数: ";
                cin.clear(); // 清除输入错误标记
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入，直到换行符
                continue; // 回到循环开始，等待有效输入
            }
            if (requestNum >= PROCESSNUMEBR || requestNum < 0) {
                cout << format("输入非法进程号,进程号应在0~{} (0,1,2...),请重新输入: ", PROCESSNUMEBR - 1);
                continue;
            }
            break;
        }

        // 查找申请资源的进程名
        string requestProName = "P" + to_string(requestNum);
        auto iter = find_if(initProcesses.begin(), initProcesses.end(), [&](const process& rhs) { return rhs.getName() == requestProName; });

        if (iter != processes.end()) {
            process requestProcess = *iter;
            int requestA { 0 };
            int requestB { 0 };
            int requestC { 0 };
            cout << "请输入A,B,C三类的资源请求数目(以空格分开): ";
            resource requestResource {};
            // 循环直到用户输入的值合法
            while (true) {
                cin >> requestA >> requestB >> requestC;
                if (cin.fail()) {
                    cout << "输入非法资源数目,请输入整数: ";
                    cin.clear(); // 清除输入错误标记
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入，直到换行符
                    continue; // 回到循环开始，等待有效输入
                }
                requestResource = { requestA, requestB, requestC };
                if (requestResource > requestProcess.getNeed()) {
                    cout << "分配资源大于进程所需资源,不予分配!" << endl;
                    return;
                }
                if (requestResource > available) {
                    cout << "分配资源大于系统持有资源,不予分配!" << endl;
                    return;
                }
                if (requestResource.invalidArg()) {
                    cout << "资源数目小于0,不合理的资源分配,请重新输入:";
                    continue;
                }
                break;
            }
            if (!setProcess(requestResource, requestProName)) {
                cout << "未能成功分配资源！" << endl;
                return;
            }

            processes = initProcesses;
        }

        else {
            cout << "未找到对应进程！" << endl;
            return;
        }
    }
}

// 运算符重载
auto resource::operator<=>(const resource& rhs) const -> partial_ordering
{
    if (auto cmp = m_aSrc <=> rhs.m_aSrc; cmp != 0) {
        return cmp;
    }
    if (auto cmp = m_bSrc <=> rhs.m_bSrc; cmp != 0) {
        return cmp;
    }
    return m_cSrc <=> rhs.m_cSrc;
}

auto resource::operator==(const resource& rhs) const -> bool
{
    return m_aSrc == rhs.m_aSrc && m_bSrc == rhs.m_bSrc && m_cSrc == rhs.m_cSrc;
}

auto operator+(const resource& lhs, const resource& rhs) -> resource
{
    resource temp { lhs.m_aSrc + rhs.m_aSrc, lhs.m_bSrc + rhs.m_bSrc, lhs.m_cSrc + rhs.m_cSrc };
    return temp;
}

auto resource::operator+=(const resource& rhs) -> resource&
{
    m_aSrc += rhs.m_aSrc;
    m_bSrc += rhs.m_bSrc;
    m_cSrc += rhs.m_cSrc;
    return *this;
}

auto resource::operator-(const resource& rhs) const -> resource
{
    resource temp { m_aSrc - rhs.m_aSrc, m_bSrc - rhs.m_bSrc, m_cSrc - rhs.m_cSrc };
    return temp;
}

auto resource::operator-=(const resource& rhs) -> resource&
{
    m_aSrc -= rhs.m_aSrc;
    m_bSrc -= rhs.m_bSrc;
    m_cSrc -= rhs.m_cSrc;
    return *this;
}

process::process(const process& src)
    : process(src.m_name, src.m_Max, src.m_Allocation)
{
    m_name = src.m_name;
    m_Max = src.m_Max;
    m_Allocation = src.m_Allocation;
    m_Need = src.m_Need;
}

void process::setNeed(const resource& src)
{
    m_Need.m_aSrc += src.m_aSrc;
    m_Need.m_bSrc += src.m_bSrc;
    m_Need.m_cSrc += src.m_cSrc;
}

auto process::operator==(const process& rhs) const -> bool
{
    return (m_Max == rhs.m_Max && m_Allocation == rhs.m_Allocation && m_Need == rhs.m_Need);
}

auto main() -> int
{

    Bank bank;

    cout << "初始进程如下: " << endl;
    printProcessWithoutAvailable(bank.getProcesses(), bank.getavailable());
    cout << endl
         << endl;

    bank.bankAlgorithm();
    return 0;
}
