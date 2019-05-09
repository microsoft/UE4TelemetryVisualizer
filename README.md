# UE4 Telemetry Visualizer
![View Telemetry](docs/images/points.png)

This is a plugin for adding telemetry to your UE4 game and visualizing that telemetry in the UE4 editor. It is featured in the [Azure Gaming Reference Architectures](https://docs.microsoft.com/en-us/gaming/azure/reference-architectures/analytics-in-editor-debugging). There is also a [Unity version of this plugin](https://github.com/Microsoft/UnityTelemetryVisualizer). 

## Summary
Game telemetry itself is nothing new, but the number of "out of the box" solutions for games to leverage while ___in development___ are next to none.  Historically, developers would use log files to debug issues, which worked fine for local debugging. Today's games more closely resemble distributed systems, interacting with other clients, servers, and various cloud services regularly.

This project was inspired by developers needing to answer questions for which their own development systems could not provide all the answers, and who were tired of asking people to email them log files!

## Goals
* Make it as simple as possible for __anyone__ on a development team to add and view telemetry - No dedicated engineers required!
* Iterative development model - You won't know what you'll need up front, so make it as easy as possible to add things along the way!
* Low cost of operation

## Features
* Telemetry generation API and asynchronous upload with configurable buffers and send intervals,
  * Optimized for a large volume of telemetry (10's to 100's per second) from a small number of clients (10-30)
  * Batching and compression out of the box, common fields denormalized server side to reduce bandwidth requirements
* Simple UI to build queries in editor, wraps a JSON defined query language usable by external tools
  * Customizable colors and shapes, as well as heatmaps

## Example use cases for telemetry during development
### Programmers
* Code Correctness: Asserts, non-critical errors, unlikely code paths taken
* Profiling: Tracking system counters over thresholds (RAM, CPU, frame render time)

### Artists and Game Designers
* Game asset tracking (missing assets, over-budget assets, hitches)
* Playtesting analytics in realtime (focus groups, friends and family alpha builds)
* Game mechanics validation (AI, balancing) 

### Testing and Quality
* Testing test coverage per build
* Bug correlation, reproduction, regression


## Setup Instructions
This plugin requires a server component to send telemetry to, which can be found here: https://github.com/Azure-Samples/gaming-in-editor-telemetry.

Plugin setup instructions can be found [here](docs/UE4_Instructions.md).

If you find issues, please add them to the [issue tracker](https://github.com/Microsoft/UE4TelemetryVisualizer/issues). 

## Contributing
[Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct)

We welcome contributions to this project.  This project  Please read the [contributing](/CONTRIBUTING.md) information before submitting a pull request. If you need inspiration, please check the [issue tracker](https://github.com/Microsoft/UE4TelemetryVisualizer/issues). We are using the [GitHub Flow](https://guides.github.com/introduction/flow/) development model.
