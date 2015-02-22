# Genova
Genetic Algorithm designer

GENOVA is a Genetic algorithm designer made with Qt 5.4. It features a simple GUI to set the algorithm's parameters and supports scripting of the problem-specific fitness function that is central to every genetic algorithm. 

The script is written in QScript, Qt's ECMA-compliant scripting language (like JavaScript). It is a text file that can have any name but the function must be named fitness(). Because of Qt's meta-object compiler and some other design elements, a QScript function can be passed any number of arguments from the code even with no formal parameters in the script's function signature, and these arguments can be accessed through the special array called arguments[] even though it is not defined in the script.
In the current implementation, the first argument is the length of the bit string of the "genome" and the rest of the arguments are the bits.
