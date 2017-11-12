//////////////////////////////////////////////////////////////////////////////
// HexadokuSolver
//////////////////////////////////////////////////////////////////////////////

This little project is implemented as an automatic scanning application with
the purpose of solving Sudokus, in especially Hexadokus as provided by the
Elektor magazine.

The usage of this little application is pretty simple - attach some Imaging
device to your computer and hit the start button. The application will then
acquire the image from the Imaging device, do some Image processing magic and 
tries to classificate the type of Sudoku you have scanned in (user-confirmation
for correctness!).
After performing some Optical Character Recognition operations we will have 
some pre-filled-in Sudokus with a given type - now we are able to apply the 
Sudoku rules for the given data set and perform the solving mechanism.

The reason why I am doing this is just for establishing some 
reusability-oriented workflows for software construction with C++. A second 
criterion is learning the usage of the relatively complex Windows Imaging 
Acquision API (WIA 2.0). And of course: programming is fun... :)

NOTE: this whole application is a just-for-fun project - there are no costs 
(except my spare time), no time pressure and there is no product management 
mechanism or even product considerations. 

If you want to play with this application, have some questions or ideas - 
feel free to contact the author.

Falk Schilling <falk.schilling.de (at) ieee.org >