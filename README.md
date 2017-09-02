# eti stuff

"Eti-stuff" is an attempt to understand the eti structure as defined in ETS 300 799.

# eti-cmdline

which is derived from the dab-cmdline program and shares the relevant commnd line parameters.

# Syntax

If you want to emulate a full dab-receiver, then just do

      eti-cmdline -C 11C -G 80 | eti-backend -s -
      
## etc-backend
eti-backend is a stripped version of qt-dab, taking as input a file encoded in the eti format.
While the program does work, it was merely a hack to validate the output of the eti-frontend (not supported anymore) and the eti-cmdline programs.

# Disclaimer

The software is under development and most likely contains errors.

eti-stuff is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.


# Copyright

Copyright Jan van Katwijk <J.vanKatwijk@gmail.com>.

This software is part of the Qt-DAB, Qt-DAB is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version, taking into account the licensing conditions of the parts of the software that are derived from wotk of others.

This software uses parts of dabtools. Excerpt from the README of dabtools reads

"dabtools is written by Dave Chapman <dave@dchapman.com>
   
Parts of the code in eti-backend are copied verbatim (or with trivial modifications) from David Crawley's OpenDAB and hence retain his copyright."

Obviously, the copyrights for the parts copied (or directly derived) from the dabtools remain with Dave Chapman.
