﻿/* Copyright (C) 2002 Jean-Marc Valin 
   File: high_lsp_tables.c
   Codebooks for high-band LSPs in SB-CELP mode
  
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

   1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.  

   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/
 
const signed char high_lsp_cdbk[512]={
39,12,-14,-20,-29,-61,-67,-76,
-32,-71,-67,68,77,46,34,5,
-13,-48,-46,-72,-81,-84,-60,-58,
-40,-28,82,93,68,45,29,3,
-19,-47,-28,-43,-35,-30,-8,-13,
-39,-91,-91,-123,-96,10,10,-6,
-18,-55,-60,-91,-56,-36,-27,-16,
-48,-75,40,28,-10,-28,35,9,
37,19,1,-20,-31,-41,-18,-25,
-35,-68,-80,45,27,-1,47,13,
0,-29,-35,-57,-50,-79,-73,-38,
-19,5,35,14,-10,-23,16,-8,
5,-24,-40,-62,-23,-27,-22,-16,
-18,-46,-72,-77,43,21,33,1,
-80,-70,-70,-64,-56,-52,-39,-33,
-31,-38,-19,-19,-15,32,33,-2,
7,-15,-15,-24,-23,-33,-41,-56,
-24,-57,5,89,64,41,27,5,
-9,-47,-60,-97,-97,-124,-20,-9,
-44,-73,31,29,-4,64,48,7,
-35,-57,0,-3,-26,-47,-3,-6,
-40,-76,-79,-48,12,81,55,10,
9,-24,-43,-73,-57,-69,16,5,
-28,-53,18,29,20,0,-4,-11,
6,-13,23,7,-17,-35,-37,-37,
-30,-68,-63,6,24,-9,-14,3,
21,-13,-27,-57,-49,-80,-24,-41,
-5,-16,-5,1,45,25,12,-7,
3,-15,-6,-16,-15,-8,6,-13,
-42,-81,-80,-87,14,1,-10,-3,
-43,-69,-46,-24,-28,-29,36,6,
-43,-56,-12,12,54,79,43,9,
54,22,2,8,-12,-43,-46,-52,
-38,-69,-89,-5,75,38,33,5,
-13,-53,-62,-87,-89,-113,-99,-55,
-34,-37,62,55,33,16,21,-2,
-17,-46,-29,-38,-38,-48,-39,-42,
-36,-75,-72,-88,-48,-30,21,2,
-15,-57,-64,-98,-84,-76,25,1,
-46,-80,-12,18,-7,3,34,6,
38,31,23,4,-1,20,14,-15,
-43,-78,-91,-24,14,-3,54,16,
0,-27,-28,-44,-56,-83,-92,-89,
-3,34,56,41,36,22,20,-8,
-7,-35,-42,-62,-49,3,12,-10,
-50,-87,-96,-66,92,70,38,9,
-70,-71,-62,-42,-39,-43,-11,-7,
-50,-79,-58,-50,-31,32,31,-6,
-4,-25,7,-17,-38,-70,-58,-27,
-43,-83,-28,59,36,20,31,2,
-27,-71,-80,-109,-98,-75,-33,-32,
-31,-2,33,15,-6,43,33,-5,
0,-22,-10,-27,-34,-49,-11,-20,
-41,-91,-100,-121,-39,57,41,10,
-19,-50,-38,-59,-60,-70,-18,-20,
-8,-31,-8,-15,1,-14,-26,-25,
33,21,32,17,1,-19,-19,-26,
-58,-81,-35,-22,45,30,11,-11,
3,-26,-48,-87,-67,-83,-58,3,
-1,-26,-20,44,10,25,39,5,
-9,-35,-27,-38,7,10,4,-9,
-42,-85,-102,-127,52,44,28,10,
-47,-61,-40,-39,-17,-1,-10,-33,
-42,-74,-48,21,-4,70,52,10};


const signed char high_lsp_cdbk2[512]={
-36,-62,6,-9,-10,-14,-56,23,
1,-26,23,-48,-17,12,8,-7,
23,29,-36,-28,-6,-29,-17,-5,
40,23,10,10,-46,-13,36,6,
4,-30,-29,62,32,-32,-1,22,
-14,1,-4,-22,-45,2,54,4,
-30,-57,-59,-12,27,-3,-31,8,
-9,5,10,-14,32,66,19,9,
2,-25,-37,23,-15,18,-38,-31,
5,-9,-21,15,0,22,62,30,
15,-12,-14,-46,77,21,33,3,
34,29,-19,50,2,11,9,-38,
-12,-37,62,1,-15,54,32,6,
2,-24,20,35,-21,2,19,24,
-13,55,4,9,39,-19,30,-1,
-21,73,54,33,8,18,3,15,
6,-19,-47,6,-3,-48,-50,1,
26,20,8,-23,-50,65,-14,-55,
-17,-31,-37,-28,53,-1,-17,-53,
1,57,11,-8,-25,-30,-37,64,
5,-52,-45,15,23,31,15,14,
-25,24,33,-2,-44,-56,-18,6,
-21,-43,4,-12,17,-37,20,-10,
34,15,2,15,55,21,-11,-31,
-6,46,25,16,-9,-25,-8,-62,
28,17,20,-32,-29,26,30,25,
-19,2,-16,-17,26,-51,2,50,
42,19,-66,23,29,-2,3,19,
-19,-37,32,15,6,30,-34,13,
11,-5,40,31,10,-42,4,-9,
26,-9,-70,17,-2,-23,20,-22,
-55,51,-24,-31,22,-22,15,-13,
3,-10,-28,-16,56,4,-63,11,
-18,-15,-18,-38,-35,16,-7,34,
-1,-21,-49,-47,9,-37,7,8,
69,55,20,6,-33,-45,-10,-9,
6,-9,12,71,15,-3,-42,-7,
-24,32,-35,-2,-42,-17,-5,0,
-2,-33,-54,13,-12,-34,47,23,
19,55,7,-8,74,31,14,16,
-23,-26,19,12,-18,-49,-28,-31,
-20,2,-14,-20,-47,78,40,13,
-23,-11,21,-6,18,1,47,5,
38,35,32,46,22,8,13,16,
-14,18,51,19,40,39,11,-26,
-1,-17,47,2,-53,-15,31,-22,
38,21,-15,-16,5,-33,53,15,
-38,86,11,-3,-24,49,13,-4,
-11,-18,28,20,-12,-27,-26,35,
-25,-35,-3,-20,-61,30,10,-55,
-12,-22,-52,-54,-14,19,-32,-12,
45,15,-8,-48,-9,11,-32,8,
-16,-34,-13,51,18,38,-2,-32,
-17,22,-2,-18,-28,-70,59,27,
-28,-19,-10,-20,-9,-9,-8,-21,
21,-8,35,-2,45,-3,-9,12,
0,30,7,-39,43,27,-38,-91,
30,26,19,-55,-4,63,14,-17,
13,9,13,2,7,4,6,61,
72,-1,-17,29,-1,-22,-17,8,
-28,-37,63,44,41,3,2,14,
9,-6,75,-8,-7,-12,-15,-12,
13,9,-4,30,-22,-65,15,0,
-45,4,-4,1,5,22,11,23};
