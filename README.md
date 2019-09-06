# Minichess

This is a text-based Minichess game in which the T-Rex evolutionary neural network learns to play from scratch. After completing a workout, T-Rex plays against you through a basic text interface.

T-Rex trains playing against mutant clones of itself until it reaches 1000000 consecutive victories. Winners are stored in a list, so the next winner must win not only the current winner but also the previous ones. The current winner is stored in the file **neural_network.json** and loaded in the next training. Each training is limited to 1000000 consecutive wins so that the human player does not have to wait too long.

## Installing dependencies

You must compile T-Rex as a shared library:

https://github.com/Kenshiro-28/T-Rex

Copy the generated file **libT-Rex.so** in the folder /usr/local/lib

Copy the header files in /usr/local/include in a folder named T-Rex:

``` 
 T-Rex
    ├── data_tier
    │   └── DataManager.h
    ├── logic_tier
    │   ├── NeuralNetwork.h
    │   └── Neuron.h
    └── presentation_tier
        └── ConsoleManager.h
```
## Building

Run this command to build the program:

```
$ make
```

## Running

Run this command to start the program:

```
$ ./Minichess
```

## Cleaning

Run this command to delete the generated files:

```
$ make clean
```

----- Work in progress -----
