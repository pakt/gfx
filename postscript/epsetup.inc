/page-begin {
  newpath
  0 0 moveto
  w 0 lineto
  w h lineto
  0 h lineto
  clip
  gsave
  w 0.5 mul h 0.5 mul translate
  72 dup scale
  1 72 div setlinewidth
  1 setlinecap
  1 setlinejoin
} def

/page-end {
  grestore
  showpage
} def

/floor [0 1 0 3] def

/A 40 def

% -------------------------------------------

