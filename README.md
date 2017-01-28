# AI for Robotics project

## Goal

The goal of this project is to reproduce the results of the article [Predator confusion is sufficient to evolve swarming behaviour](http://rsif.royalsocietypublishing.org/content/10/85/20130305)  
by Randal S. Olson, Arend Hintze, Fred C. Dyer, David B. Knoester, Christoph Adami.

![Application in simulation mode](https://gitlab.com/phlf/IAR_project/raw/master/agents_with_fov.png)

## Dependencies

- [SDL2](https://www.libsdl.org/download-2.0.php)
- [QtCreator](https://www.qt.io/ide/) to handle the project's file

## How to use

1. Compile project
2. Move the [Resources](https://gitlab.com/phlf/IAR_project/tree/master/Resources) folder into the application's working directory
3. Move the [settings.txt](https://gitlab.com/phlf/IAR_project/blob/master/Resources/settings.txt) file into the application's working directory
4. Create an empty *Predator* folder into the application's working directory
5. Setup the [settings.txt](https://gitlab.com/phlf/IAR_project/blob/master/Resources/settings.txt) file according to your needs
6. Launch the application with a terminal

## Settings

### Global
- headless 1
- threads 8
- win_w 768
- win_h 768

### Simulation
- grid_w 512
- grid_h 512
- ticks 2000
- predators 1
- preys 50

### Predators
- pred_speed 1
- pred_turn_speed 8
- pred_retina_cells 12
- pred_los 100
- pred_fov 180

### Preys
- prey_speed 1
- prey_turn_speed 8
- prey_retina_cells_by_layer 12
- prey_los 100
- prey_fov 180

### Predators' evolution
- evolve_pred 1
- pred_generations 500
- pred_children 500

### Preys' evolution
- evolve_prey 0
- prey_generations 400
- prey_children 400

## TODO
- Prey evolution
- Evaluate evolutions' results (load evolved Markov Brains for predators and preys)
- Multithreading?