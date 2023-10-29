# CAB401

## Overview

This solution contains three projects:
- **DMA-Core**: The core library for the DMA algorithm compiled as a static library
- **DMA-CLI**: A command line interface for the application
  - This application demonstrates all capabilites of the core FFT/Onset library
  - Missing proper note error calculation and output
  - Currently just prints the measured notes and the expected notes from the sheet music
  - Also prints the execution time
- **DMA-GUI**: A graphical user interface for the application
  - This application was intended to replace the original C# GUI application
  - It is useful for testing the FFT, HFC and Detection algorithms output
  - Also missing ability to display notes and their errors

## Building

Set either the `DMA-CLI` or `DMA-GUI` project as the startup project and build the solution. The `DMA-Core` project will be built automatically as a dependency. Note, `Release` mode is recommended for optimal performance.

There are also various pre-processing directives that can be used to modify the behaviour of certain features, these directives can be found in the `fft.cpp` and `onset.cpp` files within the `DMA-Core` project.

| Pre-Processor Directive | Description | Default |
|-------------------------|-------------|---------|
| `_USE_PARALLEL_STFT` | Enable multithreading for the STFT function | `true` |
| `_USE_PARALLEL_ANALYZE` | Enable multithreading for the analyze function | `true` |
| `_USE_PARALLEL_IDENT` | Enable multithreading for the identify function | `true` |
| `_STFT_PARALLEL_SCALE` | Scale the number of threads used by the STFT function | `2` |
| `_ANALYZE_PARALLEL_SCALE` | Scale the number of threads used by the analyze function | `1` |
| `_IDENT_PARALLEL_SCALE` | Scale the number of threads used by the identify function | `1` |
| `_USE_ITERATIVE_FFT` | Enable the iterative implementation of the FFT function | `true` |

## Dependencies

`DMA-GUI` requires DirectX 11 support.

## Sample Data

Sample data is provided in the `Sample Data` folder as part of the submission. The `DMA-CLI` application expects the data to be in the parent folder of the solution directory. The `DMA-GUI` does not require the data to be in any specific location as it will prompt the user to select the desired file.