参考
www.jamesmolloy.co.uk 
Roll your own toy UNIX-clone OS
以及
hurlex
https://github.com/hurley25/hurlex

跟着james的教程做，每一节对应的程序留在step_by_step
https://github.com/Nov11/jamesos_step_by_step

如果你也觉得这个实现做到分页已经很恶心了
如果你想知道除了开启分页模式之前全做成对等映射之外，页表到底怎么动态设置
如果你觉得堆分配这种事情应该搞得简单点，做成sbrk包裹函数那种东西
那么看看这些：
http://wiki.osdev.org/Paging 开分页没法再用物理地址了怎么修改页表
http://wiki.osdev.org/Memory_Management_Unit 常用的0xfffff000访问页目录具体过程

google code上还有个稍微新一点实现： james molloy tutorial v2
文档么 上面的hurlex 的doc里有一份

个人认为应该在5的基础上搭虚拟内存部分，堆管理依赖分页虚拟内存
接下来是任务切换 加上特权级转换 cpu编程就差不多了 快告别保护模式了
