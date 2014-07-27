nBinaryPuzzle
=============

Binary Puzzle for the TI-NSpire calculator

About:
This is a puzzle game for the TI-nSpire!  It works on the TI-NSpire CX CAS with ndless 3.1 and should work on the TI-nspire models.  In each level you are to fill in the cells with ones and zeros according to the rules.  And by doing so you beat the level.  There is a few different sizes and difficulties.

I hope you enjoy it!

Rules:
1. Each cell must contain either a zero or a one.
2. There cannot be three or more consecutive ones or consecutive zeroes in any row or column
3. Each row and each column must contain an equal number of zeroes and ones.
4. Each row is unique and each column is unique.

How to put on calculator:
Start up TI-NSpire Student Software or whatever you use to put it on the calculator.  Then just drag-n-drop the nBinary-sdl.tns file into the application to transfer it to your calculator.  You have to have ndless 3.1 on your calculator in order to run it!

After you run it; you can add and play additional levels.  With ndless it allows start up files; and so you can add more level files to your calculator and play them!

How to play:
Navigate the menu using the arrow keys (or 2 and 8 keys) and the Enter key to start a new game.
Use the arrow keys to move the selector around the board
Type the number or cycle through the numbers with the Enter (or Ctrl) key.
Use the - and + keys to undo and redo your moves.
Also I added in the 'A' key to auto find a value.  You can use this if you are finding it tedious to complete certain parts of the puzzle.  There is some calculating part so if your calculator appears to stop (the moving background stops) for a little while (say 15 seconds) that is normal.

Extra levels:
This program works with external level files!  There is only a sample level included.  See the source code file BinaryLevels.h to see the make-up of the puzzle files should you want to make your own.  Extra level files which can be placed in any folder on the calculator.  A subfolder is a great place to put them.  When you first ran nBinary-sdl it registered the extension 'binary' and so because the levels have the extension 'binary' at the end it is associated with the nBinary-sdl program.  Let me know if you'd be interested in a level editor or additional levels!  The make-up of a level is rather simple; and so if you take a look at the source and/or the level file you'll probably be able to figure it out.

Source:
I included my source.  I programmed with C++ which you can find out more here: http://blog.tangrs.id.au/?p=712  It uses nSDL as well as the ndless Software Development Kit.

Contact Me:
A.J. Orians
E-mail: ajorians@gmail.com
