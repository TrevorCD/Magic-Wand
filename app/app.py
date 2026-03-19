# Host side application for magic wand communications
# written to Python 3.14.3 specifications
#
#-------------------------------------------------------------------------------
#
#   Copyright 2026 Trevor B. Calderwood
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#-------------------------------------------------------------------------------

import bleak    # bluetooth lib
import shutil   # shell utilities
import signal   # signal catching
import curses   # terminal printing

# Globals ----------------------------------------------------------------------

# Handlers ---------------------------------------------------------------------

# Helper functions -------------------------------------------------------------


# Main -------------------------------------------------------------------------

def main(stdscr):

    # curses setup
    curses.noecho() # turns off automatic echoing of keys to screen
    
    # maybe take CLA for size of device area, as in x centimeters of device
    # movement translates to y characters on the terminal
    
    #terminal_columns = shutil.get_terminal_size().columns
    #terminal_lines   = shutil.get_terminal_size().lines

    #print(terminal_columns)
    #print(terminal_lines)
    
    while(1):
        #assert (terminal_columns != -1 and terminal_lines != -1)
        
        stdscr.refresh()
        
    #end while
    
# end main

curses.wrapper(main) # curses wrapper calls main
