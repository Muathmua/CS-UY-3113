/**
* Author: Muath Alghamdi
* Assignment: Rise of the AI
* Date due: 2025-04-07, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
* --------------------------------------------------------
* KNOWN WEIRD BEHAVIOR (BUGS?)
* --------------------------------------------------------
* 1. Sometimes enemies require multiple switches to a scene to spawn.
*    This was also present in code recieved from the lectures, so it's likely an issue on my PC.
* 2. If played on a laptop whose performance decreases when not plugged to a charger, sonic's acceleration
*    is bugged.
* 3. Scene switches that occur when losing all 3 lives or completing all 3 levels rarely throw exceptions
*
* ---Behavior 1 and 3 is very variable, I am unsure how to debug it; as they both rely on code proven to work
*    in other scenarios.
*
* --------------------------------------------------------
* CONTROLS
* --------------------------------------------------------
*  Right arrow key --> move right
*  Left arrow key --> move left
*  Down arrow key --> roll
*  Space --> jump
*  A --> Spin dash left
*  D --> Spin dash right
*
*  0 --> Switch to menu
*  1 --> Switch to level 1
*  2 --> Switch to level 2
*  3 --> Switch to level 3
*  8 --> Switch to game win screen
*  9 --> Switch to game lose screen
**/

#include "Scene.h"
