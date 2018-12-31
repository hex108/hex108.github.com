---
title: "GDB符号处理简介"
date: 2013-10-14T20:00:00+08:00
draft: false
category: GDB 
tags: [GDB]
---

符号处理(Symbol Handling)是GDB的一个重要组成部分，它包括以下几部分：读取目标文件(objfile file readers)，解释调试信息(debug info interpreters)，管理符号表(symbol table management)，解析源语言的表达式(source language expression parsing)，打印类型和值(type and value printing)。以下面的例子简要说明GDB怎么处理符号的。

{% highlight c %}
// test.c
int mini = 3;

void foo(void){
​    int mini_in_foo = 5;  // 可以把它和mini的DIE结构进行对比
}
{% endhighlight %}

    $ gcc -fPIC -g -c -o test.o test.c
    $ gdb ./test.o
    Reading symbols from /home/hex108/test.o...done.
    (gdb) p mini + 5
    8

## 符号处理过程
这个过程可以通过GDB自己的调试信息直观地看到。

    (gdb) set debug parser
    (gdb) set debug dwarf2-die 3
    (gdb) set debug expression 3
    (gdb) p mini + 5
    ...（此处省略1W字）

### 读取目标文件 + 解释调试信息
运行`gdb ./test.o`时，GDB会把`test.o`的调试信息都读到内存里，`Reading symbols from /home/hex108/test.o...done`就是在做这事。

现在常用的调试信息的格式是[DWARF](http://en.wikipedia.org/wiki/DWARF)，通过阅读文章《[Introduction to the DWARF Debugging Format](http://www.dwarfstd.org/doc/Debugging%20using%20DWARF.pdf)》，就能对它有一个宏观的概念。最基本的单元是DIE (Debug Information Entry)，`mini`在`test.o`中的DIE信息如下所示：

    <1><54>: Abbrev Number: 5 (DW_TAG_variable)    // 表示这是一个变量
       <55>   DW_AT_name        : (indirect string, offset: 0x3a): mini	 // 变量名
       <59>   DW_AT_decl_file   : 1	 
       <5a>   DW_AT_decl_line   : 1	
       <5b>   DW_AT_type        : <0x4d>	// 类型信息
       <5f>   DW_AT_external    : 1	        // 全局变量
       <60>   DW_AT_location    : 5 byte block: 3 0 0 0 0 	(DW_OP_addr: 0)  // 变量地址

每个目标文件是一个编译单元(Compilation Unint, CU)，一个编译单元包含多个DIE。

GDB加载目标文件时，会调用[BFD库](http://en.wikipedia.org/wiki/Binary_File_Descriptor_library)从目标文件里读取DIEs。
然后把这些信息存到符号表中，以后直接从符号表中查询即可。如果目标文件很大，这个过程会很消耗长时间，因此GDB做了一个优化，先只存储部分信息：名字和地址等简单信息。
存储这些部分信息的表叫Partial Symbol Table(PST)。

主要的函数调用路径为： `read_psyms` -> `scan_partial_symbols` -> `add_partial_symbol`。
如果某个符号未被加入PST中，可以着重查看函数`add_partial_symbol`。

### 解析源语言的表达式 + 管理符号表 
解析源语言的表达式的过程包括词法分析和语法分析两部分。

输入`p mini + 5`后，GDB进行词法分析，首先得到的是`p`，因此会相应地调用`print_command`对接下来的表达式`mini + 5`进行解析（如果想调试GDB，可以在类似`print_command`的函数设置断点，进行单步调试）。
`print_command`继续对表达式进行解析，得到`mini`，从所有已加载的目标文件对应的PST（是所有的目标文件，没看错）里查询它，查到以后就会它对应的目标文件`test.o`里的符号信息全部展开并存储到Full Symbol Tabls(FST)，最后保存FST里指向`mini`的指针。

主要的函数调用路径为：`print_command` -> `parse_expression` -> `lookup_partial_symbol` -> `psymtab_to_symtab`（存储符号信息到FST）。

接下来就是一些语法的归约(reduction)了。

解析完后的结果是如下的一个栈（栈是一个前缀表达式）：

    0  BINOP_ADD             
    1  OP_VAR_VALUE          Block @0x0, symbol @0x97bced0 (mini)   // mini在FST中的symbol指针
    5  OP_LONG               Type @0x978d098 (int), value 5 (0x5)   // 数字5

### 打印值
对前缀表达式进行求值，其中`mini`对应的值根据symbol指针可以得到，数字`5`的值就是5。

然后打印即可。

## 参考资料
1. [GDBInt](https://sourceware.org/gdb/onlinedocs/gdbint/)
2. [The Architecture of Open Source Applications, Volume II](http://aosabook.org/en/gdb.html)
