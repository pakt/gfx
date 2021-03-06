% - This is ps3d.inc ----------------------------------

% - Copyright Bill Casselman
% - Original version 1.0 November, 1998
% - Version 1.1 December, 1999
% -      Took out aliases for moveto etc.
% -      Made display-matrix a 3 x 4 homogeneous matrix, added it to the 3d gstack 
% -      Allowed arbitrary eye location, 3d -> 2d projects onto z = 0 plane
% -      Although fancy stuff not yet implemented
% -      Made ght a variable
% -      Version 1.1 is *not* backwards compatible!
% - Version 1.2 August, 2002
% -      Thorough interpretation of matrices as arrays of columns and point vectors as rows
% -      And some speed up
% -      Similar rewriting of matrix.inc
% -      Again, backwards incompatible!
% - Version 1.3 August, 2003
% -      Rewriting shade for efficiency
% - Thanks to Jim Blinn's book `A trip down the graphics pipeline'
% - for several suggestions that (I hope) made this code cleaner 
% - by suggesting how useful homogeneous coordinates were.
% November 10, 2003: added fancier shading
% December 17, 2003: changed arguments for mkpath procedures

% ------------------------------------------------


% - Inserting matrix.inc ----------------------

% - Vector calculations (usually good in any number of dimensions) ----------

% - matrices in this package are usually laid out in a single array by columns ---
% - i.e. [ column1 column 2 ... ]
% - but sometimes as a double array [ [ column1] [column2] ... ] 

% <double array>  /to-single-array
% <single-array> /to-double-array
% <n> /identity
% <u> <v> /dot-product
% <u> <c> /vector-scale
% <u> <v> /vector-add
% <u> <v> /vectorssub
% <u> /vector-length
% <u> <v> cross-product
% <axis> <angle> /rotation-matrix3d
% v [a b c] /euclidean-reflect
% [a b c] /reflection-matrix-3d
% <m> <n> /matrix-mul
% <m> <v> /matrix-vector
% <v> <m> /vector-matrix 
% <m> /transpose
% <m> 3x3-det
% <m> /3x3-inverse
% <u> <v> /angle-between
% <x> /acos
% <x> <a> <a^v> /skew-reflect
% <a> <a^v> /skew-reflection-matrix

% - matrices in this package are laid out in a single array by columns ---------

% a double array: cannot be empty - just lays out all items
/to-single-array {
% [ [. . . ][ . . . ] ] => [ . . . . . . ]
[
	exch { % successive rows
		aload pop
	} forall
]
} def

% ----------------------------------------------

% [ ... ] a square matrix made into an array of columns
/to-double-array { 4 dict begin
/A exch def
/N A length sqrt round cvi def
/i 0 def
[
N {
	[
	N {
		A i get
		/i i 1 add def
	} repeat
	]
} repeat
]
end } def

% ----------------------------------------

% returns the nxn identity matrix as single array
/identity { 1 dict begin
/n exch def
[
  n 1 sub {
    1 
    n {
      0
	} repeat 
  } repeat
  1
]
end } def

% --- vector algebra --------------------------------

% u v -> u.v
/dot-product { 1 dict begin
/v exch def
0 0	     		% u s i
3 2 roll { 		% s i u[i]
  v 			% s i u[i] v
  2 index get mul 	% s i u[i]*v[i]
  3 2 roll		% i u[i]*v[i] s
  add exch 1 add	% s i
} forall
pop
end } def

% v c -> c.v
/vector-scale { 1 dict begin
/c exch def
[ exch
{ 		% s i u[i]
  c mul			% s i u[i] v 
} forall
]
end } def

% u v -> u+v
/vector-add { 1 dict begin
/v exch def
[ exch
0 	     	% u i
exch { 		% i u[i]
  v 		% i u[i] v
  2 index get add 	% i u[i]+v[i]
  exch 1 add	% i
} forall
pop
]
end } def

% u v -> u-v
/vector-sub { 1 dict begin
/v exch def
[ exch
0 	     	% u i
exch { 		% i u[i]
  v 		% i u[i] v
  2 index get sub 	% i u[i]+v[i]
  exch 1 add	% i
} forall
pop
]
end } def

% [x y z ... ] -> r
% watch out for overflow

/vector-length { 1 dict begin
dup
% find maximum entry
/max 0 def
{ % max 
  abs dup max gt {
    % if abs gt max
    /max exch def
  } {
    pop
  } ifelse
} forall

max 0 ne {
  0 exch 
  {  % 0 v[i]
    max div dup mul add
  } forall
  sqrt
  max mul
} {
  pop 0
} ifelse
end } def

% v -> v/|v|
/normalized { 1 dict begin
dup 			% v v
vector-length /r exch def
[ exch
  {
    r div
  } forall
]
end } def

% u v
% u0 u1 u2
% v0 v1 v2
% -> u x v
/cross-product {
2 dict begin
/v exch def
/u exch def
[
  u 1 get v 2 get mul
  v 1 get u 2 get mul sub

  v 0 get u 2 get mul
  u 0 get v 2 get mul sub

  u 0 get v 1 get mul
  v 0 get u 1 get mul sub
]
end
} def

% --------------------------------------------------------------

% axis A -> a matrix
/rotation-matrix3d {
8 dict begin
dup 
cos /c exch def
sin /s exch def
/a exch def
/r a vector-length def
/a0 a 0 get r div def
/a1	a 1 get r div def
/a2 a 2 get r div def

[

% e = [1 0 0] etc.
% e0 = (e.a)a, e# = e - e0, e* = a x e = a x e0 + a x e# = a x e#
/x a0 def
/e0 [a0 x mul
     a1 x mul 
     a2 x mul] def
/e# [1 e0 0 get sub 
     e0 1 get neg 
     e0 2 get neg] def
% [a0 a1 a2]
% [ 1  0  0]
/e* [0 
     a2 
     a1 neg
    ] def
e# 0 get c mul e* 0 get s mul add e0 0 get add 
e# 1 get c mul e* 1 get s mul add e0 1 get add 
e# 2 get c mul e* 2 get s mul add e0 2 get add 

/x a1 def
/e0 [a0 x mul a1 x mul a2 x mul] def
/e# [e0 0 get neg 
     1 e0 1 get sub 
     e0 2 get neg] def
% [a0 a1 a2]
% [ 0  1  0]
/e* [a2 neg 
     0
     a0
    ] def
e# 0 get c mul e* 0 get s mul add e0 0 get add 
e# 1 get c mul e* 1 get s mul add e0 1 get add 
e# 2 get c mul e* 2 get s mul add e0 2 get add 

/x a2 def
/e0 [a0 x mul a1 x mul a2 x mul] def
/e# [e0 0 get neg
     e0 1 get neg
     1 e0 2 get sub] def
% [a0 a1 a2]
% [ 0  0  1]
/e* [a1 
     a0 neg
     0
    ] def
e# 0 get c mul e* 0 get s mul add e0 0 get add 
e# 1 get c mul e* 1 get s mul add e0 1 get add 
e# 2 get c mul e* 2 get s mul add e0 2 get add 

]
% [ r0 r1 r2 r3 r4 r5 r6 r7 r8 ] -> [r0 r3 r6 r1 r4 r7 r2 r5 r8 ]
/r exch def
[
	r 0 get 
	r 3 get
	r 6 get
	r 1 get
	r 4 get
	r 7 get
	r 2 get
	r 5 get
	r 8 get
] 
end
} def

% v a -> v - 2(a.v)/(a.a) a
/euclidean-reflect { 16 dict begin
/a exch def
/v exch def
/N a length def
/d a v dot-product a dup dot-product div 2 mul def
[
  0
  v {
    % i v[i]
    exch dup 1 add					% v[i] i i+1
	3 1 roll							  % i+1 v[i] i
    a exch get d mul 				% i+1 v[i] a[i]*d
	sub										% i+1 v[i]-d*a[i]
	exch									% rv[i] i+1
  } forall
  pop
]
end } def

% f = [A B C] => linear 3d transformation: f not necessarily normalized
% Rv = v - 2 <f', v> f'

/reflection-matrix3d {
3 dict begin
aload pop
/C exch def
/B exch def
/A exch def
/r [ A B C ] vector-length def
/A A r div def
/B B r div def
/C C r div def
[
  1 A A mul dup add sub   B A mul dup add neg   C A mul dup add neg
    A B mul dup add neg 1 B B mul dup add sub   C B mul dup add neg
    A C mul dup add neg   B C mul dup add neg 1 C C mul dup add sub
]
end
} def

/matrix-mul {
8 dict begin
/B exch def
/A exch def
/n A length sqrt round cvi def
%  0    1  ...
%  n  n+1 ...
% 2n 2n+1 ...
[
  0
  n {
    % i = initial index of the column on the stack = 0, n, 2n ... 
    % after all previous entries
    dup n add exch 0	
    n {
	  % i+n i j on stack
      % j = initial index of the row
	  2 copy 1 add % i+n i j i j+1
	  4 2 roll	% i+n i j+1 i j
	  /ell exch def
      /k exch def
	  % i+n i j+1 
      0
      n {
		% i+n i j+1 s
        A ell get B k get mul add
        /k k 1 add def
        /ell ell n add def
      } repeat
	  4 1 roll
	  % s i+n i j+1
   	} repeat
    pop pop	% s i+n
  } repeat
  pop
]
end
} def

% A v: A = [ column 1, column 2, ... , column n ]
/matrix-vector {
8 dict begin
/v exch def
/r v length def
/A exch def
/c A length r idiv def
[
  0 1 c 1 sub {
	/i exch def
    % i = initial index of the row
    0 0
    r {
		% s j on stack
		dup 1 add	% s j j+1
		3 1 roll		% j+1 s j
      	v exch get A i get mul add	% j+1 s
		exch
   	  	/i i r add def
    } repeat
	% s r
	pop
  } for
]
end
} def

% v A: A = [ column1 column2 ... ]
/vector-matrix {
8 dict begin
/A exch def
/v exch def
/c v length def
/r A length c idiv def
[
  /i 0 def 
  r {
    % i = initial index of the row
    /j 0 def
    0
    c {
      A i get v j get mul add
      /j j 1 add def
   	  /i i 1 add def
    } repeat
  } repeat
]
end
} def

% a square matrix m x m
% [i, j] = n*i + j

/transpose {
4 dict begin
/M exch def
/n M length sqrt round cvi def
[
/k 0 def
n {
  /i k def
  n { 
    M i get
    /i i n add def
  } repeat
  /k k 1 add def
} repeat
]

end
} def

/3x3-det {
1 dict begin
/m exch def

m 0 get 
m 4 get mul
m 8 get mul

m 1 get 
m 5 get mul
m 6 get mul
add

m 2 get 
m 3 get mul
m 7 get mul
add

m 2 get 
m 4 get mul
m 6 get mul
sub

m 1 get 
m 3 get mul
m 8 get mul
sub

m 0 get 
m 5 get mul
m 7 get mul
sub

end
} def

/3x3-inverse {
2 dict begin
/m exch def
/d m 3x3-det def
[
   m 4 get m 8 get mul 
   m 5 get m 7 get mul sub
   d div
   
   m 2 get m 7 get mul 
   m 1 get m 8 get mul sub
   d div
   
   m 1 get m 5 get mul 
   m 4 get m 2 get mul sub
   d div

   m 5 get m 6 get mul 
   m 3 get m 8 get mul sub
   d div
   
   m 0 get m 8 get mul 
   m 2 get m 6 get mul sub
   d div
   
   m 2 get m 3 get mul 
   m 0 get m 5 get mul sub
   d div

   m 3 get m 7 get mul 
   m 6 get m 4 get mul sub
   d div
   
   m 1 get m 6 get mul 
   m 0 get m 7 get mul sub
   d div
   
   m 0 get m 4 get mul 
   m 1 get m 3 get mul sub
   d div
]
end
} def

/acos {
dup dup % x x x
mul 1 sub neg % x 1-x^2
sqrt exch atan
} def

% u v

/angle-between {
dup vector-length
% u v |v|
3 1 roll
% |v| u v
1 index 
% |v| u v u
dot-product
% |v| u u.v
exch vector-length
% |v| u.v |u|
div exch div
acos
} def

% x a av -> x - <a, x> av

/skew-reflect { 4 dict begin
/av exch def
/a exch def
/x exch def
/d x a dot-product def
[
	0 1 x length 1 sub {
		/i exch def
		x i get av i get d mul sub
	} for
]
end } def

% a av -> matrix
/skew-reflection-matrix { 8 dict begin
/av exch def
/a exch def
/n a length def
[
	0 1 n 1 sub {
		/i exch def
		[ n {0} repeat ] dup i 1 put
		% e[i] 
		a av skew-reflect
	} for
]
to-single-array
transpose
end } def

% - closing matrix.inc ------------------------

% - Defining PostScript commands' equivalents ----------

% - Coordinates in three dimensions -------------------

% There are three steps to drawing something in 3D:
% 1.  Converting from user 3d coords to default 3d coords
% 2.  Projecting onto (x, y)-plane
% 3.  Drawing the image on that plane
% These are more or less independent.  
% The last step is completely controlled by the usual PS stuff.
% The first and second are handled by 3d stuff here.

% - Initialize and manipulate the 3d gstack -----------

/gmax 64 def
/ght 0 def

% gstack = [[t0 d0 dm0] [t1 d2 dm1] [t2 d2 dm2] ... [t(gmax-1) d(gmax-1) dm(gmax-1)] ]

% the ctm is t[ght]
% the dual ctm is at d[ght]
% they are 4 x 4
% display-matrix = 3 x 4

/gstack3d gmax array def

% start with orthogonal projection to positive z-axis
gstack3d 0 [
  [ 1 0 0 0 
    0 1 0 0
    0 0 1 0
    0 0 0 1] 
  [ 1 0 0 0 
    0 1 0 0
    0 0 1 0
    0 0 0 1]
  [ 1 0 0 0
    0 1 0 0
    0 0 0 1]
] put

/gsave3d {
  /ctm gstack3d ght get def
  /ght ght 1 add def 
  ght gmax eq {
    (3d graphics stack overflow!) ==
    quit
  } if
  gstack3d 
    ght
    [ ctm 0 get ctm 1 get ctm 2 get ] 
  put
} def

/grestore3d {
  /ght ght 1 sub def
  ght 0 lt {
     (3d graphics stack underflow!) ==
     quit
  } if
} def

% n - restores to depth n

/gpop3d {
/ght exch def
} def

% [T T* dm]: sets cgfx3d = [T T* dm]

/gset3d {
gstack3d  		% [T T* d] g 
ght			    % [T T* d] g ght
3 2 roll 		    % g ght [T T* d]
put 
} def

% => [T T* dm]

/cgfx3d {
  gstack3d ght get
} def

/ctm3d {
  gstack3d ght get 0 get
} def

/cim3d {
  gstack3d ght get 1 get
} def

/cdm3d {
  gstack3d ght get 2 get
} def

% cpt3d isthe last 3d point drawn to

/currentpoint3d {
cpt3d
cgfx3d 1 get transform3d
aload pop     % x y z w 
pop			
} def

% - Sets up projection into 2D -------------------
		
% O = [x y z w] 
% sets display-matrix perspective onto z = 0 plane

/display-matrix {
  cgfx3d 2 get
} def

% [z0 0 -x0 0
%  0 z0 -y0 0
%  0 0 -w0 z0]
% (transposed)
% gives perspective onto point in z=0 plane
% from [x0 y0 z0 w0]

/set-eye {
4 dict begin
aload pop
/w0 exch def
/z0 exch def
/y0 exch def
/x0 exch def
gstack3d ght get
2 
[ z0 0 x0 neg 0 
  0 z0 y0 neg 0
  0 0 w0 neg z0]
put
end
} def

/get-eye { 1 dict begin
/d display-matrix def
[d 2 get neg d 6 get neg d 0 get d 10 get neg]
end } def

/get-virtual-eye {
get-eye cgfx3d 1 get transform3d
} def

% - backwards compatibility -----------------------------

/origin { get-eye } def
/eye { get-eye } def
/set-display { set-eye } def

% - Manipulate the current transformation matrix -----

% x y z

/translate3d {
8 dict begin
/z exch def
/y exch def
/x exch def
/ctm cgfx3d def
/T ctm 0 get def
[
[
  T 0 get 
  T 1 get 
  T 2 get 
  T 0 get x  mul
  T 1 get y  mul
  add
  T 2 get z  mul
  add
  T 3 get 
  add
  
  T 4 get 
  T 5 get 
  T 6 get
  T 4 get x  mul
  T 5 get y  mul
  add
  T 6 get z  mul
  add
  T 7 get 
  add
   
  T 8 get 
  T 9 get 
  T 10 get
  T 8 get x  mul
  T 9 get y  mul
  add
  T 10 get z  mul
  add
  T 11 get 
  add
   
  T 12 get 
  T 13 get 
  T 14 get
  T 12 get x  mul
  T 13 get y  mul
  add
  T 14 get z  mul
  add
  T 15 get 
  add
]
/T ctm 1 get def
[
  T 0 get T 12 get x mul sub 
  T 1 get T 13 get x mul sub 
  T 2 get T 14 get x mul sub 
  T 3 get T 15 get x mul sub 

  T 4 get T 12 get y mul sub 
  T 5 get T 13 get y mul sub 
  T 6 get T 14 get y mul sub 
  T 7 get T 15 get y mul sub 

  T 8 get T 12 get z mul sub 
  T 9 get T 13 get z mul sub 
  T 10 get T 14 get z mul sub 
  T 11 get T 15 get z mul sub 

  T 12 get
  T 13 get
  T 14 get
  T 15 get
]
  ctm 2 get
]
end
gset3d
} def

% ------------------------------------------------------

% axis A

/rotate3d { 4 dict begin
rotation-matrix3d
/R exch def
/C cgfx3d def
/T C 0 get def
[
[
  % first row
  T 0 get R 0 get mul
  T 1 get R 3 get mul
  add
  T 2 get R 6 get mul
  add
  
  T 0 get R 1 get mul
  T 1 get R 4 get mul
  add
  T 2 get R 7 get mul
  add
  
  T 0 get R 2 get mul
  T 1 get R 5 get mul
  add
  T 2 get R 8 get mul
  add
  
  T 3 get
  
  % second row
  T 4 get R 0 get mul
  T 5 get R 3 get mul
  add
  T 6 get R 6 get mul
  add
  
  T 4 get R 1 get mul
  T 5 get R 4 get mul
  add
  T 6 get R 7 get mul
  add
  
  T 4 get R 2 get mul
  T 5 get R 5 get mul
  add
  T 6 get R 8 get mul
  add
  
  T 7 get
  
  % third row
  T 8 get R 0 get mul
  T 9 get R 3 get mul
  add
  T 10 get R 6 get mul
  add
  
  T 8 get R 1 get mul
  T 9 get R 4 get mul
  add
  T 10 get R 7 get mul
  add
  
  T 8 get R 2 get mul
  T 9 get R 5 get mul
  add
  T 10 get R 8 get mul
  add
  
  T 11 get
  
  % fourth row
  T 12 get R 0 get mul
  T 13 get R 3 get mul
  add
  T 14 get R 6 get mul
  add
  
  T 12 get R 1 get mul
  T 13 get R 4 get mul
  add
  T 14 get R 7 get mul
  add
  
  T 12 get R 2 get mul
  T 13 get R 5 get mul
  add
  T 14 get R 8 get mul
  add
  
  T 15 get
]
/T C 1 get def
% T = T^-1
% => R^-1 T^-1 
[
  R 0 get T 0 get mul
  R 3 get T 4 get mul add
  R 6 get T 8 get mul add
  
  R 0 get T 1 get mul
  R 3 get T 5 get mul add
  R 6 get T 9 get mul add
  
  R 0 get T 2 get mul
  R 3 get T 6 get mul add
  R 6 get T 10 get mul add
  
  R 0 get T 3 get mul
  R 3 get T 7 get mul add
  R 6 get T 11 get mul add
  
  % ------------------------
  
  R 1 get T 0 get mul
  R 4 get T 4 get mul add
  R 7 get T 8 get mul add
  
  R 1 get T 1 get mul
  R 4 get T 5 get mul add
  R 7 get T 9 get mul add
  
  R 1 get T 2 get mul
  R 4 get T 6 get mul add
  R 7 get T 10 get mul add
  
  R 1 get T 3 get mul
  R 4 get T 7 get mul add
  R 7 get T 11 get mul add
  
  % ------------------------
  
  R 2 get T 0 get mul
  R 5 get T 4 get mul add
  R 8 get T 8 get mul add
  
  R 2 get T 1 get mul
  R 5 get T 5 get mul add
  R 8 get T 9 get mul add
  
  R 2 get T 2 get mul
  R 5 get T 6 get mul add
  R 8 get T 10 get mul add
  
  R 2 get T 3 get mul
  R 5 get T 7 get mul add
  R 8 get T 11 get mul add

  T 12 get
  T 13 get
  T 14 get
  T 15 get  
]
  C 2 get
]
end
gset3d
} def


% f = [A B C D] P
% f = 0 is the *affine* reflection plane
% v = v* + v0 with v* on f = 0 and v0 in P-direction => v* - v0
% The map is Q => f(P)*Q - 2*f(Q)P
% It is of order two.
%
% f(P) I -
%
% 2A*P[0] 2B*P[0] 2C*P[0] 2D*P[0]
% 2A*P[1] 2B*P[1] 2C*P[1] 2D*P[1]
% 2A*P[2] 2B*P[2] 2C*P[2] 2D*P[2]
% 2A*P[3] 2B*P[3] 2C*P[3] 2D*P[3]
%
% Matrix = f(P) I - P f
% set s0 = (T row 0)*P
% T x this =
% f(P)T[0,0]-A*s -B*s -C*s -D*s

/affine-reflect3d { 4 dict begin
aload pop
/P3 exch def
/P2 exch def
/P1 exch def
/P0 exch def
aload pop
/D exch def
/C exch def
/B exch def
/A exch def

/fP 
  A P0 mul 
  B P1 mul add
  C P2 mul add
  D P3 mul add
def
/A A dup add def
/B B dup add def
/C C dup add def
/D D dup add def
[
/T cgfx3d 0 get def
[
  /s % = (T row 1)*P
  T 0 get P0 mul
  T 1 get P1 mul add
  T 2 get P2 mul add
  T 3 get P3 mul add
  def
  
  fP T 0 get mul
    A s mul sub
  fP T 1 get mul
    B s mul sub
  fP T 2 get mul
    C s mul sub
  fP T 3 get mul
    D s mul sub

  /s % = (T row 1)*P
  T 4 get P0 mul
  T 5 get P1 mul add
  T 6 get P2 mul add
  T 7 get P3 mul add
  def
  
  fP T 4 get mul
    A s mul sub
  fP T 5 get mul
    B s mul sub
  fP T 6 get mul
    C s mul sub
  fP T 7 get mul
    D s mul sub

  /s % = (T row 2)*P
  T 8 get P0 mul
  T 9 get P1 mul add
  T 10 get P2 mul add
  T 11 get P3 mul add
  def
  
  fP T 8 get mul
    A s mul sub
  fP T 9 get mul
    B s mul sub
  fP T 10 get mul
    C s mul sub
  fP T 11 get mul
    D s mul sub

  /s % = (T row 3)*P
  T 12 get P0 mul
  T 13 get P1 mul add
  T 14 get P2 mul add
  T 15 get P3 mul add
  def
  
  fP T 12 get mul
    A s mul sub
  fP T 13 get mul
    B s mul sub
  fP T 14 get mul
    C s mul sub
  fP T 15 get mul
    D s mul sub
]
/T cgfx3d 1 get def
/f0 		
  % f paired with columns of T
  T 0 get A mul
  T 4 get B mul add
  T 8 get C mul add
  T 12 get D mul add
def

/f1 		
  % f paired with columns of T
  T 1 get A mul
  T 5 get B mul add
  T 9 get C mul add
  T 13 get D mul add
def

/f2 		
  % f paired with columns of T
  T 2 get A mul
  T 6 get B mul add
  T 10 get C mul add
  T 14 get D mul add
def

/f3 		
  % f paired with columns of T
  T 3 get A mul
  T 7 get B mul add
  T 11 get C mul add
  T 15 get D mul add
def

[
  fP T 0 get mul
    f0 P0 get mul sub
  fP T 1 get mul
    f1 P0 get mul sub
  fP T 2 get mul
    f2 P0 get mul sub
  fP T 3 get mul
    f3 P0 get mul sub
  fP T 4 get mul
    f0 P1 get mul sub
  fP T 5 get mul
    f1 P1 get mul sub
  fP T 6 get mul
    f2 P1 get mul sub
  fP T 7 get mul
    f3 P1 get mul sub
  fP T 8 get mul
    f0 P2 get mul sub
  fP T 9 get mul
    f1 P2 get mul sub
  fP T 10 get mul
    f2 P2 get mul sub
  fP T 11 get mul
    f3 P2 get mul sub
  fP T 12 get mul
    f0 P3 get mul sub
  fP T 13 get mul
    f1 P3 get mul sub
  fP T 14 get mul
    f2 P3 get mul sub
  fP T 15 get mul
    f3 P3 get mul sub
]
  cgfx3d 2 get
] 
end
gset3d
} def

% 3x3 M 

/concat3d {
4 dict begin
/M exch def
[
/T cgfx3d 0 get def
  [
    T 0 get M 0 get mul
    T 1 get M 3 get mul add
    T 2 get M 6 get mul add

    T 0 get M 1 get mul
    T 1 get M 4 get mul add
    T 2 get M 7 get mul add
     
    T 0 get M 2 get mul
    T 1 get M 5 get mul add
    T 2 get M 8 get mul add
    
    T 3 get
     
    T 4 get M 0 get mul
    T 5 get M 3 get mul add
    T 6 get M 6 get mul add

    T 4 get M 1 get mul
    T 5 get M 4 get mul add
    T 6 get M 7 get mul add
     
    T 4 get M 2 get mul
    T 5 get M 5 get mul add
    T 6 get M 8 get mul add
    
    T 7 get
     
    T 8 get M 0 get mul
    T 9 get M 3 get mul add
    T 10 get M 6 get mul add

    T 8 get M 1 get mul
    T 9 get M 4 get mul add
    T 10 get M 7 get mul add
     
    T 8 get M 2 get mul
    T 9 get M 5 get mul add
    T 10 get M 8 get mul add
    
    T 11 get
     
    T 12 get M 0 get mul
    T 13 get M 3 get mul add
    T 14 get M 6 get mul add

    T 12 get M 1 get mul
    T 13 get M 4 get mul add
    T 14 get M 7 get mul add
     
    T 12 get M 2 get mul
    T 13 get M 5 get mul add
    T 14 get M 8 get mul add
    
    T 15 get
     
  ]
/T cgfx3d 1 get def
/M M 3x3-inverse def
  [
    M 0 get T 0 get mul
    M 1 get T 4 get mul add
    M 2 get T 8 get mul add
    
    M 0 get T 1 get mul
    M 1 get T 5 get mul add
    M 2 get T 9 get mul add
    
    M 0 get T 2 get mul
    M 1 get T 6 get mul add
    M 2 get T 10 get mul add
    
    M 0 get T 3 get mul
    M 1 get T 7 get mul add
    M 2 get T 11 get mul add
    
    % -----------------------------
    
    M 3 get T 0 get mul
    M 4 get T 4 get mul add
    M 5 get T 8 get mul add
    
    M 3 get T 1 get mul
    M 4 get T 5 get mul add
    M 5 get T 9 get mul add
    
    M 3 get T 2 get mul
    M 4 get T 6 get mul add
    M 5 get T 10 get mul add
    
    M 3 get T 3 get mul
    M 4 get T 7 get mul add
    M 5 get T 11 get mul add
    
    % -----------------------------
    
    M 6 get T 0 get mul
    M 7 get T 4 get mul add
    M 8 get T 8 get mul add
    
    M 6 get T 1 get mul
    M 7 get T 5 get mul add
    M 8 get T 9 get mul add
    
    M 6 get T 2 get mul
    M 7 get T 6 get mul add
    M 8 get T 10 get mul add
    
    M 6 get T 3 get mul
    M 7 get T 7 get mul add
    M 8 get T 11 get mul add
    
    % -----------------------------
    
    T 12 get
    T 13 get
    T 14 get
    T 15 get
  ]
  cgfx3d 2 get
]
end
gset3d
} def

%
% v => v - 2 <v, a> a
%
% Matrix = I - 2 a a
% a

/reflect3d { 4 dict begin
reflection-matrix3d 
concat3d
end } def

% [x y z w] [a00 a01 a02 a03 ... ]
% but the vector is a linear function
% so it is multiplying by transpose
% if T is the current ctm3d, the point P -> P T transform3d
% but the plane f=0 -> f T^{-1} dual-transform3d

/dual-transform3d { 4 dict begin
/v exch def
/T exch def
[
  v 0 get T 0 get mul
  v 1 get T 4 get mul add
  v 2 get T 8 get mul add
  v 3 get T 12 get mul add
  
  v 0 get T 1 get mul
  v 1 get T 5 get mul add
  v 2 get T 9 get mul add
  v 3 get T 13 get mul add
  
  v 0 get T 2 get mul
  v 1 get T 6 get mul add
  v 2 get T 10 get mul add
  v 3 get T 14 get mul add
  
  v 0 get T 3 get mul
  v 1 get T 7 get mul add
  v 2 get T 11 get mul add
  v 3 get T 15 get mul add
]
end } def

% 4d to 3d homogeneous
/project3d { 4 dict begin
/T exch def
/v exch def
[
  T 0 get v 0 get mul
  T 1 get v 1 get mul
  add 
  T 2 get v 2 get mul 
  add
  T 3 get v 3 get mul 
  add

  T 4 get v 0 get mul
  T 5 get v 1 get mul
  add 
  T 6 get v 2 get mul 
  add
  T 7 get v 3 get mul 
  add

  T 8 get v 0 get mul
  T 9 get v 1 get mul
  add 
  T 10 get v 2 get mul 
  add
  T 11 get v 3 get mul 
  add
] 
end } def

% [x y z w] [a00 a01 a02 a03 ... ]

/transform3d {
4 dict begin
/T exch def
/v exch def
[
T 0 get v 0 get mul
T 1 get v 1 get mul
add 
T 2 get v 2 get mul 
add
T 3 get v 3 get mul 
add

T 4 get v 0 get mul
T 5 get v 1 get mul
add 
T 6 get v 2 get mul 
add
T 7 get v 3 get mul 
add

T 8 get v 0 get mul
T 9 get v 1 get mul
add 
T 10 get v 2 get mul 
add
T 11 get v 3 get mul 
add

T 12 get v 0 get mul
T 13 get v 1 get mul
add 
T 14 get v 2 get mul 
add
T 15 get v 3 get mul 
add

] 
end
} def

% sx sy sz

/scale3d {
8 dict begin
/sz exch def
/sy exch def
/sx exch def
/T cgfx3d 0 get def
[
[
  T 0 get sx mul
  T 1 get sy mul
  T 2 get sz mul
  T 3 get 
  
  T 4 get sx mul
  T 5 get sy mul
  T 6 get sz mul
  T 7 get 
  
  T 8 get sx mul
  T 9 get sy mul
  T 10 get sz mul
  T 11 get 
 
  T 12 get sx mul
  T 13 get sy mul
  T 14 get sz mul
  T 15 get 
]
/T cgfx3d 1 get def
[
  T 0 get sx div
  T 1 get sx div
  T 2 get sx div
  T 3 get sx div
  
  T 4 get sy div
  T 5 get sy div
  T 6 get sy div
  T 7 get sy div
  
  T 8 get sz div
  T 9 get sz div
  T 10 get sz div
  T 11 get sz div
 
  T 12 get 
  T 13 get 
  T 14 get 
  T 15 get 
]
  cgfx3d 2 get
]
end
gset3d
} def

% [ <9> ] i

/row {
4 dict begin
/i exch def
/a exch def
a length 9 eq {
  /i i 3 mul def
  /n i 2 add def
} {
  /i i 4 mul def
  /n i 3 add def
} ifelse
[
  i 1 n {
    a exch get
  } for
]
end
} def

% projects from P onto f=Ax+By+Cz+D=0
% two parameters: f = [A B C D] and P
% The map is Q => f(P)*Q - f(Q)P
% It is idempotent.
%
% f(P) I -
%
% A*P[0] A*P[1] A*P[2] A*P[3]
% B*P[0] B*P[1] B*P[2] B*P[3]
% C*P[0] C*P[1] C*P[2] C*P[3]
% D*P[0] D*P[1] D*P[2] D*P[3]
%
% Matrix = f(P) I - P f
% set s0 = (T row 0)*P
% T x this =
% f(P)T[0,0]-A*s -B*s -C*s -D*s

/plane-project {
12 dict begin
aload pop
/P3 exch def
/P2 exch def
/P1 exch def
/P0 exch def
aload pop
/D exch def
/C exch def
/B exch def
/A exch def

/fP 
  A P0 mul 
  B P1 mul add
  C P2 mul add
  D P3 mul add
def
[
/T cgfx3d 0 get def
[
  /s % = (T row 0)*P
  	T 0 get P0 mul
  	T 1 get P1 mul add
  	T 2 get P2 mul add
  	T 3 get P3 mul add
  def
  
  fP T 0 get mul
    A s mul sub
  fP T 1 get mul
    B s mul sub
  fP T 2 get mul
    C s mul sub
  fP T 3 get mul
    D s mul sub


  /s % = (T row 1)*P
  T 4 get P0 mul
  T 5 get P1 mul add
  T 6 get P2 mul add
  T 7 get P3 mul add
  def
  
  fP T 4 get mul
    A s mul sub
  fP T 5 get mul
    B s mul sub
  fP T 6 get mul
    C s mul sub
  fP T 7 get mul
    D s mul sub


  /s % = (T row 2)*P
  T 8 get P0 mul
  T 9 get P1 mul add
  T 10 get P2 mul add
  T 11 get P3 mul add
  def
  
  fP T 8 get mul
    A s mul sub
  fP T 9 get mul
    B s mul sub
  fP T 10 get mul
    C s mul sub
  fP T 11 get mul
    D s mul sub

  /s % = (T row 3)*P
  T 12 get P0 mul
  T 13 get P1 mul add
  T 14 get P2 mul add
  T 15 get P3 mul add
  def
  
  fP T 12 get mul
    A s mul sub
  fP T 13 get mul
    B s mul sub
  fP T 14 get mul
    C s mul sub
  fP T 15 get mul
    D s mul sub
]
/T cgfx3d 1 get def
[
  /s T 0 get T 1 get add T 2 get add T 3 get add def
  s A mul
  s B mul
  s C mul
  s D mul
  /s T 4 get T 5 get add T 6 get add T 7 get add def
  s A mul
  s B mul
  s C mul
  s D mul
  /s T 8 get T 9 get add T 10 get add T 11 get add def
  s A mul
  s B mul
  s C mul
  s D mul
  /s T 12 get T 13 get add T 14 get add T 15 get add def
  s A mul
  s B mul
  s C mul
  s D mul
]
  cgfx3d 2 get
] 
end
gset3d
} def

% - Drawing commands in 3D --------------------------

% P displayed = [x y w z] => [X Y W] -> [X/W Y/W]

/render {
8 dict begin
aload pop
/v3 exch def
/v2 exch def
/v1 exch def
/v0 exch def
/T display-matrix def
/x
  T 0 get v0 mul 
  T 1 get v1 mul add
  T 2 get v2 mul add
  T 3 get v3 mul add
def
/y 
  T 4 get v0 mul 
  T 5 get v1 mul add
  T 6 get v2 mul add
  T 7 get v3 mul add
def
/w 
  T 8 get v0 mul 
  T 9 get v1 mul add
  T 10 get v2 mul add
  T 11 get v3 mul add
def
w 0 eq {
  (Perspective: division by zero!) ==
  quit
} if
x w div
y w div
end
} def

%  x y z -> x y
/transformto2d {
[ 4 1 roll 1 ] ctm3d transform3d render 
} def

/cpt3d 4 array def
/lm3d 4 array def

% cpt3d is a point in the "real" 3d world

% Should we build the current 3d path for reuse?

% x y z
/moveto3d {
1 [ 5 1 roll ] 
cgfx3d 0 get 
transform3d 
aload pop cpt3d astore 
render moveto
cpt3d aload pop lm3d astore pop
} def

% x y z

/lineto3d {
1 [ 5 1 roll ] 
cgfx3d 0 get 
transform3d 
aload pop cpt3d astore 
render lineto
} def

% x y z

/rmoveto3d {
0 [ 5 1 roll ] cgfx3d 0 get
transform3d 			% [dx dy dz 0]
dup 0 get cpt3d 0 get add	% [dx dy dz 0] x+dx
exch 
dup 1 get cpt3d 1 get add	% x+dx [dx dy dz dw] y+dy
exch
dup 2 get cpt3d 2 get add	% x+dx x+dy [dx dy dz dw] z+dz
exch				% x+dx y+dy z+dz [dx dy dz dw]
3 get cpt3d 3 get add		% x+dx x+dy z+dz w+dw
cpt3d astore
render moveto
cpt3d aload pop lm3d astore pop
} def

% x y z

/rlineto3d {
0 [ 5 1 roll ] cgfx3d 0 get
transform3d 		% [dx dy dz 0]
dup 0 get cpt3d 0 get add	% [dx dy dz 0] x+dx
exch 
dup 1 get cpt3d 1 get add	% x+dx [dx dy dz dw] y+dy
exch
dup 2 get cpt3d 2 get add	% x+dx x+dy [dx dy dz dw] z+dz
exch				% x+dx y+dy z+dz [dx dy dz dw]
3 get cpt3d 3 get add		% x+dx x+dy z+dz w+dw
cpt3d astore  
render lineto
} def

% x1 y1 z1 x2 y2 z2 x3 y3 z3 

/curveto3d { 16 dict begin
/z3 exch def
/y3 exch def
/x3 exch def
/z2 exch def
/y2 exch def
/x2 exch def
/z1 exch def
/y1 exch def
/x1 exch def
% F(t) 
/P0 cpt3d display-matrix project3d def
/P1 [x1 y1 z1 1] cgfx3d 0 get transform3d display-matrix project3d def
/w P0 2 get def
/c P1 2 get w div 1 sub def
/x0 P0 0 get w div def
/x1 P1 0 get w div def
/y0 P0 1 get w div def
/y1 P1 1 get w div def
x1 x0 c mul sub 
y1 y0 c mul sub 
[x3 y3 z3 1] cgfx3d 0 get transform3d aload pop cpt3d astore 
display-matrix project3d /P3 exch def
/P2 [x2 y2 z2 1] cgfx3d 0 get transform3d display-matrix project3d def
% We are assuming the display-matrix has images on { z = 0 }
/w P3 2 get def
/c P2 2 get w div 1 sub def
/x3 P3 0 get w div def
/x2 P2 0 get w div def
/y3 P3 1 get w div def
/y2 P2 1 get w div def
x2 x3 c mul sub 
y2 y3 c mul sub 
x3 y3
curveto
end } def

% - are the next two used? --------------------------------

% dP dQ Q

/dcurveto3d { 16 dict begin
/z3 exch def
/y3 exch def
/x3 exch def
/dz3 exch def
/dy3 exch def
/dx3 exch def
/dz0 exch def
/dy0 exch def
/dx0 exch def
/P0 cpt3d display-matrix project3d def
/dP0 [dx0 dy0 dz0 0] cgfx3d 0 get transform3d display-matrix project3d def
/w P0 3 get def 
% c = 1 - w'/w
/c 1 dP0 3 get w div sub def
  P0 0 get w div c mul dP0 0 get w div add
  P0 1 get w div c mul dP0 1 get w div add
[x3 y3 z3 1] cgfx3d 0 get transform3d aload pop cpt3d astore 
display-matrix project3d /P3 exch def
/dP3 [dx3 dy3 dz3 0] cgfx3d 0 get transform3d display-matrix project3d def
/w P3 3 get def
/c 1 dP3 3 get w div add def
  P3 0 get w div c mul dP3 0 get w div sub
  P3 1 get w div c mul dP3 1 get w div sub
  P3 0 get w div
  P3 1 get w div
curveto
end
} def

% dP dQ Q

/dcurveto { 8 dict begin
/y3 exch def
/x3 exch def
/dy3 exch def
/dx3 exch def
/dy0 exch def
/dx0 exch def
currentpoint dy0 add exch dx0 add exch
x3 dx3 sub y3 dy3 sub
x3 y3 
curveto
end } def

% - are the above two used? ----------------------------

/closepath3d {
closepath
lm3d aload pop cpt3d astore pop
} def

% - Conversion from 2d to 3D ---------------------------

/2d-path-convert {
[
  { % x y
    [ 3 1 roll 0 {moveto3d}]
  }
  { % x y
    [ 3 1 roll 0 {lineto3d}]
  }
  { % x1 y1 x2 y2 x3 y3
    [ 7 1 roll
    0 5 1 roll
    0 3 1 roll
    0 {curveto3d} ]
  }
  {
    [ {closepath3d} ]
  }
  pathforall
]
newpath
{
  aload pop exec
} forall
} def

% -----------------------------------------------

% For a simple currentpoint:

/mkpath3dDict 8 dict def

mkpath3dDict begin

/pathcount {
0 
{pop pop 
  pop 1 exit}
{pop pop 
  pop 1 exit}
{pop pop pop pop pop pop 
  pop 1 exit}
{ pop 1 exit}
pathforall
} def

/thereisacurrentpoint {
pathcount
0 gt
{true}
{false}
ifelse
} def

end

% ---------------------------------------------

% stack: t0 t1 N [parameters] /f 

/mkpath3d { load 
/f exch def
/pars exch def
12 dict begin
/N exch def
/t1 exch def
/t exch def

/h t1 t sub N div def
/h3 h 0.333333 mul def

pars t f aload pop
/velocity exch def
/position exch def
/p [ position aload pop 1 ] 
cgfx3d 0 get transform3d display-matrix project3d def
/v [ velocity aload pop 0 ] 
cgfx3d 0 get transform3d display-matrix project3d def

% p v = homogeneous position and velocity
% p/p[3] v/p[3] - (v[3]/p[3])(p/p[3]) = inhomogeneous ones
% = p/w v/w - c*p/w

/w p 3 get def
/c v 3 get w div def
thereisacurrentpoint {
  p 0 get w div p 1 get w div lineto
} {
  p 0 get w div p 1 get w div moveto
} ifelse

N {				% x y = currentpoint
  p 0 get 
    v 0 get 
    p 0 get c mul 
    sub 
    h3 mul 
  add 
  w div
  
  p 1 get 
    v 1 get 
    p 1 get c mul 
    sub 
    h3 mul 
  add
  w div

/t t h add def

pars t f aload pop
/velocity exch def
/position exch def
/p [ position aload pop 1 ] 
cgfx3d 0 get transform3d display-matrix project3d def
/v [ velocity aload pop 0 ] 
cgfx3d 0 get transform3d display-matrix project3d def
/w p 3 get def
/c v 3 get w div def

  p 0 get 
    v 0 get 
    p 0 get c mul 
    sub 
    h3 mul 
  sub 
  w div
  
  p 1 get 
    v 1 get 
    p 1 get c mul 
    sub 
    h3 mul 
  sub
  w div

p 0 get w div
p 1 get w div
curveto

} repeat
end % local dict
} def

% makes polygon out of control points

/mkcontrolpath3d { load
mkpath3dDict begin
1 dict begin
/f exch def
/pars exch def
/N exch def
/t1 exch def
/t exch def

/h t1 t sub N div def
/h3 h 0.333333 mul def

pars t f aload pop
/velocity exch def
/position exch def

position aload pop
thereisacurrentpoint {
	lineto3d
} {
	moveto3d
} ifelse

N {				% x y = currentpoint
% currentpoint pixel pop

position 0 get
velocity 0 get		% x dx/dt
h3 mul add
position 1 get
velocity 1 get		% y dy/dt
h3 mul add
position 2 get
velocity 2 get		% z dz/dt
h3 mul add

lineto3d

/t t h add def

pars t f aload pop
/velocity exch def
/position exch def

position 0 get
velocity 0 get 
h3 mul sub
position 1 get
velocity 1 get 
h3 mul sub
position 2 get
velocity 2 get 
h3 mul sub

lineto3d

position 0 get
position 1 get
position 2 get

lineto3d

} repeat
end % local dict
end % mkpath3d dict
} def

% -----------------------------------------------

% makes polygon from endpoints

/mkpolypath3d { load
/f exch def
/pars exch def
mkpath3dDict begin
1 dict begin
/N exch def
/t1 exch def
/t exch def

/h t1 t sub N div def
/h3 h 0.333333 mul def

pars t f aload pop
/velocity exch def
/position exch def

position aload pop
thereisacurrentpoint {
	lineto3d
} {
	moveto3d
} ifelse

N {				% x y = currentpoint
% currentpoint pixel pop

/t t h add def

pars t f aload pop
/velocity exch def
/position exch def

position 0 get
position 1 get
position 2 get
lineto3d

} repeat
end % local dict
end % mkpath3d dict
} def

% ---------------------------------------------

% length width

/plainarrow3d {
5 dict begin
/shaftwidth exch def
/arrowlength exch def
/headwidth shaftwidth 3 mul def
/headlength headwidth def
/shaftlength arrowlength shaftwidth 2.5 mul sub def
0 0 0 moveto3d
0 shaftwidth 0.5 mul 0 lineto3d
% shaftlength 0 0 rlineto3d
shaftlength shaftwidth 0.5 mul 0 lineto3d
arrowlength headlength sub headwidth 0.5 mul 0 lineto3d
arrowlength 0 0 lineto3d
arrowlength headlength sub headwidth -0.5 mul 0 lineto3d
shaftlength shaftwidth -0.5 mul 0 lineto3d
0 shaftwidth -0.5 mul 0 lineto3d
0 0 0 lineto3d
end
} def

% length width

/plainarrowtail3d {
5 dict begin
/shaftwidth exch def
/arrowlength exch def
0 0 0 moveto3d
0 shaftwidth 0.5 mul 0 lineto3d
arrowlength 0 0 rlineto3d
0 shaftwidth neg 0 rlineto3d
arrowlength neg 0 0 rlineto3d
0 0 0 lineto3d
end
} def

% --- shading ------------------------------------------------------------------

% all the shade input routines have as one argument a number between -1 and 1
% the result of a calculation of the dot-product of two unit vectors

% linear interpolation: s [a b] -> a + (b-a)*t

/lshade { % s [a b]
exch 1 add 2 div 	% t in [0 1]
% [a b] t
exch aload 0 get		% t a b a
4 1 roll 				% a t a b
sub mul sub			% a - t(a-b)
} def

%  # in [-1 1] & coefficient array [A B C D]: 
% A etc = control points, A = min, D = max
% 1 = facing towards, -1 = facing away from light
% x -> (x+1)/2 = 0.5(x+1) takes [-1, 1] -> [0, 1]
% evaluate by Horner's method

/shade {                             % t [array]
exch                                 % [array] t
1 add 0.5 mul 	                     % a t now in [0 1]
1 1 index sub                        % a t s=1-t
dup dup mul                          % a t s s^2
dup 2 index mul                      % a t s s^2 s^3
5 -1 roll aload pop                  % t s s^2 s^3 a0 a1 a2 a3=P0
7 index mul                          % t s s^2 s^3 a0 a1 a2 a3.t
exch                                 % t s s^2 s^3 a0 a1 a3.t a2
7 -1 roll                            % t s^2 s^3 a0 a1 a3.t a2 s
mul 3 mul add                        % t s^2 s^3 a0 a1 a3.t+3.a2.s=P1
5 index mul                          % t s^2 s^3 a0 a1 P1.t
exch                                 % t s^2 s^3 a0 P1.t a1
5 -1 roll                            % t s^3 a0 P1.t a1 s^2
mul 3 mul add                        % t s^3 a0 P1.t+3.a1.s^2=P2
4 -1 roll mul                        % s^3 a0 P2.t
3 1 roll mul add                     % P2.t + a0.s^3=P3
} def

% t y=[ y0 y1 ... yn ]
/bernstein { % t y 
  % constants y n t s=1-t
  % variables k C P
  dup length % t y n+1
  1 sub      % t y n
  3 -1 roll 1 % y n t 1
  1 index sub % y n t s
  % constants in place
  1           % y n t s k
  3 index 3 index mul % y n t s k C=nt
  5 index 0 get       % y n t s k C P=y0
  5 index {           % y n t s k C P
    % P -> s.P + C.y[k]
    % C -> C.t.(n-k)/(k+1) 
    % k -> k+1
    3 index mul       % y n t s k C P.s
    1 index           % y n t s k C P.s C
    7 index           % y n t s k C P.s C y 
    4 index get mul add  % y n t s k C P.s+C.y[k]=new P
    3 1 roll          % y n t s P* k C
    5 index           % y n t s P* k C n
    2 index sub mul   % y n t s P* k C.(n-k)
    1 index 1 add div % y n t s P* k C.(n-k)/(k+1)
    4 index mul       % y n t s P* k C*
    3 1 roll 1 add    % y n t s C* P* k*
    3 1 roll          % y n t s k* C* P*
  } repeat
  7 1 roll 6 { pop } repeat
} def

% shading: s in [-1 1] and y a Bernstein array B -> t -> B(t)
/bshade { exch 
1 add 2 div 
exch bernstein
} def

% ---------------------------------------------------------------------------------

% input: [pars] /fcn s0 s1 t0 t1 ns nt
% the fcn: [pars] s t -> f_{pars}(s, t)
% output: a polygonal surface of faces [ normal-fcn triangle ]

/mksurface {
16 dict begin
/nt exch def
/ns exch def
/t1 exch def
/t0 exch def
/s1 exch def
/s0 exch def
/ds s1 s0 sub ns div def
/dt t1 t0 sub nt div def
/f exch cvx def
/pars exch def
/P [
  /s s0 def
  ns 1 add {
    [
      /t t0 def
      nt 1 add {
        pars s t f
        /t t dt add def
      } repeat
    ]
    /s s ds add def
  } repeat
] def
% P[i][j] = f(s0 + i.ds, t0 + j.dt)
[
  0 1 ns 1 sub {
    /i exch def
    0 1 nt 1 sub {
      /j exch def
      % an array of triangles (i, j, 0) + (i, j, 1) 
      % dividing the rectangles in two
      /P00 P i get j get def
      /P10 P i 1 add get j get def
      /P01 P i get j 1 add get def
      /P11 P i 1 add get j 1 add get def
      % normal
      /Q P10 P00 vector-sub P01 P00 vector-sub cross-product def
      /r Q vector-length def
      r 0 ne {
        [
          [ Q aload pop Q P10 dot-product neg ]
          % array of pointers to three vertices
          [ P00 P10 P01 ]
        ] 
      } if
      /Q P01 P11 vector-sub P10 P11 vector-sub cross-product def
      /r Q vector-length def
      r 0 ne {
        [
          [ Q aload pop Q P01 dot-product neg ]
          % array of pointers to three vertices
          [ P10 P11 P01 ]
        ]
      } if
    } for
  } for
]
end
} def

% an array of vertices 
% traversed according to right hand rule
% output normalized

/normal-function {
2 dict begin
/a exch def
/n 
  a 1 get a 0 get vector-sub 
  a 2 get a 1 get vector-sub 
  cross-product 
def
/r n 0 get dup mul n 1 get dup mul add n 2 get dup mul add sqrt def
r 0 gt {
  /n [ n 0 get r div n 1 get r div n 2 get r div ] def
  [
    n aload pop
    a 0 get n dot-product neg
  ]
}{ [] } ifelse
end
} def

% --- light ------------------------------------------------

% should be part of the graphics environment

/set-light { /light-source exch def } def

/get-virtual-light {
  light-source cgfx3d 1 get transform3d
} def






