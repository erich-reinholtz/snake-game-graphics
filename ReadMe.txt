For this week's assignment, I decided to add graphics to my previous program, the Snake game.
The player controls the direction in which the snake moves using the arrow keys.
The purpose is to collect as much food as possible. 
After eating each piece of food, the snake grows in length and a new random piece of food is generated.
The game ends when the snake crashes either into one of the walls or into itself.

The images required to run the program were found online, and are located in the "images" folder. I've also added a Makefile, 
in order to pass the -lSDL2 to the compiler. 

Probably the most tricky step in completing this program was adding the flip functions. 
Each component of the snake, depending on the current and previous directions, can be displayed by either two or four different images. 
Selecting the appropriate image for each component is the purpose of these new functions.
