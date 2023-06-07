# Tetris
This is a console solution for classic brick game written on c++. 
There are realised all the classic mechanics with different colours in ascii symbols.

# Included libs:
There are no external libs to build this solution, i've used only built in libs:
```
<iostream>
<chrono>
<pthreads>
```

# How to start up
You can use tetris.bin from the latest release or do this:

```
git clone https://github.com/SkullofGods/Tetris_inside_console
cd Tetris_inside_console
g++ -pthread main.cpp -o tetris.bin  // or clang++ -pthread main.cpp -o tetris.bin
./tetris.bin
```

# Also
There are a solution for auto build&release on push git-action what you can use in your projects. 

## How to:
1. Allow actions to use your GITHUB_TOKEN in repository Settings->Actions->General->Workflow permissions->Read and write permissions
2. Copy file release.yml from /.github/workflows
3. Copy file "build" from /.github and rewrite it to your build settings

Or just use guides from used repos:

  [skx/github-action-build](https://github.com/skx/github-action-build)
  [svenstaro/upload-release-action](https://github.com/svenstaro/upload-release-action)
