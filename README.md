# Pentris
Pentomino based puzzle game

The gameplay loop involves having to arrange falling pentominoes in an uninterrupted line.

Includes a solver that works by enumerating simple terminal positions of the current pentomino and the next one, and optimizes for certain desirable game field attributes (e.g. low stack height, no overhangs).

The final-position-enumeration algorithm takes into account pentomino symmetries: Note that for a given pentomino, the enumeration of possible terminal position involves (among lateral and vertical translations) rotating and reflecting said pentomino. The translation step has complexity O(3n)=O(n) with n being the number of columns, since the algorithm looks at drops, drop-and-left, drop-and-right movement chains (right and left to fill in overhangs). Now, this translation step is done exactly once for each possible distinct orientation of a pentomino, hence taking into account its rotational and reflectional symmetries in order to be more efficient. (Mathematically speaking, the translation step is done exactly once for each orbit of the pentomino under the action of the Dihedral group D4. In an extreme case, the "x" shaped pentomino has full D4 as its symmetry group, i.e. it has only a single orbit under rotations and reflections and hence the enumeration complexity is lowest here. See https://en.wikipedia.org/wiki/Pentomino#Symmetry for details, and https://en.wikipedia.org/wiki/Dihedral_group)

![Pentris](https://github.com/BaranCanOener/Pentris/blob/main/Capture.PNG)
