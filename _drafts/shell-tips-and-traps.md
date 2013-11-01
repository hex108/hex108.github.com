---
layout: post
title: Bash Shell tips and traps
category: Shell
summary: Shell是Linux上主要的工具语言，虽然简单，但里面有一些需要注意的地方。如果需要用它写比较长的程序，系统地学习还是会有一些帮助的，能扫除盲点。
tags: [Shell]
---

{{ page.title }}
================

{{ page.summary }}

最重要的参考和学习资料是官方的[Bash Reference](http://www.gnu.org/software/bash/manual/bash.html)。我的学习经验告诉我官方的文档和学习手册一般都写得很好（要不然怎么推广），而且最有准确度。

## Tips
### Shell的工作机制

### set -option
在程序的开头加上`set -u`，防止使用没有定义的变量。

### command命令
Shell允许重载命令，用command命令可以调用它内置的命令。

我刚学这个命令不久就用上了，还是有点喜出望外的。当时在项目中遇到一个问题:系统中的tar命令消耗了大量的缓存。因此我们写了一个包装了read和write的库，进行直接读出和写入，不进行缓存，需要对它进行测试。不能对别的程序有影响（如果在程序开头就export LD_LIBRARY_PATH=$path_to_the_lib:$LD_LIBRARY_PATH，则会影响到其他的程序）。
{% highlight bash %}
tar(){
    export LD_LIBRARY_PATH=$path_to_the_lib:$LD_LIBRARY_PATH
    command tar "$*"
}
{% endhighlight %}

### [ ] 以及 [[ ]] 以及 test 是一个事，
这样就能理解为什么
    if [ ]
    if [[ ]]
    if test ...
    以及最重要的是  if cmd 这种形式了

## Traps
### 使用字符串时加引号
Shell是解释型语言，首先会对变量进行替换。

刚开始写Shell时比较容易犯的错误是使用字符串时不加引号，举几个常见的例子:`if [ $var1 -eq $var2 ]`。

如果把`if [ -z $var ]; then ...; fi`修改成`if [ -n $var ]; then ...; fi`;你就等着出BUG吧。

####空字符串
    key=""
    value=3
    set_key_value  $key $value

### 信号
如果某个父进程在**同步**运行子进程，向该父进程发信号是不会有响应的。[Bash Reference](http://www.gnu.org/software/bash/manual/bash.html#Signals)上是这样说的：
>If Bash is waiting for a command to complete and receives a signal for which a trap has been set, the trap will not be executed until the command completes. When Bash is waiting for an asynchronous command via the wait builtin, the reception of a signal for which a trap has been set will cause the wait builtin to return immediately with an exit status greater than 128, immediately after which the trap is executed.

### File name expansion
After word splitting, unless the -f option has been set (see Section 2.3.2), Bash scans each word for the characters "*", "?", and "[". If one of these characters appears, then the word is regarded as a PATTERN, and replaced with an alphabetically sorted list of file names matching the pattern. If no matching file names are found, and the shell option nullglob is disabled, the word is left unchanged. If the nullglob option is set, and no matches are found, the word is removed. If the shell option nocaseglob is enabled, the match is performed without regard to the case of alphabetic characters.

When a pattern is used for file name generation, the character "." at the start of a file name or immediately following a slash must be matched explicitly, unless the shell option dotglob is set. When matching a file name, the slash character must always be matched explicitly. In other cases, the "." character is not treated specially.

The GLOBIGNORE shell variable may be used to restrict the set of file names matching a pattern. If GLOBIGNORE is set, each matching file name that also matches one of the patterns in GLOBIGNORE is removed from the list of matches. The file names . and .. are always ignored, even when GLOBIGNORE is set. However, setting GLOBIGNORE has the effect of enabling the dotglob shell option, so all other file names beginning with a "." will match. To get the old behavior of ignoring file names beginning with a ".", make ".*" one of the patterns in GLOBIGNORE. The dotglob option is disabled when GLOBIGNORE is unset.

### True/False

### 捕获命令输出

