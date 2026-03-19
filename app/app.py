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

# Globals ----------------------------------------------------------------------
terminal_columns = -1 # terminal width in characters
terminal_lines   = -1 # terminal height in characters


# Handlers ---------------------------------------------------------------------

def handle_resize(signum, frame):
    global terminal_columns, terminal_lines
    assert signum == signal.SIGWINCH, f"Unexpected signal: {signum}"
    terminal_columns = shutil.get_terminal_size().columns
    terminal_lines = shutil.get_terminal_size().lines
    
# end handle_resize


# Helper functions -------------------------------------------------------------


# Main -------------------------------------------------------------------------

def main(argv):

    global terminal_columns, terminal_lines
    
    signal.signal(signal.SIGWINCH, handle_resize)
    
    # maybe take CLA for size of device area, as in x centimeters of device
    # movement translates to y characters on the terminal
    
    terminal_columns = shutil.get_terminal_size().columns
    terminal_lines   = shutil.get_terminal_size().lines

    print(terminal_columns)
    
    while(1):
        assert (terminal_columns != -1 and terminal_lines != -1)
        
        
    #end while
    
# end main

if __name__ == "__main__":
    import sys
    main(sys.argv)
