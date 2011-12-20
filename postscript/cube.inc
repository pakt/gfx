% cube.inc

/cv [
[0 0 0]
[0 1 0]
[1 1 0]
[1 0 0]
[0 0 1]
[0 1 1]
[1 1 1]
[1 0 1]
] def

/cube [
% face 0: -z > 0
[
  [0 0 -1 0]
  [
    cv 0 get
    cv 1 get
    cv 2 get
    cv 3 get
  ]
]
% face 1: z - 1 > 0
[
  [0 0 1 -1]
  [
    cv 4 get
    cv 7 get
    cv 6 get
    cv 5 get
  ]
]
% face 2: -y > 0
[
  [0 -1 0 0]
  [
    cv 0 get
    cv 3 get
    cv 7 get
    cv 4 get
  ]
]
% face 3: y - 1 > 0
[
  [0 1 0 -1]
  [
    cv 1 get
    cv 5 get
    cv 6 get
    cv 2 get
  ]
]
% face 4: -x > 0
[
  [-1 0 0 0]
  [
    cv 0 get
    cv 4 get
    cv 5 get
    cv 1 get
  ]
]
% face 5: x - 1 > 0
[
  [1 0 0 -1]
  [
    cv 3 get
    cv 2 get
    cv 6 get
    cv 7 get
  ]
]

] def
