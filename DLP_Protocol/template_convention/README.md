# Template program and coding convention for DLP endpoint agent software

## Template program files
1. First in the file is a *prologue* that tells what is in that file. A description of the purpose of the objects in the files (whether they be functions, external data declarations or definitions, or something else) is more useful than a list of the object names. The prologue may optionally contain author(s), revision control information, references, etc.

2. Second *header file* includes. The reason of including should be commented. 

3. Third, *defines and typedefs* that apply to the file. One normal order is to have "constant" macros first, then "function" macros, then typedefs and enums.

4. Next come the global (external) data declarations, usually in the order: **externs, non-static globals, static globals**. If a set of defines applies to a particular piece of global data (such as a flags word), the defines should be immediately after the data declaration or embedded in structure declarations, indented to put the defines one level deeper than the first keyword of the declaration to which they apply.

5. The functions come last, and should be in some sort of meaningful order. Like functions should appear together. A "breadth-first" approach (functions on a similar level of abstraction together) is preferred over depth-first (functions defined as soon as possible before or after their calls). Considerable judgement is called for here. If defining large numbers of essentially-independent utility functions, consider alphabetical order.

## Naming Conventions
- Avoid nameing with leading and trailing underscores because they are reserved for system purposes
- Naming functions **descriptively**, avoid **ambiguity**, 
- Naming function in **snakecase** Ex: this_is_my_function()
- #define constants are all **CAPS**.
- Enum constants are **Capitalized**
- Function, typedef, and variable names, as well as struct, union, and enum tag names should be in **lower case**.
- Avoid names that differ only in case, like foo and Foo. Similarly, avoid foobar and foo_bar.
- Avoid names that look like each other. On many terminals and printers, 'l', '1' and 'I' look quite similar. A variable named 'l' is particularly bad because it looks so much like the constant '1'.