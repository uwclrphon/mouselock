# IFLOW 上下文文档

## 项目概述

这是一个名为 "MouseLock" 的Windows应用程序，旨在在不锁屏的情况下限制对计算机的访问。它通过监控鼠标和键盘输入来实现这一功能，并可以配置锁定时间和模拟蓝屏等行为。

主要技术栈：
- C++ (C++17标准)
- Qt6框架 (用于GUI界面)
- CMake构建系统
- Windows API (用于底层输入监控)

项目架构采用模块化设计，包含以下主要组件：
1. 核心模块(core)：包含线程池管理、全局计数器、蓝屏模拟等功能
2. 工具模块(tools)：实现具体的时间更新、鼠标点击监控、键盘监听等功能
3. GUI模块：基于Qt的图形用户界面
4. 配置管理：使用JSON文件进行配置管理
5. 日志系统：自定义的日志记录功能

## 构建和运行

### 构建要求
- CMake 3.15或更高版本
- Qt6 (推荐6.9.0版本)
- 支持C++17的编译器
- Windows平台 (项目依赖Windows API)

### 构建步骤
1. 配置Qt路径：
   ```bash
   cmake -DQT_ROOT="Qt安装路径" -DQT_VERSION="6.9.0" -DQT_ARCH="mingw_64" .
   ```
   
2. 构建项目：
   ```bash
   cmake --build .
   ```

3. 运行程序：
   编译后的可执行文件位于 `build/bin/` 目录下，运行 `mouse_lock_main.exe`

### 调试模式
可以通过CMake选项启用调试模式：
```bash
cmake -Dif_debug=ON .
```
这将启用控制台输出和更详细的日志记录。

## 开发约定

### 代码风格
- 使用C++17标准
- 遵循现代C++编程实践
- 使用RAII原则管理资源
- 使用std::atomic进行线程间状态同步
- 使用std::scoped_lock进行互斥锁管理

### 项目结构
- `include/` - 头文件目录
- `src/` - 源代码目录
  - `core/` - 核心功能模块
  - `utils/` - 工具类
  - `json_utils/` - JSON配置管理
- `build/` - 构建输出目录

### 配置管理
项目使用JSON文件(`global_config.json`)进行配置管理，支持以下配置项：
- `mouse_lock_time`: 鼠标锁定时间(秒)
- `blue_screen_time`: 蓝屏模拟时间(秒)
- `mouse_lock_enabled`: 是否启用鼠标锁定
- `blue_screen_enabled`: 是否启用蓝屏模拟

### 日志系统
使用自定义的Logger类进行日志记录，日志文件默认保存在`logs/mouselock.log`。

### 线程管理
项目使用线程池(ThreadPool)管理多个后台线程：
- 时间更新线程
- 鼠标点击监控线程
- 键盘监听线程
- 蓝屏模拟线程

所有线程在程序退出时都会被正确地停止和清理。