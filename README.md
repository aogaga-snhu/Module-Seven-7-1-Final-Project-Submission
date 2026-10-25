# CS 330: Computational Graphics and Visualization
## Module Seven — 7-1 Final Project Submission

**Student:** Ogheneogaga Agi  
**Course:** CS 330 — Computational Graphics and Visualization  
**Language and graphics API:** C++ and OpenGL

## Project overview

For my final project, I developed an interactive 3D tabletop scene featuring a ceramic coffee mug with coffee, a wooden desk, a hardcover book, a plate, and a decorative sphere. I assembled the objects from reusable primitive meshes, including a plane, box, cylinder, tapered cylinder, torus, and sphere. The scene uses image textures, material properties, and multiple light sources to distinguish surfaces such as wood, ceramic, and coffee.

Users can explore the scene using keyboard and mouse controls and switch between perspective and orthographic projections. The project demonstrates object transformations, texture mapping, lighting, camera navigation, and shader-based rendering.

## How do I approach designing software?

This project reinforced the value of beginning with a clear visual goal and breaking it into manageable components. I first considered the objects that would make the scene recognizable, then identified which primitive shapes could be combined to construct them. For example, the mug required separate geometry for its body, rim, handle, and coffee surface. Designing these parts independently made it easier to adjust proportions, positioning, and appearance without rebuilding the entire scene.

I also learned to think about materials and lighting during the design process rather than treating them as finishing touches. Texture selection, UV scaling, surface shininess, and light placement all influence how users perceive an object. In future projects, I can apply this approach by defining requirements early, building reusable components, and evaluating visual and functional results throughout development.

## How do I approach developing programs?

I developed the scene iteratively across the course milestones. I started with geometry and object placement, added camera movement and projection controls, introduced textures, and then configured lighting and material properties. Each stage built on the previous one, making it easier to isolate problems and verify changes.

One important lesson was that code can compile successfully while the rendered result is still incorrect. Troubleshooting shader paths, texture loading, transformations, and lighting required checking both the source code and the application at runtime. I used functions to separate scene preparation, rendering, lighting, and camera behavior, which helped keep the project understandable as it grew. This iterative, modular approach is useful well beyond graphics programming, particularly when developing and debugging larger applications.

## How can computer science help me reach my goals?

My professional background is in software engineering, including backend systems, cloud services, and distributed applications. CS 330 broadened my understanding of how software can create interactive visual experiences. The mathematical concepts behind transformations, camera movement, and lighting also strengthened my appreciation for the relationship between algorithms and what users see on screen.

I am interested in expanding these skills through data visualization, simulation, and applications that combine graphics with artificial intelligence and machine learning. For example, interactive 3D environments could help communicate complex information or visualize the behavior of systems. The problem-solving, debugging, and modular design practices I used in this course will also support my continued education and professional development.

## Project structure

```text
7-1_FinalProjectMilestones/
├── 7-1_FinalProjectMilestones.sln
├── 7-1_FinalProjectMilestones.vcxproj
├── Source/
│   ├── MainCode.cpp
│   ├── SceneManager.cpp
│   ├── SceneManager.h
│   ├── ViewManager.cpp
│   └── ViewManager.h
├── shaders/
│   ├── vertexShader.glsl
│   └── fragmentShader.glsl
└── textures/
    ├── wood.png
    ├── ceramic_body.png
    ├── ceramic_trim.png
    ├── coffee.png
    ├── book_cover.png
    ├── plate_glaze.png
    └── ornament.png
```

The project may also require the shared `CS330Content` course folders (`Utilities`, `3DShapes`, and `Libraries`) to build in Visual Studio.

## Controls

| Input | Action |
| --- | --- |
| W / S | Move forward / backward |
| A / D | Move left / right |
| Q / E | Move down / up |
| Mouse movement | Change camera orientation |
| Mouse scroll wheel | Adjust camera movement speed |
| P | Perspective projection |
| O | Orthographic projection |
| Esc | Exit application |

## Building and running

1. Place the project in the appropriate SNHU `CS330Content/Projects` directory so that shared course dependencies can be found.
2. Open `7-1_FinalProjectMilestones.sln` in Visual Studio on Windows.
3. Select the configuration and platform supported by your course setup.
4. Choose **Build → Rebuild Solution**.
5. Start the application using **Local Windows Debugger**.
6. Confirm that the objects, textures, lighting, and camera controls work as expected.

> **Note:** The source archive used to prepare this README does not contain a rebuilt executable. Include the executable from your successful local build if your instructor requires it.

## Portfolio artifacts

- **Module Seven final project:** Source code and completed 3D scene.
- **Design Decisions:** The accompanying final-project reflection document.
- **README:** This document, reflecting on my design process, development practices, and future goals.

## AI use acknowledgment

I used ChatGPT to assist with code organization, troubleshooting, and refining the written project documentation. I reviewed and tested the project in my development environment.
