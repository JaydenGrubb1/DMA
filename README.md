# Digital Music Analyser

This project was completed as part of a university project (CAB401) with the goal of parallelising and increasing the performance of an application. The source code for the original application can be found [here](https://github.com/JaydenGrubb1/DtmfDetection). The core focus of the assignment was the FFT algorithm so other aspects are not fully complete.

## Overview
The project is broken down into three sections:

- **DMA-Core**: Provides the core functionality of the application
  - FFT and STFT algorithms
  - Onset detection
  - WAV file loader
  - Sheet music parser
- **DMA-CLI**: CLI interface for the application
  - Loads sample audio file and music sheet
  - Outputs detected notes
  - Outputs execution time
- **DMA-GUI**: GUI interface for the application
  - Frequency spectrum display
  - High Frequency Content display
  - Onset/Offset markers
  - Time/Frequency domain plot for signal windows 

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

- All projects require C++ 20 support
- All projects require Visual Studio 2022 (v143)
  - other versions may work but have not been tested
- `DMA-GUI` requires DirectX 11 support.

## Sample Data

Sample data is provided in the `Sample Data` folder as part of the submission. The `DMA-CLI` application expects the data to be in the `..\data\` folder relative to the executable and to be named `sample.wav` and `sample.xml`. If the application is run from within Visual Studio, there is already a `data` folder present. The `DMA-GUI` does not require the data to be in any specific location as it will prompt the user to select the desired file.
