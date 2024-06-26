# OsLab
操作系统实验

# **实验一 进程调度**

**一、实验目的**

多道程序设计中，经常是若干个进程同时处于就绪状态，必须依照某种策略来决定那个进程优先占有处理机。因而引起进程调度。本实验模拟在单处理机情况下的处理机调度问题，加深对进程调度的理解。

**二、实验内容**

1、优先权法、轮转法

简化假设

1）进程为计算型的（无I/O）

2）进程状态：ready、running、finish

3）进程需要的CPU时间以时间片为单位确定

2、算法描述

1）优先权法

2）轮转法

**三、实验梯度要求**

【基础】优先级法：1、静态的优先级、静态的到达时间和服务时间；

​         2、进程起始全部到达。

​    轮转法 ：1、静态的到达时间和服务时间；

2、进程起始全部到达。

【进阶】优先级法：1、优先级、到达时间、服务时间随机生成。

​         2、完成时间、周转时间、带权周转时间、平均周转时间、平均带权周转时间全表格生成。

​    轮转法 ：1、到达时间、服务时间随机生成。

2、完成时间、周转时间、带权周转时间、平均周转时间、平均带权周转时间全表格生成。

【登顶】优先级法：1、实现图形化演示；

​    轮转法 ：1、实现图形化演示。

【挑战】优先级法：1、对算法进行不改变基本定义之上的提升改进；

​    轮转法 ：1、对算法进行不改变基本定义之上的提升改进。

**四、实验要求**

1．如采用随机数，随机数的取值范围应加以限制，如所需的CPU时间限制在1~20之间。

2．进程数n不要太大通常取4~8个

3．独立编程

4．二种调度算法

5．若有可能请在图形方式下，将PCB的调度用图形成动画显示。

------

# **实验二 避免死锁**

**一、实验目的**

死锁会引起计算机工作僵死，因此操作系统中必须防止。本实验的目的在于让学生独立的使用高级语言编写和调试一个系统动态分配资源的简单模拟程序，了解死锁产生的条件和原因，并采用银行家算法有效地防止死锁的发生，以加深对课堂上所讲授的知识的理解。

**二、实验内容**

设计有 n 个进程共享 m 个系统资源的系统， 基于银行家算法和安全性算法， 进程可动态的申请和释放资源，系统按各进程的申请动态的分配资源。

**三、实验梯度要求**

【基础】 1、静态的资源数目、 静态的资源初始状态；

【进阶】 1、资源数、 资源分配状况全随机生成。 注意资源初始分配随机生成的合理性。

【登顶】 1、实现安全序列的全求解。

【挑战】 1、实现图形化演示。

**四、实验要求**

1、产生的各种随机数的取值范围加以限制，如所需的各类资源数限制在 1~20之间。

2、进程数 n 不要太大通常取 4~8 个

3、使用动态数据结构

4、独立编程

5、若有可能请在图形方式下，将 PCB 的调度用图形成动画显示。

------

# **实验三 存储管理**

**实验学时：4学时**

**实验类型：设计**

**实验要求：必修**

**一、实验目的**

存储管理的主要功能之一是合理地分配空间。请求页式管理是一种常用的虚拟存储管理技术。 本实验的目的是通过请求页式管理中页面置换算法模拟设计，了解虚拟存储技术的特点，掌握请求页式存储管理的页面置换算法。

**二、实验内容**

通过计算不同算法的命中率比较算法的优劣。同时也考虑了用户内存容量对命中率的影响。

命中率 = 1 - 缺页率

页面失效次数为每次访问相应指令时，该指令所对应的页不在内存中的次数。

在本实验中，假定页面大小为 1k，用户虚存容量为 32k，用户内存容量为 2 页到 32 页。

通过随机数产生一个指令序列，共 320 条指令。

**A**、指令的地址按下述原则生成：

1） 50%的指令是顺序执行的

2） 25%的指令是均匀分布在前地址部分

3） 25%的指令是均匀分布在后地址部分

**B**、具体的实施方法是：

1）在[0,319]的指令地址之间随机选取一起点 m；

2）顺序执行一条指令，即执行地址为 m+1 的指令；

3）在前地址[0,m+1]中随机选取一条指令并执行，该指令的地址为 m’；

4）顺序执行一条指令，地址为 m’+1 的指令

9

5）在后地址[m’+2,319]中随机选取一条指令并执行；

6）重复上述步骤 1） ~5），直到执行 320 次指令

**C**、将指令序列变换称为页地址流:

在用户虚存中，按每 k 存放 10 条指令排列虚存地址，即 320 条指令在虚存中的存放方式为：

第 0 条~第 9 条指令为第 0 页（对应虚存地址为[0，9]）；

第 10 条~第 19 条指令为第 1 页（对应虚存地址为[10，19]）；

。。。。。。

第 310 条~第 319 条指令为第 31 页（对应虚存地址为[310，319]）；

按以上方式，用户指令可组成 32 页。

**三、 实验梯度要求**

【基础】 1、实现 FIFO 算法；

【进阶】 1、实现 FIFO 和 LRU 算法；

【登顶】 1、实现 FIFO、LRU 和 OPT 算法。

【挑战】 1、实现 FIFO、LRU、OPT 算法和 Clock 算法。

**四、实验要求**

1、计算相应算法的用户内存从 2k~32k 时的命中率；

2、比较各种算法的命中率；

3、分析当用户内存容量增加是对命中率的影响。

------

# 实验四 磁盘调度

**实验学时：** **2学时**

**实验类型：设计**

**实验要求：必修**

**一、实验目的**

磁盘是高速、大容量、旋转型、可直接存取的存储设备。它作为计算机系统的辅助存储器，担负着繁重的输入输出工作，在现代计算机系统中往往同时会有若干个要求访问磁盘的输入输出要求。系统可采用一种策略，尽可能按最佳次序执行访问磁盘的请求。由于磁盘访问时间主要受寻道时间 T 的影响，为此需要采用合适的寻道算法，以降低寻道时间。本实验要求学生模拟设计一个磁盘调度程序，观察调度程序的动态运行过程。通过实验让学生理解和掌握磁盘调度的职能。

**二、** **实验内容**

模拟扫描调度算法，对磁盘进行移臂操作。

**三、** **实验梯度要求**

【基础】 1、实现 SCAN 算法；

【进阶】 1、实现 CSCAN 算法；

【登顶】 1、实现 SCAN 算法的改进，解决磁臂粘着问题；

【挑战】 1、实现图形化演示。

**四、实验要求**

1、假设磁盘只有一个盘面，并且磁盘是可移动头磁盘。

2、访问磁道随机生成， 适当控制范围。

3、实验过程重代码， 实验报告重分析。
