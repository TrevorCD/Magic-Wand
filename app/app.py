# Trevor B. Calderwood
# Host side application for magic wand communications
# written to Python 3.14.3 specifications

import bleak    # bluetooth lib
import shutil   # shell utilities
import signal   # signal catching

# Globals ----------------------------------------------------------------------
terminal_columns = -1 # terminal width in characters
terminal_lines   = -1 # terminal height in characters


# Handlers ---------------------------------------------------------------------

def handle_resize(signum, frame):
    assert signum == signal.SIGWINCH, f"Unexpected signal: {signum}"
    terminal_size = shutil.get_terminal_size()
    terminal_columns = terminal_size.columns
    terminal_lines = terminal_size.lines
    
# end handle_resize


# Helper functions -------------------------------------------------------------


# Main -------------------------------------------------------------------------

def main(argv):

    signal.signal(signal.SIGWINCH, handle_resize)
    
    # maybe take CLA for size of device area, as in x centimeters of device
    # movement translates to y characters on the terminal
    
    terminal_size = shutil.get_terminal_size().columns # .columns .lines

    while(1):

        
    #end while
    
# end main
