# Qt-Minesweeper
## Controls
* `LMB`: Open a cell.
* `RMB`: Place/remove a flag.
* `Middle Mouse Button (MMB)`: Quickly open all surrounding unopened cells if all nearby mines have been flagged.

## Features
* `Left-handed Mode`: Allows swapping the actions of left and right mouse buttons.
* `Flag Counter`: Displays the difference between the total number of mines and placed flags.
* `Question Mark on Flag`: Adds a question mark on a flagged cell using the right mouse button `(RMB)`. It doesn’t affect the mine counter and prevents opening the cell with the left mouse button `(LMB)`.
* `Auto Reveal`: Automatically opens all surrounding cells when an empty cell is clicked if there are no mines nearby.
* `Customizable Game`: Set the grid size and number of mines through a dialog box.
* `Save and Restore Game State`: The current game state is saved upon closing the application and restored on the next launch.
* `Debug Mode`: When compiled with the dbg argument, a button is displayed to reveal hidden mines.

## Demonstration
![showcase](https://github.com/999ashu/Qt-Minesweeper/blob/main/showcase.gif)

## `Debug Mode` Demonstration
![showcasedbg](https://github.com/999ashu/Qt-Minesweeper/blob/main/showcasedbg.gif)
