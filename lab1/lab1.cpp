#include <iomanip>

#include <iostream>

#include <queue>
#include <random>

using namespace std;
const int PROCESSNUMBER = 4; // 进程数
const int Q = 2; // 时间片长度

// 进程结构体
struct process {
    int priority; // 进程优先级，小的优先级高
    int timeofArrival; // 到达时间
    int serviceTime; // 剩余服务时间
    int completionTime; // 完成时间
    int originalServiceTime; // 原始服务时间
    process() = default;
    process(int p, int t, int s)
        : priority(p)
        , timeofArrival(t)
        , serviceTime(s)
        , completionTime(0)
        , originalServiceTime(s)
    {
    }
};

auto compareByPriority = [](const process& a, const process& b) {
    if (a.priority != b.priority)
        return a.priority > b.priority;
    else
        return a.serviceTime > b.serviceTime;
}; // 优先级比较lambda表达式(从小到大),如果相同的话,剩余服务时间少的先运行,减少长任务对后续任务的影响
auto compareByArrivalTime = [](const process& a, const process& b) {
    if (a.timeofArrival != b.timeofArrival)
        return a.timeofArrival > b.timeofArrival;
    else
        return a.priority > b.priority;
}; // 到达时间比较lambda表达式(从早到晚),如果相同的话,优先级高的在前面

using ReadyQueue = priority_queue<process, vector<process>, decltype(compareByPriority)>;
using ReserveQueue = priority_queue<process, vector<process>, decltype(compareByArrivalTime)>;
using CompletionQueue = priority_queue<process, vector<process>, decltype(compareByArrivalTime)>;

// 函数声明
CompletionQueue priorityFirst(ReserveQueue reserveProcesses); // 优先级算法
CompletionQueue RR(ReserveQueue reserveProcesses); // 轮转算法
void printList(CompletionQueue& q); // 随机生成进程
void generateProcesses(ReserveQueue& reserveProcesses); // 随机生成进程

int main()
{

    ReserveQueue reserveProcesses(compareByArrivalTime); // 后备队列进程，按照到达时间排序（从早到晚）

    generateProcesses(reserveProcesses); // 随机生成进程
    CompletionQueue priorityProcesses = priorityFirst(reserveProcesses); // 调用优先级优先算法
    cout << "优先级优先算法:\n";
    printList(priorityProcesses); // 输出列表

    CompletionQueue rrProcesses = RR(reserveProcesses); // 调用轮转算法
    cout << "\nRR算法:\n";
    printList(rrProcesses); // 输出列表

    return 0;
}

// 优先级优先算法,如果优先级相同应选择剩余服务时间短的先运行
CompletionQueue priorityFirst(ReserveQueue reserveProcesses)
{
    int currTime = 0; // 当前时间
    CompletionQueue completionProcesses(compareByArrivalTime);
    ReadyQueue readyProcesses(compareByPriority); // 就绪队列进程，按照优先级排序（从高到低）
    while (!reserveProcesses.empty() || !readyProcesses.empty()) // 两个队列都为空才结束循环
    {
        // 如果后备队列为空
        if (reserveProcesses.empty()) {
            while (!readyProcesses.empty()) // 循环就绪队列直至空
            {
                process readyProcess = readyProcesses.top(); // 从就绪队列取出队首元素使用cpu
                readyProcess.completionTime = currTime + readyProcess.serviceTime; // 计算完成时间
                currTime += readyProcess.serviceTime; // 更新当前时间
                readyProcesses.pop(); // 删除队首元素
                completionProcesses.push(readyProcess); // 插入到完成队列
            }
            return completionProcesses;
        }

        // 如果没有就绪队列就从后备队列取一个
        else if (readyProcesses.empty()) {
            process pushProcess = reserveProcesses.top();
            currTime = max(currTime, pushProcess.timeofArrival); // 时间调整
            reserveProcesses.pop();
            readyProcesses.push(pushProcess);
        }

        else // 两个队列都非空
        {
            process readyProcess = readyProcesses.top();
            process reserveProcess = reserveProcesses.top();
            // 执行进程无法在下一个进程到达前结束任务
            if (readyProcess.serviceTime + currTime > reserveProcess.timeofArrival) {
                int time = reserveProcess.timeofArrival - currTime; // 可运行时间
                readyProcess.serviceTime -= time; // 更新服务时间
                currTime += time; // 更新当前时间

                // 更新队列
                readyProcesses.pop();
                reserveProcesses.pop();
                readyProcesses.push(reserveProcess);
                readyProcesses.push(readyProcess);
            }
            // 执行进程可以完成任务
            else {
                readyProcess.completionTime = currTime + readyProcess.serviceTime; // 计算完成时间
                currTime += readyProcess.serviceTime; // 更新当前时间
                readyProcess.serviceTime = 0; // 剩余服务时间为0
                readyProcesses.pop(); // 移除队首元素
                completionProcesses.push(readyProcess); // 插入到完成队列
            }
        }
    }
    return completionProcesses;
}

// 轮转算法
CompletionQueue RR(ReserveQueue reserveProcesses)
{
    queue<process> readyProcesses;
    CompletionQueue completionProcesses(compareByArrivalTime);
    int currTime = 0;
    while (!reserveProcesses.empty() || !readyProcesses.empty()) // 两个队列都为空才结束循环
    {
        // 如果后备队列为空
        if (reserveProcesses.empty()) {
            while (!readyProcesses.empty()) // 循环就绪队列直至空
            {
                process readyProcess = readyProcesses.front(); // 从就绪队列取出队首元素使用cpu
                readyProcesses.pop(); // 删除队首元素

                // 可以在时间片内完成
                if (readyProcess.serviceTime <= Q) {
                    readyProcess.completionTime = currTime + readyProcess.serviceTime;
                    currTime += readyProcess.serviceTime;
                    readyProcess.serviceTime = 0;
                    completionProcesses.push(readyProcess);
                }
                // 无法在时间片内完成就执行一个完整的时间片
                else {
                    readyProcess.serviceTime -= Q;
                    currTime += Q;
                    readyProcesses.push(readyProcess); // 放回队列末尾
                }
            }
            return completionProcesses;
        }

        // 如果没有就绪队列就从后备队列取一个
        else if (readyProcesses.empty()) {
            process pushProcess = reserveProcesses.top();
            currTime = max(currTime, pushProcess.timeofArrival); // 时间调整
            reserveProcesses.pop();
            readyProcesses.push(pushProcess);
        }

        else // 两个队列都非空
        {
            process readyProcess = readyProcesses.front();
            // 可以在时间片内完成
            if (readyProcess.serviceTime <= Q) {
                readyProcess.completionTime = currTime + readyProcess.serviceTime;
                currTime += readyProcess.serviceTime;
                readyProcess.serviceTime = 0;
                completionProcesses.push(readyProcess);
            }
            // 无法在时间片内完成就执行一个完整的时间片
            else {
                readyProcess.serviceTime -= Q;
                readyProcesses.front().serviceTime -= Q;
                currTime += Q;
            }
            // 将后备队列可加入进程全部添加到就绪队列队尾
            process reserveProcess = reserveProcesses.top();
            while (reserveProcess.timeofArrival <= currTime && !reserveProcesses.empty()) // 后备队列非空才取出
            {
                readyProcesses.push(reserveProcess);
                reserveProcesses.pop();
                if (!reserveProcesses.empty())
                    reserveProcess = reserveProcesses.top();
            }
            readyProcesses.pop();
            if (readyProcess.serviceTime != 0) {
                readyProcesses.push(readyProcess);
            }
        }
    }
    return completionProcesses;
}

void printList(CompletionQueue& q)
{
    char processName = 'A';
    double turnaroundTimeSum = 0;
    double turnaroundTimeWithWeightSum = 0;
    // 遍历优先队列，每次取出队首元素并输出
    cout << "进程名" << setw(12) << "优先级" << setw(14) << "到达时间" << setw(16) << "服务时间" << setw(16) << "完成时间"
         << setw(16) << "周转时间" << setw(20) << "带权周转时间" << endl;
    while (!q.empty()) {
        process currentProcess = q.top();
        int turnaroundTime = currentProcess.completionTime - currentProcess.timeofArrival; // 周转时间
        double turnaroundTimeWithWeight = static_cast<double>(turnaroundTime) / static_cast<double>(currentProcess.originalServiceTime); // 带权周转时间
        // 输出每个进程的信息，并设置字段宽度
        cout
            << setw(3) << processName << setw(9) << currentProcess.priority << setw(9) << currentProcess.timeofArrival << setw(12) << currentProcess.originalServiceTime
            << setw(13) << currentProcess.completionTime << setw(12) << turnaroundTime << setw(13) << fixed << setprecision(2)
            << turnaroundTimeWithWeight << endl;
        q.pop();
        processName++;
        turnaroundTimeSum += turnaroundTime;
        turnaroundTimeWithWeightSum += turnaroundTimeWithWeight;
    }
    cout << "平均周转时间: " << fixed << setprecision(2) << turnaroundTimeSum / PROCESSNUMBER << endl
         << "平均带权周转时间:" << turnaroundTimeWithWeightSum / PROCESSNUMBER << endl;
}

void generateProcesses(ReserveQueue& reserveProcesses)
{
    // 设置随机数生成器
    random_device rd; // 获取随机设备的种子
    mt19937 gen(rd()); // 使用 Mersenne Twister 引擎，种子为 rd()

    // 生成随机整数
    uniform_int_distribution<> int_dist(1, 20); // 生成 [1, 20] 之间的随机整数

    // 随机生成进程并进入后备队列
    for (int i = 0; i < PROCESSNUMBER; ++i) {
        // int random_p = int_dist(gen);
        // int random_t = int_dist(gen);
        // int random_s = int_dist(gen);
        // process p{random_p, random_s, random_t};
        process p { int_dist(gen) % 10 + 1, int_dist(gen), int_dist(gen) };
        reserveProcesses.push(p);
    }
}
