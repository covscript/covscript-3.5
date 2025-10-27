# CovScript 3.5 设计文档

## 设计目标

 + 升级到 C++ 17 标准
 + 语法兼容 CovScript 3.4
 + 兼容现有 CNI Extensions
 + 全面 IR 化，提高执行性能
 + 原生 Unicode 支持
 + 取代引用计数 GC
   + 栈对象在框架内用 View 和 Move 取代引用计数
   + 堆对象用 Mark-Sweep GC
 + 所有 Blocking I/O 全部支持异步
 + 为 JIT 预留接口
 + 尝试支持多线程

## 框架

内建类型
 + cs::numeric：高性能浮点/整数复合类型
 + cs::string：UTF-8 字符串