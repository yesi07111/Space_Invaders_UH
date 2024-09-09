# Space Invaders

To run this project you need:

1. GCC Installed (recommended version is `10.2.1 20210110` but it should not matter which version you use).
2. Open a terminal in the same directory of the `main.c` file.
3. Run the command:
    ```bash
    gcc -o ng main.c -pthread && ./ng
    ```

If you are running the project in WSL you should know that:

1. The distro you choose will not affect the performance of this project. It was tested in ArchLinux and Debian.
2. Some distros use only one CPU thread and 512MB of RAM by default, this will make the project to run slower since it uses multiple threads. To fix this:
    
    1. Go to your `C:\Users\<YourUser>` folder
    1. Create or edit a file called `.wslconfig`
    1. Paste this inside:

        ```
        [wsl2]
        memory=8GB
        processors=4
        ```
    
    1. Make a `wsl --shutdown` in a CMD before opening the WSL console again.

1. If the game blinks too much while running consider using another terminal. The smoother experience was found to be in the VSCode integrated terminal.

## About this project

The project has a major focus in the code so the graphic (in console) are very minimalist. To achieve the goal of putting all of the Operative Systems's course in this project we made:

1. An MLFQ algorith for the enemies movement: the game generates enemies following a stage approach, all this enemies are then put into an MLFQ scheduler. The time this algorithm uses to update priorities and so is the time moving, so each time the algorithm returns an enemy, is this the one that will move on the screen. This causes interesting behaviors when there is a large number of enemies in game.
1. Multithreading: the enemies, the bullets, the global timer, the collisions and the UI are running in separated threads.
1. File management: the records of the game are stored in a file in the hard drive of the computer. Only the top 10 records will be kept.
1. Memory: the game replaces the use of the `malloc(size)` and `free(ptr*)` functions by using a custom implementation. For this, a `char` array is used to simulate the memory of the program, and the functions `p_malloc(size)` and `p_free(ptr*)` (the 'p' stands for 'personalized') are defined. Each call to `p_malloc(size)` seeks in a free-list Data Structure for an available memory block using the next-fit policy and returns a pointer to it. When `p_free(ptr*)` is called, the coresponding memory block is released. The game will display the memory usage in real time.
    
