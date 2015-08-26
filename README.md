# Genova
Genetic Algorithm designer

GENOVA is a Genetic algorithm designer made with Qt 5.4. It features a simple GUI to set the algorithm's parameters and supports scripting of the problem-specific fitness function that is central to every genetic algorithm. 

The script is written in QScript, Qt's ECMA-compliant scripting language. The script file can have any name but the function must be named fitness(). A QScript function can be passed any number of arguments from the code with no formal parameters in the script's function signature. In the current implementation, the first argument is the length of the bit string of the "genome" and the rest of the arguments are the bits.
