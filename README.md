# 🎮 Space Invaders OS

## 📖 About
A Space Invaders-style console game that serves as a practical demonstration of core Operating Systems concepts. Built entirely in C, this project implements fundamental OS mechanisms like scheduling, memory management, and multithreading within the context of a classic arcade game.

## 🚀 Quick Start

### 🛠️ Prerequisites
- GCC Compiler (recommended version: 10.2.1 or higher)
- POSIX-compliant system (Linux/Unix)
- pthread library

### ⚙️ Installation
1. 📥 Clone the repository:
```bash
git clone https://github.com/yourusername/space-invaders-os
```
2. 📂 Navigate to the project directory:
```bash
cd space-invaders-os
```
3. ⚡ Compile and run:
```css
gcc -o ng main.c -pthread && ./ng
```

### 🎮 Controls
- `A` - Move left
- `D` - Move right
- `W` - Toggle firing mode
- `Space` - Stop movement

### 🖥️ WSL Configuration
If you're running the game in Windows Subsystem for Linux (WSL):

1. 📝 Create/edit `.wslconfig` in `C:\Users\<YourUser>` and write in it:
```bash
[wsl2]
memory=8GB
processors=4
```

2. 🔄 Restart WSL:
```bash
wsl --shutdown
```

💡 For optimal performance, use VSCode's integrated terminal to minimize screen flickering.


## 🎯 Features

### 🔄 Multi-Level Feedback Queue (MLFQ)
The game implements a sophisticated MLFQ scheduler to manage enemy movement and behavior:

#### 🎮 Core Components
- Three priority queues (High, Medium, Low)
- Dynamic priority adjustment
- Anti-starvation mechanism
- Configurable quantum and update times

#### ⚙️ Technical Details
- Update Time: 20 units
- Time Slice (Quantum): 8 units
- Priority degradation based on execution time
- Periodic priority boost to prevent starvation

#### 🛠️ Implementation
- Enemies start in high priority queue
- Movement time tracking per enemy
- Priority decreases as execution time increases
- Periodic reset of all enemies to high priority

### 🧵 Multithreading System
The game utilizes multiple threads to handle different aspects of gameplay:

#### 🎯 Main Threads
- 🎮 Game Loop Thread: Main game state and rendering
- 👾 Enemy Manager Thread: MLFQ scheduling and enemy updates
- 💥 Collision Detection Thread: Hit detection and resolution
- ⏲️ Timer Thread: Game time management
- 🔫 Bullet Management Thread: Projectile updates

#### 🔐 Synchronization
- Mutex locks for shared resource access
- Thread-safe memory operations
- Controlled thread termination
- Race condition prevention

### 🗃️ Custom Memory Management
Implementation of a custom memory management system:

#### 🔧 Features
- 1MB (1 << 20) simulated memory space
- Next-Fit allocation policy
- Block metadata management
- Memory fragmentation handling

#### 🧱 Memory Block Structure
- Size tracking
- Free/Used status
- Previous/Next block pointers
- Coalescing of adjacent free blocks

#### ⚡ Memory Operations
- `p_malloc()`: Custom memory allocation
- `p_free()`: Custom memory deallocation
- Real-time memory usage monitoring
- Memory state visualization

### 📁 File System
Implementation of a persistent record system:

#### 📊 Record Management
- Top 10 high scores tracking
- Score-based sorting
- Persistent storage in files
- Structured record format

#### 📈 Record Data
- Player score
- Game duration
- Stage reached
- Automatic updates

#### 🗄️ File Operations
- Binary file handling
- Record sorting and filtering
- Atomic file operations
- Error handling and validation

### 🎲 Game Mechanics

#### 👾 Enemy Types
The game features 10 different enemy types with increasing difficulty:
- Basic enemies (Type 1-3): Low HP, straight movement
- Advanced enemies (Type 4-6): Medium HP, zigzag movement
- Elite enemies (Type 7-8): High HP, complex movement
- Boss enemies (Type 9-10): Maximum HP, advanced patterns

#### 🌟 Stage System
- Progressive difficulty increase
- Enemy count grows with stages
- Mixed enemy type spawning
- Dynamic spawn patterns

#### ⚔️ Combat System
- Player projectile management
- Collision detection
- HP-based enemy resistance
- Score calculation

## 🖥️ Technical Details

### 🎯 Performance Considerations
- Efficient thread management
- Optimized memory usage
- Minimal screen flickering
- Resource cleanup

### ⚙️ Configuration Options
- Customizable screen dimensions
- Adjustable enemy limits
- Configurable time steps
- Memory size settings

### 🐛 Debug Features
- Memory state monitoring
- Real-time performance stats
- Error logging
- State visualization

## 📝 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
